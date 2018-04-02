//
//   Project Name:        KratosSolidMechanicsApplication $
//   Created by:          $Author:            JMCarbonell $
//   Last modified by:    $Co-Author:                     $
//   Date:                $Date:              August 2017 $
//   Revision:            $Revision:                  0.0 $
//
//

// System includes 

// External includes 

// Project includes
#include "custom_python/add_custom_processes_to_python.h"

// Processes
#include "custom_processes/transfer_entities_between_model_parts_process.h"
#include "custom_processes/transfer_nodes_to_model_part_process.h"
#include "custom_processes/assign_scalar_field_to_nodes_process.h"
#include "custom_processes/assign_scalar_field_to_conditions_process.h"
#include "custom_processes/assign_scalar_variable_to_nodes_process.h"
#include "custom_processes/assign_scalar_variable_to_conditions_process.h"
#include "custom_processes/assign_vector_variable_to_conditions_process.h"
#include "custom_processes/assign_vector_field_to_conditions_process.h"
#include "custom_processes/fix_scalar_dof_process.h"
#include "custom_processes/free_scalar_dof_process.h"
#include "custom_processes/add_dofs_process.h"
#include "custom_processes/assign_rotation_field_about_an_axis_to_nodes_process.h"
#include "custom_processes/assign_torque_field_about_an_axis_to_conditions_process.h"
#include "custom_processes/build_string_skin_process.h"

namespace Kratos
{
	
namespace Python
{

typedef std::vector<Flags>  FlagsContainer;
    
    
void Push_Back_Flags( FlagsContainer& ThisFlagContainer,
                      Flags ThisFlag )
{
  ThisFlagContainer.push_back( ThisFlag );
}

    
void  AddCustomProcessesToPython(pybind11::module& m)
{

  using namespace pybind11;

      
  class_< FlagsContainer >(m,"FlagsContainer")
      .def( init<>() )
      .def( "PushBack", Push_Back_Flags )
      ;
    

  //**********TRANSFER NODES TO MODEL PART*********//

  class_<TransferNodesToModelPartProcess, Process>(m,"TransferNodesProcess")
      .def(init<ModelPart&, ModelPart&, const FlagsContainer&>())
      .def(init<ModelPart&, ModelPart&, const FlagsContainer&, const FlagsContainer& >())
      .def("Execute", &TransferNodesToModelPartProcess::Execute)
      ;
      
  //**********TRANSFER ENTITIES BETWEEN MODEL PARTS*********//

  class_<TransferEntitiesBetweenModelPartsProcess, Process>(m,"TransferEntitiesProcess")
      .def(init<ModelPart&, ModelPart&, const std::string>())	
      .def(init<ModelPart&, ModelPart&, const std::string, const FlagsContainer&>())
      .def(init<ModelPart&, ModelPart&, const std::string, const FlagsContainer&, const FlagsContainer& >())
      .def("Execute", &TransferEntitiesBetweenModelPartsProcess::Execute)
      ;
      
      
  //**********ASSIGN VALUES TO VARIABLES PROCESSES*********//

  class_<AssignScalarVariableToNodesProcess, Process>(m,"AssignScalarToNodesProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init< ModelPart&, Parameters& >())
      .def(init<ModelPart&, const VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > >&, double>())
      .def(init<ModelPart&, const Variable<double>&, double>())
      .def(init<ModelPart&, const Variable<int>&, int>())
      .def(init<ModelPart&, const Variable<bool>&, bool>())
      .def("Execute", &AssignScalarVariableToNodesProcess::Execute)

      ;

  class_<AssignScalarFieldToNodesProcess, Process>(m,"AssignScalarFieldToNodesProcess")
      .def(init<ModelPart&, pybind11::object&, const std::string, const bool, Parameters>())
      .def(init< ModelPart&, pybind11::object&, const std::string, const bool, Parameters& >())
      .def("Execute", &AssignScalarFieldToNodesProcess::Execute)

      ;

  class_<AssignScalarVariableToConditionsProcess, Process>(m,"AssignScalarToConditionsProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init< ModelPart&, Parameters& >())
      .def(init<ModelPart&, const Variable<double>&, double>())
      .def(init<ModelPart&, const Variable<int>&, int>())
      .def(init<ModelPart&, const Variable<bool>&, bool>())
      .def("Execute", &AssignScalarVariableToConditionsProcess::Execute)
      ;

  class_<AssignVectorVariableToConditionsProcess, Process>(m,"AssignVectorToConditionsProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init< ModelPart&, Parameters& >())
      .def(init<ModelPart&, const Variable<array_1d<double,3> >&, array_1d<double,3>&>())
      .def("Execute", &AssignVectorVariableToConditionsProcess::Execute)
      ;

  class_<AssignScalarFieldToConditionsProcess, Process>(m,"AssignScalarFieldToConditionsProcess")
      .def(init<ModelPart&, pybind11::object&,const std::string,const bool, Parameters>())
      .def(init< ModelPart&, pybind11::object&,const std::string,const bool, Parameters& >())
      .def("Execute", &AssignScalarFieldToConditionsProcess::Execute)

      ;

  class_<AssignVectorFieldToConditionsProcess, Process>(m,"AssignVectorFieldToConditionsProcess")
      .def(init<ModelPart&, pybind11::object&,const std::string,const bool, Parameters>())
      .def(init< ModelPart&, pybind11::object&,const std::string,const bool, Parameters& >())
      .def("Execute", &AssignVectorFieldToConditionsProcess::Execute)

      ;
	
  //**********FIX AND FREE DOFS PROCESSES*********//

  class_<FixScalarDofProcess, Process>(m,"FixScalarDofProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init<ModelPart&, Parameters&>())
      .def(init<ModelPart&, const VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > >&>())
      .def(init<ModelPart&, const Variable<double>&>())
      .def(init<ModelPart&, const Variable<int>&>())
      .def(init<ModelPart&, const Variable<bool>&>())
      .def("Execute", &FixScalarDofProcess::Execute)

      ;


  class_<FreeScalarDofProcess, Process>(m,"FreeScalarDofProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init<ModelPart&, Parameters&>())
      .def(init<ModelPart&, const VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > >&>())
      .def(init<ModelPart&, const Variable<double>&>())
      .def(init<ModelPart&, const Variable<int>&>())
      .def(init<ModelPart&, const Variable<bool>&>())
      .def("Execute", &FreeScalarDofProcess::Execute)

      ;

 
  //**********ADD DOFS PROCESS*********//

  class_<AddDofsProcess, Process>(m,"AddDofsProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init<ModelPart&, Parameters&>())
      .def(init<ModelPart&, const pybind11::list&, const pybind11::list&>())
      .def("Execute", &AddDofsProcess::Execute)

      ;


  //**********ASSIGN ROTATION ABOUT AND AXIS*********//

  class_<AssignRotationAboutAnAxisToNodesProcess, Process>(m,"AssignRotationAboutAnAxisToNodesProcess")
      .def(init<ModelPart&, Parameters>())
      .def(init< ModelPart&, Parameters& >())
      .def("Execute", &AssignRotationAboutAnAxisToNodesProcess::Execute)

      ;

      
  class_<AssignRotationFieldAboutAnAxisToNodesProcess, Process>(m,"AssignRotationFieldAboutAnAxisToNodesProcess")
      .def(init<ModelPart&, pybind11::object&, const std::string,const bool, Parameters>())
      .def(init< ModelPart&, pybind11::object&, const std::string,const bool, Parameters& >())
      .def("Execute", &AssignRotationFieldAboutAnAxisToNodesProcess::Execute)

      ;

  //**********ASSIGN TORQUE ABOUT AN AXIS*********//

  class_<AssignTorqueAboutAnAxisToConditionsProcess, Process>(m,"AssignTorqueAboutAnAxisToConditionsProcess")
      .def(init< ModelPart&, Parameters >())
      .def(init< ModelPart&, Parameters& >())
      .def("Execute", &AssignTorqueAboutAnAxisToConditionsProcess::Execute)

      ;

      
  class_<AssignTorqueFieldAboutAnAxisToConditionsProcess, Process>(m,"AssignTorqueFieldAboutAnAxisToConditionsProcess")
      .def(init< ModelPart&, pybind11::object&,const std::string,const bool, Parameters >())
      .def(init< ModelPart&, pybind11::object&,const std::string,const bool, Parameters& >())
      .def("Execute", &AssignTorqueFieldAboutAnAxisToConditionsProcess::Execute)

      ;


  //**********BUILD STRING SKIN PROCESS*********//

  class_<BuildStringSkinProcess, Process>(m,"BuildStringSkinProcess")
      .def(init<ModelPart&, unsigned int, double>())
      .def("ExecuteInitialize", &BuildStringSkinProcess::ExecuteInitialize)
      .def("ExecuteFinalizeSolutionStep", &BuildStringSkinProcess::ExecuteFinalizeSolutionStep)
      .def("ExecuteBeforeOutputStep", &BuildStringSkinProcess::ExecuteBeforeOutputStep)	
      .def("ExecuteAfterOutputStep", &BuildStringSkinProcess::ExecuteAfterOutputStep)
      ;
            
}
 
}  // namespace Python.

} // Namespace Kratos

