from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

# Importing the Kratos Library
import KratosMultiphysics as KM

# Check that applications were imported in the main script
KM.CheckRegisteredApplications("StructuralMechanicsApplication")
      
import KratosMultiphysics.StructuralMechanicsApplication as SMA

def Factory(settings, Model):
    if(type(settings) != KM.Parameters):
        raise Exception("Expected input shall be a Parameters object, encapsulating a json string")
    return SPRISMProcess(Model, settings["Parameters"])

import python_process

# All the processes python processes should be derived from "python_process"

class SPRISMProcess(python_process.PythonProcess):
    """This class is used in order to compute some pre and post process on the SPRISM solid shell elements

    Only the member variables listed below should be accessed directly.

    Public member variables:
    model_part -- the model part used to construct the process.
    settings -- Kratos parameters containing the settings.
    """

    def __init__(self, model_part, settings):
        """ The default constructor of the class

        Keyword arguments:
        self -- It signifies an instance of a class.
        model_part -- the model part used to construct the process.
        settings -- Kratos parameters containing solver settings.
        """

        # Settings string in json format
        default_parameters = KM.Parameters("""
        {
            "mesh_id"                        : 0,
            "model_part_name"                : "Structure",
            "preprocess_shell_to_solidshell" : {
                "element_name"              : "SolidShellElementSprism3D6N",
                "new_constitutive_law_name" : "LinearElastic3DLaw",
                "number_of_layers"          : 1,
                "export_to_mdpa"            : false,
                "output_name"               : "output",
                "initialize_elements"       : false
            }
        }
        """)

        # Overwrite the default settings with user-provided parameters
        self.settings = settings
        self.settings.RecursivelyValidateAndAssignDefaults(default_parameters)

        # We define the model parts
        self.main_model_part = model_part
        self.solid_shell_model_part = model_part[self.settings["model_part_name"].GetString()]
        
        # We create the process to compute the neighbours (should be run each time we recompute connectivity)
        self.sprism_neighbour_search = SMA.PrismNeighboursProcess(self.solid_shell_model_part)

        # We create the process to compute the thickness of the solid shell (post-process info)
        self.thickness_compute_process = SMA.SolidShellThickComputeProcess(self.solid_shell_model_part)

    def ExecuteInitialize(self):
        """ This method is executed at the begining to initialize the process

        Keyword arguments:
        self -- It signifies an instance of a class.
        """

        # We preprocess from triangle shells to SPRISM solid-shells
        shell_to_solid_shell_parameters = KM.Parameters("""{}""")
        shell_to_solid_shell_parameters.AddValue("element_name", self.settings["preprocess_shell_to_solidshell"]["element_name"])
        shell_to_solid_shell_parameters.AddValue("new_constitutive_law_name", self.settings["preprocess_shell_to_solidshell"]["new_constitutive_law_name"])
        shell_to_solid_shell_parameters.AddValue("model_part_name", self.settings["model_part_name"])
        shell_to_solid_shell_parameters.AddValue("number_of_layers", self.settings["preprocess_shell_to_solidshell"]["number_of_layers"])
        shell_to_solid_shell_parameters.AddValue("export_to_mdpa", self.settings["preprocess_shell_to_solidshell"]["export_to_mdpa"])
        shell_to_solid_shell_parameters.AddValue("output_name", self.settings["preprocess_shell_to_solidshell"]["output_name"])
        shell_to_solid_shell_parameters.AddValue("initialize_elements", self.settings["preprocess_shell_to_solidshell"]["initialize_elements"])

        preprocess_shell_to_solidshell = SMA.TriangleShellToSolidShellProcess(self.main_model_part, shell_to_solid_shell_parameters)
        preprocess_shell_to_solidshell.Execute()

        # We compute the neighbours
        self.sprism_neighbour_search.Execute()
    
    def ExecuteBeforeSolutionLoop(self):
        """ This method is executed before starting the time loop

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        pass
    
    def ExecuteInitializeSolutionStep(self):
        """ This method is executed in order to initialize the current step

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        # We compute the neighbours if we have remeshed the problem
        if (self.main_model_part.Is(KM.MODIFIED)):
            self.sprism_neighbour_search.Execute()

    def ExecuteFinalizeSolutionStep(self):
        """ This method is executed in order to finalize the current step

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        # We compute the thickness of the solid shell element
        self.thickness_compute_process.Execute()
              
    def ExecuteBeforeOutputStep(self):
        """ This method is executed right before the ouput process computation

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        pass

    def ExecuteAfterOutputStep(self):
        """ This method is executed right after the ouput process computation

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        pass

    def ExecuteFinalize(self):
        """ This method is executed in order to finalize the current computation

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        pass
