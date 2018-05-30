from __future__ import print_function, absolute_import, division

# importing the Kratos Library
from KratosMultiphysics import *
import os

def CreateResponseFunction(response_id, response_settings, model_part):
    return AnalysisDriverBasedResponseFunction(response_id, response_settings, model_part)

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

class AnalysisDriverBasedResponseFunction(ResponseFunctionBase):

    def __init__(self, response_id, response_settings, model_part):
        self.model_part = model_part
        self.response_settings = response_settings
        self.response_id = response_id

        self.model_part_filename = response_settings["optimization_model_part_name"].GetString()
        self.analysis_driver_name = response_settings["analysis_driver"].GetString()
        self.log_file = "%s.log" % self.analysis_driver_name
        self.results_file = "%s.results.h5" % self.analysis_driver_name

        self.analysis_driver = __import__(self.analysis_driver_name)

        self.is_analysis_step_completed = False


    def InitializeSolutionStep(self):
        self.is_analysis_step_completed = self.__IsAnalysisCompleted()
        
        if not self.is_analysis_step_completed:
            model_part_io = ModelPartIO(self.model_part_filename, ModelPartIO.WRITE)
            model_part_io.WriteModelPart(self.model_part)
        else:
            # read results from the file.
            self.__ReadResults()

    def CalculateValue(self):
        if not self.is_analysis_step_completed:
            self.response_data = {}
            self.analysis_driver.Run(self.model_part, self.response_data)

    def GetValue(self):
        return self.response_data["response_value"]

    def GetShapeGradient(self):
        return self.response_data["response_gradients"]           

    def FinalizeSolutionStep(self):
        if not self.is_analysis_step_completed:
            # write the final results to have restart capabilities
            self.__WriteResults()

            # file_append = open(self.log_file, "a")
            # file_append.write("KRATOS ANALYSIS STEP COMPLETED.")
            # file_append.close()
    
    def __ReadResults(self):
        pass

    def __WriteResults(self):
        pass

    def __IsAnalysisCompleted(self):
        if os.path.isfile(self.log_file):
            with open(self.log_file, "r") as file_input:
                _lines = file_input.readlines()
            file_input.close()

            for i in range(0, len(_lines)):
                _line = _lines[-i-1].strip()
                if _line != "":
                    if _line == "KRATOS ANALYSIS STEP COMPLETED.":
                        return True
                    else:
                        return False
        return False



