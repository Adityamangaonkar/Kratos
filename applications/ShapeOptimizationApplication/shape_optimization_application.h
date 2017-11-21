// ==============================================================================
//  KratosShapeOptimizationApplication
//
//  License:         BSD License
//                   license: ShapeOptimizationApplication/license.txt
//
//  Main authors:    Baumgaertner Daniel, https://github.com/dbaumgaertner
//                   Geiser Armin, https://github.com/armingeiser
//
// ==============================================================================

#if !defined(KRATOS_SHAPEOPTIMIZATION_APPLICATION_H_INCLUDED )
#define  KRATOS_SHAPEOPTIMIZATION_APPLICATION_H_INCLUDED

// ------------------------------------------------------------------------------
// System includes
// ------------------------------------------------------------------------------

#include <string>
#include <iostream> 

// ------------------------------------------------------------------------------
// External includes
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// Project includes
// ------------------------------------------------------------------------------
#include "includes/define.h"
#include "includes/kratos_application.h"

// elements
#include "custom_elements/small_displacement_analytic_sensitivity_element.hpp"

//conditions
#include "custom_conditions/shape_optimization_condition.h"

// Variables
#include "includes/variables.h"

#include "custom_utilities/finite_differences_utilities.h" //MFusseder

// ==============================================================================

namespace Kratos
{

	///@name Kratos Globals
	///@{ 

	// Geometry variables
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(NORMALIZED_SURFACE_NORMAL);

    // Optimization variables
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(OBJECTIVE_SENSITIVITY);
    KRATOS_DEFINE_VARIABLE(double,OBJECTIVE_SURFACE_SENSITIVITY);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(MAPPED_OBJECTIVE_SENSITIVITY);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(CONSTRAINT_SENSITIVITY);
    KRATOS_DEFINE_VARIABLE(double,CONSTRAINT_SURFACE_SENSITIVITY);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(MAPPED_CONSTRAINT_SENSITIVITY);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(SEARCH_DIRECTION);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(DESIGN_UPDATE);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(DESIGN_CHANGE_ABSOLUTE);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(SHAPE_UPDATE);
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(SHAPE_CHANGE_ABSOLUTE);

	// For edge damping
	KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(DAMPING_FACTOR);

    // For mapping
    KRATOS_DEFINE_VARIABLE(int,MAPPING_ID);

    // For Structure Sensitivity Analysis
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(STRAIN_ENERGY_SHAPE_GRADIENT);
	KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(MASS_SHAPE_GRADIENT);
	KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(EIGENFREQUENCY_SHAPE_GRADIENT);
	KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(LOCAL_STRESS_GRADIENT);
    KRATOS_DEFINE_VARIABLE(int,ACTIVE_NODE_INDEX);
	KRATOS_DEFINE_VARIABLE(Vector,DKDXU);
    KRATOS_DEFINE_VARIABLE(Vector,DKDXU_X);
    KRATOS_DEFINE_VARIABLE(Vector,DKDXU_Y);
    KRATOS_DEFINE_VARIABLE(Vector,DKDXU_Z);
	// Variables for output of sensitivities
	KRATOS_DEFINE_VARIABLE(double, CROSS_AREA_SENSITIVITY );
	KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS( POINT_LOAD_SENSITIVITY );
    KRATOS_DEFINE_VARIABLE(double, IY_SENSITIVITY );
    KRATOS_DEFINE_VARIABLE(double, IZ_SENSITIVITY );
    KRATOS_DEFINE_VARIABLE(double, THICKNESS_SENSITIVITY );
	KRATOS_DEFINE_VARIABLE(double, YOUNG_MODULUS_SENSITIVITY );
    // Stock Variable for Sensitivity Analysis
    KRATOS_DEFINE_3D_VARIABLE_WITH_COMPONENTS(STOCK_SENSITIVITY);
	// Variables to for computing parts of sensitivity analysis
	KRATOS_DEFINE_VARIABLE(std::string, TRACED_STRESS_TYPE); 
	KRATOS_DEFINE_VARIABLE(Matrix, STRESS_DISP_DERIV_ON_NODE); 
	KRATOS_DEFINE_VARIABLE(Matrix, STRESS_DISP_DERIV_ON_GP); 
	KRATOS_DEFINE_VARIABLE(Matrix, STRESS_DV_DERIV_ON_NODE); 
	KRATOS_DEFINE_VARIABLE(Matrix, STRESS_DV_DERIV_ON_GP); 
	KRATOS_DEFINE_VARIABLE(Vector, STRESS_ON_GP ); 
	KRATOS_DEFINE_VARIABLE(Vector, STRESS_ON_NODE ); 
	KRATOS_DEFINE_VARIABLE(std::string, DESIGN_VARIABLE_NAME); 
	KRATOS_DEFINE_VARIABLE( FiniteDifferencesUtilities::Pointer, FINITE_DIFFERENCE_INFORMATION );
	


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
	class KratosShapeOptimizationApplication : public KratosApplication
	{
	public:
		///@name Type Definitions
		///@{
		

		/// Pointer definition of KratosShapeOptimizationApplication
		KRATOS_CLASS_POINTER_DEFINITION(KratosShapeOptimizationApplication);

		///@}
		///@name Life Cycle 
		///@{ 

		/// Default constructor.
		KratosShapeOptimizationApplication();

		/// Destructor.
		virtual ~KratosShapeOptimizationApplication(){}


		///@}
		///@name Operators 
		///@{


		///@}
		///@name Operations
		///@{

		virtual void Register();



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
		virtual std::string Info() const
		{
			return "KratosShapeOptimizationApplication";
		}

		/// Print information about this object.
		virtual void PrintInfo(std::ostream& rOStream) const
		{
			rOStream << Info();
			PrintData(rOStream);
		}

		///// Print object's data.
      virtual void PrintData(std::ostream& rOStream) const
      {
      	KRATOS_WATCH("in my application");
      	KRATOS_WATCH(KratosComponents<VariableData>::GetComponents().size() );
		rOStream << "Variables:" << std::endl;
		KratosComponents<VariableData>().PrintData(rOStream);
		rOStream << std::endl;
		rOStream << "Elements:" << std::endl;
		KratosComponents<Element>().PrintData(rOStream);
		rOStream << std::endl;
		rOStream << "Conditions:" << std::endl;
		KratosComponents<Condition>().PrintData(rOStream);
      }


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



		//       static const ApplicationCondition  msApplicationCondition; 

		///@} 
		///@name Member Variables 
		///@{ 

        // elements

        // for structural optimization
      	const SmallDisplacementAnalyticSensitivityElement mSmallDisplacementAnalyticSensitivityElement3D4N;
      	const SmallDisplacementAnalyticSensitivityElement mSmallDisplacementAnalyticSensitivityElement3D10N;
      	const SmallDisplacementAnalyticSensitivityElement mSmallDisplacementAnalyticSensitivityElement3D8N;
      	const SmallDisplacementAnalyticSensitivityElement mSmallDisplacementAnalyticSensitivityElement3D20N;

        //conditions
        const ShapeOptimizationCondition mShapeOptimizationCondition3D3N;
		const ShapeOptimizationCondition mShapeOptimizationCondition3D4N;
        const ShapeOptimizationCondition mShapeOptimizationCondition2D2N;
		const ShapeOptimizationCondition mShapeOptimizationCondition3D2N; //fusseder


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
		KratosShapeOptimizationApplication& operator=(KratosShapeOptimizationApplication const& rOther);

		/// Copy constructor.
		KratosShapeOptimizationApplication(KratosShapeOptimizationApplication const& rOther);


		///@}    

	}; // Class KratosShapeOptimizationApplication 

	///@} 


	///@name Type Definitions       
	///@{ 


	///@} 
	///@name Input and output 
	///@{ 

	///@} 


}  // namespace Kratos.

#endif // KRATOS_SHAPEOPTIMIZATION_APPLICATION_H_INCLUDED  defined 


