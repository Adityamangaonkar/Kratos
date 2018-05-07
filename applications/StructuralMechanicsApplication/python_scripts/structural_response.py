"""This module contains the available structural response functions and their base class"""
from __future__ import print_function, absolute_import, division

# importing the Kratos Library
from KratosMultiphysics import *
import structural_mechanics_analysis

import time as timer

# ==============================================================================
class ResponseFunctionBase(object):
    """The base class for structural response functions. Each response function
    is able to calculate its response value and gradient.
    All the necessary steps have to be implemented, like e.g. initializing,
    solving of primal (and adjoint) analysis ...
    """

    def RunCalculation(self, calculate_gradient):
        self.Initialize()
        self.InitializeSolutionStep()
        self.CalculateValue()
        if calculate_gradient:
            self.CalculateGradient()
        self.FinalizeSolutionStep()
        self.Finalize()

    def Initialize(self):
        pass

    def InitializeSolutionStep(self):
        pass

    def CalculateValue(self):
        raise NotImplementedError("CalculateValue needs to be implemented by the derived class")

    def CalculateGradient(self):
        raise NotImplementedError("CalculateGradient needs to be implemented by the derived class")

    def FinalizeSolutionStep(self):
        pass

    def Finalize(self):
        pass

    def GetValue(self):
        raise NotImplementedError("GetValue needs to be implemented by the derived class")

    def GetShapeGradient(self):
        raise NotImplementedError("GetShapeGradient needs to be implemented by the derived class")

# ==============================================================================
class StrainEnergyResponseFunction(ResponseFunctionBase):
    """Linear strain energy response function. It triggers the primal analysis and
    uses the primal analysis results to calculate response value and gradient.

    Attributes
    ----------
    primal_analysis : Primal analysis object of the response function
    response_function_utility: Cpp utilities object doing the actual computation of response value and gradient.
    """

    def __init__(self, identifier, response_settings, model_part = None):
        self.identifier = identifier

        self.response_function_utility = StructuralMechanicsApplication.StrainEnergyResponseFunctionUtility(model_part, response_settings)

        with open(response_settings["primal_settings"].GetString()) as parameters_file:
            ProjectParametersPrimal = Parameters(parameters_file.read())

        self.primal_analysis = structural_mechanics_analysis.StructuralMechanicsAnalysis(ProjectParametersPrimal, model_part)
        self.primal_analysis.GetModelPart().AddNodalSolutionStepVariable(SHAPE_SENSITIVITY)

    def Initialize(self):
        self.primal_analysis.Initialize()
        self.response_function_utility.Initialize()

    def InitializeSolutionStep(self):
        self.primal_analysis.InitializeTimeStep()

    def CalculateValue(self):
        Logger.PrintInfo("\n> Starting primal analysis for response", self.identifier)

        startTime = timer.time()
        self.primal_analysis.SolveTimeStep()
        Logger.PrintInfo("> Time needed for solving the primal analysis",round(timer.time() - startTime,2),"s")

        startTime = timer.time()
        value = self.response_function_utility.CalculateValue()
        self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE] = value
        Logger.PrintInfo("> Time needed for calculating the response value",round(timer.time() - startTime,2),"s")

    def CalculateGradient(self):
        Logger.PrintInfo("\n> Starting gradient calculation for response", self.identifier)

        startTime = timer.time()
        self.response_function_utility.CalculateGradient()
        Logger.PrintInfo("> Time needed for calculating gradients",round(timer.time() - startTime,2),"s")

    def FinalizeSolutionStep(self):
        # TODO we should not use the output of the primal analysis to output sensitivities
        self.primal_analysis.FinalizeTimeStep()

    def Finalize(self):
        self.primal_analysis.Finalize()

    def GetValue(self):
        return self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE]

    def GetShapeGradient(self):
        gradient = {}
        for node in self.primal_analysis.GetModelPart().Nodes:
            gradient[node.Id] = node.GetSolutionStepValue(SHAPE_SENSITIVITY)
        return gradient

# ==============================================================================
class EigenFrequencyResponseFunction(StrainEnergyResponseFunction):
    """Eigenfrequency response function. The internal procedure is the same as
    for the StrainEnergyResponseFunction. It triggers the primal analysis and
    uses the primal analysis results to calculate response value and gradient.
    Only the response_function_utility is a different object.

    Attributes
    ----------
    primal_analysis : Primal analysis object of the response function
    response_function_utility: Cpp utilities object doing the actual computation of response value and gradient.
    """

    def __init__(self, identifier, response_settings, model_part = None):
        self.identifier = identifier

        self.response_function_utility = StructuralMechanicsApplication.EigenfrequencyResponseFunctionUtility(model_part, response_settings)

        with open(response_settings["primal_settings"].GetString()) as parameters_file:
            ProjectParametersPrimal = Parameters(parameters_file.read())

        eigen_solver_settings = ProjectParametersPrimal["solver_settings"]["eigensolver_settings"]

        max_required_eigenfrequency = int(max(response_settings["traced_eigenfrequencies"].GetVector()))
        if max_required_eigenfrequency is not eigen_solver_settings["number_of_eigenvalues"].GetInt():
            print("\n> WARNING: Specified number of eigenvalues in the primal analysis and the max required eigenvalue according the response settings do not match!!!")
            print("  Primal parameters were adjusted accordingly!\n")
            eigen_solver_settings["number_of_eigenvalues"].SetInt(max_required_eigenfrequency)

        if not eigen_solver_settings.Has("normalize_eigenvectors"):
            eigen_solver_settings.AddEmptyValue("normalize_eigenvectors")
            eigen_solver_settings["normalize_eigenvectors"].SetBool(True)
            print("\n> WARNING: Eigenfrequency response function requires mass normalization of eigenvectors!")
            print("  Primal parameters were adjusted accordingly!\n")

        if not eigen_solver_settings["normalize_eigenvectors"].GetBool():
            eigen_solver_settings["normalize_eigenvectors"].SetBool(True)
            print("\n> WARNING: Eigenfrequency response function requires mass normalization of eigenvectors!")
            print("  Primal parameters were adjusted accordingly!\n")

        self.primal_analysis = structural_mechanics_analysis.StructuralMechanicsAnalysis(ProjectParametersPrimal, model_part)
        self.primal_analysis.GetModelPart().AddNodalSolutionStepVariable(SHAPE_SENSITIVITY)

# ==============================================================================
class MassResponseFunction(ResponseFunctionBase):
    """Mass response function. It reads the materials for the model part and
    calculates response value and gradient.

    Attributes
    ----------
    model_part : Model part object of the response function
    response_function_utility: Cpp utilities object doing the actual computation of response value and gradient.
    """

    def __init__(self, identifier, response_settings, model_part):
        self.identifier = identifier
        self.response_settings = response_settings

        self.response_function_utility = StructuralMechanicsApplication.MassResponseFunctionUtility(model_part, response_settings)

        self.model_part = model_part
        self.model_part.AddNodalSolutionStepVariable(SHAPE_SENSITIVITY)

    def Initialize(self):
        import read_materials_process
        # Create a dictionary of model parts.
        model = Model()
        model.AddModelPart(self.model_part)
        # Add constitutive laws and material properties from json file to model parts.
        read_materials_process.ReadMaterialsProcess(model, self.response_settings["material_import_settings"])
        self.response_function_utility.Initialize()

    def CalculateValue(self):
        Logger.PrintInfo("\n> Starting primal analysis for response", self.identifier)

        startTime = timer.time()
        value = self.response_function_utility.CalculateValue()
        self.model_part.ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE] = value
        Logger.PrintInfo("> Time needed for calculating the response value = ",round(timer.time() - startTime,2),"s")

    def CalculateGradient(self):
        Logger.PrintInfo("\n> Starting gradient calculation for response", self.identifier)

        startTime = timer.time()
        self.response_function_utility.CalculateGradient()
        Logger.PrintInfo("> Time needed for calculating gradients",round(timer.time() - startTime,2),"s")

    def GetValue(self):
        return self.model_part.ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE]

    def GetShapeGradient(self):
        gradient = {}
        for node in self.model_part.Nodes:
            gradient[node.Id] = node.GetSolutionStepValue(SHAPE_SENSITIVITY)
        return gradient

# ==============================================================================
class AdjointResponseFunction(ResponseFunctionBase):
    """Linear static adjoint response function.
    - runs the primal analysis (writes the primal results to an .h5 file)
    - reads the primal results from the .h5 file into the adjoint model part
    - uses primal results to calculate value
    - uses primal results to calculate gradient by running the adjoint analysis

    Attributes
    ----------
    primal_analysis : Primal analysis object of the response function
    adjoint_analysis : Adjoint analysis object of the response function
    response_function_utility: Cpp utilities object doing the actual computation of response value and gradient.
    """
    def __init__(self, identifier, project_parameters, model_part = None):
        self.identifier = identifier

        # Create the primal solver
        ProjectParametersPrimal = Parameters( open(project_parameters["primal_settings"].GetString(),'r').read() )
        self.primal_analysis = structural_mechanics_analysis.StructuralMechanicsAnalysis(ProjectParametersPrimal, model_part)

        # Create the adjoint solver
        ProjectParametersAdjoint = Parameters( open(project_parameters["adjoint_settings"].GetString(),'r').read() )
        ProjectParametersAdjoint["solver_settings"].AddValue("response_function_settings", project_parameters)
        self.adjoint_analysis = structural_mechanics_analysis.StructuralMechanicsAnalysis(ProjectParametersAdjoint)
        # TODO find out why it is not possible to use the same model_part

    def Initialize(self):
        self.primal_analysis.Initialize()
        self.adjoint_analysis.Initialize()

        # TODO should be created here, not in solver!
        self.response_function_utility = self.adjoint_analysis.GetSolver().response_function

    def InitializeSolutionStep(self):
        # synchronize the modelparts # TODO this should happen automatically
        print("\n> Synchronize primal and adjoint modelpart for response:", self.identifier)
        for node in self.adjoint_analysis.GetModelPart().Nodes:
            ref_node = self.primal_analysis.GetModelPart().Nodes[node.Id]
            node.X0 = ref_node.X0
            node.Y0 = ref_node.Y0
            node.Z0 = ref_node.Z0
            node.X = ref_node.X
            node.Y = ref_node.Y
            node.Z = ref_node.Z

        # Run the primal analysis.
        # TODO if primal_analysis.status==solved: return
        print("\n> Starting primal analysis for response:", self.identifier)
        startTime = timer.time()
        self.primal_analysis.InitializeTimeStep()
        self.primal_analysis.SolveTimeStep()
        self.primal_analysis.FinalizeTimeStep()
        print("> Time needed for solving the primal analysis = ",round(timer.time() - startTime,2),"s")

        self.adjoint_analysis.InitializeTimeStep()

    def CalculateValue(self):
        startTime = timer.time()
        value = self.response_function_utility.CalculateValue(self.primal_analysis.GetModelPart())
        print("> Time needed for calculating the response value = ",round(timer.time() - startTime,2),"s")

        self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE] = value

    def CalculateGradient(self):
        print("\n> Starting adjoint analysis for response:", self.identifier)
        startTime = timer.time()
        self.adjoint_analysis.SolveTimeStep()
        print("> Time needed for solving the adjoint analysis = ",round(timer.time() - startTime,2),"s")

    def GetValue(self):
        return self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE]

    def GetShapeGradient(self):
        gradient = {}
        for node in self.adjoint_analysis.GetModelPart().Nodes:
            gradient[node.Id] = node.GetSolutionStepValue(SHAPE_SENSITIVITY)
        return gradient
        # TODO reset DISPLACEMENT, ROTATION ADJOINT_DISPLACEMENT and ADJOINT_ROTATION

    def FinalizeSolutionStep(self):
        self.adjoint_analysis.FinalizeTimeStep()

    def Finalize(self):
        self.primal_analysis.Finalize()
        self.adjoint_analysis.Finalize()

# ==============================================================================
class AdjointStrainEnergyResponse(ResponseFunctionBase):
    """Linear static adjoint response function.
    - runs the primal analysis (writes the primal results to an .h5 file)
    - reads the primal results from the .h5 file into the adjoint model part
    - uses primal results to calculate value
    - uses primal results to calculate gradient

    Note: because of the special property of the adjoint strain energy response,
    no adjoint solution is necessary. except from that it works the same as
    'AdjointResponseFunction'.

    Attributes
    ----------
    primal_analysis : Primal analysis object of the response function
    response_function_utility: Cpp utilities object doing the actual computation of response value and gradient.
    """
    def __init__(self, identifier, project_parameters, model_part = None):
        self.identifier = identifier
        self.response_function_utility = StructuralMechanicsApplication.AdjointStrainEnergyResponseFunction( model_part, project_parameters )

        # Create the primal solver
        self.ProjectParametersPrimal = Parameters( open(project_parameters["primal_settings"].GetString(),'r').read() )
        self.primal_analysis = structural_mechanics_analysis.StructuralMechanicsAnalysis(self.ProjectParametersPrimal, model_part)

        self.primal_analysis.GetModelPart().AddNodalSolutionStepVariable(SHAPE_SENSITIVITY)

        # Add variables to save the solution of the adjoint problem
        self.primal_analysis.GetModelPart().AddNodalSolutionStepVariable(StructuralMechanicsApplication.ADJOINT_DISPLACEMENT)
        if self.ProjectParametersPrimal["solver_settings"]["rotation_dofs"].GetBool():
            self.primal_analysis.GetModelPart().AddNodalSolutionStepVariable(StructuralMechanicsApplication.ADJOINT_ROTATION)
        #self.primal_analysis.GetModelPart().AddNodalSolutionStepVariable(StructuralMechanicsApplication.POINT_LOAD_SENSITIVITY)
        # TODO: Is it necessary to add other variables (e.g. POINT_LOAD_SENSITIVITY)?

        self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.IS_ADJOINT] = False

    def Initialize(self):
        self.primal_analysis.Initialize()

    def InitializeSolutionStep(self):
        print("\n> Starting primal analysis for response:", self.identifier)
        startTime = timer.time()
        self.primal_analysis.InitializeTimeStep()
        self.primal_analysis.SolveTimeStep()
        self.primal_analysis.FinalizeTimeStep()
        print("> Time needed for solving the primal analysis = ",round(timer.time() - startTime,2),"s")

    def CalculateValue(self):
        startTime = timer.time()
        value = self.response_function_utility.CalculateValue(self.primal_analysis.GetModelPart())
        print("> Time needed for calculating the response value = ",round(timer.time() - startTime,2),"s")
        self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE] = value

    def CalculateGradient(self):
        # Replace elements and conditions by its adjoint equivalents
        self.__performReplacementProcess(True)
        self.response_function_utility.Initialize()
        self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.IS_ADJOINT] = True

        # 'Solve' the adjoint problem
        for node in self.primal_analysis.GetModelPart().Nodes:
            adjoint_displacement = 0.5 * node.GetSolutionStepValue(DISPLACEMENT)
            node.SetSolutionStepValue(StructuralMechanicsApplication.ADJOINT_DISPLACEMENT, adjoint_displacement)
            if self.primal_analysis.solver.settings["rotation_dofs"].GetBool():
                adjoint_rotation = 0.5 * node.GetSolutionStepValue(ROTATION)
                node.SetSolutionStepValue(StructuralMechanicsApplication.ADJOINT_ROTATION, adjoint_rotation)

        # Compute Sensitivities in a post-processing step
        self.response_function_utility.FinalizeSolutionStep()

        # Replace elements and conditions back to its origins
        self.__performReplacementProcess(False)
        self.__initializeAfterReplacement(self.primal_analysis.GetModelPart())
        self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.IS_ADJOINT] = False

    def GetValue(self):
        return self.primal_analysis.GetModelPart().ProcessInfo[StructuralMechanicsApplication.RESPONSE_VALUE]

    def GetShapeGradient(self):
        gradient = {}
        for node in self.primal_analysis.GetModelPart().Nodes:
            gradient[node.Id] = node.GetSolutionStepValue(SHAPE_SENSITIVITY)
        return gradient

    #TODO: Is it necessary to reset some variables (DISPLACEMENT, ROTATION, ADJOINT_DISPLACEMENT and ADJOINT_ROTATION)
    def Finalize(self):
        self.primal_analysis.Finalize()

    def __performReplacementProcess(self, from_primal_to_adjoint=True):
        self.ProjectParametersPrimal.AddEmptyValue("element_replace_settings")
        if(self.primal_analysis.GetModelPart().ProcessInfo[DOMAIN_SIZE] == 3):
            if(from_primal_to_adjoint == True):
                self.ProjectParametersPrimal["element_replace_settings"] = Parameters("""
                    {
                    "add_string": "Adjoint",
                    "add_before_in_element_name": "Element",
                    "add_before_in_condition_name": "Condition",
                    "elements_conditions_to_ignore": "ShapeOptimizationCondition",
                    "from_primal_to_adjoint": true
                    }
                    """)
            else:
                self.ProjectParametersPrimal["element_replace_settings"] = Parameters("""
                    {
                    "add_string": "Adjoint",
                    "add_before_in_element_name": "Element",
                    "add_before_in_condition_name": "Condition",
                    "elements_conditions_to_ignore": "ShapeOptimizationCondition",
                    "from_primal_to_adjoint": false
                    }
                    """)

        elif(self.primal_analysis.GetModelPart().ProcessInfo[DOMAIN_SIZE] == 2):
            raise Exception("there is currently no 2D adjoint element")
        else:
            raise Exception("domain size is not 2 or 3")
        StructuralMechanicsApplication.ReplaceElementsAndConditionsForAdjointProblemProcess(
            self.primal_analysis.GetModelPart(), self.ProjectParametersPrimal["element_replace_settings"]).Execute()

    def __initializeAfterReplacement(self, model_part):
        for element in model_part.Elements:
            element.Initialize()
        for condition in model_part.Conditions:
            condition.Initialize()


