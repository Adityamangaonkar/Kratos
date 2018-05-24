// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:         BSD License
//                   license: structural_mechanics_application/license.txt
//
//  Main authors:    Alejandro Cornejo
//

#if !defined(KRATOS_GENERIC_CONSTITUTIVE_LAW_INTEGRATOR_PLASTICITY_H_INCLUDED)
#define  KRATOS_GENERIC_CONSTITUTIVE_LAW_INTEGRATOR_PLASTICITY_H_INCLUDED

// System includes
#include <string>
#include <iostream>

// Project includes
#include "includes/define.h"
#include "includes/serializer.h"
#include "includes/properties.h"
#include "utilities/math_utils.h"

namespace Kratos
{
///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{
/**
 * @class GenericConstitutiveLawIntegratorPlasticity
 * @ingroup StructuralMechanicsApplication
 * @brief
 * @details
 * @tparam TYieldSurfaceType
 * @author Alejandro Cornejo
 */
template <class TYieldSurfaceType, class TVoigtSize>
class KRATOS_API(STRUCTURAL_MECHANICS_APPLICATION) GenericConstitutiveLawIntegratorPlasticity
{
public:
    ///@name Type Definitions
    ///@{


    /// The type of yield surface
    typedef typename TYieldSurfaceType YieldSurfaceType;

    /// The type of plastic potential 
    typedef typename YieldSurfaceType::TPlasticPotentialType PlasticPotentialType;

    /// Counted pointer of GenericConstitutiveLawIntegratorPlasticity
    KRATOS_CLASS_POINTER_DEFINITION(GenericConstitutiveLawIntegratorPlasticity);

    /// Initialization constructor
    GenericConstitutiveLawIntegratorPlasticity()
    {
    }

    /// Copy constructor
    GenericConstitutiveLawIntegratorPlasticity(GenericConstitutiveLawIntegratorPlasticity const& rOther)
    {
    }

    /// Assignment operator
    GenericConstitutiveLawIntegratorPlasticity& operator=(GenericConstitutiveLawIntegratorPlasticity const& rOther)
    {
        return *this;
    }

    /// Destructor
    virtual ~GenericConstitutiveLawIntegratorPlasticity()
    {
    }

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    static void IntegrateStressVector(
        Vector& PredictiveStressVector,
        Vector& StrainVector, 
        double& UniaxialStress, 
        double& Threshold, 
        double& PlasticDenominator, 
        Vector& Fflux, 
        Vector& Gflux, 
        double& PlasticDissipation, 
        Vector& PlasticStrainIncrement,  
        const Matrix& C, 
        Vector& PlasticStrain, 
        const Properties& rMaterialProperties,
        const double CharacteristicLength
    )
    {
        bool is_converged = false; 
        int iteration = 0, max_iter = 9000; 
        BoundedVector<double, TVoigtSize> DSigma, DS; 
        double PlasticConsistencyFactorIncrement = 0.0;  // Lambda

        // Backward Euler  
        while (is_converged == false && iteration <= max_iter) 
        {
            PlasticConsistencyFactorIncrement = UniaxialStress * PlasticDenominator; 
            if (PlasticConsistencyFactorIncrement < 0.0) PlasticConsistencyFactorIncrement = 0.0; 

            noalias(PlasticStrainIncrement) = PlasticConsistencyFactorIncrement * Gflux; 
            noalias(PlasticStrain) += PlasticStrainIncrement; 
            noalias(DS) = prod(C, PlasticStrainIncrement); 
            noalias(DSigma) -= DS; 
            noalias(PredictiveStressVector) -= DSigma; 

            CalculatePlasticParameters(PredictiveStressVector, StrainVector, UniaxialStress, Threshold, 
                PlasticDenominator, Fflux, Gflux, PlasticDissipation, PlasticStrainIncrement, 
                C, rMaterialProperties, CharacteristicLength); 

            const double F = UniaxialStress - Threshold; 

            if (F < std::abs(1.0e-8 * Threshold)) // Has converged 
            { 
                is_converged = true; 
            }
            else iteration++ 
        } 
        // if (iteration == max_iter) KRATOS_ERROR << "Reached max iterations inside the Plasticity loop" << std::endl; 
    }

    static void CalculatePlasticParameters(
        Vector& PredictiveStressVector, 
        Vector& StrainVector,
        double& UniaxialStress, 
        double& Threshold,
        double& PlasticDenominator, 
        Vector& Fflux, 
        Vector& Gflux, 
        double& PlasticDissipation, 
        Vector& PlasticStrainIncrement,
        const Matrix& C, 
        const Properties& rMaterialProperties,
        const double CharacteristicLength
    )
    {
        BoundedVector<double, TVoigtSize> Deviator = ZeroVector(TVoigtSize); 
        BoundedVector<double, TVoigtSize> HCapa    = ZeroVector(TVoigtSize);
        double J2 = 0.0, r0 = 0.0, r1 = 0.0, Slope = 0.0, HardParam = 0.0;

        YieldSurfaceType::CalculateEquivalentStress(PredictiveStressVector, StrainVector, UniaxialStress, rMaterialProperties);
        CalculateDeviatorVector(PredictiveStressVector, Deviator, J2);
        CalculateFFluxVector(PredictiveStressVector, Deviator, J2, Fflux, rMaterialProperties);
        CalculateGFluxVector(PredictiveStressVector, Deviator, J2, Gflux, rMaterialProperties);
        CalculateRFactors(PredictiveStressVector, r0, r1);
        CalculatePlasticDissipation(PredictiveStressVector, r0, r1, PlasticStrainIncrement,
            PlasticDissipation, HCapa, rMaterialProperties, CharacteristicLength);
        CalculateEquivalentStressThreshold(PlasticDissipation, r0,
            r1, Threshold, Slope, rMaterialProperties);
        CalculateHardeningParameter(Fflux, Slope, HCapa, HardParam); 
        CalculatePlasticDenominator(Fflux, C, HardParam, PlasticDenominator);
    }

    // DF/DS
    static void CalculateFFluxVector(
        const Vector& StressVector, 
        const Vector& Deviator,
        const double J2, 
        Vector& FFluxVector,
        const Properties& rMaterialProperties
    )
    {
        YieldSurfaceType::CalculateYieldSurfaceDerivative(StressVector, Deviator, J2, 
            FFluxVector, rMaterialProperties);
    }

    // DG/DS
    static void CalculateGFluxVector(
        const Vector& StressVector, 
        const Vector& Deviator,
        const double J2, 
        Vector& GFluxVector,
        const Properties& rMaterialProperties
    )
    {
        YieldSurfaceType::CalculatePlasticPotentialDerivative(StressVector, Deviator, J2, 
            GFluxVector, rMaterialProperties);
    }
    
    // Calculates the McAully factors 
    /* These "r"  differentiate between the 
    tensile/compressive state            */
    static void CalculateRFactors(
        const Vector& StressVector,
        double& r0,
        double& r1
    )
    {
        Vector PrincipalStresses = ZeroVector(3);
		CalculatePrincipalStresses(PrincipalStresses, StressVector);

		double suma = 0.0, sumb = 0.0, sumc = 0.0;
		Vector SA = ZeroVector(3) , SB = ZeroVector(3), SC = ZeroVector(3);

		for (int i = 0; i < 3; i++)
		{
			SA[i] = std::abs(PrincipalStresses[i]);
			SB[i] = 0.5*(PrincipalStresses[i]  + SA[i]);
			SC[i] = 0.5*(-PrincipalStresses[i] + SA[i]);

			suma += SA[i];
			sumb += SB[i];
			sumc += SC[i];
		}
		if (suma != 0.0)
		{
			r0 = sumb/suma;
			r1 = sumc/suma;
		}
		else
		{
			r0 = sumb;
			r1 = sumc;
        }
    }

    // Calculates Plastic Dissipation
    static void CalculatePlasticDissipation(
        const Vector& StressVector, 
        const double r0,
        const double r1, 
        const Vector& PlasticStrainInc, 
        double& rPlasticDissipation, 
        Vector& rHCapa,
        const Properties& rMaterialProperties,
        const double CharacteristicLength
    )
    {
        const double Young = rMaterialProperties[YOUNG_MODULUS];
        const double YieldCompression = rMaterialProperties[YIELD_STRESS_COMPRESSION];
        const double YieldTension     = rMaterialProperties[YIELD_STRESS_TENSION];
        const double n = YieldCompression / YieldTension;
        const double Gf = rMaterialProperties[FRACTURE_ENERGY]; // Frac energy in tension
        const double Gfc = rMaterialProperties[FRACTURE_ENERGY] * std::pow(n, 2); // Frac energy in compression

        const double gf  = Gf / CharacteristicLength;
        const double gfc = Gfc / CharacteristicLength;

        const double hlim = 2.0*Young*gfc / (std::pow(YieldCompression, 2));
        if (CharacteristicLength > hlim) KRATOS_THROW_ERROR(std::invalid_argument, "The Fracture Energy is to low ", gfc)

        double Const0 = 0.0, Const1 = 0.0, DPlasticdissipation = 0.0;
        if (gf > 0.000001)
        {
            Const0 = r0 / gf;
            Const1 = r1 / gfc;
        }
        const double Const = Const0 + Const1;

        for (int i = 0; i < 6; i++)
        {
            rHCapa[i] = Const*StressVector[i];
            DPlasticdissipation += rHCapa[i]*PlasticStrainInc[i];
        }

        if (DPlasticdissipation > 0.0 | DPlasticdissipation > 1.0) DPlasticdissipation = 0.0;

        rPlasticDissipation += DPlasticdissipation;
        if (rPlasticDissipation >= 1.0) rPlasticDissipation = 0.9999; // warning vicente
    }

    // Calculates the stress threshold 
    static void CalculateEquivalentStressThreshold(
        const double PasticDissipation, 
        const double r0,
        const double r1, 
        double& rEquivalentStressThreshold, 
        double& rSlope,
        const Properties& rMaterialProperties
    )
    {
        const int CurveType = rMaterialProperties[HARDENING_CURVE];
        // ....
		const double YieldCompr   = rMaterialProperties[YIELD_STRESS_COMPRESSION];
		const double YieldTension = rMaterialProperties[YIELD_STRESS_TENSION];
        const double n = YieldCompr / YieldTension;

        BoundedVector<double,2> Gf, Slopes, EqThrsholds;

        Gf[0] = rMaterialProperties[FRACTURE_ENERGY];
        Gf[1] = std::pow(n, 2)*Gf[0];

        for (int i = 0; i < 2; i++) // i:0 Tension ; i:1 Comrpession
        {
            switch(CurveType)
            {
                case 1:
                    CalculateEqStressThresholdHardCurve1(PlasticDissipation, r0, r1,
                        EqThrsholds[i], Slopes[i], rMaterialProperties);
                case 2:
                    CalculateEqStressThresholdHardCurve2(PlasticDissipation, r0, r1,
                        EqThrsholds[i], Slopes[i], rMaterialProperties);
                case 3:
                    CalculateEqStressThresholdHardCurve3(PlasticDissipation, r0, r1,
                        EqThrsholds[i], Slopes[i], rMaterialProperties);                    
                // Add more cases...
            }
        }

        rEquivalentStressThreshold = r0*EqThrsholds[0] + r1*EqThrsholds[1];
        rSlope = rEquivalentStressThreshold*((r0*Slopes[0] / EqThrsholds[0]) + (r1*Slopes[1] / EqThrsholds[1]));
    }

    // Softening with straight line
    static void CalculateEqStressThresholdHardCurve1(        
        const double PasticDissipation, 
        const double r0,
        const double r1, 
        double& rEquivalentStressThreshold, 
        double& rSlope,
        const Properties& rMaterialProperties
    )
    {
        const double InitialThreshold = rMaterialProperties[YIELD_STRESS_COMPRESSION];

        rEquivalentStressThreshold = InitialThreshold * std::sqrt(1 - PasticDissipation);
        rSlope = -0.5*(std::pow(InitialThreshold, 2) / (rEquivalentStressThreshold));
    }

    // Softening with exponential function
    static void CalculateEqStressThresholdHardCurve2(        
        const double PasticDissipation, 
        const double r0,
        const double r1, 
        double& rEquivalentStressThreshold, 
        double& rSlope,
        const Properties& rMaterialProperties
    )
    {
        const double InitialThreshold = rMaterialProperties[YIELD_STRESS_COMPRESSION];

        rEquivalentStressThreshold = InitialThreshold * (1 - PasticDissipation);
        rSlope = -0.5*InitialThreshold;
    }

    // Initial hardening up to UltimateStress
    // followed by exponential softening
    static void CalculateEqStressThresholdHardCurve3(        
        const double PasticDissipation, 
        const double r0,
        const double r1, 
        double& rEquivalentStressThreshold, 
        double& rSlope,
        const Properties& rMaterialProperties
    )
    {
        const double InitialThreshold = rMaterialProperties[YIELD_STRESS_COMPRESSION];  // sikma
        const double UltimateStress = rMaterialProperties[MAXIMUM_STRESS];              // sikpi
        const double MaxStressPosition = rMaterialProperties[MAXIMUM_STRESS_POSITION];  // cappi

        if (PlasticDissipation < 1.0)
        {
            const double Ro = std::sqrt(1.0 - InitialThreshold / UltimateStress);
            double Alpha = std::log((1.0 - (1.0 - Ro)*(1.0 - Ro)) / ((3.0 - Ro)*(1.0 + Ro)*MaxStressPosition));
            Alpha = std::exp(Alpha / (1.0 - MaxStressPosition));
            const double Phi = std::pow((1.0 - Ro), 2) + ((3.0 - Ro)*(1.0 + Ro)*PlasticDissipation*(std::pow(Alpha, (1.0 - PlasticDissipation))));

            rEquivalentStressThreshold = UltimateStress*(2.0*std::sqrt(Phi) - Phi);
            rSlope = UltimateStress*((1.0 / std::sqrt(Phi)) - 1.0)*(3.0 - Ro)*(1.0 + Ro)*(std::pow(Alpha, (1.0 - PlasticDissipation)))*
                (1.0 - std::log(Alpha)*PlasticDissipation);
        }
    }

    static void CalculateHardeningParameter(
        const Vector& GFlux, 
        const double SlopeThreshold,
        const Vector& HCapa, 
        double& rHardParameter
    ) 
    {
        rHardParameter = -SlopeThreshold;
        double aux = 0.0;
        for (int i = 0; i < 6; i++) aux += HCapa[i] * GFlux[i];
        if (aux != 0.0) rHardParameter *= aux;
    }

    static void CalculatePlasticDenominator(
        const Vector& FFlux, 
        const Vector& GFlux,
        const Matrix& C,
        const double HardParam, 
        double& PlasticDenominator
    )
    {
        const Vector noalias(DVect) = prod(C, GFlux);

        double A1 = 0.0;
        for (int i = 0; i < 6; i++) A1 += FFlux[i] * DVect[i];

        const double A2 = 0.0; // Only for isotropic hard
        const double A3 = HardParam;

        rHardParameter = 1 / (A1 + A2 + A3);
    }

    static void CalculatePrincipalStresses(
        Vector& rPrincipalStressVector, 
        const Vector StressVector
    )
	{
		rPrincipalStressVector.resize(3);
		double I1, I2, I3, phi, Num, Denom, II1;
		I1 = YieldSurfaceType::CalculateI1Invariant(StressVector, I1);
		I2 = YieldSurfaceType::CalculateI2Invariant(StressVector, I2);
		I3 = YieldSurfaceType::CalculateI3Invariant(StressVector, I3);
		II1 = I1*I1;

		Num = (2.0*II1 - 9.0*I2)*I1 + 27.0*I3;
		Denom = (II1 - 3.0*I2);

		if (Denom != 0.0)
		{
			phi = Num / (2.0*Denom*sqrt(Denom));

			if (std::abs(phi) > 1.0)
			{
				if (phi > 0.0) phi = 1.0;
				else phi = -1.0;
			}

			const double acosphi = std::acos(phi);
			phi = acosphi / 3.0;

			const double aux1 = 2.0/3.0*std::sqrt(II1 - 3.0*I2);
			const double aux2 = I1 / 3.0;

			rPrincipalStressVector[0] = aux2 + aux1*std::cos(phi);
			rPrincipalStressVector[1] = aux2 + aux1*std::cos(phi - 2.09439510239);
			rPrincipalStressVector[2] = aux2 + aux1*std::cos(phi - 4.18879020478);
		}
		else 
		{
			rPrincipalStressVector = ZeroVector(3);
		}
    }

    static void CalculateDeviatorVector(const Vector& StressVector, Vector& rDeviator, const double rJ2)
    {
        rDeviator = StressVector;
        const double I1 = StressVector[0] + StressVector[1] + StressVector[2];
        const double Pmean = I1 / 3.0;

        rDeviator[0] -= Pmean;
        rDeviator[1] -= Pmean;
        rDeviator[2] -= Pmean;

        rJ2 = 0.5*(rDeviator[0]*rDeviator[0] + Deviator[1]*rDeviator[1] + rDeviator[2]*rDeviator[2]) +
            (rDeviator[3]*rDeviator[3] + rDeviator[4]*rDeviator[4] + rDeviator[5]*rDeviator[5]);
    }


    ///@}
    ///@name Access
    ///@{

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{

    ///@}
    ///@name Protected Operations
    ///@{

    ///@}
    ///@name Protected  Access
    ///@{

    ///@}
    ///@name Protected Inquiry
    ///@{

    ///@}
    ///@name Protected LifeCycle
    ///@{

    ///@}

private:
    ///@name Static Member Variables
    ///@{

    ///@}
    ///@name Member Variables
    ///@{

    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    ///@}
    ///@name Private  Access
    ///@{

    ///@}
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    // Serialization

    friend class Serializer;

    void save(Serializer& rSerializer) const
    {
    }

    void load(Serializer& rSerializer)
    {
    }

    ///@}

}; // Class GenericYieldSurface

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

///@}

}// namespace Kratos.
#endif
