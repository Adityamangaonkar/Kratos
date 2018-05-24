//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Albert Puigferrat Perez
//                   Ignasi de Pouplana
//

// Application includes
#include "custom_elements/steady_convection_diffusion_FIC_element.hpp"
#include <math.h>

namespace Kratos
{

template< unsigned int TDim, unsigned int TNumNodes >
Element::Pointer SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::Create( IndexType NewId, NodesArrayType const& ThisNodes, PropertiesType::Pointer pProperties ) const
{
    return Element::Pointer( new SteadyConvectionDiffusionFICElement( NewId, this->GetGeometry().Create( ThisNodes ), pProperties ) );
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
Element::Pointer SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::Create(IndexType NewId, GeometryType::Pointer pGeom, PropertiesType::Pointer pProperties) const
{
    return Element::Pointer( new SteadyConvectionDiffusionFICElement( NewId, pGeom, pProperties ) );
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
int SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::Check( const ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    const GeometryType& Geom = this->GetGeometry();

    // Base class checks for positive area and Id > 0
    int ierr = Element::Check(rCurrentProcessInfo);
    if(ierr != 0) return ierr;

    if(Geom.DomainSize() < 1.0e-15)
        KRATOS_ERROR << "DomainSize < 1.0e-15 for the element " << this->Id() << std::endl;

    // Verify ProcessInfo variables
    if ( CONVECTION_DIFFUSION_SETTINGS.Key() == 0 )
        KRATOS_ERROR << "CONVECTION_DIFFUSION_SETTINGS has Key zero at element " << this->Id() << std::endl;


    ConvectionDiffusionSettings::Pointer my_settings = rCurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // verify nodal variables and dofs

    const bool IsDefinedUnknownVariable = my_settings->IsDefinedUnknownVariable();
    if (IsDefinedUnknownVariable == false)
    {
        KRATOS_ERROR << "UnknownVariable is not defined" << std::endl;
    }

    const bool IsDefinedVelocityVariable = my_settings->IsDefinedVelocityVariable();
    if (IsDefinedVelocityVariable == false)
    {
        KRATOS_ERROR << "VelocityVariable is not defined" << std::endl;
    }

    const bool IsDefinedMeshVelocityVariable = my_settings->IsDefinedMeshVelocityVariable();
    if (IsDefinedMeshVelocityVariable == false)
    {
        KRATOS_ERROR << "MeshVelocityVariable is not defined" << std::endl;
    }

    const bool IsDefinedVolumeSourceVariable = my_settings->IsDefinedVolumeSourceVariable();
    if (IsDefinedVolumeSourceVariable == false)
    {
        KRATOS_ERROR << "VolumeSourceVariable is not defined" << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Verify properties

    const bool IsDefinedDensityVariable = my_settings->IsDefinedDensityVariable();
    if (IsDefinedDensityVariable == false)
    {
        KRATOS_ERROR << "DensityVariable is not defined" << std::endl;
    }

    const bool IsDefinedSpecificHeatVariable = my_settings->IsDefinedSpecificHeatVariable();
    if (IsDefinedSpecificHeatVariable == false)
    {
        KRATOS_ERROR << "SpecificHeatVariable is not defined" << std::endl;
    }

    const bool IsDefinedDiffusionVariable = my_settings->IsDefinedDiffusionVariable();
    if (IsDefinedDiffusionVariable == false)
    {
        KRATOS_ERROR << "DiffusionVariable is not defined" << std::endl;
    }

    const bool IsDefinedReactionVariable = my_settings->IsDefinedReactionVariable();
    if (IsDefinedReactionVariable == false)
    {
        KRATOS_ERROR << "ReactionVariable is not defined" << std::endl;
    }

    // If this is a 2D problem, check that nodes are in XY plane
    if ( TDim == 2 )
    {
        for (unsigned int i=0; i<TNumNodes; i++)
        {
            if(Geom[i].Z() != 0.0)
                KRATOS_ERROR << "Node with non-zero Z coordinate found. Id: " << Geom[i].Id() << std::endl;
        }
    }

    return ierr;

    KRATOS_CATCH( "" );
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
GeometryData::IntegrationMethod SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::GetIntegrationMethod() const
{
    return GeometryData::GI_GAUSS_2;
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::GetDofList(DofsVectorType& rElementalDofList, ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY

        GeometryType& rGeom = this->GetGeometry();

        ConvectionDiffusionSettings::Pointer my_settings = rCurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);
        const Variable<double>& rUnknownVar = my_settings->GetUnknownVariable();

        if (rElementalDofList.size() != TNumNodes)
            rElementalDofList.resize(TNumNodes);

        for (unsigned int i = 0; i < TNumNodes; i++)
        {
            rElementalDofList[i] = rGeom[i].pGetDof(rUnknownVar);
        }


        KRATOS_CATCH("")
    }

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateLocalSystem( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    const unsigned int element_size = TNumNodes;

    //Resetting the LHS
    if ( rLeftHandSideMatrix.size1() != element_size )
        rLeftHandSideMatrix.resize( element_size, element_size, false );
    noalias( rLeftHandSideMatrix ) = ZeroMatrix( element_size, element_size );

    //Resetting the RHS
    if ( rRightHandSideVector.size() != element_size )
        rRightHandSideVector.resize( element_size, false );
    noalias( rRightHandSideVector ) = ZeroVector( element_size );

    this->CalculateAll(rLeftHandSideMatrix, rRightHandSideVector, rCurrentProcessInfo);

    KRATOS_CATCH( "" )
}
//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateLeftHandSide( MatrixType& rLeftHandSideMatrix, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY;

    KRATOS_THROW_ERROR(std::logic_error,"SteadyConvectionDiffusionFICElement::CalculateLeftHandSide not implemented","");

    KRATOS_CATCH("");
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateRightHandSide( VectorType& rRightHandSideVector, ProcessInfo& rCurrentProcessInfo )
{
    KRATOS_TRY

    const unsigned int element_size = TNumNodes;

    //Resetting the RHS
    if ( rRightHandSideVector.size() != element_size )
        rRightHandSideVector.resize( element_size, false );
    noalias( rRightHandSideVector ) = ZeroVector( element_size );

    this->CalculateRHS(rRightHandSideVector, rCurrentProcessInfo);

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim, TNumNodes>::EquationIdVector(EquationIdVectorType& rResult, ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    GeometryType& rGeom = this->GetGeometry();
    const unsigned int element_size = TNumNodes;

    ConvectionDiffusionSettings::Pointer my_settings = rCurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);
    const Variable<double>& rUnknownVar = my_settings->GetUnknownVariable();

    if (rResult.size() != element_size)
        rResult.resize(element_size, false);

    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        rResult[i] = rGeom[i].GetDof(rUnknownVar).EquationId();
    }

    KRATOS_CATCH("")
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAll( MatrixType& rLeftHandSideMatrix, VectorType& rRightHandSideVector, const ProcessInfo& CurrentProcessInfo )
{
    KRATOS_TRY

    //Previous definitions
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    GeometryData::IntegrationMethod ThisIntegrationMethod = GetIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& integration_points = Geom.IntegrationPoints( ThisIntegrationMethod );
    const unsigned int NumGPoints = integration_points.size();

    //Containers of variables at all integration points
    const Matrix& NContainer = Geom.ShapeFunctionsValues( ThisIntegrationMethod );
    GeometryType::ShapeFunctionsGradientsType DN_DXContainer(NumGPoints);
    Vector detJContainer(NumGPoints);
    Geom.ShapeFunctionsIntegrationPointsGradients(DN_DXContainer,detJContainer,ThisIntegrationMethod);

    //Element variables
    ElementVariables Variables;
    this->InitializeElementVariables(Variables,Geom,Prop,CurrentProcessInfo);

    noalias(Variables.VelInter) = ZeroVector(TDim);
    noalias(Variables.DifMatrix) = ZeroMatrix( TDim, TDim );

    //Loop over integration points
    for( unsigned int GPoint = 0; GPoint < NumGPoints; GPoint++)
    {
        //Compute GradNT
        noalias(Variables.GradNT) = DN_DXContainer[GPoint];

        //Compute N and Interpolated velocity
        noalias(Variables.N) = row(NContainer,GPoint);

        Variables.QSource = 0.0;
        for(unsigned int i=0; i<TNumNodes; i++)
        {
            Variables.QSource += Variables.N[i]*Variables.NodalQSource[i];
        }

        // TODO: This will be moved to an utility
        InterpolateVariableWithComponents(Variables.VelInter,NContainer,Variables.NodalVel,GPoint);
        this->CalculateDiffusivityVariables(Variables,Prop,CurrentProcessInfo);
        this->CalculateHVector(Variables,Prop,CurrentProcessInfo);

        // Compute DifMatrixK
        for (unsigned int i = 0; i < TDim; i++)
        {
            Variables.DifMatrix(i,i) = Variables.DifMatrixK(i,i)
                                       + Variables.DifMatrixV(i,i)
                                       + Variables.DifMatrixS(i,i)
                                       + Variables.DifMatrixR(i,i)
                                       + Variables.DifMatrixSC(i,i);
        }

        //Compute weighting coefficient for integration
        this->CalculateIntegrationCoefficient(Variables.IntegrationCoefficient, detJContainer[GPoint], integration_points[GPoint].Weight() );

        //Contributions to the left hand side
        this->CalculateAndAddLHS(rLeftHandSideMatrix, Variables);

        //Contributions to the right hand side
        this->CalculateAndAddRHS(rRightHandSideVector, Variables);

    }


    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateRHS( VectorType& rRightHandSideVector, const ProcessInfo& CurrentProcessInfo )
{
    KRATOS_TRY

    //Previous definitions
    const PropertiesType& Prop = this->GetProperties();
    const GeometryType& Geom = this->GetGeometry();
    GeometryData::IntegrationMethod ThisIntegrationMethod = GetIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& integration_points = Geom.IntegrationPoints( ThisIntegrationMethod );
    const unsigned int NumGPoints = integration_points.size();

    //Containers of variables at all integration points
    const Matrix& NContainer = Geom.ShapeFunctionsValues( ThisIntegrationMethod );
    GeometryType::ShapeFunctionsGradientsType DN_DXContainer(NumGPoints);
    Vector detJContainer(NumGPoints);
    Geom.ShapeFunctionsIntegrationPointsGradients(DN_DXContainer,detJContainer,ThisIntegrationMethod);

    //Element variables
    ElementVariables Variables;
    this->InitializeElementVariables(Variables,Geom,Prop,CurrentProcessInfo);

    noalias(Variables.VelInter) = ZeroVector(TDim);
    noalias(Variables.DifMatrix) = ZeroMatrix( TDim, TDim );

    //Loop over integration points
    for( unsigned int GPoint = 0; GPoint < NumGPoints; GPoint++)
    {
        //Compute GradNT
        noalias(Variables.GradNT) = DN_DXContainer[GPoint];

        //Compute N and Interpolated velocity
        noalias(Variables.N) = row(NContainer,GPoint);

        Variables.QSource = 0.0;
        for(unsigned int i=0; i<TNumNodes; i++)
        {
            Variables.QSource += Variables.N[i]*Variables.NodalQSource[i];
        }

        // TODO: This will be moved to an utility
        InterpolateVariableWithComponents(Variables.VelInter,NContainer,Variables.NodalVel,GPoint);
        this->CalculateDiffusivityVariables(Variables,Prop,CurrentProcessInfo);
        this->CalculateHVector(Variables,Prop,CurrentProcessInfo);

        // Compute DifMatrixK
        for (unsigned int i = 0; i < TDim; i++)
        {
            Variables.DifMatrix(i,i) = Variables.DifMatrixK(i,i)
                                       + Variables.DifMatrixV(i,i)
                                       + Variables.DifMatrixS(i,i)
                                       + Variables.DifMatrixR(i,i)
                                       + Variables.DifMatrixSC(i,i);
        }

        //Compute weighting coefficient for integration
        this->CalculateIntegrationCoefficient(Variables.IntegrationCoefficient, detJContainer[GPoint], integration_points[GPoint].Weight() );

        //Contributions to the right hand side
        this->CalculateAndAddRHS(rRightHandSideVector, Variables);

    }


    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::InitializeElementVariables(ElementVariables& rVariables,
                                                                                  const GeometryType& Geom, const PropertiesType& Prop, const ProcessInfo& CurrentProcessInfo)
{
    KRATOS_TRY

    ConvectionDiffusionSettings::Pointer my_settings = CurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);

    const GeometryType& Geom = this->GetGeometry();

    //Properties variables
    const Variable<double>& rDensityVar = my_settings->GetDensityVariable();
    double FluidDensity = Prop[rDensityVar];

    const Variable<double>& rDiffusionVar = my_settings->GetDiffusionVariable();
    double conductivity = Prop[rDiffusionVar];

    const Variable<double>& rSpecificHeatVar = my_settings->GetSpecificHeatVariable();
	double specific_heat = Prop[rSpecificHeatVar];

    const Variable<double>& rUnknownVar = my_settings->GetUnknownVariable();

    // Compute rho*c
    rVariables.rho_dot_c = FluidDensity*specific_heat;

    // Compute DifMatrixK
    noalias(rVariables.DifMatrixK) = ZeroMatrix( TDim, TDim );
    for (unsigned int i = 0; i < TDim; i++)
    {
        rVariables.DifMatrixK(i,i) = conductivity;
    }

    //Nodal Variables
    for (unsigned int i = 0; i < TNumNodes; i++)
    {
        rVariables.NodalPhi[i] = Geom[i].FastGetSolutionStepValue(rUnknownVar);

        rVariables.NodalVel[i]=ZeroVector(3);
        const Variable<array_1d<double, 3 > >& rVelocityVar = my_settings->GetVelocityVariable();
		const Variable<array_1d<double, 3 > >& rMeshVelocityVar = my_settings->GetMeshVelocityVariable();

        rVariables.NodalVel[i] = Geom[i].FastGetSolutionStepValue(rVelocityVar) - Geom[i].FastGetSolutionStepValue(rMeshVelocityVar);

        const Variable<double>& rVolumeSourceVar = my_settings->GetVolumeSourceVariable();
        rVariables.NodalQSource[i] = Geom[i].FastGetSolutionStepValue(rVolumeSourceVar);
    }

    KRATOS_CATCH( "" )
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateDiffusivityVariables(ElementVariables& rVariables, const PropertiesType& Prop, const ProcessInfo& CurrentProcessInfo)
{
    GeometryType& rGeom = this->GetGeometry();

    ConvectionDiffusionSettings::Pointer my_settings = CurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);

    //Properties variables
    const Variable<double>& rDiffusionVar = my_settings->GetDiffusionVariable();
    double conductivity = Prop[rDiffusionVar];

    const Variable<double>& rReactionVar = my_settings->GetReactionVariable();
    rVariables.absorption = Prop[rReactionVar];

    BoundedMatrix<double,TDim,TDim> BaricenterMatrix;

    double NormVel = rVariables.VelInter[0]*rVariables.VelInter[0];
    for (unsigned int d = 1; d < TDim; d++)
        NormVel += rVariables.VelInter[d]*rVariables.VelInter[d];
    NormVel = std::sqrt(NormVel);

    rVariables.GradPhi = prod(trans(rVariables.GradNT), rVariables.NodalPhi);

    rVariables.NormGradPhi = rVariables.GradPhi[0]*rVariables.GradPhi[0];
    for (unsigned int d = 1; d < TDim; d++)
        rVariables.NormGradPhi += rVariables.GradPhi[d]*rVariables.GradPhi[d];
    rVariables.NormGradPhi = std::sqrt(rVariables.NormGradPhi);

    // Unitary velocity vector
    for (unsigned int i = 0; i < TDim; i++)
    {
        rVariables.VelInterHat[i] = rVariables.VelInter[i]/NormVel;
    }

    rVariables.AuxDiffusion = inner_prod(prod(trans(rVariables.VelInterHat), rVariables.DifMatrixK), rVariables.VelInterHat);

    double Domain = rGeom.DomainSize();

    if (TDim == 2)
    {
        rVariables.lv = std::sqrt(2.0*Domain);
    }
    else
    {
        rVariables.lv = pow( (6.0*Domain/Globals::Pi) , (1.0/3.0) );
    }

    rVariables.Peclet = NormVel * rVariables.lv * rVariables.rho_dot_c / (2.0 * rVariables.AuxDiffusion );

    rVariables.OmegaV = rVariables.absorption * rVariables.lv * rVariables.lv * rVariables.rho_dot_c / rVariables.AuxDiffusion;

    rVariables.SigmaV = rVariables.OmegaV / (2.0 * rVariables.Peclet);

    rVariables.LambdaV = std::sqrt(rVariables.Peclet * rVariables.Peclet + rVariables.SigmaV);

    rVariables.XiV = (cosh(rVariables.LambdaV) / cosh(rVariables.Peclet));

    rVariables.AlphaVBar = (1.0/tanh(rVariables.Peclet)-1.0/rVariables.Peclet);

    if(rVariables.SigmaV < 0.00024414) // 2^-12
    {
        rVariables.AlphaV = rVariables.SigmaV / 3.0 + rVariables.AlphaVBar * (1.0 - rVariables.SigmaV / rVariables.Peclet);
    }
    else
    {
       rVariables.AlphaV = 2.0 / rVariables.SigmaV * (1.0 - (rVariables.SigmaV * tanh (rVariables.Peclet)) / (rVariables.XiV - 1.0));
    }

    noalias(rVariables.DifMatrixV) = 0.5 * rVariables.lv * rVariables.AlphaV * outer_prod(rVariables.VelInterHat , rVariables.VelInterHat);

    noalias(BaricenterMatrix) = ZeroMatrix(TDim,TDim);

    for(unsigned int i=0; i<TNumNodes; i++)
    {
        noalias(BaricenterMatrix) += outer_prod((this->GetGeometry()[i] - this->GetGeometry().Center()) , (this->GetGeometry()[i] - this->GetGeometry().Center()));
    }

    noalias(rVariables.DifMatrixS) = rVariables.absorption / (TNumNodes + 1) * BaricenterMatrix;

    // phi = 2 in 2D and 3D

    if (rVariables.OmegaV == 0.0)
    {
        rVariables.AlphaR = 0.0;
    }
    else
    {
        rVariables.AlphaR = rVariables.Peclet * (0.5 * rVariables.SigmaV * ((rVariables.XiV + 1.0) / (rVariables.XiV - 1.0)) - rVariables.AlphaV)
                                    - 1.0 - (1 / conductivity) * inner_prod(rVariables.VelInterHat, prod(rVariables.DifMatrixS, rVariables.VelInterHat));
    }

    noalias(rVariables.DifMatrixR) = rVariables.AlphaR * conductivity * outer_prod(rVariables.VelInterHat , rVariables.VelInterHat);

    noalias(rVariables.DifMatrixAux) = prod(rVariables.GradNT,rVariables.DifMatrixK);
    noalias(rVariables.MatrixAux) = prod(rVariables.DifMatrixAux,trans(rVariables.GradNT));

    double NormAux1 = 0.0;

    for (unsigned int i = 0 ; i < TDim ; i++ )
    {
        NormAux1 += rVariables.MatrixAux (i,i) * rVariables.NodalPhi [i];
    }

    rVariables.Residual = (rVariables.rho_dot_c * inner_prod (rVariables.VelInter , rVariables.GradPhi)
                            - NormAux1
                            + rVariables.absorption * inner_prod(rVariables.N, rVariables.NodalPhi)
                            - rVariables.QSource) * Domain;

    // Identity matrix
    noalias(rVariables.IdentityMatrix) = ZeroMatrix(TDim,TDim);
    for(unsigned int i = 0; i < TDim; i++)
    {
        rVariables.IdentityMatrix(i,i) = 1;
    }

    if(rVariables.NormGradPhi == 0)
    {
        rVariables.Beta = 1.0;
    }
    else
    {
        rVariables.Beta = inner_prod(rVariables.VelInterHat, rVariables.GradPhi / rVariables.NormGradPhi);
    }

    BoundedMatrix<double,TDim,TDim> AuxMatrix;
    BoundedMatrix<double,TDim,TDim> AuxMatrix2;
    BoundedMatrix<double,TDim,TDim> AuxMatrix3;
    BoundedMatrix<double,TDim,TDim> AuxMatrix4;

    noalias(AuxMatrix2) = outer_prod(rVariables.VelInterHat , rVariables.VelInterHat);

    for (unsigned int i = 0 ; i < TDim ; i++ )
    {
        for (unsigned int j = 0 ; j < TDim ; j++ )
        {
            AuxMatrix(i,j) = rVariables.IdentityMatrix(i,j) - AuxMatrix2(i,j);
        }
    }

    // Double dot product
    AuxMatrix4 = prod((rVariables.DifMatrixK + rVariables.DifMatrixS), AuxMatrix);
    double DoubleDotScalar = 0.0;

    for (unsigned int i = 0 ; i < TDim ; i++ )
    {
        DoubleDotScalar += AuxMatrix4 (i,i);
    }

    rVariables.DifSC = (0.5 * rVariables.lv * std::abs (rVariables.Residual) / rVariables.NormGradPhi
                                        - DoubleDotScalar) * (1.0 - rVariables.Beta * rVariables.Beta);

    if (rVariables.NormGradPhi < 0.0000001)
    {
        rVariables.DifSC = - DoubleDotScalar * (1.0 - rVariables.Beta * rVariables.Beta);
    }

    noalias(rVariables.DifMatrixSC) = rVariables.DifSC * rVariables.IdentityMatrix;

        KRATOS_WATCH (rVariables.DifMatrixSC)

        KRATOS_WATCH ("________________________________________________________")

}

//----------------------------------------------------------------------------------------

// TODO: This will be moved to an utility
template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateHVector(ElementVariables& rVariables, const PropertiesType& Prop, const ProcessInfo& CurrentProcessInfo)
{
    ConvectionDiffusionSettings::Pointer my_settings = CurrentProcessInfo.GetValue(CONVECTION_DIFFUSION_SETTINGS);

    //Properties variables
    double NormVel = rVariables.VelInter[0]*rVariables.VelInter[0];
    for (unsigned int d = 1; d < TDim; d++)
        NormVel += rVariables.VelInter[d]*rVariables.VelInter[d];
    NormVel = std::sqrt(NormVel);

    // Compute HvVector
    for (unsigned int i = 0; i < TDim; i++)
    {
        rVariables.HvVector[i] = rVariables.AlphaVBar * rVariables.lv * rVariables.VelInterHat[i];
    }

    // Compute HrVector
    BoundedMatrix<double,TDim,TDim> AuxMatrix;
    BoundedMatrix<double,TDim,TDim> AuxMatrix2;
    BoundedMatrix<double,TDim,TDim> AuxMatrix3;
    BoundedMatrix<double,TDim,TDim> AuxMatrix4;

    if (rVariables.Residual < 0.0000001)
    {
        for (unsigned int i = 0 ; i < TDim ; i++ )
        {
            rVariables.HrVector [i] = 0.0;
        }
    }
    else
    {
        AuxMatrix3 = (2.0 / rVariables.Residual ) * (rVariables.DifMatrixS
                        + rVariables.AlphaR * rVariables.AuxDiffusion * outer_prod(rVariables.VelInterHat, rVariables.VelInterHat));

        rVariables.HrVector = prod(AuxMatrix3, rVariables.GradPhi);
    }

    // Compute HscVector
    if (rVariables.NormGradPhi < 0.0000001)
    {
        for (unsigned int i = 0 ; i < TDim ; i++ )
        {
            rVariables.HscVector [i] = 0.0;
        }
    }
    else
    {
        noalias(AuxMatrix2) = outer_prod(rVariables.VelInterHat , rVariables.VelInterHat);

        for (unsigned int i = 0 ; i < TDim ; i++ )
        {
            for (unsigned int j = 0 ; j < TDim ; j++ )
            {
                AuxMatrix(i,j) = rVariables.IdentityMatrix(i,j) - AuxMatrix2(i,j);
            }
        }

        // Double dot product
        AuxMatrix4 = prod((rVariables.DifMatrixK + rVariables.DifMatrixS), AuxMatrix);
        double DoubleDotScalar = 0.0;

        for (unsigned int i = 0 ; i < TDim ; i++ )
        {
            DoubleDotScalar += AuxMatrix4 (i,i);
        }


        double AuxScalar = (rVariables.lv * (rVariables.Residual / abs(rVariables.Residual)) - 2.0 * rVariables.NormGradPhi / rVariables.Residual
                            * DoubleDotScalar) * (1.0 - rVariables.Beta * rVariables.Beta);

        rVariables.HscVector = AuxScalar * rVariables.GradPhi / rVariables.NormGradPhi;
    }

    // Compute HVector
    rVariables.HVector = rVariables.HvVector + rVariables.HrVector + rVariables.HscVector;

        KRATOS_WATCH (rVariables.Residual)
        KRATOS_WATCH (rVariables.NormGradPhi)
        KRATOS_WATCH (rVariables.Beta)

        KRATOS_WATCH ("________________________________________________________")
        KRATOS_WATCH ("________________________________________________________")


}

//----------------------------------------------------------------------------------------

//TODO: This will be moved to an utility
template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::InterpolateVariableWithComponents(array_1d<double,TDim>& rVector,const Matrix& Ncontainer,
                                        const array_1d<array_1d<double,TDim>, TNumNodes>& VariableWithComponents,const unsigned int& GPoint)
{
    noalias(rVector) = ZeroVector(TDim);

    for(unsigned int i=0; i<TNumNodes; i++)
    {

        rVector[0] += Ncontainer(GPoint,i)*VariableWithComponents[i][0];
        rVector[1] += Ncontainer(GPoint,i)*VariableWithComponents[i][1];

        if(TDim>2)
        {
            rVector[2] += Ncontainer(GPoint,i)*VariableWithComponents[i][2];
        }

    }
}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddLHS(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{

    this->CalculateAndAddAdvectionMatrix(rLeftHandSideMatrix,rVariables);

    this->CalculateAndAddDiffusiveMatrix(rLeftHandSideMatrix,rVariables);

    this->CalculateAndAddAbsorptionMatrix(rLeftHandSideMatrix, rVariables);

    this->CalculateAndAddFICMatrix(rLeftHandSideMatrix,rVariables);

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddAdvectionMatrix(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{

    noalias(rVariables.AdvMatrixAux) = rVariables.rho_dot_c * outer_prod(rVariables.N,rVariables.VelInter);

    noalias(rLeftHandSideMatrix) += prod(rVariables.AdvMatrixAux,trans(rVariables.GradNT))*rVariables.IntegrationCoefficient;

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddDiffusiveMatrix(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{

    noalias(rVariables.DifMatrixAux) = prod(rVariables.GradNT,rVariables.DifMatrix);

    noalias(rLeftHandSideMatrix) += prod(rVariables.DifMatrixAux,trans(rVariables.GradNT))*rVariables.IntegrationCoefficient;

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddAbsorptionMatrix(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{

    noalias(rLeftHandSideMatrix) += rVariables.absorption * outer_prod(rVariables.N,rVariables.N);

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddFICMatrix(MatrixType& rLeftHandSideMatrix, ElementVariables& rVariables)
{

    noalias(rVariables.FICVectorAuxOne) = rVariables.HvVector * rVariables.absorption * 0.5;
    noalias(rVariables.FICMatrixAuxOne) = outer_prod(rVariables.FICVectorAuxOne,rVariables.N);

    noalias(rLeftHandSideMatrix) -= prod(rVariables.GradNT,rVariables.FICMatrixAuxOne)*rVariables.IntegrationCoefficient;

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddRHS(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{
    //Calculates -r = -K*phi+f

    //-K*Phi
    this->CalculateAndAddRHSAdvection(rRightHandSideVector, rVariables);

    this->CalculateAndAddRHSDiffusive(rRightHandSideVector, rVariables);

    this->CalculateAndAddRHSAbsorption(rRightHandSideVector, rVariables);

    this->CalculateAndAddRHSFIC(rRightHandSideVector, rVariables);

    //+f
    this->CalculateAndAddSourceForce(rRightHandSideVector, rVariables);

}
//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddRHSAdvection(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{

    noalias(rVariables.AdvMatrixAux) = rVariables.rho_dot_c * outer_prod(rVariables.N,rVariables.VelInter);
    noalias(rVariables.AdvMatrixAuxTwo) = prod(rVariables.AdvMatrixAux,trans(rVariables.GradNT))*rVariables.IntegrationCoefficient;

    noalias(rRightHandSideVector) -= prod(rVariables.AdvMatrixAuxTwo, rVariables.NodalPhi);

}
//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddRHSDiffusive(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{

    noalias(rVariables.DifMatrixAux) = prod(rVariables.GradNT,rVariables.DifMatrix);
    noalias(rVariables.DifMatrixAuxTwo) = prod(rVariables.DifMatrixAux,trans(rVariables.GradNT))*rVariables.IntegrationCoefficient;

    noalias(rRightHandSideVector) -= prod(rVariables.DifMatrixAuxTwo, rVariables.NodalPhi);

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddRHSAbsorption(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{

    noalias(rVariables.AbpMatrixAux) = rVariables.absorption * outer_prod(rVariables.N,rVariables.N);

    noalias(rRightHandSideVector) -= prod(rVariables.AbpMatrixAux, rVariables.NodalPhi);

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddRHSFIC(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{

    noalias(rVariables.FICVectorAuxOne) = rVariables.HvVector * rVariables.absorption * 0.5;
    noalias(rVariables.FICMatrixAuxOne) = outer_prod(rVariables.FICVectorAuxOne,rVariables.N);
    noalias(rVariables.FICMatrixAuxTwo) = prod(rVariables.GradNT,rVariables.FICMatrixAuxOne)*rVariables.IntegrationCoefficient;

    noalias(rRightHandSideVector) += prod(rVariables.FICMatrixAuxTwo, rVariables.NodalPhi);

}
//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateAndAddSourceForce(VectorType& rRightHandSideVector, ElementVariables& rVariables)
{

    noalias(rRightHandSideVector) += (rVariables.N + 0.5 * prod(rVariables.GradNT,rVariables.HVector))*rVariables.QSource*rVariables.IntegrationCoefficient;

}

//----------------------------------------------------------------------------------------

template< unsigned int TDim, unsigned int TNumNodes >
void SteadyConvectionDiffusionFICElement<TDim,TNumNodes>::CalculateIntegrationCoefficient(double& rIntegrationCoefficient, const double& detJ, const double& weight)
{
    // Considering thickness equals 1 in 2D
    rIntegrationCoefficient = weight * detJ;
}

//----------------------------------------------------------------------------------------

template class SteadyConvectionDiffusionFICElement<2,3>;
template class SteadyConvectionDiffusionFICElement<2,4>;
template class SteadyConvectionDiffusionFICElement<3,4>;
template class SteadyConvectionDiffusionFICElement<3,8>;

} // Namespace Kratos
