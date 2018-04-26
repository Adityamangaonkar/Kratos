//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp Bucher
//

// Project includes
#include "testing/testing.h"
#include "geometries/quadrilateral_2d_4.h"
#include "processes/structured_mesh_generator_process.h"
#include "utilities/compare_elements_and_conditions_utility.h"
#include "custom_utilities/interface_preprocessor.h"

namespace Kratos {
namespace Testing {

KRATOS_TEST_CASE_IN_SUITE(InterfacePreprocessorNodeBasedConditions, KratosMappingApplicationSerialTestSuite)
{
    Node<3>::Pointer p_point1(new Node<3>(1, 0.00, 0.00, 0.00));
    Node<3>::Pointer p_point2(new Node<3>(2, 0.00, 10.00, 0.00));
    Node<3>::Pointer p_point3(new Node<3>(3, 10.00, 10.00, 0.00));
    Node<3>::Pointer p_point4(new Node<3>(4, 10.00, 0.00, 0.00));

    Quadrilateral2D4<Node<3> > geometry(p_point1, p_point2, p_point3, p_point4);

    ModelPart model_part("Generated");

    Parameters mesher_parameters(R"(
    {
        "number_of_divisions" : 3,
        "element_name"        : "Element2D3N",
        "create_skin_sub_model_part": false
    }  )");

    StructuredMeshGeneratorProcess(geometry, model_part, mesher_parameters).Execute();

    ModelPart::Pointer interface_model_part = Kratos::make_shared<ModelPart>("pForTest");

    InterfacePreprocessor interface_preprocess(model_part, interface_model_part);

    // the "use_nodes" setting is enabled by default!
    Parameters interface_parameters(R"( { "mapper_condition_name" : "NearestNeighborCondition"} )");
    const std::string mapper_condition_name = interface_parameters["mapper_condition_name"].GetString();

    interface_preprocess.GenerateInterfaceModelPart(interface_parameters);

    const bool use_nodes = interface_parameters["use_nodes"].GetBool();

    KRATOS_CHECK(use_nodes); // this setting is validated in "GenerateInterfaceModelPart", it is the default value

    // Some basic checks
    KRATOS_CHECK_EQUAL(model_part.NumberOfNodes(), interface_model_part->NumberOfNodes());
    KRATOS_CHECK_EQUAL(model_part.NumberOfNodes(), interface_model_part->NumberOfConditions());
    KRATOS_CHECK_EQUAL(interface_model_part->NumberOfElements(), 0);
    KRATOS_CHECK_EQUAL(interface_model_part->NumberOfProperties(), 1);

    // Checking that the correct Conditions are being created (only the first one is being checked bcs they are all the same)
    std::string condition_name;
    CompareElementsAndConditionsUtility::GetRegisteredName(*(interface_model_part->ConditionsBegin()), condition_name);
    KRATOS_CHECK_EQUAL(mapper_condition_name, condition_name);
}

}  // namespace Testing
}  // namespace Kratos