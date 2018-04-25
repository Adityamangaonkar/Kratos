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

#if !defined(KRATOS_INTERFACE_PREPROCESSOR_H)
#define  KRATOS_INTERFACE_PREPROCESSOR_H

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/model_part.h"
#include "includes/kratos_parameters.h"


namespace Kratos
{
///@addtogroup ApplicationNameApplication
///@{

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
class InterfacePreprocessor
{
    public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of InterfacePreprocessor
    KRATOS_CLASS_POINTER_DEFINITION(InterfacePreprocessor);

    using ModelPartPointerType = ModelPart::Pointer;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    InterfacePreprocessor(ModelPart& rModelPartDestination, ModelPartPointerType pInterfaceModelPart);

    /// Destructor.
    virtual ~InterfacePreprocessor() {}


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    void GenerateInterfaceModelPart(Parameters InterfaceParameters);


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
        return "InterfacePreprocessor";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const {}

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const {}


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

    ModelPart& mrModelPartDestination;
    ModelPartPointerType mpInterfaceModelPart;

    Parameters mDefaultParameters = Parameters( R"(
    {
        "mapper_condition_name" : "",
        "use_nodes"             : true
    }  )" );
    // Use "use_nodes" if the geometry on the destination is not needed



    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{

    void CheckAndValidateParameters(Parameters InterfaceParameters);

    void CreateMapperConditions();


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
    // InterfacePreprocessor& operator=(InterfacePreprocessor const& rOther) {}

    /// Copy constructor.
    // InterfacePreprocessor(InterfacePreprocessor const& rOther) {}

    ///@}

    }; // Class InterfacePreprocessor

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


///@}

///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_INTERFACE_PREPROCESSOR_H  defined