import KratosMultiphysics
import KratosMultiphysics.FluidDynamicsApplication as KratosFluid

import KratosMultiphysics.KratosUnittest as UnitTest

import os

class WorkFolderScope:
    def __init__(self, work_folder):
        self.currentPath = os.getcwd()
        self.scope = os.path.abspath(os.path.join(os.path.dirname(os.path.realpath(__file__)),work_folder))

    def __enter__(self):
        os.chdir(self.scope)

    def __exit__(self, type, value, traceback):
        os.chdir(self.currentPath)

class EmbeddedAusasCouetteTest(UnitTest.TestCase):

    def setUp(self):
        self.distance = 0.5
        self.check_tolerance = 1e-6
        self.print_output = False
        self.print_reference_values = False
        self.work_folder = "EmbeddedAusasCouetteTest"
        self.settings = "EmbeddedAusasCouetteTestParameters.json"

    def tearDown(self):
        with WorkFolderScope(self.work_folder):
            try:
                os.remove(self.ProjectParameters["solver_settings"]["model_import_settings"]["input_filename"].GetString()+'.time')
            except FileNotFoundError as e:
                pass

    def testEmbeddedAusasCouette(self):
        with WorkFolderScope(self.work_folder):
            self.setUp()
            self.setUpProblem()
            self.setUpDistanceBoundaryConditions()
            self.runTest()
            self.tearDown()
            self.checkResults()

    def setUpProblem(self):
        with WorkFolderScope(self.work_folder):
            parameter_file = open(self.settings, 'r')

            self.ProjectParameters = KratosMultiphysics.Parameters(parameter_file.read())

            self.main_model_part = KratosMultiphysics.ModelPart(self.ProjectParameters["problem_data"]["model_part_name"].GetString())
            self.main_model_part.ProcessInfo.SetValue(KratosMultiphysics.DOMAIN_SIZE, self.ProjectParameters["problem_data"]["domain_size"].GetInt())

            Model = {self.ProjectParameters["problem_data"]["model_part_name"].GetString() : self.main_model_part}

            ## Solver construction
            import python_solvers_wrapper_fluid
            self.solver = python_solvers_wrapper_fluid.CreateSolver(self.main_model_part, self.ProjectParameters)

            self.solver.AddVariables()

            ## Read the model - note that SetBufferSize is done here
            self.solver.ImportModelPart()

            ## Add AddDofs
            self.solver.AddDofs()

            ## Solver initialization
            self.solver.Initialize()

            ## Get the list of the skin submodel parts in the object Model
            for i in range(self.ProjectParameters["solver_settings"]["skin_parts"].size()):
                skin_part_name = self.ProjectParameters["solver_settings"]["skin_parts"][i].GetString()
                Model.update({skin_part_name: self.main_model_part.GetSubModelPart(skin_part_name)})

            ## Get the gravity submodel part in the object Model
            for i in range(self.ProjectParameters["gravity"].size()):
                gravity_part_name = self.ProjectParameters["gravity"][i]["Parameters"]["model_part_name"].GetString()
                Model.update({gravity_part_name: self.main_model_part.GetSubModelPart(gravity_part_name)})

            ## Processes construction
            import process_factory
            self.list_of_processes  = process_factory.KratosProcessFactory(Model).ConstructListOfProcesses( self.ProjectParameters["gravity"] )
            self.list_of_processes += process_factory.KratosProcessFactory(Model).ConstructListOfProcesses( self.ProjectParameters["boundary_conditions_process_list"] )

            ## Processes initialization
            for process in self.list_of_processes:
                process.ExecuteInitialize()

    def setUpDistanceBoundaryConditions(self):
        # Set the distance function
        for node in self.main_model_part.Nodes:
            distance = node.Z-self.distance
            node.SetSolutionStepValue(KratosMultiphysics.DISTANCE, 0, distance)

        # Set the inlet
        for node in self.main_model_part.GetSubModelPart("Inlet3D").Nodes:
            if (node.GetSolutionStepValue(KratosMultiphysics.DISTANCE) > 0.0):
                vel = KratosMultiphysics.Vector(3)
                vel[0] = (node.Z-self.distance)/(2-self.distance)
                vel[1] = 0.0
                vel[2] = 0.0

                node.Fix(KratosMultiphysics.VELOCITY_X)
                node.Fix(KratosMultiphysics.VELOCITY_Y)
                node.Fix(KratosMultiphysics.VELOCITY_Z)
                node.SetSolutionStepValue(KratosMultiphysics.VELOCITY, 0, vel)   

    def runTest(self):
        with WorkFolderScope(self.work_folder):
            if (self.print_output):
                gid_mode = KratosMultiphysics.GiDPostMode.GiD_PostBinary
                multifile = KratosMultiphysics.MultiFileFlag.SingleFile
                deformed_mesh_flag = KratosMultiphysics.WriteDeformedMeshFlag.WriteUndeformed
                write_conditions = KratosMultiphysics.WriteConditionsFlag.WriteElementsOnly
                gid_io = KratosMultiphysics.GidIO(self.ProjectParameters["solver_settings"]["model_import_settings"]["input_filename"].GetString(),gid_mode,multifile,deformed_mesh_flag, write_conditions)

                mesh_name = 0.0
                gid_io.InitializeMesh( mesh_name)
                gid_io.WriteMesh( self.main_model_part.GetMesh() )
                gid_io.FinalizeMesh()
                gid_io.InitializeResults(mesh_name,(self.main_model_part).GetMesh())

            end_time = self.ProjectParameters["problem_data"]["end_time"].GetDouble()

            time = 0.0
            step = 0

            for process in self.list_of_processes:
                process.ExecuteBeforeSolutionLoop()

            while(time <= end_time):

                Dt = self.solver.ComputeDeltaTime()
                time = time + Dt
                step = step + 1
                self.main_model_part.CloneTimeStep(time)

                for process in self.list_of_processes:
                    process.ExecuteInitializeSolutionStep()

                if(step >= 3):
                    self.solver.Solve()

                for process in self.list_of_processes:
                    process.ExecuteFinalizeSolutionStep()

                for process in self.list_of_processes:
                    process.ExecuteBeforeOutputStep()

                if (self.print_output):
                    gid_io.WriteNodalResults(KratosMultiphysics.VELOCITY,self.main_model_part.Nodes,time,0)
                    gid_io.WriteNodalResults(KratosMultiphysics.PRESSURE,self.main_model_part.Nodes,time,0)
                    gid_io.WriteNodalResults(KratosMultiphysics.DISTANCE,self.main_model_part.Nodes,time,0)

                for process in self.list_of_processes:
                    process.ExecuteAfterOutputStep()

            for process in self.list_of_processes:
                process.ExecuteFinalize()

            if (self.print_output):
                gid_io.FinalizeResults()

    def checkResults(self):
        pass

        # if self.print_reference_values:
        #     with open(self.reference_file+'.csv','w') as ref_file:
        #         ref_file.write("#ID, VELOCITY_X, VELOCITY_Y, TEMPERATURE\n")
        #         for node in self.fluid_model_part.Nodes:
        #             vel = node.GetSolutionStepValue(VELOCITY,0)
        #             temp = node.GetSolutionStepValue(TEMPERATURE,0)
        #             ref_file.write("{0}, {1}, {2}, {3}\n".format(node.Id, vel[0], vel[1], temp))
        # else:
        #     with open(self.reference_file+'.csv','r') as reference_file:
        #         reference_file.readline() # skip header
        #         line = reference_file.readline()
        #         node_iter = self.fluid_model_part.Nodes

        #         for node in self.fluid_model_part.Nodes:
        #             values = [ float(i) for i in line.rstrip('\n ').split(',') ]
        #             node_id = values[0]
        #             reference_vel_x = values[1]
        #             reference_vel_y = values[2]
        #             reference_temp = values[3]

        #             velocity = node.GetSolutionStepValue(VELOCITY)
        #             self.assertAlmostEqual(reference_vel_x, velocity[0], delta=self.check_tolerance)
        #             self.assertAlmostEqual(reference_vel_y, velocity[1], delta=self.check_tolerance)
        #             temperature = node.GetSolutionStepValue(TEMPERATURE)
        #             self.assertAlmostEqual(reference_temp, temperature, delta=self.check_tolerance)

        #             line = reference_file.readline()
        #         if line != '': # If we did not reach the end of the reference file
        #             self.fail("The number of nodes in the mdpa is smaller than the number of nodes in the output file")

if __name__ == '__main__':
    test = EmbeddedAusasCouetteTest()
    test.setUp()
    test.print_output = True
    #test.print_reference_values = True
    test.setUpProblem()
    test.setUpDistanceBoundaryConditions()
    test.runTest()
    test.tearDown()
    test.checkResults()
    
