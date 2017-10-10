// ==============================================================================
//  KratosShapeOptimizationApplication
//
//  License:         BSD License
//                   license: ShapeOptimizationApplication/license.txt
//
//  Main authors:    Baumgaertner Daniel, https://github.com/dbaumgaertner
//
// ==============================================================================

#ifndef CAD_RECONSTRUCTION_SOLVER_H
#define CAD_RECONSTRUCTION_SOLVER_H

// ------------------------------------------------------------------------------
// System includes
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// External includes
// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// Project includes
// ------------------------------------------------------------------------------
#include "includes/define.h"

// ==============================================================================

namespace Kratos
{
class CADReconstructionSolver
{
public:
    ///@name Type Definitions
    ///@{

    typedef UblasSpace<double, SparseMatrix, Vector> SparseSpaceType;
    typedef UblasSpace<double, CompressedMatrix, Vector> CompressedSpaceType;
    typedef UblasSpace<double, Matrix, Vector> DenseSpaceType;
    typedef LinearSolver<CompressedSpaceType, DenseSpaceType > CompressedLinearSolverType;

    /// Pointer definition of CADReconstructionSolver
    KRATOS_CLASS_POINTER_DEFINITION(CADReconstructionSolver);

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    CADReconstructionSolver( ReconstructionDataBase& reconstruction_data_base, 
                             ReconstructionConditionContainer& condition_container, 
                             CompressedLinearSolverType::Pointer linear_solver )
    : mrReconstructionDataBase( reconstruction_data_base ),
      mrReconstructionConditions( condition_container.GetReconstructionConditions() ),
      mrReconstructionConstraints( condition_container.GetReconstructionConstraints() ),
      mrRegularizationConditions( condition_container.GetRegularizationConditions() ),      
      mpLinearSolver( linear_solver )     
    {      
    }

    /// Destructor.
    virtual ~CADReconstructionSolver()
    {
    }

    // --------------------------------------------------------------------------
    void InitializeEquationSystem()
    {
        IdentifyControlPointsRelevantForReconstruction();
        AssignEquationIdToControlPointsRelevantForReconstruction();
        InitializeConditions();   
        InitializeSystemLHSAndRHS();
    }

    // --------------------------------------------------------------------------
    void IdentifyControlPointsRelevantForReconstruction()
    {
        std::cout << "\n> Start identifying control points relevant for reconstruction..." << std::endl;           
        boost::timer timer;      

        for(auto & condition_i : mrReconstructionConditions)
            condition_i->FlagControlPointsRelevantForReconstruction();

        for(auto & condition_i : mrReconstructionConstraints)
            condition_i->FlagControlPointsRelevantForReconstruction();              

        std::cout << "> Time needed for identifying control points relevant for reconstruction: " << timer.elapsed() << " s" << std::endl;    
    }      

    // --------------------------------------------------------------------------
    void AssignEquationIdToControlPointsRelevantForReconstruction()
    {
        std::cout << "\n> Start assigning equation ID to control points relevant for reconstruction..." << std::endl;           
        boost::timer timer;      

        // Equation id starts with zero since it shall also determin the row position in the equation system and C++ starts counting with 0
        int equation_id = 0;

        for(auto & patch_i : mrReconstructionDataBase.GetPatchVector()) 
        {
            for(auto & control_point_i : patch_i.GetSurfaceControlPoints())
            {
                if(control_point_i.IsRelevantForReconstruction())
                {
                            control_point_i.SetEquationId(equation_id);
                            ++mNumberOfRelevantControlPoints;
                            ++equation_id;
                }
                ++mNumberOfControlPoints;
            }        
        }      

        std::cout << "> Number of control points in total = " << mNumberOfControlPoints << "." << std::endl;
        std::cout << "> Number of control points relevant for mapping = " << mNumberOfRelevantControlPoints << "." << std::endl;  
        std::cout << "> Time needed for assigning equation ID to control points relevant for reconstruction: " << timer.elapsed() << " s" << std::endl;      
    }  

    // --------------------------------------------------------------------------
    void InitializeConditions()
    {
        std::cout << "\n> Initializing conditions ..." << std::endl;            
        boost::timer timer; 
             
        for(auto condition_i : mrReconstructionConditions)
            condition_i->Initialize();
        
        for(auto condition_i : mrReconstructionConstraints)            
            condition_i->Initialize();

        for(auto condition_i : mrRegularizationConditions)            
            condition_i->Initialize(); 
            
        std::cout << "> Time needed for initializing conditions: " << timer.elapsed() << " s" << std::endl;                  
    }

    // --------------------------------------------------------------------------
    void InitializeSystemLHSAndRHS()
    {
        std::cout << "\n> Initializing System LHS and RHS..." << std::endl;            

        mLHS.resize(3*mNumberOfRelevantControlPoints,3*mNumberOfRelevantControlPoints);
        mLHSConstantPart.resize(3*mNumberOfRelevantControlPoints,3*mNumberOfRelevantControlPoints);
        mRHS.resize(3*mNumberOfRelevantControlPoints);
        mLHS.clear();
        mLHSConstantPart.clear();
        mRHS.clear();

        mSolutionVector.resize(3*mNumberOfRelevantControlPoints);
        mSolutionVector.clear();

        std::cout << "> Finished initialization of System LHS and RHS." << std::endl;            
    }

    // --------------------------------------------------------------------------
    void ComputeLHS()
    {
        std::cout << "\n> Start computing LHS..." << std::endl;           
        boost::timer timer; 
        
        mLHS.clear();        
   
        // Compute contribution from reconstruction conditions
        if(isLHSWithoutConstraintsComputed==false)
        {
            isLHSWithoutConstraintsComputed = true;
            for(auto condition_i : mrReconstructionConditions)
                condition_i->ComputeAndAddLHSContribution( mLHSConstantPart );
        }

        mLHS = mLHSConstantPart;

        // Compute contribution from reconstruction constraints
        for(auto condition_i : mrReconstructionConstraints)
            condition_i->ComputeAndAddLHSContribution( mLHS ); 
            
        CheckLHSConditionBeforeRegularization();

        // Compute contribution from regularization conditions
        for(auto condition_i : mrRegularizationConditions)
            condition_i->ComputeAndAddLHSContribution( mLHS );
            
        std::cout << "> Time needed for computing LHS: " << timer.elapsed() << " s" << std::endl;            
    }

    // --------------------------------------------------------------------------
    void CheckLHSConditionBeforeRegularization()
    {
        int number_of_small_values_on_main_diagonal = 0;
        double zero_threshold = 1e-10;

        for(int i_itr=0; i_itr<mLHS.size1(); i_itr++)
            if( std::abs(mLHS(i_itr,i_itr)) < zero_threshold )
                number_of_small_values_on_main_diagonal++;

        if(number_of_small_values_on_main_diagonal>0)
            std::cout << "> WARNING, before regularization, number of values on main diagonal < " << zero_threshold << ": " << number_of_small_values_on_main_diagonal << " !!!!!!!!!!!!!!! " << std::endl;
    }

    // --------------------------------------------------------------------------
    void ComputeRHS()
    {
        std::cout << "\n> Start computing RHS..." << std::endl;           
        boost::timer timer;  

        mRHS.clear();        

        // Compute contribution from reconstruction conditions
        for(auto condition_i : mrReconstructionConditions)
            condition_i->ComputeAndAddRHSContribution( mRHS );

        // Compute contribution from reconstruction constraints
        for(auto condition_i : mrReconstructionConstraints)
            condition_i->ComputeAndAddRHSContribution( mRHS );
            
        // Compute contribution from regularization conditions
        for(auto condition_i : mrRegularizationConditions)
            condition_i->ComputeAndAddRHSContribution( mRHS );

        std::cout << "> Max value in RHS vector = " << *std::max_element(mRHS.begin(),mRHS.end()) << std::endl;
        std::cout << "> L2 norm of RHS vector = " << norm_2(mRHS) << std::endl;            
            
        std::cout << "> Time needed for computing RHS: " << timer.elapsed() << " s" << std::endl;            
    }

    // --------------------------------------------------------------------------
    void SolveEquationSystem()
    {
        std::cout << "\n> Start solving reconstruction equation..." << std::endl;  
        boost::timer timer;

        mSolutionVector.clear();        
        mpLinearSolver->Solve(mLHS, mSolutionVector, mRHS);

        Vector residual_vector = mRHS - prod(mLHS,mSolutionVector);
        std::cout << "> Max value in residual vector = " << *std::max_element(residual_vector.begin(),residual_vector.end()) << std::endl;
        std::cout << "> L2 norm of residual vector = " << norm_2(residual_vector) << std::endl;      

        std::cout << "> Time needed for solving reconstruction equation: " << timer.elapsed() << " s" << std::endl;
    }

    // --------------------------------------------------------------------------
    void UpdateControlPointsAccordingReconstructionStrategy(std::string reconstruction_strategy )
    {
        std::cout << "\n> Start updating control points..." << std::endl;  
        boost::timer timer;

        if(reconstruction_strategy.compare("mapping") == 0)
            mrReconstructionDataBase.AddToControlPointDisplacements( mSolutionVector );
        else
            KRATOS_THROW_ERROR(std::invalid_argument, "Reconstruction strategy specified to update control points is not recognized!","");

        std::cout << "> Time needed for updating control points: " << timer.elapsed() << " s" << std::endl;          
    }    

    // --------------------------------------------------------------------------
    void MultiplyAllPenaltyFactorsByInputFactor( double factor )
    {
        for(auto condition_i : mrReconstructionConstraints)
            condition_i->Set( "PENALTY_MULTIPLIER", factor );  
    }    

    // ==============================================================================

    /// Turn back information as a string.
    virtual std::string Info() const
    {
		return "CADReconstructionSolver";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream &rOStream) const
    {
		rOStream << "CADReconstructionSolver";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream &rOStream) const
    {
    }

private:

    // ==============================================================================
    // Initialized by class constructor
    // ==============================================================================
    ReconstructionDataBase& mrReconstructionDataBase;    
    std::vector<ReconstructionCondition::Pointer>& mrReconstructionConditions;
    std::vector<ReconstructionConstraint::Pointer>& mrReconstructionConstraints;
    std::vector<RegularizationCondition::Pointer>& mrRegularizationConditions;    
    CompressedLinearSolverType::Pointer mpLinearSolver;

    // ==============================================================================
    // Member variables
    // ==============================================================================
    int mNumberOfControlPoints = 0;
    int mNumberOfRelevantControlPoints = 0;
    CompressedMatrix mLHS;
    CompressedMatrix mLHSConstantPart;
    bool isLHSWithoutConstraintsComputed = false;
    Vector mRHS;
    Vector mSolutionVector;

    /// Assignment operator.
    //      CADReconstructionSolver& operator=(CADReconstructionSolver const& rOther);

    /// Copy constructor.
    //      CADReconstructionSolver(CADReconstructionSolver const& rOther);

}; // Class CADReconstructionSolver
} // namespace Kratos.

#endif // CAD_RECONSTRUCTION_SOLVER_H
