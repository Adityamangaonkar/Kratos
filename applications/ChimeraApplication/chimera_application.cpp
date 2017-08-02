//
// ==============================================================================
//  ChimeraApplication
//
//  License:         BSD License
//                   license: ChimeraApplication/license.txt
//
//  Main authors:    Aditya Ghantasala, https://github.com/adityaghantasala
//                   Navaneeth K Narayanan
//
// ==============================================================================

// System includes

// External includes

// Project includes
#include "chimera_application.h"
#include "chimera_application_variables.h"

#include "geometries/triangle_2d_3.h"
#include "geometries/tetrahedra_3d_4.h"
#include "geometries/line_2d_2.h"
#include "geometries/triangle_3d_3.h"

namespace Kratos
{

KratosChimeraApplication::KratosChimeraApplication():
    mSkSyFluidElement2D3N(0, Element::GeometryType::Pointer(new Triangle2D3<Node<3> >(Element::GeometryType::PointsArrayType(3)))),
    mSkSyFluidElement3D4N(0, Element::GeometryType::Pointer(new Tetrahedra3D4<Node<3> >(Element::GeometryType::PointsArrayType(4)))),
    mChimeraFluidCouplingCondition2D(0, Element::GeometryType::Pointer( new Line2D2<Node<3> >( Element::GeometryType::PointsArrayType(2) ) ) ),
    mChimeraFluidCouplingCondition3D(0, Element::GeometryType::Pointer( new Triangle3D3<Node<3> >( Element::GeometryType::PointsArrayType(3) ) ) ),
    mChimeraThermalCouplingCondition2D(0, Element::GeometryType::Pointer( new Line2D2<Node<3> >( Element::GeometryType::PointsArrayType(2) ) ) ),
    mChimeraThermalCouplingCondition3D(0, Element::GeometryType::Pointer( new Triangle3D3<Node<3> >( Element::GeometryType::PointsArrayType(3) ) ) )
{}

void KratosChimeraApplication::Register()
{
  // calling base class register to register Kratos components
  KratosApplication::Register();
  std::cout << "Initializing KratosChimeraApplication... " << std::endl;

  KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(CHIM_NEUMANN_COND)
  // For MPC implementations
  KRATOS_REGISTER_VARIABLE(MPC_DATA_CONTAINER)
  //KRATOS_REGISTER_VARIABLE(IS_WEAK);

  KRATOS_REGISTER_VARIABLE(BOUNDARY_NODE);
  KRATOS_REGISTER_VARIABLE(FLUX);
  KRATOS_REGISTER_3D_VARIABLE_WITH_COMPONENTS(TRACTION);

  KRATOS_REGISTER_ELEMENT("SkSyFluidElement2D3N",mSkSyFluidElement2D3N);
  KRATOS_REGISTER_ELEMENT("SkSyFluidElement3D4N",mSkSyFluidElement3D4N);

  KRATOS_REGISTER_CONDITION("ChimeraFluidCouplingCondition2D",mChimeraFluidCouplingCondition2D);
  KRATOS_REGISTER_CONDITION("ChimeraFluidCouplingCondition3D",mChimeraFluidCouplingCondition3D);
  KRATOS_REGISTER_CONDITION("ChimeraThermalCouplingCondition2D",mChimeraThermalCouplingCondition2D);
  KRATOS_REGISTER_CONDITION("ChimeraThermalCouplingCondition3D",mChimeraThermalCouplingCondition3D);
}
} // namespace Kratos.
