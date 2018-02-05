# ==============================================================================
#  KratosShapeOptimizationApplication
#
#  License:         BSD License
#                   license: ShapeOptimizationApplication/license.txt
#
#  Main authors:    Baumgaertner Daniel, https://github.com/dbaumgaertner
#
# ==============================================================================

# Making KratosMultiphysics backward compatible with python 2.6 and 2.7
from __future__ import print_function, absolute_import, division 

# importing the Kratos Library
from KratosMultiphysics import *
from KratosMultiphysics.ALEApplication import *
from KratosMultiphysics.ShapeOptimizationApplication import *

# check that KratosMultiphysics was imported in the main script
CheckForPreviousImport()

# Additional imports
import time as timer

from mesh_controller_base import MeshController

# # ==============================================================================
class MeshControllerUsingALESolver( MeshController) :
    # --------------------------------------------------------------------------
    def __init__( self, OptimizationModelPart, MeshMotionSettings ):
        self.OptimizationModelPart = OptimizationModelPart
        self.MeshMotionSettings = MeshMotionSettings

        mesh_solver_module = __import__(self.MeshMotionSettings["mesh_solver_settings"]["solver_type"].GetString())
        self.mesh_solver = mesh_solver_module.CreateSolver(self.OptimizationModelPart, self.MeshMotionSettings["mesh_solver_settings"])
        
        self.mesh_solver.AddVariables()

    # --------------------------------------------------------------------------    
    def Initialize( self ):
        self.mesh_solver.AddDofs()
        self.mesh_solver.Initialize()
        self.mesh_solver.SetEchoLevel(0)    

    # --------------------------------------------------------------------------
    def UpdateMeshAccordingInputVariable( self, InputVariable ):
        print("\n> Starting to update the mesh")
        startTime = timer.time()

        # Extract surface nodes
        sub_model_part_of_surface = "surface_nodes"     
        GeometryUtilities(self.OptimizationModelPart).ExtractSurfaceNodes(sub_model_part_of_surface)

        # Apply shape update as boundary condition for computation of mesh displacement 
        for node in self.OptimizationModelPart.GetSubModelPart(sub_model_part_of_surface).Nodes:
            node.Fix(MESH_DISPLACEMENT_X)
            node.Fix(MESH_DISPLACEMENT_Y)
            node.Fix(MESH_DISPLACEMENT_Z)              
            disp = Vector(3)
            disp[0] = node.GetSolutionStepValue(SHAPE_UPDATE_X)
            disp[1] = node.GetSolutionStepValue(SHAPE_UPDATE_Y)
            disp[2] = node.GetSolutionStepValue(SHAPE_UPDATE_Z)
            node.SetSolutionStepValue(MESH_DISPLACEMENT,0,disp)

        # Solve for mesh-update
        self.mesh_solver.Solve()

        # Update reference mesh (Since shape updates are imposed as incremental quantities)
        self.mesh_solver.get_mesh_motion_solver().UpdateReferenceMesh()

        # Log absolute mesh displacement
        for node in self.OptimizationModelPart.Nodes:
            mesh_change = Vector(3)
            mesh_change[0] = node.GetSolutionStepValue(MESH_CHANGE_X) + node.GetSolutionStepValue(MESH_DISPLACEMENT_X)
            mesh_change[1] = node.GetSolutionStepValue(MESH_CHANGE_Y) + node.GetSolutionStepValue(MESH_DISPLACEMENT_Y)
            mesh_change[2] = node.GetSolutionStepValue(MESH_CHANGE_Z) + node.GetSolutionStepValue(MESH_DISPLACEMENT_Z)
            node.SetSolutionStepValue(MESH_CHANGE,0,mesh_change)     

        print("> Time needed for updating the mesh = ",round(timer.time() - startTime,2),"s")    

    # # --------------------------------------------------------------------------
    # def computeAndAddMeshDerivativesToGradient( self, gradientOnDesignSurface, gradientForCompleteModelPart):
    
    #     # Here we solve the pseudo-elastic mesh-motion system again using modified BCs
    #     # The contributions from the mesh derivatives appear as reaction forces
    #     for node in main_model_part.Nodes:

    #         # Apply dirichlet conditions
    #         if node.Id in gradientOnDesignSurface.keys():
    #             node.Fix(MESH_DISPLACEMENT_X)
    #             node.Fix(MESH_DISPLACEMENT_Y)
    #             node.Fix(MESH_DISPLACEMENT_Z)              
    #             xs = Vector(3)
    #             xs[0] = 0.0
    #             xs[1] = 0.0
    #             xs[2] = 0.0
    #             node.SetSolutionStepValue(MESH_DISPLACEMENT,0,xs)
    #         # Apply RHS conditions       
    #         else:
    #             rhs = Vector(3)
    #             rhs[0] = gradientForCompleteModelPart[node.Id][0]
    #             rhs[1] = gradientForCompleteModelPart[node.Id][1]
    #             rhs[2] = gradientForCompleteModelPart[node.Id][2]
    #             node.SetSolutionStepValue(MESH_RHS,0,rhs)

    #     # Solve mesh-motion problem with previously modified BCs
    #     mesh_solver.Solve()

    #     # Compute and add gradient contribution from mesh motion
    #     for node_id in gradientOnDesignSurface.keys():
    #         node = main_model_part.Nodes[node_id]
    #         sens_contribution = Vector(3)
    #         sens_contribution = node.GetSolutionStepValue(MESH_REACTION)
    #         gradientOnDesignSurface[node.Id] = gradientOnDesignSurface[node_id] + sens_contribution    

# ==============================================================================