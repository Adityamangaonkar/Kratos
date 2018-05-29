/*
==============================================================================
KratosTestApplication 
A library based on:
Kratos
A General Purpose Software for Multi-Physics Finite Element Analysis
Version 1.0 (Released on march 05, 2007).

Copyright 2007
Pooyan Dadvand, Riccardo Rossi
pooyan@cimne.upc.edu 
rrossi@cimne.upc.edu
- CIMNE (International Center for Numerical Methods in Engineering),
Gran Capita' s/n, 08034 Barcelona, Spain


Permission is hereby granted, free  of charge, to any person obtaining
a  copy  of this  software  and  associated  documentation files  (the
"Software"), to  deal in  the Software without  restriction, including
without limitation  the rights to  use, copy, modify,  merge, publish,
distribute,  sublicense and/or  sell copies  of the  Software,  and to
permit persons to whom the Software  is furnished to do so, subject to
the following condition:

Distribution of this code for  any  commercial purpose  is permissible
ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

The  above  copyright  notice  and  this permission  notice  shall  be
included in all copies or substantial portions of the Software.

THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

==============================================================================
*/
 
//   
//   Project Name:        Kratos       
//   Last modified by:    $Author:  $
//   Date:                $Date:  $
//   Revision:            $Revision: 1.2 $
//
//


// System includes 

// External includes 
#include <pybind11/pybind11.h>

// Project includes
#include "includes/define_python.h"
#include "processes/process.h"
#include "custom_python/add_custom_utilities_to_python.h"
#include "custom_utilities/add_mean_velocity_lagrange_multiplier_conditions.h"
#include "custom_utilities/add_velocity_gradients_lagrange_multiplier_conditions.h"
#include "custom_utilities/add_periodic_conditions.h"
#include "custom_utilities/shear_terms_computation_utility.h"

namespace Kratos
{
	
namespace Python
{

	
void  AddCustomUtilitiesToPython(pybind11::module& m)
  {
	//using namespace boost::python;
using namespace pybind11;



                   
        class_<AddMeanVelocityLagrangeMultiplierConditions2D > (m,"AddMeanVelocityLagrangeMultiplierConditions2D").def(init<ModelPart&>())
                .def("AddThem", &AddMeanVelocityLagrangeMultiplierConditions2D::AddThem)
                ;            
        class_<AddVelocityGradientsLagrangeMultiplierConditions2D > (m,"AddVelocityGradientsLagrangeMultiplierConditions2D").def(init<ModelPart&>())
                .def("AddThem", &AddVelocityGradientsLagrangeMultiplierConditions2D::AddThem)
                .def("AddThem2", &AddVelocityGradientsLagrangeMultiplierConditions2D::AddThem2)
                ;  
        class_<AddPeriodicConditionsNormalOnly2D > (m,"AddPeriodicConditionsNormalOnly2D").def(init<ModelPart&>())
                .def("AddThem", &AddPeriodicConditionsNormalOnly2D::AddThem)
                .def("AddThemWithTangentInversePeriodicity", &AddPeriodicConditionsNormalOnly2D::AddThemWithTangentInversePeriodicity)
		.def("AddThemWithTangentInversePeriodicityOnlyAtVericalWallsAboveGivenY", &AddPeriodicConditionsNormalOnly2D::AddThemWithTangentInversePeriodicityOnlyAtVericalWallsAboveGivenY)
		.def("AddThemWithTangentInversePeriodicityAllWallsBelowGivenY", &AddPeriodicConditionsNormalOnly2D::AddThemWithTangentInversePeriodicityAllWallsBelowGivenY)
                .def("AddThemWithNormalInversePeriodicity", &AddPeriodicConditionsNormalOnly2D::AddThemWithNormalInversePeriodicity)
		.def("AddThemWithTangentInversePeriodicityForBoundryElementWithPeriodicityIntheMidH", &AddPeriodicConditionsNormalOnly2D::AddThemWithTangentInversePeriodicityForBoundryElementWithPeriodicityIntheMidH)

                ;          
        class_<ShearTermsComputationUtility2D > (m,"ShearTermsComputationUtility2D").def(init<ModelPart&>())
                .def("ComputeTau", &ShearTermsComputationUtility2D::ComputeTau)
				;
        
}
	




}  // namespace Python.

} // Namespace Kratos

