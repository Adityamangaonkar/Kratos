// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:		 BSD License
//					 license: structural_mechanics_application/license.txt
//
//  Main authors:    Riccardo Rossi
//

// System includes

// External includes

// Project includes
#include "includes/define_python.h"
#include "includes/model_part.h"
#include "processes/process.h"
#include "custom_python/add_custom_utilities_to_python.h"

//Utilities
#include "custom_utilities/read_materials.hpp"

namespace Kratos
{
namespace Python
{

void  AddCustomUtilitiesToPython(pybind11::module& m)
{
    using namespace pybind11;

    class_<ReadMaterialProcess, Process>(m, "ReadMaterialProcess")
        .def(init< ModelPart&, Parameters >())
        .def("Execute",&ReadMaterialProcess::Execute)
        ;
}

}  // namespace Python.  

} // Namespace Kratos

