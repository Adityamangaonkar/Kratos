//
//   Project Name:        KratosSolidMechanicsApplication $
//   Created by:          $Author:            JMCarbonell $
//   Last modified by:    $Co-Author:                     $
//   Date:                $Date:            November 2017 $
//   Revision:            $Revision:                  0.0 $
//
//

#if !defined(KRATOS_TIME_INTEGRATION_METHODS_CONTAINER_H_INCLUDED)
#define  KRATOS_TIME_INTEGRATION_METHODS_CONTAINER_H_INCLUDED

// System includes

// External includes

// Project includes
#include "custom_solvers/time_integration_methods/time_integration_method.hpp"

namespace Kratos
{
///@addtogroup SolidMechanicsApplication
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

/** A container for time integration methods.     
 * This class implements a container for setting all methods in ProcessInfo     
 */
class KRATOS_API(SOLID_MECHANICS_APPLICATION) TimeIntegrationMethodsContainer
{
public:
  ///@name Type Definitions
  ///@{

  /// Pointer definition of TimeIntegrationMethodsContainer
  KRATOS_CLASS_POINTER_DEFINITION(TimeIntegrationMethodsContainer);
    
  typedef array_1d<double, 3>                                                                VectorType;
  typedef VariableComponent< VectorComponentAdaptor< VectorType > >               VariableComponentType;
  typedef TimeIntegrationMethod<VariableComponentType, double>                TimeIntegrationMethodType;
  typedef typename TimeIntegrationMethodType::Pointer                  TimeIntegrationMethodPointerType;
  typedef std::string LabelType;

  ///@name Life Cycle
  ///@{

  /// Default constructor.
  TimeIntegrationMethodsContainer() {}

  /// Copy constructor.
  TimeIntegrationMethodsContainer(TimeIntegrationMethodsContainer const& rOther)
      :mTimeIntegrationMethods(rOther.mTimeIntegrationMethods)
  {
  }

  /// Destructor.
  virtual ~TimeIntegrationMethodsContainer() {}

  ///@}
  ///@name Operators
  ///@{
  ///@}
  ///@name Operations
  ///@{
  ///@}
  ///@name Access
  ///@{

  void Set(LabelType const& rLabel, TimeIntegrationMethodPointerType pTimeIntegrationMethod)
  {
    mTimeIntegrationMethods[rLabel] = pTimeIntegrationMethod;
  }

  TimeIntegrationMethodPointerType Get(LabelType const& rLabel)
  {
    return mTimeIntegrationMethods[rLabel];
  }

  bool Has(LabelType const& rLabel)
  {
    typename std::map<LabelType,TimeIntegrationMethodPointerType>::iterator it_method = mTimeIntegrationMethods.find(rLabel);

    if ( it_method != mTimeIntegrationMethods.end() )
      return true;
    else
      return false;
  }
  
      
  ///@}
  ///@name Inquiry
  ///@{
  ///@}
  ///@name Input and output
  ///@{

  /// Turn back information as a string.
  virtual std::string Info() const
  {
    std::stringstream buffer;
    buffer << "TimeIntegrationMethodsContainer";
    return buffer.str();
  }
  
  /// Print information about this object.
  virtual void PrintInfo(std::ostream& rOStream) const
  {
    rOStream << "TimeIntegrationMethodsContainer";
  }
  
  /// Print object's data.
  virtual void PrintData(std::ostream& rOStream) const
  {
    rOStream << "TimeIntegrationMethodsContainer Data";     
  }
  
  ///@}
  ///@name Friends
  ///@{


  ///@}

protected:   
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

  std::map<LabelType,TimeIntegrationMethodPointerType> mTimeIntegrationMethods;

  ///@}
  ///@name Private Operators
  ///@{
  ///@}
  ///@name Private Operations
  ///@{
  ///@}
  ///@name Serialization
  ///@{
  friend class Serializer;

  virtual void save(Serializer& rSerializer) const
  {
  };
  
  virtual void load(Serializer& rSerializer)
  {
  };
  
  ///@}
  ///@}
  ///@name Private Inquiry
  ///@{

  ///@}
  ///@name Un accessible methods
  ///@{
  
  ///@}
    
public:

  DECLARE_HAS_THIS_TYPE_PROCESS_INFO
  DECLARE_ADD_THIS_TYPE_TO_PROCESS_INFO
  DECLARE_GET_THIS_TYPE_FROM_PROCESS_INFO
 
}; // Class TimeIntegrationMethodsContainer
///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{

  inline std::istream & operator >> (std::istream & rIStream, TimeIntegrationMethodsContainer& rThis)
  {
    return rIStream;
  }

  inline std::ostream & operator << (std::ostream & rOStream, const TimeIntegrationMethodsContainer& rThis)
  {
    return rOStream << rThis.Info();
  }


///@}

///@} addtogroup block
  
}  // namespace Kratos.

#endif // KRATOS_TIME_INTEGRATION_METHODS_CONTAINER_H_INCLUDED defined
