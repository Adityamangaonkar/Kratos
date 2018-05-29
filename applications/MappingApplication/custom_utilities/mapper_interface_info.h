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

#if !defined(KRATOS_MAPPER_INTERFACE_INFO_H_INCLUDED)
#define  KRATOS_MAPPER_INTERFACE_INFO_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/node.h"
#include "geometries/geometry.h"
#include "custom_searching/interface_object.h"


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
class MapperInterfaceInfo
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of MapperInterfaceInfo
    KRATOS_CLASS_POINTER_DEFINITION(MapperInterfaceInfo);

    using IndexType = std::size_t;

    using CoordinatesArrayType = typename InterfaceObject::CoordinatesArrayType;

    using NodeType = Node<3>;
    using GeometryType = Geometry<NodeType>;

    ///@}
    ///@name  Enum's
    ///@{

    enum InfoType
    {
        Dummy
    };

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    MapperInterfaceInfo() {}

    MapperInterfaceInfo(const IndexType SourceLocalSystemIndex, const IndexType SourceRank)
        : mSourceLocalSystemIndex(SourceLocalSystemIndex),
          mSourceRank(SourceRank)
    {
    }

    /// Destructor.
    virtual ~MapperInterfaceInfo() {
        std::cout << "Destructor of MapperInterfaceInfo called" << std::endl;
    }


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    virtual void ProcessSearchResult(InterfaceObject::Pointer pInterfaceObject, const double NeighborDistance) = 0;

    virtual MapperInterfaceInfo::Pointer Create(const IndexType SourceLocalSystemIndex,
                                                const IndexType SourceRank=0) const = 0;

    virtual void Clear()
    {
        mLocalSearchWasSuccessful = false;
    }

    IndexType GetLocalSystemIndex() const { return mSourceLocalSystemIndex; }

    IndexType GetSourceRank() const { return mSourceRank; }

    bool GetLocalSearchWasSuccessful() const { return mLocalSearchWasSuccessful; }

    void SetLocalSearchWasSuccessful()
    {
        mLocalSearchWasSuccessful = true;
    }


    ///@}
    ///@name Access
    ///@{

    virtual void GetValue(int& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(std::size_t& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(double& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(bool& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(GeometryType& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }

    virtual void GetValue(std::vector<int>& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(std::vector<std::size_t>& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(std::vector<double>& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(std::vector<bool>& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }
    virtual void GetValue(std::vector<GeometryType>& rValue, const InfoType ValueType=InfoType::Dummy) const { KRATOS_ERROR << "Base class function called!" << std::endl; }


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const
    {
        return "MapperInterfaceInfo";
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

    // These variables need serialization
    IndexType mSourceLocalSystemIndex;

    // These variables are NOT being serialized bcs they are not needed after searching!
    IndexType mSourceRank = 0;

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

    bool mLocalSearchWasSuccessful = false; // this is not being serialized since it is not needed after mpi-data-exchange!


    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{

    friend class Serializer;

    virtual void save(Serializer& rSerializer) const
    {
        rSerializer.save("LocalSysIdx", mSourceLocalSystemIndex);
    }

    virtual void load(Serializer& rSerializer)
    {
        rSerializer.load("LocalSysIdx", mSourceLocalSystemIndex);
    }

    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    // MapperInterfaceInfo& operator=(MapperInterfaceInfo const& rOther) {}

    /// Copy constructor.
    // MapperInterfaceInfo(MapperInterfaceInfo const& rOther) {}

    ///@}

}; // Class MapperInterfaceInfo

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{

// inline std::istream & operator >> (std::istream& rIStream, MapperInterfaceInfo& rThis);

// /// output stream function
// inline std::ostream & operator << (std::ostream& rOStream, const MapperInterfaceInfo& rThis) {
// //   rThis.PrintInfo(rOStream);
//   rOStream << " : " << std::endl;
// //   rThis.PrintData(rOStream);
//   return rOStream;
// }

// /// output stream function
// inline std::ostream & operator << (std::ostream& rOStream, const std::vector<MapperInterfaceInfo::Pointer>& rThis) {
// //   rThis.PrintInfo(rOStream);
//   rOStream << " : " << std::endl;
// //   rThis.PrintData(rOStream);
//   return rOStream;
// }


///@}

///@} addtogroup block

}  // namespace Kratos.

#endif // KRATOS_MAPPER_INTERFACE_INFO_H_INCLUDED  defined
