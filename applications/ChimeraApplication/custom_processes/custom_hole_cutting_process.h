// ==============================================================================
//  ChimeraApplication
//
//  License:         BSD License
//                   license: ChimeraApplication/license.txt
//
//  Main authors:    Aditya Ghantasala, https://github.com/adityaghantasala
//
// ==============================================================================
//

#if !defined(KRATOS_CUSTOM_HOLE_CUTTING_PROCESS_H_INCLUDED )
#define  KRATOS_CUSTOM_HOLE_CUTTING_PROCESS_H_INCLUDED


// System includes
// Please put system includes in custom_hole_cutting_process.h

// External includes
// Please put external includes in custom_hole_cutting_process.h

// Project includes

#include "custom_processes/custom_hole_cutting_process.h"

// System includes
#include <iostream>
#include <string>
#include <algorithm>
#include "math.h"

// External includes
#include <boost/python.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
// Project includes
#include "includes/define.h"
#include "includes/element.h"
#include "includes/node.h"
#include "includes/model_part.h"
#include "includes/process_info.h"
// Application includes
#include "processes/process.h"
#include "processes/find_nodal_neighbours_process.h" // To find node neighbours using elements
#include "processes/find_conditions_neighbours_process.h" // To find node neighbours using conditions
#include "processes/node_erase_process.h" // To delete empty nodes
#include "utilities/normal_calculation_utils.h" // To calculate element's normal
#include "geometries/triangle_3d_3.h" // Skin face geometry template
#include "geometries/line_2d_2.h"


namespace Kratos {

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

class CustomHoleCuttingProcess{
public:
	// Needed structures for the ExtractSurfaceMesh operation
	struct KeyComparor
	{
		bool operator()(const vector<unsigned int>& lhs, const vector<unsigned int>& rhs) const
		{
			if(lhs.size() != rhs.size())
				return false;

			for(unsigned int i=0; i<lhs.size(); i++)
			{
				if(lhs[i] != rhs[i]) return false;
			}

			return true;
		}
	};

	struct KeyHasher
	{
		std::size_t operator()(const vector<int>& k) const
		{
			return boost::hash_range(k.begin(), k.end());
		}
	};


	///@name Type Definitions
	///@{

	///@}
	///@name Pointer Definitions
	/// Pointer definition of CustomHoleCuttingProcess
	KRATOS_CLASS_POINTER_DEFINITION(CustomHoleCuttingProcess);

	///@}
	///@name Life Cycle
	///@{

	CustomHoleCuttingProcess() {

		
	}

	/// Destructor.
	virtual ~CustomHoleCuttingProcess() {
	}

	///@}
	///@name Operators
	///@{

	void operator()() {
		Execute();
	}

	///@}
	///@name Operations
	///@{

	/// For CHIMERA boundary condition purposes: Extracts a volume mesh with a certain threshold value based on centroid distance
	void ExtractMeshAtCentroidDistance(ModelPart& rModelPart, ModelPart& rExtractedModelPart, double distance)
	{

		KRATOS_TRY;

		std::cout<<"\n::[Mesh Extraction]::"<<std::endl;
		ModelPart rExtractedModelPart;

		// Initializing mesh nodes
		//rExtractedModelPart.Nodes() = rModelPart.Nodes();

		// Extracting mesh elements which are only above the threshold value
		std::cout<<"  Extracting elements with in a distance of > " << fabs(distance) <<std::endl;
		Element::Pointer pElem;
		std::vector<unsigned int> vector_of_node_ids;
		for(ModelPart::ElementsContainerType::iterator it = rModelPart.ElementsBegin(); it != rModelPart.ElementsEnd(); ++it)
		{
			double elementDistance = 0.0;
			unsigned int j = 0;
			for (j = 0 ; j < it->GetGeometry().PointsNumber(); j++){
				elementDistance += it->GetGeometry()[j].FastGetSolutionStepValue(DISTANCE);
			}
			// Element distance is the average of the nodal distances.
			elementDistance = (elementDistance/j);

			if(elementDistance < distance)
			{
				//Adding elements in the ModelPart
				pElem = Element::Pointer(new Element(*it));
				rExtractedModelPart.Elements().push_back(pElem);
				
				//Adding node all the node Ids of the elements satisfying the condition
				for(j =0; j < pElem->GetGeometry().PointsNumber(); j++)
				vector_of_node_ids.push_back(pElem->GetGeometry()[j].Id());



			}
		}

						
		//sorting and making unique list of node ids

		std::set<unsigned int> s( vector_of_node_ids.begin(), vector_of_node_ids.end() );
		vector_of_node_ids.assign( s.begin(), s.end() );  

		// Add unique nodes in the ModelPart

		for (auto it = vector_of_node_ids.begin(); it != vector_of_node_ids.end(); it++)
		{

			Node < 3 >::Pointer pnode = rModelPart.Nodes()(*it);
			rExtractedModelPart.AddNode(pnode);

		}
		
		

		std::cout<<"  Successful extraction of the Mesh !! "<<rExtractedModelPart.GetMesh()<<"\b"<<std::endl;
		//ExtractSurfaceMesh(rExtractedModelPart, rExtractedSurfaceModelPart);

		KRATOS_CATCH("");

	}



	/// For CHIMERA boundary condition purposes: Extracts a  mesh with a certain threshold value
	void ExtractMeshBetweenLimits(ModelPart& rModelPart,ModelPart& rExtractedModelPart, double lLimit, double uLimit)
	{

		KRATOS_TRY;

		std::cout<<"\n::[Mesh Extraction]::"<<std::endl;

		// Initializing mesh nodes
		//rExtractedModelPart.Nodes() = rModelPart.Nodes();

		// Extracting mesh elements which are only above the threshold value
		std::cout<<"  Extracting elements between " << lLimit <<" and "<<uLimit<<std::endl;
		Element::Pointer pElem;
		std::vector<unsigned int> vector_of_node_ids;
		for(ModelPart::ElementsContainerType::iterator it = rModelPart.ElementsBegin(); it != rModelPart.ElementsEnd(); ++it)
		{
			double elementDistance = 0.0;
			int numPointsInside = 0;
			unsigned int j = 0;
			for (j = 0 ; j < it->GetGeometry().PointsNumber(); j++){
				elementDistance = it->GetGeometry()[j].FastGetSolutionStepValue(DISTANCE);
				if(elementDistance > lLimit && elementDistance < uLimit )
				{
				  numPointsInside++;
				}
			}

			if(numPointsInside > 0)
			{
				pElem = Element::Pointer(new Element(*it));
				rExtractedModelPart.Elements().push_back(pElem);

				//Adding node all the node Ids of the elements satisfying the condition
				for(j =0; j < pElem->GetGeometry().PointsNumber(); j++)
				vector_of_node_ids.push_back(pElem->GetGeometry()[j].Id());
			}
		}

	    //sorting and making unique list of node ids

		std::set<unsigned int> s( vector_of_node_ids.begin(), vector_of_node_ids.end() );
		vector_of_node_ids.assign( s.begin(), s.end() );  

		// Add unique nodes in the ModelPart

		for (auto it = vector_of_node_ids.begin(); it != vector_of_node_ids.end(); it++)
		{

			Node < 3 >::Pointer pnode = rModelPart.Nodes()(*it);
			rExtractedModelPart.AddNode(pnode);

		}

		std::cout<<" ########  Successful extraction of the Mesh !! "<<rExtractedModelPart.GetMesh()<<"\b"<<std::endl;
		KRATOS_CATCH("");
	}

    

	void CreateHoleAfterDistance(ModelPart& rModelPart, ModelPart& rExtractedModelPart, ModelPart& rExtractedBoundaryModelPart, double distance)

	{

		KRATOS_TRY;

		std::cout<<"\n::[Creating Hole]::"<<std::endl;

		//Initialising mesh nodes
		//rExtractedModelPart.Nodes() = rModelPart.Nodes();

		

		// Extracting mesh elements which are only above the threshold value
		std::cout<<"  Extracting elements after " << distance << std::endl;
		Element::Pointer pElem;
		std::vector<unsigned int> vector_of_node_ids;

		//For signed distance
		
		distance *= -1;
		std::cout<<"Updated distance"<<distance<<std::endl;
		 

		for(ModelPart::ElementsContainerType::iterator it = rModelPart.ElementsBegin(); it != rModelPart.ElementsEnd(); ++it)
		{
			double elementDistance = 0.0;
			int numPointsOutside = 0;
			unsigned int j = 0;
			Geometry<Node<3> >& geom = it->GetGeometry();
			for (j = 0 ; j < geom.size(); j++){
				elementDistance = it->GetGeometry()[j].FastGetSolutionStepValue(DISTANCE);
				if(elementDistance < distance )

				{
				  numPointsOutside++;
				}
			}
			
			if(numPointsOutside == geom.size())
			//if(numPointsOutside > 0)
			{
				
				it->Set(ACTIVE,false);
				/*std::cout<<"Elements Nr "<<it->Id()<<"inactive"<<std::endl;*/
				pElem = Element::Pointer(new Element(*it));
				rExtractedModelPart.Elements().push_back(pElem);
				//Adding node all the node Ids of the elements satisfying the condition
				for(j =0; j < pElem->GetGeometry().PointsNumber(); j++)
				vector_of_node_ids.push_back(pElem->GetGeometry()[j].Id());
				
			}

			/*if(!(it->IsDefined(ACTIVE)))
				std::cout<<"Elements Nr "<<it->Id()<<"active"<<std::endl;*/
		}

	    //sorting and making unique list of node ids

		std::set<unsigned int> s( vector_of_node_ids.begin(), vector_of_node_ids.end() );
		vector_of_node_ids.assign( s.begin(), s.end() );  

		// Add unique nodes in the ModelPart

		for (auto it = vector_of_node_ids.begin(); it != vector_of_node_ids.end(); it++)
		{

			Node < 3 >::Pointer pnode = rModelPart.Nodes()(*it);
			rExtractedModelPart.AddNode(pnode);

		}

		std::cout<<" ########  Successful hole cutting of the Mesh !!########## "<<std::endl;
		KRATOS_CATCH("");
       unsigned int n_nodes = rModelPart.ElementsBegin()->GetGeometry().size();
	  

	   if(n_nodes == 3){
		 
		 ExtractBoundaryMesh(rExtractedModelPart,rExtractedBoundaryModelPart);
		   
	   }

	   else if (n_nodes == 4){
		 ExtractSurfaceMesh(rExtractedModelPart,rExtractedBoundaryModelPart);

	   }

	   else std::cout<<"Hole cutting process is only supported for tetrahedral and triangular elements"<<std::endl;



	}

	

	/// For Topology Optimization purposes: Extracts a surface mesh from a provided volume mesh
	void ExtractSurfaceMesh(ModelPart& rExtractedVolumeModelPart, ModelPart& rExtractedSurfaceModelPart)
	{
		KRATOS_TRY;

		std::cout<<"::[Surface Mesh Extraction]::"<<std::endl;

		// Some type-definitions
		typedef boost::unordered_map<vector<unsigned int>, unsigned int, KeyHasher, KeyComparor > hashmap;
		typedef boost::unordered_map<vector<unsigned int>, vector<unsigned int>, KeyHasher, KeyComparor > hashmap_vec;

		// Create map to ask for number of faces for the given set of node ids representing on face in the model part
		hashmap n_faces_map;

		// Fill map that counts number of faces for given set of nodes
		for (ModelPart::ElementIterator itElem = rExtractedVolumeModelPart.ElementsBegin(); itElem != rExtractedVolumeModelPart.ElementsEnd(); itElem++)
		{
			Element::GeometryType::GeometriesArrayType faces = itElem->GetGeometry().Faces();

			for(unsigned int face=0; face<faces.size(); face++)
			{
				// Create vector that stores all node is of current face
				vector<unsigned int> ids(faces[face].size());

				// Store node ids
				for(unsigned int i=0; i<faces[face].size(); i++)
					ids[i] = faces[face][i].Id();

				//*** THE ARRAY OF IDS MUST BE ORDERED!!! ***
				std::sort(ids.begin(), ids.end());

				// Fill the map
				n_faces_map[ids] += 1;
			}
		}

		// Create a map to get nodes of skin face in original order for given set of node ids representing that face
		// The given set of node ids may have a different node order
		hashmap_vec ordered_skin_face_nodes_map;

		// Fill map that gives original node order for set of nodes
		for (ModelPart::ElementIterator itElem = rExtractedVolumeModelPart.ElementsBegin(); itElem != rExtractedVolumeModelPart.ElementsEnd(); itElem++)
		{
			Element::GeometryType::GeometriesArrayType faces = itElem->GetGeometry().Faces();

			for(unsigned int face=0; face<faces.size(); face++)
			{
				// Create vector that stores all node is of current face
				vector<unsigned int> ids(faces[face].size());
				vector<unsigned int> unsorted_ids(faces[face].size());

				// Store node ids
				for(unsigned int i=0; i<faces[face].size(); i++)
				{
					ids[i] = faces[face][i].Id();
					unsorted_ids[i] = faces[face][i].Id();
				}

				//*** THE ARRAY OF IDS MUST BE ORDERED!!! ***
				std::sort(ids.begin(), ids.end());

				if(n_faces_map[ids] == 1)
					ordered_skin_face_nodes_map[ids] = unsorted_ids;
			}
		}
		// First assign to skin model part all nodes from original model_part, unnecessary nodes will be removed later
		unsigned int id_condition = 1;
		//rExtractedSurfaceModelPart.Nodes() = rExtractedVolumeModelPart.Nodes();

		// Add skin faces as triangles to skin-model-part (loop over all node sets)
		std::cout<<"  Extracting surface mesh and computing normals" <<std::endl;
		std::vector<unsigned int> vector_of_node_ids;
		for(typename hashmap::const_iterator it=n_faces_map.begin(); it!=n_faces_map.end(); it++)
		{
			// If given node set represents face that is not overlapping with a face of another element, add it as skin element
			if(it->second == 1)
			{
				// If skin face is a triangle store triangle in with its original orientation in new skin model part
				if(it->first.size()==3)
				{
					// Getting original order is important to properly reproduce skin face including its normal orientation
					vector<unsigned int> original_nodes_order = ordered_skin_face_nodes_map[it->first];
					Node < 3 >::Pointer pnode1 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[0]);
					Node < 3 >::Pointer pnode2 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[1]);
					Node < 3 >::Pointer pnode3 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[2]);

					//Storing the node ids list
					vector_of_node_ids.push_back(original_nodes_order[0]);
					vector_of_node_ids.push_back(original_nodes_order[1]);
					vector_of_node_ids.push_back(original_nodes_order[2]);
					Properties::Pointer properties = rExtractedSurfaceModelPart.rProperties()(0);
					Condition const& rReferenceTriangleCondition = KratosComponents<Condition>::Get("Condition3D");

					// Skin faces are added as conditions
					Triangle3D3< Node<3> > triangle1(pnode1, pnode2, pnode3);
					Condition::Pointer p_condition1 = rReferenceTriangleCondition.Create(id_condition++, triangle1, properties);
					rExtractedSurfaceModelPart.Conditions().push_back(p_condition1);
				}
				// If skin face is a quadrilateral then divide in two triangles and store them with their original orientation in new skin model part
				if(it->first.size()==4)
				{
					// Getting original order is important to properly reproduce skin including its normal orientation
					vector<unsigned int> original_nodes_order = ordered_skin_face_nodes_map[it->first];

					Node < 3 >::Pointer pnode1 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[0]);
					Node < 3 >::Pointer pnode2 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[1]);
					Node < 3 >::Pointer pnode3 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[2]);
					Node < 3 >::Pointer pnode4 = rExtractedVolumeModelPart.Nodes()(original_nodes_order[3]);
					//Storing the node ids list
					vector_of_node_ids.push_back(original_nodes_order[0]);
					vector_of_node_ids.push_back(original_nodes_order[1]);
					vector_of_node_ids.push_back(original_nodes_order[2]);
					vector_of_node_ids.push_back(original_nodes_order[3]);
					Properties::Pointer properties = rExtractedSurfaceModelPart.rProperties()(0);
					Condition const& rReferenceTriangleCondition = KratosComponents<Condition>::Get("Condition3D");

					// Add triangle one as condition
					Triangle3D3< Node<3> > triangle1(pnode1, pnode2, pnode3);
					Condition::Pointer p_condition1 = rReferenceTriangleCondition.Create(id_condition++, triangle1, properties);
					rExtractedSurfaceModelPart.Conditions().push_back(p_condition1);

					// Add triangle two as condition
					Triangle3D3< Node<3> > triangle2(pnode1, pnode3, pnode4);
					Condition::Pointer p_condition2 = rReferenceTriangleCondition.Create(id_condition++, triangle2, properties);
					rExtractedSurfaceModelPart.Conditions().push_back(p_condition2);
				}
			}
		}

		//sorting and making unique list of node ids

		std::set<unsigned int> s( vector_of_node_ids.begin(), vector_of_node_ids.end() );
		vector_of_node_ids.assign( s.begin(), s.end() ); 

		for (auto it = vector_of_node_ids.begin(); it != vector_of_node_ids.end(); it++)
		
		{
			//Adding the nodes to the rExtractedSurfaceModelPart
			Node < 3 >::Pointer pnode = rExtractedVolumeModelPart.Nodes()(*it);
			rExtractedSurfaceModelPart.AddNode(pnode);
		

		}

		std::cout<<"Successful extraction of the Surface "<<rExtractedSurfaceModelPart.GetMesh()<<std::endl;

		KRATOS_CATCH("");
	}


void ExtractBoundaryMesh(ModelPart& rSurfaceModelPart, ModelPart& rExtractedBoundaryModelPart)
	{
		KRATOS_TRY;

		std::cout<<"::[Boundary Mesh Extraction]::"<<std::endl;

		// Some type-definitions
		typedef boost::unordered_map<vector<unsigned int>, unsigned int, KeyHasher, KeyComparor > hashmap;
		typedef boost::unordered_map<vector<unsigned int>, vector<unsigned int>, KeyHasher, KeyComparor > hashmap_vec;

		// Create map to ask for number of edges for the given set of node ids representing on edge in the model part
		hashmap n_edges_map;

		// Fill map that counts number of edges for given set of nodes
		for (ModelPart::ElementIterator itElem = rSurfaceModelPart.ElementsBegin(); itElem != rSurfaceModelPart.ElementsEnd(); itElem++)
		{
			Element::GeometryType::GeometriesArrayType edges = itElem->GetGeometry().Edges();

			for(unsigned int edge=0; edge<edges.size(); edge++)
			{
				// Create vector that stores all node is of current edge
				vector<unsigned int> ids(edges[edge].size());

				// Store node ids
				for(unsigned int i=0; i<edges[edge].size(); i++)
					ids[i] = edges[edge][i].Id();

				//*** THE ARRAY OF IDS MUST BE ORDERED!!! ***
				std::sort(ids.begin(), ids.end());

				// Fill the map
				n_edges_map[ids] += 1;
			}
		}

		// Create a map to get nodes of skin edge in original order for given set of node ids representing that edge
		// The given set of node ids may have a different node order
		hashmap_vec ordered_skin_edge_nodes_map;

		// Fill map that gives original node order for set of nodes
		for (ModelPart::ElementIterator itElem = rSurfaceModelPart.ElementsBegin(); itElem != rSurfaceModelPart.ElementsEnd(); itElem++)
		{
			Element::GeometryType::GeometriesArrayType edges = itElem->GetGeometry().Edges();

			for(unsigned int edge=0; edge<edges.size(); edge++)
			{
				// Create vector that stores all node is of current edge
				vector<unsigned int> ids(edges[edge].size());
				vector<unsigned int> unsorted_ids(edges[edge].size());

				// Store node ids
				for(unsigned int i=0; i<edges[edge].size(); i++)
				{
					ids[i] = edges[edge][i].Id();
					unsorted_ids[i] = edges[edge][i].Id();
				}

				//*** THE ARRAY OF IDS MUST BE ORDERED!!! ***
				std::sort(ids.begin(), ids.end());

				if(n_edges_map[ids] == 1)
					ordered_skin_edge_nodes_map[ids] = unsorted_ids;
			}
		}
		// First assign to skin model part all nodes from original model_part, unnecessary nodes will be removed later
		unsigned int id_condition = 1;
		//rExtractedBoundaryModelPart.Nodes() = rSurfaceModelPart.Nodes();

		// Add skin edges as triangles to skin-model-part (loop over all node sets)
		std::cout<<"  Extracting boundary mesh and computing normals" <<std::endl;
		std::vector<unsigned int> vector_of_node_ids;
		for(typename hashmap::const_iterator it=n_edges_map.begin(); it!=n_edges_map.end(); it++)
		{
			// If given node set represents edge that is not overlapping with a edge of another element, add it as skin element
			if(it->second == 1)
			{
				// If skin edge is a triangle store triangle in with its original orientation in new skin model part
				//std::cout<<"size of the ordered pair : "<<it->first.size()<<std::endl;
				if(it->first.size()==2)
				{
					// Getting original order is important to properly reproduce skin edge including its normal orientation
					vector<unsigned int> original_nodes_order = ordered_skin_edge_nodes_map[it->first];
					
					//std::cout<<"First Node: "<<original_nodes_order[0]<<std::endl;
					//std::cout<<"Second Node: "<<original_nodes_order[1]<<std::endl;
					
					Node < 3 >::Pointer pnode1 = rSurfaceModelPart.Nodes()(original_nodes_order[0]);
					Node < 3 >::Pointer pnode2 = rSurfaceModelPart.Nodes()(original_nodes_order[1]);

					//Storing the node ids list
					vector_of_node_ids.push_back(original_nodes_order[0]);
					vector_of_node_ids.push_back(original_nodes_order[1]);
					
					Properties::Pointer properties = rExtractedBoundaryModelPart.rProperties()(0);
					Condition const& rReferenceLineCondition = KratosComponents<Condition>::Get("Condition2D");

					// Skin edges are added as conditions
					Line2D2< Node<3> > line1(pnode1, pnode2);
					Condition::Pointer p_condition1 = rReferenceLineCondition.Create(id_condition++, line1, properties);
					rExtractedBoundaryModelPart.Conditions().push_back(p_condition1);
				}
				
				
			}
		}

		//sorting and making unique list of node ids

		std::set<unsigned int> s( vector_of_node_ids.begin(), vector_of_node_ids.end() );
		vector_of_node_ids.assign( s.begin(), s.end() ); 

		for (auto it = vector_of_node_ids.begin(); it != vector_of_node_ids.end(); it++)
		
		{
			//Adding the nodes to the rExtractedSurfaceModelPart
			Node < 3 >::Pointer pnode = rSurfaceModelPart.Nodes()(*it);
			rExtractedBoundaryModelPart.AddNode(pnode);
		

		}		

		std::cout<<"Successful extraction of the Boundary "<<rExtractedBoundaryModelPart.GetMesh()<<std::endl;
		
		KRATOS_CATCH("");
	}



	virtual void Execute() {
	}

	virtual void Clear() {
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
	virtual std::string Info() const {
		return "CustomHoleCuttingProcess";
	}

	/// Print information about this object.
	virtual void PrintInfo(std::ostream& rOStream) const {
		rOStream << "CustomHoleCuttingProcess";
	}

	/// Print object's data.
	virtual void PrintData(std::ostream& rOStream) const {
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
	CustomHoleCuttingProcess& operator=(CustomHoleCuttingProcess const& rOther);
	
	
	/// Copy constructor.
	//CustomHoleCuttingProcess(CustomHoleCuttingProcess const& rOther);

	///@}

}; // Class CustomHoleCuttingProcess

}  // namespace Kratos.

#endif // KRATOS_CUSTOM_HOLE_CUTTING_PROCESS_H_INCLUDED  defined
