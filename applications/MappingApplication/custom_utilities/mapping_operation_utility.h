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

#if !defined(KRATOS_MAPPING_OPERATION_UTILITY_H)
#define  KRATOS_MAPPING_OPERATION_UTILITY_H

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/model_part.h"
#include "includes/kratos_parameters.h"
#include "mapper_local_system.h"


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
template<class TSparseSpace, class TDenseSpace>
class MappingOperationUtility
{
    public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of MappingOperationUtility
    KRATOS_CLASS_POINTER_DEFINITION(MappingOperationUtility);

    using SizeType = std::size_t;
    using IndexType = std::size_t;

    using MapperLocalSystemPointer = Kratos::unique_ptr<MapperLocalSystem>;
    using MapperLocalSystemPointerVector = std::vector<MapperLocalSystemPointer>;
    using MapperLocalSystemPointerVectorPointer = Kratos::shared_ptr<MapperLocalSystemPointerVector>;

    using EquationIdVectorType = typename MapperLocalSystem::EquationIdVectorType;

    using TSystemMatrixType = typename TSparseSpace::MatrixType;
    using TSystemVectorType = typename TSparseSpace::VectorType;

    using TSystemMatrixUniquePointerType = Kratos::unique_ptr<TSystemMatrixType>;
    using TSystemVectorUniquePointerType = Kratos::unique_ptr<TSystemVectorType>;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    MappingOperationUtility(Parameters Settings) : mSettings(Settings)
    {
        // Note that no validation is done here,
        // this is supposed to be done in the derived classes
        if (Settings.Has("echo_level"))
            mEchoLevel = Settings["echo_level"].GetInt();
    }

    /// Destructor.
    virtual ~MappingOperationUtility() {}


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    virtual void ResizeAndInitializeVectors(
        TSystemMatrixUniquePointerType& rpMdo,
        TSystemVectorUniquePointerType& rpQo,
        TSystemVectorUniquePointerType& rpQd,
        ModelPart& rModelPartOrigin,
        ModelPart& rModelPartDestination,
        MapperLocalSystemPointerVector& rMapperLocalSystems) const = 0;

    // The "Build" function
    virtual void BuildMappingMatrix(const MapperLocalSystemPointerVector& rMapperLocalSystems,
                                    TSystemMatrixType& rMdo) const = 0;


    // The "Solve" function
    virtual void ExecuteMapping(
        const TSystemMatrixType& rMdo,
        TSystemVectorType& rQo,
        TSystemVectorType& rQd,
        ModelPart& rModelPartOrigin,
        ModelPart& rModelPartDestination,
        const Variable<double>& rOriginVariable,
        const Variable<double>& rDestinationVariable,
        const Kratos::Flags MappingOptions,
        const bool UseTranspose) const = 0;

    // The "Solve" function
    virtual void ExecuteMapping(
        const TSystemMatrixType& rMdo,
        TSystemVectorType& rQo,
        TSystemVectorType& rQd,
        ModelPart& rModelPartOrigin,
        ModelPart& rModelPartDestination,
        const Variable<array_1d<double, 3>>& rOriginVariable,
        const Variable<array_1d<double, 3>>& rDestinationVariable,
        const Kratos::Flags MappingOptions,
        const bool UseTranspose) const = 0;


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
        return "MappingOperationUtility";
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

    Parameters mSettings;


    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{

    int GetEchoLevel() { return mEchoLevel; }

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

    int mEchoLevel = 0;

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
    // MappingOperationUtility& operator=(MappingOperationUtility const& rOther) {}

    /// Copy constructor.
    // MappingOperationUtility(MappingOperationUtility const& rOther) {}

    ///@}

    }; // Class MappingOperationUtility

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


///@}

///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_MAPPING_OPERATION_UTILITY_H  defined
