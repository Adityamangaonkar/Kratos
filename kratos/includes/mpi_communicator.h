//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics 
//
//  License:		 BSD License 
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//                   Riccardo Rossi
//                    
//

#if !defined(KRATOS_MPI_COMMUNICATOR_H_INCLUDED )
#define  KRATOS_MPI_COMMUNICATOR_H_INCLUDED

// System includes
#include <string>
#include <iostream>
#include <sstream>
#include <cstddef>

// External includes
#include "mpi.h"

// Project includes
#include "includes/define.h"
#include "includes/model_part.h"

#include "utilities/openmp_utils.h"
#include "utilities/mpi_utils.h"

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
class MPICommunicator : public Communicator
{
public:
    ///@name  Enum's
    ///@{


    ///@}
    ///@name Type Definitions
    ///@{

    /// Pointer definition of MPICommunicator
    KRATOS_CLASS_POINTER_DEFINITION(MPICommunicator);

    typedef Communicator BaseType;

    typedef BaseType::IndexType IndexType;

    typedef BaseType::SizeType SizeType;

    typedef BaseType::NodeType NodeType;

    typedef BaseType::PropertiesType PropertiesType;

    typedef BaseType::ElementType ElementType;

    typedef BaseType::ConditionType ConditionType;

    typedef BaseType::NeighbourIndicesContainerType NeighbourIndicesContainerType;

    typedef BaseType::MeshType MeshType;

    typedef BaseType::MeshesContainerType MeshesContainerType;

    /// Nodes container. Which is a vector set of nodes with their Id's as key.
    typedef MeshType::NodesContainerType NodesContainerType;

    /** Iterator over the nodes. This iterator is an indirect
        iterator over Node::Pointer which turn back a reference to
        node by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::NodeIterator NodeIterator;

    /** Const iterator over the nodes. This iterator is an indirect
        iterator over Node::Pointer which turn back a reference to
        node by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::NodeConstantIterator NodeConstantIterator;

    /** Iterator over the properties. This iterator is an indirect
        iterator over Properties::Pointer which turn back a reference to
        properties by * operator and not a pointer for more convenient
        usage. */

    /// Properties container. Which is a vector set of Properties with their Id's as key.
    typedef MeshType::PropertiesContainerType PropertiesContainerType;

    /** Iterator over the Properties. This iterator is an indirect
        iterator over Node::Pointer which turn back a reference to
        node by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::PropertiesIterator PropertiesIterator;

    /** Const iterator over the Properties. This iterator is an indirect
        iterator over Properties::Pointer which turn back a reference to
        Properties by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::PropertiesConstantIterator PropertiesConstantIterator;

    /** Iterator over the properties. This iterator is an indirect
        iterator over Properties::Pointer which turn back a reference to
        properties by * operator and not a pointer for more convenient
        usage. */

    /// Element container. A vector set of Elements with their Id's as key.
    typedef MeshType::ElementsContainerType ElementsContainerType;

    /** Iterator over the Elements. This iterator is an indirect
        iterator over Elements::Pointer which turn back a reference to
        Element by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::ElementIterator ElementIterator;

    /** Const iterator over the Elements. This iterator is an indirect
        iterator over Elements::Pointer which turn back a reference to
        Element by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::ElementConstantIterator ElementConstantIterator;

    /// Condintions container. A vector set of Conditions with their Id's as key.
    typedef MeshType::ConditionsContainerType ConditionsContainerType;

    /** Iterator over the Conditions. This iterator is an indirect
       iterator over Conditions::Pointer which turn back a reference to
       Condition by * operator and not a pointer for more convenient
       usage. */
    typedef MeshType::ConditionIterator ConditionIterator;

    /** Const iterator over the Conditions. This iterator is an indirect
        iterator over Conditions::Pointer which turn back a reference to
        Condition by * operator and not a pointer for more convenient
        usage. */
    typedef MeshType::ConditionConstantIterator ConditionConstantIterator;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.

    MPICommunicator(VariablesList* Variables_list) : BaseType(), mpVariables_list(Variables_list)
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    }

    /// Copy constructor.

    MPICommunicator(MPICommunicator const& rOther) : BaseType(rOther)
    {
    }

    /// Destructor.

    virtual ~MPICommunicator()
    {
    }

    virtual Communicator::Pointer Create()
    {
        KRATOS_TRY

        return Communicator::Pointer(new MPICommunicator(mpVariables_list));

        KRATOS_CATCH("");
    }

    ///@}
    ///@name Operators
    ///@{

    /// Assignment operator.

    MPICommunicator & operator=(MPICommunicator const& rOther)
    {
        BaseType::operator=(rOther);
        return *this;
    }

    int MyPID()
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        return rank;
    }

    int TotalProcesses()
    {
        int nproc;
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        return nproc;

    }

    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Operations
    ///@{

    void Barrier()
    {
        MPI_Barrier(MPI_COMM_WORLD);
    }

    virtual bool SumAll(int& rValue)
    {
        int local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        return true;
    }

    virtual bool SumAll(double& rValue)
    {
        double local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        return true;
    }

    virtual bool SumAll(array_1d<double, 3>& rValue)
    {
        array_1d<double, 3> local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 3, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        return true;
    }

    virtual bool MinAll(int& rValue)
    {
        int local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        return true;
    }

    virtual bool MinAll(double& rValue)
    {
        double local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        return true;
    }

    virtual bool MaxAll(int& rValue)
    {
        int local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        return true;
    }

    virtual bool MaxAll(double& rValue)
    {
        double local_value = rValue;
        MPI_Allreduce(&local_value, &rValue, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        return true;
    }

    virtual bool ScanSum(const double& send_partial, double& receive_accumulated)
    {
        double copy_of_send_partial = send_partial;
        MPI_Scan(&copy_of_send_partial, &receive_accumulated, 1, MPI_DOUBLE, MPI_SUM , MPI_COMM_WORLD);
        return true;
    }

    virtual bool ScanSum(const int& send_partial, int& receive_accumulated)
    {
        int copy_of_send_partial = send_partial;
        MPI_Scan(&copy_of_send_partial, &receive_accumulated, 1, MPI_INT, MPI_SUM , MPI_COMM_WORLD);
        return true;
    }

    virtual bool SynchronizeElementalIds()
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                ElementsContainerType& r_local_elements = LocalMesh(i_color).Elements();
                ElementsContainerType& r_ghost_elements = GhostMesh(i_color).Elements();

                unsigned int elemental_data_size = sizeof (std::size_t) / sizeof (int);
                unsigned int local_elements_size = r_local_elements.size();
                unsigned int ghost_elements_size = r_ghost_elements.size();
                unsigned int send_buffer_size = local_elements_size * elemental_data_size;
                unsigned int receive_buffer_size = ghost_elements_size * elemental_data_size;

                if ((local_elements_size == 0) && (ghost_elements_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                double* send_buffer = new double[send_buffer_size];
                double* receive_buffer = new double[receive_buffer_size];

                // Filling the send buffer
                for (ModelPart::ElementIterator i_element = r_local_elements.begin(); i_element != r_local_elements.end(); ++i_element)
                {
                    *(std::size_t*) (send_buffer + position) = i_element->Id();
                    position += elemental_data_size;
                }

                MPI_Status status;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, MPI_INT, destination, send_tag, receive_buffer, receive_buffer_size, MPI_INT, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                position = 0;
                for (ModelPart::ElementIterator i_element = r_ghost_elements.begin(); i_element != r_ghost_elements.end(); ++i_element)
                {
                    i_element->SetId(*reinterpret_cast<std::size_t*> (receive_buffer + position));
                    position += elemental_data_size;
                }

                if (position > receive_buffer_size)
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer;
                delete [] receive_buffer;
            }

        return true;
    }

    virtual bool SynchronizeNodalSolutionStepsData()
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = LocalMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = GhostMesh(i_color).Nodes();

                // Calculating send and received buffer size
                // NOTE: This part works ONLY when all nodes have the same variables list size!
                unsigned int nodal_data_size = 0;
                unsigned int local_nodes_size = r_local_nodes.size();
                unsigned int ghost_nodes_size = r_ghost_nodes.size();
                unsigned int send_buffer_size = 0;
                unsigned int receive_buffer_size = 0;

                if (local_nodes_size == 0)
                {
                    if (ghost_nodes_size == 0)
                        continue; // nothing to transfer!
                    else
                    {
                        nodal_data_size = r_ghost_nodes.begin()->SolutionStepData().TotalSize();
                        receive_buffer_size = ghost_nodes_size * nodal_data_size;
                    }
                }
                else
                {
                    nodal_data_size = r_local_nodes.begin()->SolutionStepData().TotalSize();
                    send_buffer_size = local_nodes_size * nodal_data_size;
                    if (ghost_nodes_size != 0)
                        receive_buffer_size = ghost_nodes_size * nodal_data_size;
                }

                unsigned int position = 0;
                double* send_buffer = new double[send_buffer_size];
                double* receive_buffer = new double[receive_buffer_size];

                // Filling the buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                {
                    std::memcpy(send_buffer + position, i_node->SolutionStepData().Data(), nodal_data_size * sizeof (double));
                    position += nodal_data_size;
                }

                MPI_Status status;

                if (position > send_buffer_size)
                    std::cout << rank << " Error in estimating send buffer size...." << std::endl;


                int send_tag = i_color;
                int receive_tag = i_color;


                MPI_Sendrecv(send_buffer, send_buffer_size, MPI_DOUBLE, destination, send_tag, receive_buffer, receive_buffer_size, MPI_DOUBLE, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                // Updating nodes
                position = 0;
                for (ModelPart::NodeIterator i_node = GhostMesh(i_color).NodesBegin();
                        i_node != GhostMesh(i_color).NodesEnd(); i_node++)
                {
                    std::memcpy(i_node->SolutionStepData().Data(), receive_buffer + position, nodal_data_size * sizeof (double));
                    position += nodal_data_size;
                }

                if (position > receive_buffer_size)
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer;
                delete [] receive_buffer;
            }

        return true;
    }

    virtual bool SynchronizeDofs()
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = LocalMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = GhostMesh(i_color).Nodes();

                // Calculating send and received buffer size
                unsigned int send_buffer_size = 0;
                unsigned int receive_buffer_size = 0;

                for (NodesContainerType::iterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                    send_buffer_size += i_node->GetDofs().size();

                for (NodesContainerType::iterator i_node = r_ghost_nodes.begin(); i_node != r_ghost_nodes.end(); ++i_node)
                    receive_buffer_size += i_node->GetDofs().size();

                unsigned int position = 0;
                int* send_buffer = new int[send_buffer_size];
                int* receive_buffer = new int[receive_buffer_size];


                // Filling the buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                    for (ModelPart::NodeType::DofsContainerType::iterator i_dof = i_node->GetDofs().begin(); i_dof != i_node->GetDofs().end(); i_dof++)
                    {
                        send_buffer[position++] = i_dof->EquationId();
                    }


                MPI_Status status;

                if (position > send_buffer_size)
                    std::cout << rank << " Error in estimating send buffer size...." << std::endl;


                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, MPI_INT, destination, send_tag, receive_buffer, receive_buffer_size, MPI_INT, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                // Updating nodes
                position = 0;
                for (ModelPart::NodeIterator i_node = GhostMesh(i_color).NodesBegin();
                        i_node != GhostMesh(i_color).NodesEnd(); i_node++)
                    for (ModelPart::NodeType::DofsContainerType::iterator i_dof = i_node->GetDofs().begin(); i_dof != i_node->GetDofs().end(); i_dof++)
                        i_dof->SetEquationId(receive_buffer[position++]);

                if (position > receive_buffer_size)
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer;
                delete [] receive_buffer;
            }

        return true;
    }

    virtual bool SynchronizeVariable(Variable<int> const& ThisVariable)
    {
        SynchronizeVariable<int,int>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeVariable(Variable<double> const& ThisVariable)
    {
        SynchronizeVariable<double,double>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeVariable(Variable<array_1d<double, 3 > > const& ThisVariable)
    {
        SynchronizeVariable<array_1d<double, 3 >,double >(ThisVariable);
        return true;
    }

    virtual bool SynchronizeVariable(Variable<Vector> const& ThisVariable)
    {
        SynchronizeVariable<Vector,double>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeVariable(Variable<Matrix> const& ThisVariable)
    {
        SynchronizeVariable<Matrix,double>(ThisVariable);
        return true;
    }

    // This function is for test and will be changed. Pooyan.
    virtual bool SynchronizeCurrentDataToMin(Variable<double> const& ThisVariable)
    {
        SynchronizeMinThisVariable<double,double>(ThisVariable);
        return true;

    }

    virtual bool AssembleCurrentData(Variable<int> const& ThisVariable)
    {
        AssembleThisVariable<int,int>(ThisVariable);
        return true;
    }

    virtual bool AssembleCurrentData(Variable<double> const& ThisVariable)
    {
        AssembleThisVariable<double,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleCurrentData(Variable<array_1d<double, 3 > > const& ThisVariable)
    {
        AssembleThisVariable<array_1d<double,3>,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleCurrentData(Variable<Vector> const& ThisVariable)
    {
        AssembleThisVariable<Vector,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleCurrentData(Variable<Matrix> const& ThisVariable)
    {
        AssembleThisVariable<Matrix,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleNonHistoricalData(Variable<int> const& ThisVariable)
    {
        AssembleThisNonHistoricalVariable<int,int>(ThisVariable);
        return true;
    }

    virtual bool AssembleNonHistoricalData(Variable<double> const& ThisVariable)
    {
        AssembleThisNonHistoricalVariable<double,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleNonHistoricalData(Variable<array_1d<double, 3 > > const& ThisVariable)
    {
        AssembleThisNonHistoricalVariable<array_1d<double,3>,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleNonHistoricalData(Variable<vector<array_1d<double,3> > > const& ThisVariable)
    {
        AssembleThisNonHistoricalVariable<vector<array_1d<double,3> >,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleNonHistoricalData(Variable<Vector> const& ThisVariable)
    {
        AssembleThisNonHistoricalVariable<Vector,double>(ThisVariable);
        return true;
    }

    virtual bool AssembleNonHistoricalData(Variable<Matrix> const& ThisVariable)
    {
        AssembleThisNonHistoricalVariable<Matrix,double>(ThisVariable);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<int> const& ThisVariable)
    {
        SynchronizeElementalNonHistoricalVariable<int,int>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<double> const& ThisVariable)
    {
        SynchronizeElementalNonHistoricalVariable<double,double>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<array_1d<double, 3 > > const& ThisVariable)
    {
        SynchronizeElementalNonHistoricalVariable<array_1d<double,3>,double>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<vector<array_1d<double,3> > > const& ThisVariable)
    {
        SynchronizeHeterogeneousElementalNonHistoricalVariable<array_1d<double,3>,double>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<vector<int> > const& ThisVariable)
    {
        SynchronizeHeterogeneousElementalNonHistoricalVariable<int,int>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<Vector> const& ThisVariable)
    {
        SynchronizeHeterogeneousElementalNonHistoricalVariable<double,double>(ThisVariable);
        return true;
    }

    virtual bool SynchronizeElementalNonHistoricalVariable(Variable<Matrix> const& ThisVariable)
    {
        SynchronizeElementalNonHistoricalVariable<Matrix,double>(ThisVariable);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////

    /**
     * Transfer objects from a given process to a destination process
     * @param SendObjects list of objects to be send.      SendObjects[i] -> Objects to   process i
     * @param RecvObjects list of objects to be received.  RecvObjects[i] -> Objects from process i
     **/
    virtual bool TransferObjects(std::vector<NodesContainerType>& SendObjects, std::vector<NodesContainerType>& RecvObjects)
    {
        Kratos::Serializer particleSerializer;
        AsyncSendAndReceiveObjects<NodesContainerType>(SendObjects,RecvObjects,particleSerializer);
        return true;
    }

    /**
    * Transfer objects from a given process to a destination process
    * @param SendObjects list of objects to be send.      SendObjects[i] -> Objects to   process i
    * @param RecvObjects list of objects to be received.  RecvObjects[i] -> Objects from process i
    **/
    virtual bool TransferObjects(std::vector<ElementsContainerType>& SendObjects, std::vector<ElementsContainerType>& RecvObjects)
    {
        Kratos::Serializer particleSerializer;
        AsyncSendAndReceiveObjects<ElementsContainerType>(SendObjects,RecvObjects,particleSerializer);
        return true;
    }

    /**
    * Transfer objects from a given process to a destination process
    * @param SendObjects list of objects to be send.      SendObjects[i] -> Objects to   process i
    * @param RecvObjects list of objects to be received.  RecvObjects[i] -> Objects from process i
    **/
    virtual bool TransferObjects(std::vector<ConditionsContainerType>& SendObjects, std::vector<ConditionsContainerType>& RecvObjects)
    {
        Kratos::Serializer particleSerializer;
        AsyncSendAndReceiveObjects<ConditionsContainerType>(SendObjects,RecvObjects,particleSerializer);
        return true;
    }

    /**
     * Transfer objects from a given process to a destination process
     * @param SendObjects list of objects to be send.      SendObjects[i] -> Objects to   process i
     * @param RecvObjects list of objects to be received.  RecvObjects[i] -> Objects from process i
     **/
    virtual bool TransferObjects(std::vector<NodesContainerType>& SendObjects, std::vector<NodesContainerType>& RecvObjects,Kratos::Serializer& particleSerializer)
    {
        AsyncSendAndReceiveObjects<NodesContainerType>(SendObjects,RecvObjects,particleSerializer);
        return true;
    }

    /**
     * Transfer objects from a given process to a destination process
     * @param SendObjects list of objects to be send.      SendObjects[i] -> Objects to   process i
     * @param RecvObjects list of objects to be received.  RecvObjects[i] -> Objects from process i
     **/
    virtual bool TransferObjects(std::vector<ElementsContainerType>& SendObjects, std::vector<ElementsContainerType>& RecvObjects,Kratos::Serializer& particleSerializer)
    {
        AsyncSendAndReceiveObjects<ElementsContainerType>(SendObjects,RecvObjects,particleSerializer);
        return true;
    }

    /**
     * Transfer objects from a given process to a destination process
     * @param SendObjects list of objects to be send.      SendObjects[i] -> Objects to   process i
     * @param RecvObjects list of objects to be received.  RecvObjects[i] -> Objects from process i
     **/
    virtual bool TransferObjects(std::vector<ConditionsContainerType>& SendObjects, std::vector<ConditionsContainerType>& RecvObjects,Kratos::Serializer& particleSerializer)
    {
        AsyncSendAndReceiveObjects<ConditionsContainerType>(SendObjects,RecvObjects,particleSerializer);
        return true;
    }

    /**
     * Transfer meshes from a given process to a destination process. The mesh will be transfered completly.
     * In other words all nodes, elements, conditions and properties in the mesh will be transfered.
     * @param SendMeshes list of meshes to be send.      SendMeshes[i] -> Meshes to   process i
     * @param RecvMeshes list of meshes to be received.  RecvMeshes[i] -> Meshes from process i
     **/
    virtual bool TransferMesh(std::vector<MeshType::Pointer>& SendMeshes, std::vector<MeshType::Pointer>& RecvMeshes)
    {
        AsyncSendAndReceiveMeshes<MeshType::Pointer>(SendMeshes, RecvMeshes);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////

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
        return "MPICommunicator";
    }

    /// Print information about this object.

    virtual void PrintInfo(std::ostream& rOStream) const
    {
        rOStream << Info();
    }

    /// Print object's data.

    virtual void PrintData(std::ostream& rOStream) const
    {
        for (IndexType i = 0; i < mLocalMeshes.size(); i++)
        {
            rOStream << "    Local Mesh " << i << " : " << std::endl;
            LocalMesh(i).PrintData(rOStream);
            rOStream << "    Ghost Mesh " << i << " : " << std::endl;
            GhostMesh(i).PrintData(rOStream);
            rOStream << "    Interface Mesh " << i << " : " << std::endl;
            InterfaceMesh(i).PrintData(rOStream);
        }
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


    ///@}
    ///@name Member Variables
    ///@{

    //      SizeType mNumberOfColors;

    //      NeighbourIndicesContainerType mNeighbourIndices;
    //
    //      // To store all local entities
    //      MeshType::Pointer mpLocalMesh;
    //
    //      // To store all ghost entities
    //      MeshType::Pointer mpGhostMesh;
    //
    //      // To store all interface entities
    //      MeshType::Pointer mpInterfaceMesh;
    //
    //      // To store interfaces local entities
    //      MeshesContainerType mLocalMeshes;

    //      // To store interfaces ghost entities
    //      MeshesContainerType mGhostMeshes;
    //
    //      // To store interfaces ghost+local entities
    //      MeshesContainerType mInterfaceMeshes;

    VariablesList* mpVariables_list;

    int mpi_rank;
    int mpi_size;
    int mpi_erno;

    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{

    void PrintNodesId()
    {
        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        int nproc;
        MPI_Comm_size(MPI_COMM_WORLD, &nproc);
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        MPI_Barrier(MPI_COMM_WORLD);
        for (int proc_id = 0; proc_id < nproc; proc_id++)
        {
            if (proc_id == rank)
            {

                for (int i_color = 0; i_color < static_cast<int>(neighbours_indices.size()); i_color++)
                {
                    if ((neighbours_indices[i_color]) >= 0)
                    {
                        NodesContainerType& r_local_nodes = LocalMesh(i_color).Nodes();
                        NodesContainerType& r_ghost_nodes = GhostMesh(i_color).Nodes();
                        std::string tag = "Local nodes in rank ";
                        PrintNodesId(r_local_nodes, tag, i_color);
                        tag = "Ghost nodes in rank ";
                        PrintNodesId(r_ghost_nodes, tag, i_color);
                        tag = "Interface nodes in rank ";
                        PrintNodesId(InterfaceMesh(i_color).Nodes(), tag, i_color);

                    }
                }
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }

    template<class TNodesArrayType>
    void PrintNodesId(TNodesArrayType& rNodes, std::string Tag, int color)
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        std::cout << Tag << rank << " with color " << color << ":";
        for (typename TNodesArrayType::iterator i_node = rNodes.begin(); i_node != rNodes.end(); i_node++)
            std::cout << i_node->Id() << ", ";

        std::cout << std::endl;
    }

    template<class TDataType, class TSendType>
    bool AssembleThisVariable(Variable<TDataType> const& ThisVariable)
    {
        // PrintNodesId();
        /*	KRATOS_WATCH("AssembleThisVariable")
                KRATOS_WATCH(ThisVariable)*/
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();
        std::vector<TSendType*> receive_buffer(neighbours_indices.size());
        std::vector<int> receive_buffer_size(neighbours_indices.size());

        TSendType Value = TSendType();
        MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(Value);

        //first of all gather everything to the owner node
        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = InterfaceMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = InterfaceMesh(i_color).Nodes();

                // Calculating send and received buffer size
                // NOTE: This part can be optimized getting the offset from variables list and using pointers.
                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                unsigned int local_nodes_size = r_local_nodes.size();
                unsigned int ghost_nodes_size = r_ghost_nodes.size();
                unsigned int send_buffer_size = local_nodes_size * nodal_data_size;
                receive_buffer_size[i_color] = ghost_nodes_size * nodal_data_size;

                if ((local_nodes_size == 0) && (ghost_nodes_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                TSendType* send_buffer = new TSendType[send_buffer_size];
                receive_buffer[i_color] = new TSendType[receive_buffer_size[i_color]];

                // Filling the buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                {
                    *(TDataType*) (send_buffer + position) = i_node->FastGetSolutionStepValue(ThisVariable);
                    position += nodal_data_size;
                }

                MPI_Status status;

                if (position > send_buffer_size)
                    std::cout << rank << " Error in estimating send buffer size...." << std::endl;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag,
                             receive_buffer[i_color], receive_buffer_size[i_color], ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                delete [] send_buffer;
            }

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                // Updating nodes
                int position = 0;
                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                NodesContainerType& r_ghost_nodes = InterfaceMesh(i_color).Nodes();

                for (ModelPart::NodeIterator i_node = r_ghost_nodes.begin(); i_node != r_ghost_nodes.end(); ++i_node)
                {
                    i_node->FastGetSolutionStepValue(ThisVariable) += *reinterpret_cast<TDataType*> (receive_buffer[i_color] + position);
                    position += nodal_data_size;
                }

                if (position > receive_buffer_size[i_color])
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] receive_buffer[i_color];
            }

        //MPI_Barrier(MPI_COMM_WORLD);


        //SynchronizeNodalSolutionStepsData();
        SynchronizeVariable<TDataType,TSendType>(ThisVariable);




        return true;
    }

    // this function is for test only and to be removed. Pooyan.
    template<class TDataType, class TSendType>
    bool SynchronizeMinThisVariable(Variable<TDataType> const& ThisVariable)
    {
        // PrintNodesId();
        /*	KRATOS_WATCH("AssembleThisVariable")
                KRATOS_WATCH(ThisVariable)*/
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();
        std::vector<TSendType*> receive_buffer(neighbours_indices.size());
        std::vector<int> receive_buffer_size(neighbours_indices.size());

        TSendType Value = TSendType();
        MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(Value);

        //first of all gather everything to the owner node
        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = InterfaceMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = InterfaceMesh(i_color).Nodes();

                // Calculating send and received buffer size
                // NOTE: This part can be optimized getting the offset from variables list and using pointers.
                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                unsigned int local_nodes_size = r_local_nodes.size();
                unsigned int ghost_nodes_size = r_ghost_nodes.size();
                unsigned int send_buffer_size = local_nodes_size * nodal_data_size;
                receive_buffer_size[i_color] = ghost_nodes_size * nodal_data_size;

                if ((local_nodes_size == 0) && (ghost_nodes_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                TSendType* send_buffer = new TSendType[send_buffer_size];
                receive_buffer[i_color] = new TSendType[receive_buffer_size[i_color]];

                // Filling the buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                {
                    *(TDataType*) (send_buffer + position) = i_node->FastGetSolutionStepValue(ThisVariable);
                    position += nodal_data_size;
                }

                MPI_Status status;

                if (position > send_buffer_size)
                    std::cout << rank << " Error in estimating send buffer size...." << std::endl;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag,
                             receive_buffer[i_color], receive_buffer_size[i_color], ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                delete [] send_buffer;
            }

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                // Updating nodes
                int position = 0;
                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                NodesContainerType& r_ghost_nodes = InterfaceMesh(i_color).Nodes();

                for (ModelPart::NodeIterator i_node = r_ghost_nodes.begin(); i_node != r_ghost_nodes.end(); ++i_node)
                {
                    TDataType& data = i_node->FastGetSolutionStepValue(ThisVariable);
                    data = std::min(data, *reinterpret_cast<TDataType*> (receive_buffer[i_color] + position));
                    position += nodal_data_size;
                }

                if (position > receive_buffer_size[i_color])
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] receive_buffer[i_color];
            }

        //MPI_Barrier(MPI_COMM_WORLD);


        //SynchronizeNodalSolutionStepsData();
        SynchronizeVariable<TDataType,TSendType>(ThisVariable);




        return true;
    }

    template<class TDataType, class TSendType>
    bool SynchronizeVariable(Variable<TDataType> const& ThisVariable)
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        TSendType Value = TSendType();
        MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(Value);

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = LocalMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = GhostMesh(i_color).Nodes();

                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                unsigned int local_nodes_size = r_local_nodes.size();
                unsigned int ghost_nodes_size = r_ghost_nodes.size();
                unsigned int send_buffer_size = local_nodes_size * nodal_data_size;
                unsigned int receive_buffer_size = ghost_nodes_size * nodal_data_size;

                if ((local_nodes_size == 0) && (ghost_nodes_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                TSendType* send_buffer = new TSendType[send_buffer_size];
                TSendType* receive_buffer = new TSendType[receive_buffer_size];

                // Filling the send buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                {
                    *(TDataType*) (send_buffer + position) = i_node->FastGetSolutionStepValue(ThisVariable);
                    position += nodal_data_size;
                }

                MPI_Status status;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag, receive_buffer, receive_buffer_size, ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                position = 0;
                for (ModelPart::NodeIterator i_node = r_ghost_nodes.begin(); i_node != r_ghost_nodes.end(); ++i_node)
                {
                    i_node->FastGetSolutionStepValue(ThisVariable) = *reinterpret_cast<TDataType*> (receive_buffer + position);
                    position += nodal_data_size;
                }

                if (position > receive_buffer_size)
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer;
                delete [] receive_buffer;
            }

        return true;
    }

    template< class TDataType, class TSendType >
    bool AssembleThisNonHistoricalVariable(Variable<TDataType> const& ThisVariable)
    {
        // PrintNodesId();
        /*	KRATOS_WATCH("AssembleThisVariable")
                KRATOS_WATCH(ThisVariable)*/
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();
        std::vector<TSendType*> receive_buffer(neighbours_indices.size());
        std::vector<int> receive_buffer_size(neighbours_indices.size());

        TSendType Value = TSendType();
        MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(Value);

        //first of all gather everything to the owner node
        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = InterfaceMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = InterfaceMesh(i_color).Nodes();

                // Calculating send and received buffer size
                // NOTE: This part can be optimized getting the offset from variables list and using pointers.
                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                unsigned int local_nodes_size = r_local_nodes.size();
                unsigned int ghost_nodes_size = r_ghost_nodes.size();
                unsigned int send_buffer_size = local_nodes_size * nodal_data_size;
                receive_buffer_size[i_color] = ghost_nodes_size * nodal_data_size;

                if ((local_nodes_size == 0) && (ghost_nodes_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                TSendType* send_buffer = new TSendType[send_buffer_size];
                receive_buffer[i_color] = new TSendType[receive_buffer_size[i_color]];

                // Filling the buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                {
                    *(TDataType*) (send_buffer + position) = i_node->GetValue(ThisVariable);
                    position += nodal_data_size;
                }

                MPI_Status status;

                if (position > send_buffer_size)
                    std::cout << rank << " Error in estimating send buffer size...." << std::endl;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag,
                             receive_buffer[i_color], receive_buffer_size[i_color], ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                delete [] send_buffer;
            }

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                // Updating nodes
                int position = 0;
                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                NodesContainerType& r_ghost_nodes = InterfaceMesh(i_color).Nodes();

                for (ModelPart::NodeIterator i_node = r_ghost_nodes.begin(); i_node != r_ghost_nodes.end(); ++i_node)
                {
                    i_node->GetValue(ThisVariable) += *reinterpret_cast<TDataType*> (receive_buffer[i_color] + position);
                    position += nodal_data_size;
                }

                if (position > receive_buffer_size[i_color])
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] receive_buffer[i_color];
            }

        SynchronizeNonHistoricalVariable<TDataType,TSendType>(ThisVariable);




        return true;
    }

    template< class TDataType, class TSendType >
    bool SynchronizeNonHistoricalVariable(Variable<TDataType> const& ThisVariable)
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        TSendType Value = TSendType();
        MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(Value);

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {
                NodesContainerType& r_local_nodes = LocalMesh(i_color).Nodes();
                NodesContainerType& r_ghost_nodes = GhostMesh(i_color).Nodes();

                unsigned int nodal_data_size = sizeof (TDataType) / sizeof (TSendType);
                unsigned int local_nodes_size = r_local_nodes.size();
                unsigned int ghost_nodes_size = r_ghost_nodes.size();
                unsigned int send_buffer_size = local_nodes_size * nodal_data_size;
                unsigned int receive_buffer_size = ghost_nodes_size * nodal_data_size;

                if ((local_nodes_size == 0) && (ghost_nodes_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                TSendType* send_buffer = new TSendType[send_buffer_size];
                TSendType* receive_buffer = new TSendType[receive_buffer_size];

                // Filling the send buffer
                for (ModelPart::NodeIterator i_node = r_local_nodes.begin(); i_node != r_local_nodes.end(); ++i_node)
                {
                    *(TDataType*) (send_buffer + position) = i_node->GetValue(ThisVariable);
                    position += nodal_data_size;
                }

                MPI_Status status;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag, receive_buffer, receive_buffer_size, ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                position = 0;
                for (ModelPart::NodeIterator i_node = r_ghost_nodes.begin(); i_node != r_ghost_nodes.end(); ++i_node)
                {
                    i_node->GetValue(ThisVariable) = *reinterpret_cast<TDataType*> (receive_buffer + position);
                    position += nodal_data_size;
                }

                if (position > receive_buffer_size)
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer;
                delete [] receive_buffer;
            }

        return true;
    }

    template< class TDataType, class TSendType >
    bool SynchronizeElementalNonHistoricalVariable(Variable<TDataType> const& ThisVariable)
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        TSendType Value = TSendType();
        MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(Value);

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++)
            if ((destination = neighbours_indices[i_color]) >= 0)
            {

                ElementsContainerType& r_local_elements = LocalMesh(i_color).Elements();
                ElementsContainerType& r_ghost_elements = GhostMesh(i_color).Elements();

                unsigned int elemental_data_size = sizeof (TDataType) / sizeof (TSendType);
                unsigned int local_elements_size = r_local_elements.size();
                unsigned int ghost_elements_size = r_ghost_elements.size();
                unsigned int send_buffer_size = local_elements_size * elemental_data_size;
                unsigned int receive_buffer_size = ghost_elements_size * elemental_data_size;

                if ((local_elements_size == 0) && (ghost_elements_size == 0))
                    continue; // nothing to transfer!

                unsigned int position = 0;
                TSendType* send_buffer = new TSendType[send_buffer_size];
                TSendType* receive_buffer = new TSendType[receive_buffer_size];

                // Filling the send buffer
                for (ModelPart::ElementIterator i_element = r_local_elements.begin(); i_element != r_local_elements.end(); ++i_element)
                {
                    *(TDataType*) (send_buffer + position) = i_element->GetValue(ThisVariable);
                    position += elemental_data_size;
                }

                MPI_Status status;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag, receive_buffer, receive_buffer_size, ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status);

                position = 0;
                for (ModelPart::ElementIterator i_element = r_ghost_elements.begin(); i_element != r_ghost_elements.end(); ++i_element)
                {
                    //i_element->GetValue(ThisVariable) = *reinterpret_cast<TDataType*> (receive_buffer + position);
                    i_element->SetValue(ThisVariable, *reinterpret_cast<TDataType*> (receive_buffer + position) );
                    position += elemental_data_size;
                }

                if (position > receive_buffer_size)
                    std::cout << rank << " Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer;
                delete [] receive_buffer;
            }

        return true;
    }

    template< class TDataType, class TSendType >
    bool SynchronizeHeterogeneousElementalNonHistoricalVariable(Variable<vector<TDataType> > const& ThisVariable)
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++) {
            if ((destination = neighbours_indices[i_color]) >= 0) {

                ElementsContainerType& r_local_elements = LocalMesh(i_color).Elements();
                ElementsContainerType& r_ghost_elements = GhostMesh(i_color).Elements();
                unsigned int local_elements_size = r_local_elements.size();
                unsigned int ghost_elements_size = r_ghost_elements.size();

                if ((local_elements_size == 0) && (ghost_elements_size == 0))
                    continue; // nothing to transfer!

                ////////////STEP 1 : SEND SIZES OF VARIABLE OF ALL ELEMENTS

                unsigned int send_buffer_size = local_elements_size;
                unsigned int receive_buffer_size = ghost_elements_size;

                int* send_buffer_1 = new int[send_buffer_size];
                int* receive_buffer_1 = new int[receive_buffer_size];

                int dummy_int_variable = 0;
                MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(dummy_int_variable);

                unsigned int size_of_variable = 0;

                // Filling the send buffer of sizes
                std::vector<int> sent_vector_of_sizes(send_buffer_size, 0);
                unsigned int position = 0;
                for (ModelPart::ElementIterator i_element = r_local_elements.begin(); i_element != r_local_elements.end(); ++i_element) {
                    vector<TDataType>& variable = i_element->GetValue(ThisVariable);
                    size_of_variable = variable.size();
                    sent_vector_of_sizes[position] = size_of_variable;
                    send_buffer_1[position] = size_of_variable;
                    position ++;
                }

                MPI_Status status1;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer_1, send_buffer_size, ThisMPI_Datatype, destination, send_tag, receive_buffer_1, receive_buffer_size, ThisMPI_Datatype, destination, receive_tag,
                             MPI_COMM_WORLD, &status1);

                position = 0;
                std::vector<int> received_vector_of_sizes(receive_buffer_size, 0);

                for (ModelPart::ElementIterator i_element = r_ghost_elements.begin(); i_element != r_ghost_elements.end(); ++i_element) {
                    received_vector_of_sizes[position] = receive_buffer_1[position];
                    position ++;
                }

                if (position > receive_buffer_size)
                    std::cout << rank << " First step Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer_1;
                delete [] receive_buffer_1;

                ////////////////////////////////////////
                ////////////STEP 2 : SEND ACTUAL VECTORS
                ////////////////////////////////////////
                int size_of_each_component_of_the_vector = sizeof (TDataType) / sizeof (TSendType);

                int sent_total_size = 0;
                for (int i=0; i< (int) sent_vector_of_sizes.size(); i++) {
                    sent_total_size += sent_vector_of_sizes[i];
                }
                sent_total_size *= size_of_each_component_of_the_vector;

                unsigned int received_total_size = 0;
                for (int i=0; i< (int) received_vector_of_sizes.size(); i++) {
                    received_total_size += received_vector_of_sizes[i];
                }
                received_total_size *= size_of_each_component_of_the_vector;

                TSendType* send_buffer_2 = new TSendType[sent_total_size];
                TSendType* receive_buffer_2 = new TSendType[received_total_size];

                // Filling the send buffer
                int i=0;
                position = 0;
                for (ModelPart::ElementIterator i_element = r_local_elements.begin(); i_element != r_local_elements.end(); ++i_element) {

                    int size_of_this_one = sent_vector_of_sizes[i];
                    vector<TDataType>& variable_to_add = i_element->GetValue(ThisVariable);

                    for (int j=0; j<size_of_this_one; j++){
                        *(TDataType*) (send_buffer_2 + position + size_of_each_component_of_the_vector*j) = variable_to_add[j];
                    }

                    position += size_of_each_component_of_the_vector * sent_vector_of_sizes[i];
                    i++;
                }

                TSendType Value = TSendType();
                ThisMPI_Datatype = GetMPIDatatype(Value);

                MPI_Status status2;
                send_tag = i_color;
                receive_tag = i_color;
                MPI_Sendrecv(send_buffer_2, sent_total_size, ThisMPI_Datatype, destination, send_tag, receive_buffer_2, received_total_size, ThisMPI_Datatype, destination, receive_tag, MPI_COMM_WORLD, &status2);

                position = 0, i=0;
                for (ModelPart::ElementIterator i_element = r_ghost_elements.begin(); i_element != r_ghost_elements.end(); ++i_element) {
                    int size_of_this_vector = received_vector_of_sizes[i];
                    vector<TDataType>& variable_to_fill = i_element->GetValue(ThisVariable);
                    variable_to_fill.resize(size_of_this_vector);
                    for (int j=0; j<size_of_this_vector; j++){
                        variable_to_fill[j] = *reinterpret_cast<TDataType*> (receive_buffer_2 + position + size_of_each_component_of_the_vector*j);
                    }
                    position += size_of_each_component_of_the_vector * size_of_this_vector;
                    i++;
                }

                if (position > received_total_size)
                    std::cout << rank << " Second Step Error in estimating receive buffer size...." << std::endl;

                delete [] send_buffer_2;
                delete [] receive_buffer_2;
            }
        }
        return true;
    }

    bool SynchronizeElementalFlags()
    {
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        int destination = 0;

        NeighbourIndicesContainerType& neighbours_indices = NeighbourIndices();

        int size_of_flags = sizeof (Flags) / sizeof (double);

        for (unsigned int i_color = 0; i_color < neighbours_indices.size(); i_color++) {
            if ((destination = neighbours_indices[i_color]) >= 0) {

                ElementsContainerType& r_local_elements = LocalMesh(i_color).Elements();
                ElementsContainerType& r_ghost_elements = GhostMesh(i_color).Elements();
                unsigned int local_elements_size = r_local_elements.size();
                unsigned int ghost_elements_size = r_ghost_elements.size();

                if ((local_elements_size == 0) && (ghost_elements_size == 0))
                    continue; // nothing to transfer!

                unsigned int send_buffer_size = size_of_flags * local_elements_size;
                unsigned int receive_buffer_size = size_of_flags * ghost_elements_size;

                double* send_buffer = new double[send_buffer_size];
                double* receive_buffer = new double[receive_buffer_size];

                // Filling the send buffer
                unsigned int position = 0;
                for (ModelPart::ElementIterator i_element = r_local_elements.begin(); i_element != r_local_elements.end(); ++i_element) {
                    Flags aux_flags = *i_element;
                    *(Flags*) (send_buffer + position) = aux_flags;
                    position += size_of_flags;
                }

                double dummy_double_value = 0.0;
                MPI_Datatype ThisMPI_Datatype = GetMPIDatatype(dummy_double_value);

                MPI_Status status1;

                int send_tag = i_color;
                int receive_tag = i_color;

                MPI_Sendrecv(send_buffer, send_buffer_size, ThisMPI_Datatype, destination, send_tag, receive_buffer, receive_buffer_size, ThisMPI_Datatype, destination, receive_tag, MPI_COMM_WORLD, &status1);

                position = 0;

                for (ModelPart::ElementIterator i_element = r_ghost_elements.begin(); i_element != r_ghost_elements.end(); ++i_element) {
                    Flags aux_flags = *reinterpret_cast<Flags*> (receive_buffer + position);
                    i_element->AssignFlags(aux_flags);
                    position += size_of_flags;

                }

                if (position > receive_buffer_size) { std::cout << rank << " Error in estimating receive buffer size when synchronizing Element Flags..." << std::endl; }

                delete [] send_buffer;
                delete [] receive_buffer;
            }
        }
        return true;

    }

    void CalculateSendRecvMessageSize(int * msgSendSize, int * msgRecvSize) {
        mpi_erno = MPI_Alltoall(msgSendSize,1,MPI_INT,msgRecvSize,1,MPI_INT,MPI_COMM_WORLD);
        MpiUtils::HandleError(mpi_erno);
    }

    template<class TObjectType>
    void SerializePayload(Kratos::Serializer& payload_serializer, const TObjectType& payload, char ** serialized_payload, int& serialized_payload_size) {
        payload_serializer.save("VariableList",mpVariables_list);
        payload_serializer.save("MpiPayload",payload);

        std::stringstream * stream = (std::stringstream *)payload_serializer.pGetBuffer();
        const std::string & stream_str = stream->str();
        serialized_payload_size = stream_str.size()+1;
        *serialized_payload = new char[serialized_payload_size];
        memcpy(*serialized_payload, stream_str.c_str(), serialized_payload_size);
        std::cout << serialized_payload << std::endl;
    }

    template<class TObjectType>
    void DeserializePayload(Kratos::Serializer& payload_deserializer, TObjectType& payload, char ** serialized_payload, int& serialized_payload_size) {
        std::stringstream * payload_deserializer_buffer;
        VariablesList * tmp_mpVariables_list = NULL;

        payload_deserializer_buffer = (std::stringstream *)payload_deserializer.pGetBuffer();
        payload_deserializer_buffer->write(*serialized_payload, serialized_payload_size);
        payload_deserializer.load("VariableList",tmp_mpVariables_list);

        if(tmp_mpVariables_list != NULL)
            delete tmp_mpVariables_list;
        tmp_mpVariables_list = mpVariables_list;

        payload_deserializer.load("MpiPayload",payload);
    }

    template<class TObjectType>
    bool AsyncSendAndReceiveObjects(std::vector<TObjectType>& SendObjects, std::vector<TObjectType>& RecvObjects, Kratos::Serializer& externParticleSerializer)
    {
        int * msgSendSize = new int[mpi_size];
        int * msgRecvSize = new int[mpi_size];

        char ** mpi_send_buffer = new char * [mpi_size];
        char ** mpi_recv_buffer = new char * [mpi_size];

        for(int i = 0; i < mpi_size; i++)
        {
            msgSendSize[i] = 0;
            msgRecvSize[i] = 0;
        }

        for(int i = 0; i < mpi_size; i++)
        {
            if(mpi_rank != i)
            {
                SerializePayload(externParticleSerializer, SendObjects[i], &mpi_send_buffer[i], msgSendSize[i]);
            }
        }

        CalculateSendRecvMessageSize(msgSendSize, msgRecvSize);

        int NumberOfCommunicationEvents      = 0;
        int NumberOfCommunicationEventsIndex = 0;

        for(int j = 0; j < mpi_size; j++)
        {
            if(j != mpi_rank && msgRecvSize[j]) NumberOfCommunicationEvents++;
            if(j != mpi_rank && msgSendSize[j]) NumberOfCommunicationEvents++;
        }

        MPI_Request * reqs = new MPI_Request[NumberOfCommunicationEvents];
        MPI_Status * stats = new MPI_Status[NumberOfCommunicationEvents];

        //Set up all receive and send events
        for(int i = 0; i < mpi_size; i++)
        {
            if(i != mpi_rank && msgRecvSize[i])
            {
                mpi_recv_buffer[i] = (char *)malloc(sizeof(char) * msgRecvSize[i]);

                mpi_erno = MPI_Irecv(mpi_recv_buffer[i],msgRecvSize[i],MPI_CHAR,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }

            if(i != mpi_rank && msgSendSize[i])
            {
                mpi_erno = MPI_Isend(mpi_send_buffer[i],msgSendSize[i],MPI_CHAR,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }
        }

        //wait untill all communications finish
        mpi_erno = MPI_Waitall(NumberOfCommunicationEvents, reqs, stats);
        MpiUtils::HandleError(mpi_erno);

        for(int i = 0; i < mpi_size; i++)
        {
            if (i != mpi_rank && msgRecvSize[i])
            {
                DeserializePayload(externParticleSerializer, RecvObjects[i], &message[i], msgRecvSize[i]);
            }
        }

        // Free buffers
        for(int i = 0; i < mpi_size; i++)
        {
            if(msgRecvSize[i])
                free(mpi_recv_buffer[i]);

            if(msgSendSize[i])
                free(mpi_send_buffer[i]);
        }

        delete [] reqs;
        delete [] stats;

        delete [] mpi_recv_buffer;
        delete [] mpi_send_buffer;

        delete [] msgSendSize;
        delete [] msgRecvSize;

        return true;
    }

    template<class TObjectType>
    bool AsyncSendAndReceiveObjects(std::vector<TObjectType>& SendObjects, std::vector<TObjectType>& RecvObjects, std::vector<Kratos::Serializer>& SendSerializer, std::vector<Kratos::Serializer>& RecvSerializer)
    {
        int * msgSendSize = new int[mpi_size];
        int * msgRecvSize = new int[mpi_size];

        std::vector<char *> mpi_send_buffer(mpi_size);
        std::vector<char *> mpi_recv_buffer(mpi_size);

        for(int i = 0; i < mpi_size; i++)
        {
            if(mpi_rank != i)
            {
                SerializePayload(SendSerializer[i], SendObjects[i], &mpi_send_buffer[i], msgSendSize[i]);
            } else {
                msgSendSize[i] = 0;
            }

            msgRecvSize[i] = 0;
        }

        CalculateSendRecvMessageSize(msgSendSize, msgRecvSize);

        int NumberOfCommunicationEvents      = 0;
        int NumberOfCommunicationEventsIndex = 0;

        for(int i = 0; i < mpi_size; i++)
        {
            if(i != mpi_rank && msgRecvSize[i]) NumberOfCommunicationEvents++;
            if(i != mpi_rank && msgSendSize[i]) NumberOfCommunicationEvents++;
        }

        MPI_Request * reqs = new MPI_Request[NumberOfCommunicationEvents];
        MPI_Status * stats = new MPI_Status[NumberOfCommunicationEvents];

        std::cout << "LALAL" << std::endl;

        for(int i = 0; i < mpi_size; i++)
        {
            if(i != mpi_rank && msgSendSize[i])
            {
                mpi_erno = MPI_Isend(mpi_send_buffer[i],msgSendSize[i],MPI_CHAR,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }

            if(i != mpi_rank && msgRecvSize[i])
            {
                mpi_recv_buffer[i] = new char[msgRecvSize[i]];

                mpi_erno = MPI_Irecv(mpi_recv_buffer[i],msgRecvSize[i],MPI_CHAR,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }
        }

        std::cout << "LOLOL" << std::endl;

        // Wait untill all communications finish
        mpi_erno = MPI_Waitall(NumberOfCommunicationEvents, reqs, stats);
        MpiUtils::HandleError(mpi_erno);

        for(int i = 0; i < mpi_size; i++)
        {
            if (i != mpi_rank && msgRecvSize[i])
            {
                DeserializePayload(RecvSerializer[i], RecvObjects[i], &mpi_recv_buffer[i], msgRecvSize[i]);
            }
        }

        // Free buffers
        for(int i = 0; i < mpi_size; i++)
        {
            if(msgRecvSize[i])
                delete[] mpi_recv_buffer[i];

            if(msgSendSize[i])
                delete[] mpi_send_buffer[i];
        }

        delete[] reqs;
        delete[] stats;

        delete[] msgSendSize;
        delete[] msgRecvSize;

        return true;
    }

    /** Request the list of objects that the destination process needs
     * Request the list of objects that the destination process needs. The sender sends the id's of the objects to be transfered
     * and the recv will answer with the id's of the objects it already owns and its ????
     * @param Petition list of objects to be sent to each process
     * @param mpi_send_size array to store the size of the objects to be sent to each process.
     * @param mpi_send_size array to store the size of the objects to be received from each process.
     * @param mpi_send_buffer array to store the arrays of the id of the objects to be sent to each process. This function allocates this buffer.
     * @param mpi_recv_buffer array to store the arrays of the id of the objects to be received from each process. This function allocates this buffer.
     **/
    template<class TObjectType>
    void AsyncTransferPetition(
      std::vector<TObjectType>& Petition,
      int * mpi_send_size, int * mpi_recv_size,
      long ** mpi_send_buffer, long ** mpi_recv_buffer) {

        int NumberOfCommunicationEvents      = 0;
        int NumberOfCommunicationEventsIndex = 0;

        for(int i = 0; i < mpi_size; i++)
        {
            mpi_send_size[i] = 0;
            mpi_recv_size[i] = 0;
        }

        // Send number of objects to transfer
        for(int i = 0; i < mpi_size; i++)
        {
            if(i != mpi_rank) 
            {
                mpi_send_size[i] = Petition[i].size();
            }
        }

        CalculateSendRecvMessageSize(mpi_send_size, mpi_recv_size);

        // Prepare the payload, recv buffer and communication events
        for(int i = 0; i < mpi_size; i++) 
        {
            if(i != mpi_rank) 
            {
                if(mpi_send_size[i])
                {
                    mpi_send_buffer[i] = new long[mpi_send_size[i] * 2];
                    NumberOfCommunicationEvents++;
                }
                if(mpi_recv_size[i])
                {
                    mpi_recv_buffer[i] = new long[mpi_recv_size[i] * 2];
                    NumberOfCommunicationEvents++;
                }
                if(mpi_rank == 0) std::cout << Petition[i].size() << std::endl;

                std::size_t j = 0;
                for(auto& object : Petition[i])
                {
                   mpi_send_buffer[i][j++] = object.Id();
                   mpi_send_buffer[i][j++] = (long)&object;
                }
            }
        }

        MPI_Request * reqs = new MPI_Request[NumberOfCommunicationEvents];
        MPI_Status * stats = new MPI_Status[NumberOfCommunicationEvents];

        // Communicate the data
        for(int i = 0; i < mpi_size; i++)
        {
            if(i != mpi_rank && mpi_send_size[i])
            {
                mpi_erno = MPI_Isend(mpi_send_buffer[i],mpi_send_size[i]*2,MPI_LONG,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }

            if(i != mpi_rank && mpi_recv_size[i])
            {
                mpi_erno = MPI_Irecv(mpi_recv_buffer[i],mpi_recv_size[i]*2,MPI_LONG,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }
        }

        // Wait untill all communications have finished
        mpi_erno = MPI_Waitall(NumberOfCommunicationEvents, reqs, stats);
        MpiUtils::HandleError(mpi_erno);
    }

    /** Request the list of objects that the destination process needs
     * Request the list of objects that the destination process needs. The sender sends the id's of the objects to be transfered
     * and the recv will answer with the id's of the objects it already owns and its ????
     * @param Petition list of objects to be sent to each process
     * @param mpi_send_size array to store the size of the objects to be sent to each process.
     * @param mpi_send_size array to store the size of the objects to be received from each process.
     * @param mpi_send_buffer array to store the arrays of the id of the objects to be sent to each process. This function allocates this buffer.
     * @param mpi_recv_buffer array to store the arrays of the id of the objects to be received from each process. This function allocates this buffer.
     **/
    void AsyncOwnedPointers(
      int * mpi_send_size, int * mpi_recv_size,
      long ** mpi_send_buffer, long ** mpi_recv_buffer) {

        int NumberOfCommunicationEvents      = 0;
        int NumberOfCommunicationEventsIndex = 0;

        CalculateSendRecvMessageSize(mpi_send_size, mpi_recv_size);

        // Prepare the payload, recv buffer and communication events
        for(int i = 0; i < mpi_size; i++)
        {
            if(i != mpi_rank && mpi_send_size[i]) NumberOfCommunicationEvents++;
            if(i != mpi_rank && mpi_recv_size[i]) NumberOfCommunicationEvents++;
        }

        MPI_Request * reqs = new MPI_Request[NumberOfCommunicationEvents];
        MPI_Status * stats = new MPI_Status[NumberOfCommunicationEvents];

        // Communicate the data
        for(int i = 0; i < mpi_size; i++) {
            if(i != mpi_rank && mpi_send_size[i]) {
                mpi_erno = MPI_Isend(mpi_send_buffer[i],mpi_send_size[i]*2,MPI_LONG,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }

            if(i != mpi_rank && mpi_recv_size[i]) {
                mpi_erno = MPI_Irecv(mpi_recv_buffer[i],mpi_recv_size[i]*2,MPI_LONG,i,0,MPI_COMM_WORLD,&reqs[NumberOfCommunicationEventsIndex++]);
                MpiUtils::HandleError(mpi_erno);
            }
        }

        // Wait untill all communications have finished
        mpi_erno = MPI_Waitall(NumberOfCommunicationEvents, reqs, stats) ;
        MpiUtils::HandleError(mpi_erno);
    }

    /** Clears the buffers 
     * **/
    void ClearSendRecvBuffers(int * mpi_send_size, int * mpi_recv_size, long ** mpi_send_buffer, long ** mpi_recv_buffer) {
        for(int i = 0; i < mpi_size; i++) {
            if(i != mpi_rank && mpi_send_size[i]) {
                delete[] mpi_send_buffer[i];
            }

            if(i != mpi_rank && mpi_recv_size[i]) {
                delete[] mpi_recv_buffer[i];
            }
        }
    }

    template<class TMeshType>
    bool AsyncSendAndReceiveMeshes(std::vector<TMeshType>& SendMeshes, std::vector<TMeshType>& RecvMeshes)
    {
        int * mpi_send_size = new int[mpi_size];
        int * mpi_recv_size = new int[mpi_size];

        long ** mpi_send_buffer = new long * [mpi_size];
        long ** mpi_recv_buffer = new long * [mpi_size];

        std::vector<MeshType::NodesContainerType> send_mesh_nodes(mpi_size);
        std::vector<MeshType::ElementsContainerType> send_mesh_elements(mpi_size);
        std::vector<MeshType::ConditionsContainerType> send_mesh_conditions(mpi_size);

        std::vector<MeshType::NodesContainerType> recv_mesh_nodes(mpi_size);
        std::vector<MeshType::ElementsContainerType> recv_mesh_elements(mpi_size);
        std::vector<MeshType::ConditionsContainerType> recv_mesh_conditions(mpi_size);

        // Extract the objects of the mesh that may have duplicates in the destination
        for(int i = 0; i < mpi_size; i++) {
            if(i != mpi_rank) {
                send_mesh_nodes[i] = SendMeshes[i]->Nodes();
                send_mesh_elements[i] = SendMeshes[i]->Elements();
                send_mesh_conditions[i] = SendMeshes[i]->Conditions();

                recv_mesh_nodes[i] = RecvMeshes[i]->Nodes();
                recv_mesh_elements[i] = RecvMeshes[i]->Elements();
                recv_mesh_conditions[i] = RecvMeshes[i]->Conditions();
            }
        }

        // We build an array of id's for the entities we are going to transfer (nodes, elements, conditions, ...)
        // and we send them to the target process. The target process looks for the already existing entities and
        // returns a list of the entities he wants
        std::vector<Kratos::Serializer> send_serializers(mpi_size);
        std::vector<Kratos::Serializer> recv_serializers(mpi_size);
        
        // For Nodes
        AsyncTransferPetition(send_mesh_nodes, mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);

        for(int i = 0; i < mpi_size; i++) {
            if(i != mpi_rank) {
                for(int j = 0; j < mpi_recv_size[i]; j++) {
                    if(LocalMesh().HasNode(mpi_recv_buffer[i][j*2+0])) {
                        void * dst_node_addr = &LocalMesh().Nodes()(mpi_recv_buffer[i][j*2+0]);
                        void * src_node_addr = (void *)mpi_recv_buffer[i][j*2+1];
                        recv_serializers[i].RedirectLoadingPointer(src_node_addr, dst_node_addr);
                        std::cout << mpi_rank << " - Addr " << src_node_addr << " points to " << dst_node_addr << std::endl;
                        mpi_send_buffer[i][j*2+0] = mpi_recv_buffer[i][j*2+0];
                    } else {
                        mpi_send_buffer[i][j*2+0] = -1;
                    }
                }
                mpi_send_size[i] = mpi_recv_size[i];
            }
        }

        AsyncOwnedPointers(mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);

        if(mpi_rank == 0) {
            for(int i = 0; i < mpi_recv_size[1] * 2; i++)
                std::cout << mpi_recv_buffer[1][i] << std::endl;
        }

        for(int i = 0; i < mpi_size; i++) {
            if(i != mpi_rank) {
                for(int j = 0; j < mpi_recv_size[i]; j++) {
                    if(mpi_recv_buffer[i][j*2] != -1) {
                        void * src_node_addr = LocalMesh().pGetNode(mpi_recv_buffer[i][j*2]).get();
                        std::cout << mpi_rank << " - " << src_node_addr << " " << *((MeshType::NodeType *)src_node_addr) << " - " << LocalMesh().pGetNode(mpi_recv_buffer[i][j*2]) << std::endl;
                        send_serializers[i].AddToSavedPointers(src_node_addr);
                        std::cout << mpi_rank << " - Addr " << src_node_addr << " marked as saved " << std::endl;
                    }
                }
            }
        }

        ClearSendRecvBuffers(mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);

        // Transfer the Objects
        // MockupTransfer(send_mesh_nodes, recv_mesh_nodes, send_serializers, recv_serializers);

        std::cout << "Before " << std::endl;

        AsyncSendAndReceiveObjects(send_mesh_nodes, recv_mesh_nodes, send_serializers, recv_serializers);

        std::cout << "After " << std::endl;

        if(mpi_rank == 0) {
            std::cout << "RECV" << std::endl;
            std::cout << recv_mesh_nodes[1].size() << std::endl;
            KRATOS_WATCH(recv_mesh_nodes[1]);
        }

        // Extract the objects of the mesh that may have duplicates in the destination
        for(int i = 0; i < mpi_size; i++) {
            if(i != mpi_rank) {
                RecvMeshes[i]->Nodes() = recv_mesh_nodes[i];
            }
        }

        // // For Elements
        // AsyncSendTransferPetition(mesh_elements, mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);


        // ClearSendRecvBuffers(mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);

        // // For Conditions
        // AsyncSendTransferPetition(mesh_conditions, mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);


        // ClearSendRecvBuffers(mpi_send_size, mpi_recv_size, mpi_send_buffer, mpi_recv_buffer);


        delete[] mpi_send_buffer;
        delete[] mpi_recv_buffer;

        return true;
    }

    inline MPI_Datatype GetMPIDatatype(const int& Value)
    {
        return MPI_INT;
    }


    inline MPI_Datatype GetMPIDatatype(const double& Value)
    {
        return MPI_DOUBLE;
    }


    //       friend class boost::serialization::access;

    //       template<class TArchive>
    // 	  void serialize(TArchive & ThisArchive, const unsigned int ThisVersion)
    // 	  {
    // /* 	      ThisArchive & mName & mBufferSize & mCurrentIndex; */
    // 	  }

    //       void RemoveSolutionStepData(IndexType SolutionStepIndex, MeshType& ThisMesh)
    // 	{
    // 	  for(NodeIterator i_node = ThisMesh.NodesBegin() ; i_node != ThisMesh.NodesEnd() ; ++i_node)
    // 	    i_node->RemoveSolutionStepNodalData(SolutionStepIndex);
    // 	}

    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{


    ///@}

}; // Class MPICommunicator

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
inline std::istream & operator >>(std::istream& rIStream,
                                  MPICommunicator& rThis);

/// output stream function

inline std::ostream & operator <<(std::ostream& rOStream,
                                  const MPICommunicator& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}

} // namespace Kratos.

#endif // KRATOS_MPI_COMMUNICATOR_H_INCLUDED  defined
