//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Kazem Kamran
//


// System includes


// External includes


// Project includes
#include "includes/define.h"
#include "custom_elements/thermal_face2d.h"
#include "utilities/math_utils.h"
#include "includes/convection_diffusion_settings.h"
#include "thermo_mechanical_application.h"

namespace Kratos
{
//************************************************************************************
//************************************************************************************
ThermalFace2D::ThermalFace2D(IndexType NewId, GeometryType::Pointer pGeometry)
    : Condition(NewId, pGeometry)
{
    //DO NOT ADD DOFS HERE!!!
}

//************************************************************************************
//************************************************************************************
ThermalFace2D::ThermalFace2D(IndexType NewId, GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties)
    : Condition(NewId, pGeometry, pProperties)
{
}

Condition::Pointer ThermalFace2D::Create(IndexType NewId, NodesArrayType const& ThisNodes,  PropertiesType::Pointer pProperties) const
{
    return Condition::Pointer(new ThermalFace2D(NewId, GetGeometry().Create(ThisNodes), pProperties));
}

ThermalFace2D::~ThermalFace2D()
{
}


//************************************************************************************
//************************************************************************************
void ThermalFace2D::CalculateRightHandSide(VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo)
{
    //calculation flags
    bool CalculateStiffnessMatrixFlag = false;
    bool CalculateResidualVectorFlag = true;
    MatrixType temp = Matrix();

    CalculateAll(temp, rRightHandSideVector, rCurrentProcessInfo, CalculateStiffnessMatrixFlag, CalculateResidualVectorFlag);
}

//************************************************************************************
//************************************************************************************
void ThermalFace2D::CalculateLocalSystem(MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo)
{
    //calculation flags
    bool CalculateStiffnessMatrixFlag = true;
    bool CalculateResidualVectorFlag = true;

    CalculateAll(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo, CalculateStiffnessMatrixFlag, CalculateResidualVectorFlag);
}

//************************************************************************************
//************************************************************************************
void ThermalFace2D::CalculateAll(MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector,
                                 ProcessInfo& rCurrentProcessInfo,
                                 bool CalculateStiffnessMatrixFlag,
                                 bool CalculateResidualVectorFlag)
{
    KRATOS_TRY

    unsigned int number_of_nodes = GetGeometry().size();

    //resizing as needed the LHS
    unsigned int MatSize=number_of_nodes;

    //calculate lenght
    double x21 = GetGeometry()[1].X() - GetGeometry()[0].X();
    double y21 = GetGeometry()[1].Y() - GetGeometry()[0].Y();
    double lenght = x21*x21 + y21*y21;
    lenght = sqrt(lenght);


    //calculating viscosity
    ConvectionDiffusionSettings::Pointer my_settings = rCurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);
    const Variable<double>& rUnknownVar = my_settings->GetUnknownVariable();
//                 const Variable<double>& rDiffusionVar = my_settings->GetDiffusionVariable();
    const Variable<double>& rSurfaceSourceVar = my_settings->GetSurfaceSourceVariable();

    const double& ambient_temperature = GetProperties()[AMBIENT_TEMPERATURE];
    double StefenBoltzmann = 5.67e-8;
    double emissivity = GetProperties()[EMISSIVITY];
    double convection_coefficient = GetProperties()[CONVECTION_COEFFICIENT];

    const double& T0 = GetGeometry()[0].FastGetSolutionStepValue(rUnknownVar);
    const double& T1 = GetGeometry()[1].FastGetSolutionStepValue(rUnknownVar);

    const double& q0 = GetGeometry()[0].FastGetSolutionStepValue(rSurfaceSourceVar);
    const double& q1 = GetGeometry()[1].FastGetSolutionStepValue(rSurfaceSourceVar);


    if (CalculateStiffnessMatrixFlag == true) //calculation of the matrix is required
    {
        if(rLeftHandSideMatrix.size1() != MatSize )
            rLeftHandSideMatrix.resize(MatSize,MatSize,false);
        noalias(rLeftHandSideMatrix) = ZeroMatrix(MatSize,MatSize);

        rLeftHandSideMatrix(0,0) = ( convection_coefficient + emissivity*StefenBoltzmann*4.0*pow(T0,3)  )* 0.5 * lenght;
        rLeftHandSideMatrix(1,1) = ( convection_coefficient + emissivity*StefenBoltzmann*4.0*pow(T1,3)  )* 0.5 * lenght;
    }

    /*		KRATOS_WATCH(GetProperties()[AMBIENT_TEMPERATURE] );
    		KRATOS_WATCH(GetProperties()[EMISSIVITY] );
    		KRATOS_WATCH(GetProperties()[CONVECTION_COEFFICIENT] );
    		KRATOS_WATCH(convection_coefficient );
    		KRATOS_WATCH(ambient_temperature );
    		KRATOS_WATCH(T0 );
    		KRATOS_WATCH(T1 );
    		KRATOS_WATCH(convection_coefficient * ( T0 - ambient_temperature));
    		KRATOS_WATCH(convection_coefficient * ( T1 - ambient_temperature));
    */
    //resizing as needed the RHS
    double aux = pow(ambient_temperature,4);
    if (CalculateResidualVectorFlag == true) //calculation of the matrix is required
    {
        if(rRightHandSideVector.size() != MatSize )
            rRightHandSideVector.resize(MatSize,false);

        rRightHandSideVector[0] =  q0 - emissivity*StefenBoltzmann*(pow(T0,4) - aux)  -  convection_coefficient * ( T0 - ambient_temperature);

        rRightHandSideVector[1] =  q1  - emissivity*StefenBoltzmann*(pow(T1,4) - aux) -  convection_coefficient * ( T1 - ambient_temperature);

        /*			rRightHandSideVector[0] =  emissivity*q0 - emissivity*StefenBoltzmann*(pow(T0,4) - aux)
        									-  convection_coefficient * ( T0 - ambient_temperature);

        			rRightHandSideVector[1] =  emissivity*q1  - emissivity*StefenBoltzmann*(pow(T1,4) - aux)
        									-  convection_coefficient * ( T1 - ambient_temperature);
        */
        rRightHandSideVector *= 0.5*lenght;

    }

    KRATOS_CATCH("")
}



//************************************************************************************
//************************************************************************************
void ThermalFace2D::EquationIdVector(EquationIdVectorType& rResult, ProcessInfo& CurrentProcessInfo)
{
    ConvectionDiffusionSettings::Pointer my_settings = CurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);
    const Variable<double>& rUnknownVar = my_settings->GetUnknownVariable();

    unsigned int number_of_nodes = GetGeometry().PointsNumber();
    if(rResult.size() != number_of_nodes)
        rResult.resize(number_of_nodes,false);
    for (unsigned int i=0; i<number_of_nodes; i++)
    {
        rResult[i] = (GetGeometry()[i].GetDof(rUnknownVar)).EquationId();
    }
}

//************************************************************************************
//************************************************************************************
void ThermalFace2D::GetDofList(DofsVectorType& ConditionalDofList,ProcessInfo& CurrentProcessInfo)
{
    ConvectionDiffusionSettings::Pointer my_settings = CurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);
    const Variable<double>& rUnknownVar = my_settings->GetUnknownVariable();

    ConditionalDofList.resize(GetGeometry().size());
    for (unsigned int i=0; i<GetGeometry().size(); i++)
    {
        ConditionalDofList[i] = (GetGeometry()[i].pGetDof(rUnknownVar));
    }
}

} // Namespace Kratos


