// ==============================================================================
//  KratosStructuralMechanicsApplication
//
//  License:         BSD License
//                   license: StructuralMechanicsApplication/license.txt
//
//  Main authors:    Fusseder Martin
//                   martin.fusseder@tum.de
//	TODO: Check that this response function  works in a correct way for all conditions
// ==============================================================================

#ifndef ADJOINT_LOCAL_STRESS_RESPONSE_FUNCTION_H
#define ADJOINT_LOCAL_STRESS_RESPONSE_FUNCTION_H

// ------------------------------------------------------------------------------
// System includes
// ------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <algorithm>

// ------------------------------------------------------------------------------
// External includes
// ------------------------------------------------------------------------------
#include <boost/python.hpp>
#include <boost/numeric/ublas/io.hpp>

// ------------------------------------------------------------------------------
// Project includes
// ------------------------------------------------------------------------------
#include "includes/define.h"
#include "processes/process.h"
#include "includes/node.h"
#include "includes/element.h"
#include "includes/model_part.h"
#include "includes/kratos_flags.h"
#include "adjoint_structural_response_function.h"
#include "structural_mechanics_application_variables.h"


#include "includes/kratos_parameters.h"
#include "includes/ublas_interface.h"
#include "utilities/openmp_utils.h"

#include "response_data.h"

//#include "custom_utilities/finite_differences_utilities.h"

// ==============================================================================

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

/// Short class definition.
/** Detail class definition.

 */

//template<class TDenseSpace>

class AdjointLocalStressResponseFunction : public AdjointStructuralResponseFunction
{
public:
	///@name Type Definitions
	///@{

	typedef AdjointStructuralResponseFunction BaseType;
	typedef array_1d<double, 3> array_3d;



	/// Pointer definition of AdjointLocalStressResponseFunction
	KRATOS_CLASS_POINTER_DEFINITION(AdjointLocalStressResponseFunction);

	///@}
	///@name Life Cycle
	///@{

	/// Default constructor.

	AdjointLocalStressResponseFunction(ModelPart& rModelPart, Parameters& rParameters)
	: AdjointStructuralResponseFunction(rModelPart, rParameters)
	{
		ModelPart& r_model_part = this->GetModelPart();

		ResponseData::Pointer p_stress_response_data(new ResponseData());

		// Get traced element
		m_id_of_traced_element = rParameters["traced_element"].GetInt();
		mp_traced_pElement = r_model_part.pGetElement(m_id_of_traced_element);

		// Tell traced element the stress type
		TracedStressType traced_stress_type = p_stress_response_data->ConvertStressType(rParameters["stress_type"].GetString()); 
		KRATOS_ERROR_IF(traced_stress_type == StressTypeNotAvailible) << "Chosen stress type is not availible!" << std::endl;
		mp_traced_pElement->SetValue(TRACED_STRESS_TYPE, static_cast<int>(traced_stress_type) );		

		// Get info how and where to treat the stress
		m_stress_treatment = p_stress_response_data->ConvertStressTreatment( rParameters["stress_treatment"].GetString() );
		KRATOS_ERROR_IF(m_stress_treatment == StressTreatmentNotAvailible) << "Chosen option for stress treatmeant is not availible! Chose 'GP','node' or 'mean'!" << std::endl;

		if(m_stress_treatment == GP || m_stress_treatment == node)
		{
			m_id_of_location = rParameters["stress_location"].GetInt();
			KRATOS_ERROR_IF(m_id_of_location < 1) << "Chose a 'stress_location' > 0. Specified 'stress_location': " << m_id_of_location << std::endl;
		}

		m_stress_value = 0.0;
	}

	/// Destructor.
	virtual ~AdjointLocalStressResponseFunction()
	{
	}

	///@}
	///@name Operators
	///@{

	///@}
	///@name Operations
	///@{

	void Initialize() override
	{
		KRATOS_TRY;

		BaseType::Initialize();

		KRATOS_CATCH("");
	}

	// ==============================================================================
	double CalculateValue(ModelPart& rModelPart) override
	{
		KRATOS_TRY;

		// Working variables
		ProcessInfo &CurrentProcessInfo = rModelPart.GetProcessInfo();

		Vector element_stress;
		if(m_stress_treatment == mean || m_stress_treatment == GP)
			mp_traced_pElement->Calculate(STRESS_ON_GP, element_stress, CurrentProcessInfo);
		else
			mp_traced_pElement->Calculate(STRESS_ON_NODE, element_stress, CurrentProcessInfo);

		int stress_vec_size = element_stress.size();

		if(m_stress_treatment == mean)
		{
			for(int i = 0; i < stress_vec_size; i++)
				m_stress_value += element_stress[i];

			m_stress_value /= stress_vec_size;
		}
		else if(m_stress_treatment == GP)
		{
			if(stress_vec_size >= m_id_of_location)
				m_stress_value = element_stress[m_id_of_location - 1];
			else
				KRATOS_ERROR << "Chosen Gauss-Point is not availible. Chose 'stress_location' between 1 and " <<
								stress_vec_size  << "!"<< std::endl;
		}
		else if(m_stress_treatment == node)
		{
			const int num_ele_nodes = mp_traced_pElement->GetGeometry().PointsNumber();
			if(num_ele_nodes >= m_id_of_location)
				m_stress_value = element_stress[m_id_of_location - 1];
			else
				KRATOS_ERROR << "Chosen Node is not availible. The element has only " <<
								num_ele_nodes  << " nodes."<< std::endl;

		}

		// Set initial value if not done yet
		/*if(!m_initial_value_defined)
		{
			m_initial_value = m_stress_value;
			m_initial_value_defined = true;
		}*/

		return m_stress_value;

		KRATOS_CATCH("");
	}
	// --------------------------------------------------------------------------
	/*double GetInitialValue() old function, don't needed for sensitivity analysis
	{
		KRATOS_TRY;

		if(!m_initial_value_defined)
			KRATOS_THROW_ERROR(std::logi:error, "Initial value not yet defined! First compute it by calling \"CalculateValue()\"", m_initial_value_defined);

		return m_initial_value;

		KRATOS_CATCH("");
	}

	// --------------------------------------------------------------------------
	double GetValue() old function, don't needed for sensitivity analysis
	{
		KRATOS_TRY;

		return m_stress_value;

		KRATOS_CATCH("");
	}*/

	// ==============================================================================

	///@}
	///@name Access
	///@{

	///@}
	///@name Inquiry
	///@{

	///@}
	///@name Input and output
	///@{

	/// Turn back information as a string.
	/*virtual std::string Info() const            old function, don't needed for sensitivity analysis
	{
		return "AdjointLocalStressResponseFunction";
	}

	/// Print information about this object.
	virtual void PrintInfo(std::ostream &rOStream) const    old function, don't needed for sensitivity analysis
	{
		rOStream << "AdjointLocalStressResponseFunction";
	}

	/// Print object's data.
	virtual void PrintData(std::ostream &rOStream) const    old function, don't needed for sensitivity analysis
	{
	}*/

	///@}
	///@name Friends
	///@{

	///@}

	// ==============================================================================
	void CalculateGradient(const Element& rAdjointElem, const Matrix& rAdjointMatrix,
                                   Vector& rResponseGradient,
                                   ProcessInfo& rProcessInfo) override
	{

		rResponseGradient.resize(rAdjointMatrix.size1());
		rResponseGradient.clear();

		if(rAdjointElem.Id() == m_id_of_traced_element)
		{
			//------------------------------
			/*FiniteDifferencesUtilities::Pointer FD_calculate_gradient(new FiniteDifferencesUtilities());
			FD_calculate_gradient->SetDesignVariable("test_IY");

			mp_traced_pElement->SetValue(FINITE_DIFFERENCE_INFORMATION, FD_calculate_gradient);

			FiniteDifferencesUtilities::Pointer FD_calculate_gradient_2 = mp_traced_pElement->GetValue(FINITE_DIFFERENCE_INFORMATION);

			std::cout << "Test finite difference stuff = " << FD_calculate_gradient_2->GetDesignVariable() << std::endl;*/
			//------------------------------

			Matrix stress_displ_deriv;
			if(m_stress_treatment == mean || m_stress_treatment == GP)
				mp_traced_pElement->Calculate(STRESS_DISP_DERIV_ON_GP, stress_displ_deriv, rProcessInfo);
			else
				mp_traced_pElement->Calculate(STRESS_DISP_DERIV_ON_NODE, stress_displ_deriv, rProcessInfo);

			int num_of_dofs = stress_displ_deriv.size1();
			int num_of_deriv = stress_displ_deriv.size2();
			double stress_displ_deriv_value = 0.0;

			KRATOS_ERROR_IF(rResponseGradient.size() != stress_displ_deriv.size1())
				 << "Size of stress displacement derivative does not fit!" << std::endl;

			for (int dof_it = 0 ; dof_it < num_of_dofs; dof_it++)
			{
				if(m_stress_treatment == mean)
				{
					for(int GP_it = 0; GP_it < num_of_deriv; GP_it++)
						stress_displ_deriv_value += stress_displ_deriv(dof_it, GP_it);

					stress_displ_deriv_value /= num_of_deriv;
				}
				else if(m_stress_treatment == GP)
				{
					if(num_of_deriv >= m_id_of_location)
						stress_displ_deriv_value = stress_displ_deriv(dof_it, (m_id_of_location-1));
					else
						KRATOS_ERROR << "Chosen Gauss-Point is not availible. Chose 'stress_location' between 1 and " <<
									num_of_deriv  << "!"<< std::endl;
				}
				else if(m_stress_treatment == node)
				{
					if(num_of_deriv >= m_id_of_location)
						stress_displ_deriv_value = stress_displ_deriv(dof_it, (m_id_of_location-1));
					else
						KRATOS_ERROR << "Chosen node is not availible. The element has only " <<
									num_of_deriv  << " nodes."<< std::endl;

				}
				rResponseGradient[dof_it] = (-1) * stress_displ_deriv_value;
				//std::cout << ("stress_disp_deriv_value = ") << -1*stress_displ_deriv_value << std::endl;
				stress_displ_deriv_value = 0.0;
			}
		}
	}


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

	// ==============================================================================
	void CalculateSensitivityGradient(Element& rAdjointElem,
                                      const Variable<double>& rVariable,
                                      const Matrix& rDerivativesMatrix,
                                      Vector& rResponseGradient,
                                      ProcessInfo& rProcessInfo) override
    {
      	KRATOS_TRY


		if(rAdjointElem.Id() == m_id_of_traced_element)
		{
			rAdjointElem.SetValue(DESIGN_VARIABLE_NAME, rVariable.Name());

			Matrix stress_DV_deriv;
			if(m_stress_treatment == mean || m_stress_treatment == GP)
				rAdjointElem.Calculate(STRESS_DV_DERIV_ON_GP, stress_DV_deriv, rProcessInfo);
			else
				rAdjointElem.Calculate(STRESS_DV_DERIV_ON_NODE, stress_DV_deriv, rProcessInfo);

		    int num_of_DV = stress_DV_deriv.size1();
			int num_of_deriv = stress_DV_deriv.size2();
			double stress_DV_deriv_value = 0.0;

			if(rResponseGradient.size() != stress_DV_deriv.size1())
				rResponseGradient.resize(stress_DV_deriv.size1(), false);
			KRATOS_ERROR_IF(rResponseGradient.size() != rDerivativesMatrix.size1())
				 << "Size of partial stress design variable derivative does not fit!" << std::endl;

			for (int dv_it = 0 ; dv_it < num_of_DV; dv_it++)
			{
				if(m_stress_treatment == mean)
				{
					for(int GP_it = 0; GP_it < num_of_deriv; GP_it++)
						stress_DV_deriv_value += stress_DV_deriv(dv_it, GP_it);

					stress_DV_deriv_value /= num_of_deriv;
				}
				else if(m_stress_treatment == GP)
				{
					if(num_of_deriv >= m_id_of_location)
						stress_DV_deriv_value = stress_DV_deriv(dv_it, (m_id_of_location-1));
					else
						KRATOS_ERROR << "Chosen Gauss-Point is not availible. Chose 'stress_location' between 1 and " <<
									num_of_deriv  << "!"<< std::endl;
				}
				else if(m_stress_treatment == node)
				{
					if(num_of_deriv >= m_id_of_location)
						stress_DV_deriv_value = stress_DV_deriv(dv_it, (m_id_of_location-1));
					else
						KRATOS_ERROR << "Chosen node is not availible. The element has only " <<
									num_of_deriv  << " nodes."<< std::endl;
				}
				rResponseGradient[dv_it] =  stress_DV_deriv_value;
				//std::cout << ("partial derivative (prop) = ") << stress_DV_deriv_value << std::endl;
				stress_DV_deriv_value = 0.0;
			}

			rAdjointElem.SetValue(DESIGN_VARIABLE_NAME, "");
		}
		else
		{
			if (rResponseGradient.size() != rDerivativesMatrix.size1())
          			rResponseGradient.resize(rDerivativesMatrix.size1(), false);
			rResponseGradient.clear();
		}

        KRATOS_CATCH("")
	}

	// ==============================================================================
	void CalculateSensitivityGradient(Condition& rAdjointCondition,
                                     const Variable<double>& rVariable,
                                     const Matrix& rDerivativesMatrix,
                                     Vector& rResponseGradient,
                                     ProcessInfo& rProcessInfo) override
	{
		KRATOS_TRY;

		if (rResponseGradient.size() != rDerivativesMatrix.size1())
          		rResponseGradient.resize(rDerivativesMatrix.size1(), false);
		rResponseGradient.clear();

		KRATOS_CATCH("");
	}

	// ==============================================================================
	void CalculateSensitivityGradient(Element& rAdjointElem,
                                      const Variable<array_1d<double,3>>& rVariable,
                                      const Matrix& rDerivativesMatrix,
                                      Vector& rResponseGradient,
                                      ProcessInfo& rProcessInfo) override
    {
		KRATOS_TRY;

		if(rAdjointElem.Id() == m_id_of_traced_element)
		{
			rAdjointElem.SetValue(DESIGN_VARIABLE_NAME, rVariable.Name());

			Matrix stress_DV_deriv;
			if(m_stress_treatment == mean || m_stress_treatment == GP)
				rAdjointElem.Calculate(STRESS_DV_DERIV_ON_GP, stress_DV_deriv, rProcessInfo);
			else
				rAdjointElem.Calculate(STRESS_DV_DERIV_ON_NODE, stress_DV_deriv, rProcessInfo);

		    int num_of_DV = stress_DV_deriv.size1();
			int num_of_deriv = stress_DV_deriv.size2();
			double stress_DV_deriv_value = 0.0;

			if(rResponseGradient.size() != stress_DV_deriv.size1())
				rResponseGradient.resize(stress_DV_deriv.size1(), false);
			KRATOS_ERROR_IF(rResponseGradient.size() != rDerivativesMatrix.size1())
				<< "Size of partial stress design variable derivative does not fit!" << std::endl;

			for (int dv_it = 0 ; dv_it < num_of_DV; dv_it++)
			{
				if(m_stress_treatment == mean)
				{
					for(int GP_it = 0; GP_it < num_of_deriv; GP_it++)
						stress_DV_deriv_value += stress_DV_deriv(dv_it, GP_it);

					stress_DV_deriv_value /= num_of_deriv;
				}
				else if(m_stress_treatment == GP)
				{
					if(num_of_deriv >= m_id_of_location)
						stress_DV_deriv_value = stress_DV_deriv(dv_it, (m_id_of_location-1));
					else
						KRATOS_ERROR << "Chosen Gauss-Point is not availible. Chose 'stress_location' between 1 and " <<
									num_of_deriv  << "!"<< std::endl;
				}
				else if(m_stress_treatment == node)
				{
					if(num_of_deriv >= m_id_of_location)
						stress_DV_deriv_value = stress_DV_deriv(dv_it, (m_id_of_location-1));
					else
						KRATOS_ERROR << "Chosen node is not availible. The element has only " <<
									num_of_deriv  << " nodes."<< std::endl;
				}
				rResponseGradient[dv_it] = stress_DV_deriv_value;
				//std::cout << ("partiall derivative (shape) = ") << stress_DV_deriv_value << std::endl;
				stress_DV_deriv_value = 0.0;
			}

			rAdjointElem.SetValue(DESIGN_VARIABLE_NAME, "");
		}
		else
		{
			if (rResponseGradient.size() != rDerivativesMatrix.size1())
          			rResponseGradient.resize(rDerivativesMatrix.size1(), false);
			rResponseGradient.clear();
		}

		KRATOS_CATCH("");
	}

	// ==============================================================================
	void CalculateSensitivityGradient(Condition& rAdjointCondition,
                                      const Variable<array_1d<double,3>>& rVariable,
                                      const Matrix& rDerivativesMatrix,
                                      Vector& rResponseGradient,
                                      ProcessInfo& rProcessInfo)
    {
		KRATOS_TRY;

		if(rResponseGradient.size() != rDerivativesMatrix.size1())
          	rResponseGradient.resize(rDerivativesMatrix.size1(), false);
		rResponseGradient.clear();

		KRATOS_CATCH("");
	}

	// ==============================================================================

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

	double m_stress_value;
	unsigned int m_id_of_traced_element;
	int m_id_of_location;
	Element::Pointer mp_traced_pElement;
	StressTreatment m_stress_treatment;

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

	/// Assignment operator.
	//      AdjointLocalStressResponseFunction& operator=(SAdjointLocalStressResponseFunction const& rOther);

	/// Copy constructor.
	//      AdjointLocalStressResponseFunction(AdjointLocalStressResponseFunction const& rOther);

	///@}

}; // Class AdjointLocalStressResponseFunction

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

///@}

} // namespace Kratos.

#endif // ADJOINT_LOCAL_STRESS_RESPONSE_FUNCTION_H
