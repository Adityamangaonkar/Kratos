//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp Bucher, Jordi Cotela
//
// See Master-Thesis P.Bucher
// "Development and Implementation of a Parallel
//  Framework for Non-Matching Grid Mapping"

// System includes

#if defined(KRATOS_PYTHON)
// External includes

// Project includes
#include "includes/define_python.h"
#include "mapping_application.h"
#include "mapping_application_variables.h"
#include "custom_utilities/mapper_flags.h"
#include "custom_python/add_custom_mappers_to_python.h"


namespace Kratos
{

namespace Python
{

using namespace pybind11;

PYBIND11_MODULE(KratosMappingApplication, m)
{
    const auto mapping_app = class_<KratosMappingApplication,
            KratosMappingApplication::Pointer,
            KratosApplication >(m,"KratosMappingApplication")
            .def(init<>())
            ;

    // Adding the flags that can be used for mapping // TODO check why this is not working!
    mapping_app.attr("SWAP_SIGN")        = MapperFlags::SWAP_SIGN;
    mapping_app.attr("ADD_VALUES")       = MapperFlags::ADD_VALUES;
    mapping_app.attr("CONSERVATIVE")     = MapperFlags::CONSERVATIVE;
    mapping_app.attr("REMESHED")         = MapperFlags::REMESHED;

    AddCustomMappersToPython(m);
}

}  // namespace Python.

}  // namespace Kratos.

#endif // KRATOS_PYTHON defined
