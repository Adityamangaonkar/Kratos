from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7

from KratosMultiphysics import *
from KratosMultiphysics.StructuralMechanicsApplication  import *
from KratosMultiphysics.ContactStructuralMechanicsApplication  import *
# Importing the solvers (if available)
try:
    from KratosMultiphysics.ExternalSolversApplication import *
    Logger.PrintInfo("ExternalSolversApplication", "succesfully imported")
except ImportError:
    Logger.PrintInfo("ExternalSolversApplication", "not imported")
try:
    from KratosMultiphysics.EigenSolversApplication import *
    Logger.PrintInfo("EigenSolversApplication", "succesfully imported")
except ImportError:
    Logger.PrintInfo("EigenSolversApplication", "not imported")
try:
    from KratosMultiphysics.MeshingApplication import *
    Logger.PrintInfo("MeshingApplication", "succesfully imported")
except ImportError:
    Logger.PrintInfo("MeshingApplication", "not imported")

## Import define_output
parameter_file = open("ProjectParameters.json",'r')
ProjectParameters = Parameters( parameter_file.read())

## Get echo level and parallel type
echo_level = ProjectParameters["problem_data"]["echo_level"].GetInt()
parallel_type = ProjectParameters["problem_data"]["parallel_type"].GetString()

## Import parallel modules if needed
if (parallel_type == "MPI"):
    from KratosMultiphysics.mpi import *
    from KratosMultiphysics.MetisApplication import *
    from KratosMultiphysics.TrilinosApplication import *

## Structure model part definition
main_model_part = ModelPart(ProjectParameters["problem_data"]["model_part_name"].GetString())
main_model_part.ProcessInfo.SetValue(DOMAIN_SIZE, ProjectParameters["problem_data"]["domain_size"].GetInt())

## Solver construction
import python_solvers_wrapper_contact_structural
solver = python_solvers_wrapper_contact_structural.CreateSolver(main_model_part, ProjectParameters)

solver.AddVariables()

## Read the model - note that SetBufferSize is done here
solver.ImportModelPart()

## Add AddDofs
solver.AddDofs()

## Initialize GiD  I/O
output_post  = ProjectParameters.Has("output_configuration")
if (output_post is True):
    if (parallel_type == "OpenMP"):
        from gid_output_process import GiDOutputProcess
        gid_output = GiDOutputProcess(solver.GetComputingModelPart(),
                                      ProjectParameters["problem_data"]["problem_name"].GetString() ,
                                      ProjectParameters["output_configuration"])
    elif (parallel_type == "MPI"):
        from gid_output_process_mpi import GiDOutputProcessMPI
        gid_output = GiDOutputProcessMPI(solver.GetComputingModelPart(),
                                         ProjectParameters["problem_data"]["problem_name"].GetString() ,
                                         ProjectParameters["output_configuration"])

    gid_output.ExecuteInitialize()

## Creation of the Kratos model (build sub_model_parts or submeshes)
StructureModel = Model()
StructureModel.AddModelPart(main_model_part)

## Print model_part and properties
if ((parallel_type == "OpenMP") or (mpi.rank == 0)) and (echo_level > 1):
    Logger.PrintInfo("ModelPart", main_model_part)
    count = 0
    for properties in main_model_part.Properties:
        count += 1
        Logger.PrintInfo("Property " + str(properties.Id), properties)

## Processes construction
import process_factory
list_of_processes = process_factory.KratosProcessFactory(StructureModel).ConstructListOfProcesses(ProjectParameters["constraints_process_list"])
list_of_processes += process_factory.KratosProcessFactory(StructureModel).ConstructListOfProcesses(ProjectParameters["loads_process_list"])
if (ProjectParameters.Has("list_other_processes") is True):
    list_of_processes += process_factory.KratosProcessFactory(StructureModel).ConstructListOfProcesses(ProjectParameters["list_other_processes"])
if (ProjectParameters.Has("contact_process_list") is True):
    list_of_processes += process_factory.KratosProcessFactory(StructureModel).ConstructListOfProcesses(ProjectParameters["contact_process_list"])
if (ProjectParameters.Has("json_output_process") is True):
    list_of_processes += process_factory.KratosProcessFactory(StructureModel).ConstructListOfProcesses(ProjectParameters["json_output_process"])

if ((parallel_type == "OpenMP") or (mpi.rank == 0)) and (echo_level > 1):
    count = 0
    for process in list_of_processes:
        count += 1
        Logger.PrintInfo("Process " + str(count), process)

## Processes initialization
for process in list_of_processes:
    process.ExecuteInitialize()

## Add the processes to the solver
solver.AddProcessesList(list_of_processes)
if (output_post is True):
    solver.AddPostProcess(gid_output)

## Solver initialization
solver.Initialize()
solver.SetEchoLevel(echo_level)

if (output_post is True):
    gid_output.ExecuteBeforeSolutionLoop()

for process in list_of_processes:
    process.ExecuteBeforeSolutionLoop()

## Writing the full ProjectParameters file before solving
if ((parallel_type == "OpenMP") or (mpi.rank == 0)) and (echo_level > 0):
    f = open("ProjectParametersOutput.json", 'w')
    f.write(ProjectParameters.PrettyPrintJsonString())
    f.close()

## Stepping and time settings
delta_time = ProjectParameters["problem_data"]["time_step"].GetDouble()
start_time = ProjectParameters["problem_data"]["start_time"].GetDouble()
end_time = ProjectParameters["problem_data"]["end_time"].GetDouble()

time = start_time
main_model_part.ProcessInfo[STEP] = 0

if (parallel_type == "OpenMP") or (mpi.rank == 0):
    Logger.PrintInfo("::[KCSM Simulation]:: ", "Analysis -START- ")

# Solving the problem (time integration)
while(time <= end_time):

    time = time + delta_time
    main_model_part.ProcessInfo[STEP] += 1
    main_model_part.CloneTimeStep(time)

    if (parallel_type == "OpenMP") or (mpi.rank == 0):
        Logger.PrintInfo("STEP: ", main_model_part.ProcessInfo[STEP])
        Logger.PrintInfo("TIME: ", time)

    for process in list_of_processes:
        process.ExecuteInitializeSolutionStep()

    if (output_post is True):
        gid_output.ExecuteInitializeSolutionStep()

    solver.Solve()

    for process in list_of_processes:
        process.ExecuteFinalizeSolutionStep()

    if (output_post is True):
        gid_output.ExecuteFinalizeSolutionStep()

    for process in list_of_processes:
        process.ExecuteBeforeOutputStep()

    if (output_post is True) and (gid_output.IsOutputStep()):
        gid_output.PrintOutput()

    for process in list_of_processes:
        process.ExecuteAfterOutputStep()

for process in list_of_processes:
    process.ExecuteFinalize()

if (output_post is True):
    gid_output.ExecuteFinalize()

if (parallel_type == "OpenMP") or (mpi.rank == 0):
    Logger.PrintInfo("::[KCSM Simulation]:: ", "Analysis -END- ")
