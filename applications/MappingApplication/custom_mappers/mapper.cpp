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

// System includes

// External includes

// Project includes
#include "mapper.h"
#include "custom_utilities/mapper_typedefs.h"
#include "custom_searching/interface_search_structure.h"
#ifdef KRATOS_USING_MPI // mpi-parallel compilation
#include "custom_searching/interface_search_structure_mpi.h"
#endif

namespace Kratos
{
/***********************************************************************************/
/* PUBLIC Methods */
/***********************************************************************************/

/***********************************************************************************/
/* PROTECTED Methods */
/***********************************************************************************/
template<class TSparseSpace, class TDenseSpace>
Mapper<TSparseSpace, TDenseSpace>::Mapper(ModelPart& rModelPartOrigin,
                ModelPart& rModelPartDestination,
                Parameters MapperSettings) :
                    mrModelPartOrigin(rModelPartOrigin),
                    mrModelPartDestination(rModelPartDestination)
{
    // TODO throw error in case of MPI-execution with one core

    // ValidateParameters(MapperSettings);
    // mEchoLevel = MapperSettings["echo_level"].GetInt();
}

/* This function initializes the Mapper
I.e. Operations that should be performed ONLY ONCE in the livetime of the mapper
*/
template<class TSparseSpace, class TDenseSpace>
void Mapper<TSparseSpace, TDenseSpace>::Initialize()
{
    mpMapperLocalSystems = Kratos::make_shared<MapperLocalSystemPointerVector>();

    mpInterfacePreprocessor = Kratos::make_unique<InterfacePreprocessor>(mrModelPartDestination,
                                                                         mpMapperLocalSystems);
    InitializeMappingOperationUtility();

    InitializeSearchStructure();

    InitializeInterface();
}

/* Performs operations that are needed for Initialization and when the interface is updated (=> Remeshed)
I.e. Operations that can be performed several times in the livetime of the mapper
*/
template<class TSparseSpace, class TDenseSpace>
void Mapper<TSparseSpace, TDenseSpace>::InitializeInterface()
{
    mpMapperLocalSystems->clear();

    const MapperLocalSystemPointer p_ref_local_system = GetMapperLocalSystem();

    KRATOS_ERROR_IF_NOT(mpInterfacePreprocessor) << "mpInterfacePreprocessor is a nullptr!" << std::endl;
    mpInterfacePreprocessor->GenerateInterfaceModelPart(p_ref_local_system);

    BuildMappingMatrix();
}

/* Performs operations that are needed for Initialization and when the interface is updated (All cases)
I.e. Operations that can be performed several times in the livetime of the mapper
*/
template<class TSparseSpace, class TDenseSpace>
void Mapper<TSparseSpace, TDenseSpace>::BuildMappingMatrix(Kratos::Flags MappingOptions)
{
    KRATOS_ERROR_IF_NOT(mpSearchStructure) << "mpSearchStructure is a nullptr!" << std::endl;

    const MapperInterfaceInfoUniquePointerType p_ref_interface_info = GetMapperInterfaceInfo();
    const InterfaceObject::ConstructionType interface_object_construction_type_origin =
        GetInterfaceObjectConstructionTypeOrigin();
    const InterfaceObject::ConstructionType interface_object_construction_type_destination =
        GetInterfaceObjectConstructionTypeDestination();

    mpSearchStructure->ExchangeInterfaceData(MappingOptions,
                                             p_ref_interface_info,
                                             interface_object_construction_type_origin,
                                             interface_object_construction_type_destination);

    KRATOS_ERROR_IF_NOT(mpMappingOperationUtility) << "mpMappingOperationUtility is a nullptr!" << std::endl;

    // this function can always be called, it won't do anything if the sizes are correct
    mpMappingOperationUtility->ResizeAndInitializeVectors(mpMdo, mpQo, mpQd,
                                                          mrModelPartOrigin,
                                                          mrModelPartDestination,
                                                          *mpMapperLocalSystems);

    mpMappingOperationUtility->BuildMappingMatrix(*mpMapperLocalSystems, *mpMdo);
}

template<class TSparseSpace, class TDenseSpace>
void Mapper<TSparseSpace, TDenseSpace>::InitializeMappingOperationUtility()
{
    // here we could return the MatrixFree variant in the future
    Parameters utility_settings(R"({})"); // TODO fill this
    mpMappingOperationUtility = Kratos::make_unique<MatrixBasedMappingOperationUtility<TSparseSpace, TDenseSpace>>(utility_settings);
}

template<>
void Mapper<MapperDefinitions::SparseSpaceType, MapperDefinitions::DenseSpaceType>::InitializeSearchStructure()
{
    // Parameters utility_settings(R"({})"); // TODO fill this
    mpSearchStructure = Kratos::make_unique<InterfaceSearchStructure>(mrModelPartOrigin,
                                                                      mpMapperLocalSystems);
}

#ifdef KRATOS_USING_MPI // mpi-parallel compilation
template<>
void Mapper<MapperDefinitions::MPISparseSpaceType, MapperDefinitions::DenseSpaceType>::InitializeSearchStructure()
{
    // Parameters utility_settings(R"({})"); // TODO fill this
    mpSearchStructure = Kratos::make_unique<InterfaceSearchStructureMPI>(mrModelPartOrigin,
                                                                         mpMapperLocalSystems);
}
#endif

/*
This function contains the actual Implementation Of the UpdateInterface function
It is done like this bcs in this way the same operation can be called on the InverseMapper (if it is initialized)
*/
template<class TSparseSpace, class TDenseSpace>
void Mapper<TSparseSpace, TDenseSpace>::UpdateInterfaceInternal(Kratos::Flags MappingOptions, double SearchRadius)
{
    // Set the Flags according to the type of remeshing
    if (MappingOptions.Is(MapperFlags::REMESHED))
    {
        if (MappingOptions.IsDefined(MapperFlags::ORIGIN_ONLY))
        {
            KRATOS_INFO("Mapper-UpdateInterface") << "If the domain is remeshed then "
                << "setting \"ORIGIN_ONLY\" has no effect" << std::endl;
            MappingOptions.Reset(MapperFlags::ORIGIN_ONLY);
        }
        if (MappingOptions.IsDefined(MapperFlags::DESTINATION_ONLY))
        {
            KRATOS_INFO("Mapper-UpdateInterface") << "If the domain is remeshed then "
                << "setting \"DESTINATION_ONLY\" has no effect" << std::endl;
            MappingOptions.Reset(MapperFlags::DESTINATION_ONLY);
        }
    }

    BuildMappingMatrix(MappingOptions);
}


// template<class TSparseSpace, class TDenseSpace> template<typename T>
// void Mapper<TSparseSpace, TDenseSpace>::TestFunction(T someParam)
// {

// }
/***********************************************************************************/
/* PRIVATE Methods */
/***********************************************************************************/

// /// input stream function
// inline std::istream & operator >> (std::istream& rIStream, Mapper& rThis);

// /// output stream function
// inline std::ostream & operator << (std::ostream& rOStream, const Mapper& rThis) {
//   rThis.PrintInfo(rOStream);
//   rOStream << " : " << std::endl;
//   rThis.PrintData(rOStream);
//   return rOStream;
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class template instantiation
template class Mapper< MapperDefinitions::SparseSpaceType, MapperDefinitions::DenseSpaceType >;
#ifdef KRATOS_USING_MPI // mpi-parallel compilation
template class Mapper< MapperDefinitions::MPISparseSpaceType, MapperDefinitions::DenseSpaceType >;
#endif

}  // namespace Kratos.
