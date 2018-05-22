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

#if !defined(KRATOS_MAPPER_LOCAL_SYSTEM_H_INCLUDED )
#define  KRATOS_MAPPER_LOCAL_SYSTEM_H_INCLUDED


// System includes


// External includes


// Project includes
#include "includes/define.h"
#include "includes/node.h"
#include "geometries/geometry.h"

#include "mapper_interface_info.h"


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

class BaseMapperLocalSystem
{
    public:
    using MapperLocalSystemUniquePointer = Kratos::unique_ptr<BaseMapperLocalSystem>;
    using SizeType = std::size_t;
    using IndexType = std::size_t;

    using MappingWeightsVector = std::vector<double>;
    using EquationIdVectorType = std::vector<IndexType>;

    using NodeType = Node<3>;
    using NodePointerType = Node<3>::Pointer;
    using GeometryType = Geometry<NodeType>;

    virtual MapperLocalSystemUniquePointer Create(NodePointerType pNode) const = 0;
    virtual MapperLocalSystemUniquePointer Create(const GeometryType& rGeometry) const = 0;


    virtual void EquationIdVectors(EquationIdVectorType& rOriginIds,
                                   EquationIdVectorType& rDestinationIds) = 0;

    virtual void CalculateLocalSystem(MappingWeightsVector& rMappingWeights,
                              EquationIdVectorType& rOriginIds,
                              EquationIdVectorType& rDestinationIds) const = 0;

    // This specifies if Nodes should be used for the construction
    // => this is the case if the Geometry on the destination is not important
    virtual bool UseNodesAsBasis() const = 0;

    template<typename TDataHolder>
    void AddInterfaceInfo(Kratos::shared_ptr<MapperInterfaceInfo<TDataHolder>> pInterfaceInfo)
    {
        // pInterfaceInfo is a shared_ptr, therefore passing by value
        // mInterfaceInfos.push_back(pInterfaceInfo);
    }

};


/// This is the "Condition" of the Mappers.
/** Detail class definition.
*/
template<class TDataHolder>
class MapperLocalSystem : public BaseMapperLocalSystem
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of MapperLocalSystem
    KRATOS_CLASS_POINTER_DEFINITION(MapperLocalSystem);

    using BaseType = BaseMapperLocalSystem;

    using MapperLocalSystemUniquePointer = typename BaseType::MapperLocalSystemUniquePointer;
    using MapperInterfaceInfoPointer = typename MapperInterfaceInfo<TDataHolder>::Pointer;

    using SizeType = typename BaseType::IndexType;
    using IndexType = typename BaseType::IndexType;

    using MappingWeightsVector = typename BaseType::MappingWeightsVector;
    using EquationIdVectorType = typename BaseType::EquationIdVectorType;

    using NodePointerType = typename BaseType::NodePointerType;
    using GeometryType = typename BaseType::GeometryType;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    MapperLocalSystem() {}

    /// Destructor.
    virtual ~MapperLocalSystem() {}


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    // Only one of the Create functions has to be implemented, thats why they cannot be pure virtual!
    MapperLocalSystemUniquePointer Create(NodePointerType pNode) const override
    {
        KRATOS_ERROR << "Base class function called!" << std::endl;
    }

    MapperLocalSystemUniquePointer Create(const MapperLocalSystem<TDataHolder>::GeometryType& rGeometry) const override
    {
        KRATOS_ERROR << "Base class function called!" << std::endl;
    }

    void EquationIdVectors(EquationIdVectorType& rOriginIds,
                           EquationIdVectorType& rDestinationIds) override
    {
        if (!mIsComputed)
        {
            CalculateAll(mMappingWeights, mOriginIds, mDestinationIds);
            mIsComputed = true;
        }

        rOriginIds      = mOriginIds;
        rDestinationIds = mDestinationIds;
    }

    void CalculateLocalSystem(MappingWeightsVector& rMappingWeights,
                              EquationIdVectorType& rOriginIds,
                              EquationIdVectorType& rDestinationIds) const final override
    {
        if (mIsComputed)
        {
            // This will be called if the EquationIdVectors have been querried before
            // i.e. matrix-based mapping
            rMappingWeights = mMappingWeights;
            rOriginIds      = mOriginIds;
            rDestinationIds = mDestinationIds;
        }
        else
        {
            // This will be called if the EquationIdVectors have NOT been querried before
            // i.e. matrix-free mapping
            CalculateAll(rMappingWeights, rOriginIds, rDestinationIds);
        }
    }


    // // This specifies if Nodes should be used for the construction
    // // => this is the case if the Geometry on the destination is not important
    // virtual bool UseNodesAsBasis() const = 0;


    void AddInterfaceInfo(MapperInterfaceInfoPointer pInterfaceInfo)
    {
        // pInterfaceInfo is a shared_ptr, therefore passing by value
        mInterfaceInfos.push_back(pInterfaceInfo);
    }


    virtual void Clear()
    {
        mInterfaceInfos.clear();
        if (mIsComputed)
        {
            mMappingWeights.clear();
            mOriginIds.clear();
            mDestinationIds.clear();
            mIsComputed = false;
        }
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

    /// Turn back information as a string.
    virtual std::string Info() const {return "MapperLocalSystem";}

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

    std::vector<MapperInterfaceInfoPointer> mInterfaceInfos;

    bool mIsComputed = false;

    MappingWeightsVector mMappingWeights;
    EquationIdVectorType mOriginIds;
    EquationIdVectorType mDestinationIds;


    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{

    // This function calculates the components necessary for the mapping
    // Note that it is "const", therefore it can NOT modify its members
    // Whether members are to be saved is decided in other functions of this class
    virtual void CalculateAll(MappingWeightsVector& rMappingWeights,
                              EquationIdVectorType&     rOriginIds,
                              EquationIdVectorType&     rDestinationIds) const = 0;


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

    // /// Assignment operator.
    // MapperLocalSystem& operator=(MapperLocalSystem const& rOther);

    // /// Copy constructor.
    // MapperLocalSystem(MapperLocalSystem const& rOther);


    ///@}

}; // Class MapperLocalSystem

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


// /// input stream function
// inline std::istream& operator >> (std::istream& rIStream,
//                 MapperLocalSystem& rThis);

// /// output stream function
// inline std::ostream& operator << (std::ostream& rOStream,
//                 const MapperLocalSystem& rThis)
// {
//     rThis.PrintInfo(rOStream);
//     rOStream << std::endl;
//     rThis.PrintData(rOStream);

//     return rOStream;
// }
///@}

///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_MAPPER_LOCAL_SYSTEM_H_INCLUDED  defined