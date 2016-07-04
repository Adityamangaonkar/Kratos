//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    @{KRATOS_APP_AUTHOR}
//

#if !defined(KRATOS_LAGRANGIAN_MPM_APPLICATION_VARIABLES_H_INCLUDED )
#define  KRATOS_LAGRANGIAN_MPM_APPLICATION_VARIABLES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/variables.h"
#include "includes/kratos_application.h"

namespace Kratos
{
KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS( LAGRANGIAN_MPM_APPLICATION, GAUSS_POINT_COORDINATES )
KRATOS_DEFINE_APPLICATION_VARIABLE( LAGRANGIAN_MPM_APPLICATION, double, GAUSS_AREA )
KRATOS_DEFINE_APPLICATION_VARIABLE( LAGRANGIAN_MPM_APPLICATION, double, EFFECTIVE_RADIUS )

KRATOS_DEFINE_APPLICATION_VARIABLE( LAGRANGIAN_MPM_APPLICATION, Vector, SHAPE_FUNCTIONS )
KRATOS_DEFINE_APPLICATION_VARIABLE( LAGRANGIAN_MPM_APPLICATION, Matrix, SHAPE_FUNCTIONS_DERIVATIVES )

}

#endif	/* KRATOS_LAGRANGIAN_MPM_APPLICATION_VARIABLES_H_INCLUDED */
