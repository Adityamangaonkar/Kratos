// KRATOS  __  __ _____ ____  _   _ ___ _   _  ____ 
//        |  \/  | ____/ ___|| | | |_ _| \ | |/ ___|
//        | |\/| |  _| \___ \| |_| || ||  \| | |  _ 
//        | |  | | |___ ___) |  _  || || |\  | |_| |
//        |_|  |_|_____|____/|_| |_|___|_| \_|\____| APPLICATION
//
//  License:		 BSD License
//                       license: MeshingApplication/license.txt
//
//  Main authors:    Anna Rehr
//

#if !defined(KRATOS_SPR_ERROR_METRICS_PROCESS)
#define KRATOS_SPR_ERROR_METRICS_PROCESS

// Project includes
#include "utilities/math_utils.h"
#include "custom_utilities/metrics_math_utils.h"
#include "includes/kratos_parameters.h"
#include "includes/model_part.h"
#include "utilities/openmp_utils.h"
#include "meshing_application.h"
#include "processes/find_nodal_neighbours_process.h"
#include "linear_solvers/skyline_lu_factorization_solver.h"
#include "spaces/ublas_space.h"
#include "utilities/geometry_utilities.h"
namespace Kratos
{
///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

    typedef ModelPart::NodesContainerType                                     NodesArrayType;
    typedef ModelPart::ElementsContainerType                               ElementsArrayType;
    typedef ModelPart::ConditionsContainerType                           ConditionsArrayType;
    typedef Node <3>                                                                NodeType;
    typedef WeakPointerVector< Element >::iterator                         WeakElementItType;
    typedef NodesArrayType::iterator                                              NodeItType;
    typedef ElementsArrayType::iterator                                        ElementItType;
    
///@}
///@name  Enum's
///@{
    
///@}
///@name  Functions
///@{
    
///@}
///@name Kratos Classes
///@{

//// This class is can be used to compute the metrics of the model part with a superconvergent patch recovery approach
template<unsigned int TDim> 
class ComputeSPRErrorSolMetricProcess
    : public Process
{
public:

    ///@name Type Definitions
    ///@{
    
    /// Pointer definition of ComputeSPRErrorSolMetricProcess
    KRATOS_CLASS_POINTER_DEFINITION(ComputeSPRErrorSolMetricProcess);
    
    ///@}
    ///@name Life Cycle
    ///@{
     
    // Constructor
    
    /**
     * This is the default constructor
     * @param rThisModelPart: The model part to be computed
     * @param ThisParameters: The input parameters
     */
    
    ComputeSPRErrorSolMetricProcess(
        ModelPart& rThisModelPart,
        Parameters ThisParameters = Parameters(R"({})")
        );
    
    /// Destructor.
    virtual ~ComputeSPRErrorSolMetricProcess() {}
    
    ///@}
    ///@name Operators
    ///@{

    void operator()()
    {
        Execute();
    }

    ///@}
    ///@name Operations
    ///@{
    
    /**
     * We initialize the metrics of the MMG sol using the Hessian metric matrix approach
     */
    
    void Execute() override;
    
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
        return "ComputeSPRErrorSolMetricProcess";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const
    {
        rOStream << "ComputeSPRErrorSolMetricProcess";
    }

    /// Print object"s data.
    virtual void PrintData(std::ostream& rOStream) const
    {
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
    ///@name Private static Member Variables
    ///@{

    ///@}
    ///@name Private member Variables
    ///@{
    
    ModelPart& mThisModelPart;               // The model part to compute
    double mMinSize;                         // The minimal size of the elements
    double mMaxSize;                         // The maximal size of the elements
    double mPenaltyNormal;                   // The normal penalty
    double mPenaltyTangent;                  // The tangent penalty
    int mEchoLevel;                          // The echo level 
    unsigned int mSigmaSize;                 // The size of the stress vector (Voigt Notation)
    bool mSetElementNumber;                  // Determines if a target number of elements for the new mesh is set
    unsigned int mElementNumber;             // The target number of elements for the new mesh
    double mTargetError;                     // The overall target error for the new mesh
    
    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{
    
    /**
     * 
     */
    double SuperconvergentPatchRecovery();

    /**
     * 
     */
    void CalculatePatch(
        NodeItType itNode,
        NodeItType itPatchNode,
        unsigned int NeighbourSize,
        Vector& rSigmaRecovered);

    /** Calculates the recovered stress at a node in the case of a standard patch without contact BC
    * @param itNode: the node for which the recovered stress should be calculated
    * @param itPatchNode: the center node of the patch
    */
    void CalculatePatchStandard(
        NodeItType itNode,
        NodeItType itPatchNode,
        unsigned int NeighbourSize,
        Vector& rSigmaRecovered);

    /**
     * It calculates the recovered stress at a node where contact BCs are regarded
     * @param itNode: the node for which the recovered stress should be calculated
     * @param itPatchNode: the center node of the patch
     */

    void CalculatePatchContact(
        NodeItType itNode,
        NodeItType itPatchNode,
        unsigned int NeighbourSize,
        Vector& rSigmaRecovered);

    /**
     * Sets the element size
     */
    void ComputeElementSize(ElementItType itElement);
    
    ///@}
    ///@name Private  Access
    ///@{

    ///@}
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Private LifeCycle
    ///@{
    
    ///@}
    ///@name Un accessible methods
    ///@{
    
    /// Assignment operator.
    ComputeSPRErrorSolMetricProcess& operator=(ComputeSPRErrorSolMetricProcess const& rOther)
    {
        return *this;
    };

    /// Copy constructor.
    //ComputeSPRErrorSolMetricProcess(ComputeSPRErrorSolMetricProcess const& rOther);

};// class ComputeSPRErrorSolMetricProcess

};// namespace Kratos.
#endif /* KRATOS_SPR_ERROR_METRICS_PROCESS defined */
