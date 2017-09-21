// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:             BSD License
//                                       license: StructuralMechanicsApplication/license.txt
//
//  Main authors:    Vicente Mataix Ferrandiz
//

#if !defined(KRATOS_RESIDUALBASED_NEWTON_RAPHSON_CONTACT_STRATEGY_REMESHING)
#define KRATOS_RESIDUALBASED_NEWTON_RAPHSON_CONTACT_STRATEGY_REMESHING

/* System Includes */

/* External Includes */
#include "boost/smart_ptr.hpp"

/* Project includes */
#include "includes/kratos_parameters.h"
#include "includes/define.h"
#include "includes/model_part.h"
#include "includes/variables.h"
//#include "meshing_application.h"


#ifdef INCLUDE_MMG_CONTACT
    #include "../MeshingApplication/custom_processes/mmg_process.h"
#endif
#include "../MeshingApplication/custom_processes/metric_fast_init_process.h"
#include "../MeshingApplication/custom_processes/metrics_spr_error_process.h"
#include "../MeshingApplication/meshing_application.h"
#include "processes/find_nodal_h_process.h"
#include "../MeshingApplication/custom_utilities/execute_remeshing.h"
    
// Strategies
#include "solving_strategies/strategies/residualbased_newton_raphson_strategy.h"

// Utilities
#include "utilities/variable_utils.h"
#if !defined(_WIN32)
	#include "utilities/color_utilities.h"
#endif
#include "utilities/process_factory_utility.h"

// TODO: Extend the descriptions

namespace Kratos {

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{
    #if !defined(REMESHING_UTILITIES)
    #define REMESHING_UTILITIES
        enum RemeshingUtilities {MMG = 0};
    #endif
        
///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{
    
/** \brief  Short class definition.
This class 
*/

template<class TSparseSpace,
         class TDenseSpace, // = DenseSpace<double>,
         class TLinearSolver //= LinearSolver<TSparseSpace,TDenseSpace>
         >
         
class ResidualBasedNewtonRaphsonContactStrategyRemeshing :
    public ResidualBasedNewtonRaphsonContactStrategy< TSparseSpace, TDenseSpace, TLinearSolver >
{
public:
    
    typedef ConvergenceCriteria<TSparseSpace, TDenseSpace> TConvergenceCriteriaType;
    
    /** Counted pointer of ClassName*/ 
    KRATOS_CLASS_POINTER_DEFINITION( ResidualBasedNewtonRaphsonContactStrategyRemeshing );
    
    typedef SolvingStrategy<TSparseSpace, TDenseSpace, TLinearSolver>            StrategyBaseType;
    
    typedef ResidualBasedNewtonRaphsonStrategy<TSparseSpace, TDenseSpace, TLinearSolver> BaseType;

    typedef ResidualBasedNewtonRaphsonContactStrategy<TSparseSpace, TDenseSpace, TLinearSolver> ContactBaseType;
    
    typedef typename BaseType::TBuilderAndSolverType                        TBuilderAndSolverType;

    typedef typename BaseType::TDataType                                                TDataType;

    typedef TSparseSpace                                                          SparseSpaceType;

    typedef typename BaseType::TSchemeType                                            TSchemeType;

    typedef typename BaseType::DofsArrayType                                        DofsArrayType;

    typedef typename BaseType::TSystemMatrixType                                TSystemMatrixType;

    typedef typename BaseType::TSystemVectorType                                TSystemVectorType;

    typedef typename BaseType::LocalSystemVectorType                        LocalSystemVectorType;

    typedef typename BaseType::LocalSystemMatrixType                        LocalSystemMatrixType;

    typedef typename BaseType::TSystemMatrixPointerType                  TSystemMatrixPointerType;
    
    typedef typename BaseType::TSystemVectorPointerType                  TSystemVectorPointerType;
    
    typedef ModelPart::NodesContainerType                                          NodesArrayType;
    
    typedef ModelPart::ConditionsContainerType                                ConditionsArrayType;
    
    typedef boost::shared_ptr<ProcessFactoryUtility>                            ProcessesListType;
    
    /**
     * Default constructor 
     * @param rModelPart: The model part of the problem
     * @param pScheme: The integration scheme
     * @param pNewLinearSolver: The linear solver employed
     * @param pNewConvergenceCriteria: The convergence criteria employed
     * @param MaxIterationNumber: The maximum number of iterations
     * @param CalculateReactions: The flag for the reaction calculation
     * @param ReformDofSetAtEachStep: The flag that allows to compute the modification of the DOF
     * @param MoveMeshFlag: The flag that allows to move the mesh
     */
    
    ResidualBasedNewtonRaphsonContactStrategyRemeshing(
        ModelPart& rModelPart,
        typename TSchemeType::Pointer pScheme,
        typename TLinearSolver::Pointer pNewLinearSolver,
        typename TConvergenceCriteriaType::Pointer pNewConvergenceCriteria,
        unsigned int MaxIterations = 30,
        bool CalculateReactions = false,
        bool ReformDofSetAtEachStep = false,
        bool MoveMeshFlag = false,
        Parameters ThisParameters =  Parameters(R"({})"),
        Parameters ThisRemeshingParameters = Parameters(R"({})"),
        ProcessesListType pMyProcesses = nullptr
    )
        : ResidualBasedNewtonRaphsonContactStrategy<TSparseSpace, TDenseSpace, TLinearSolver>(rModelPart, pScheme, pNewLinearSolver, pNewConvergenceCriteria, MaxIterations, CalculateReactions, ReformDofSetAtEachStep, MoveMeshFlag, ThisParameters, pMyProcesses)
        //ContactBaseType::mpMyProcesses(pMyProcesses)
    {
        KRATOS_TRY;
        //ContactBaseType::mpMyProcesses = pMyProcesses;
        //ContactBaseType::mConvergenceCriteriaEchoLevel = pNewConvergenceCriteria->GetEchoLevel();
        
        /*Parameters DefaultParameters = Parameters(R"(
        {
            "adaptative_strategy"              : false,
            "split_factor"                     : 10.0,
            "max_number_splits"                : 3
            "remeshing_max_iterations"         : 3
        })" );

        ThisParameters.ValidateAndAssignDefaults(DefaultParameters);
        */

        Parameters DefaultRemeshingParameters = Parameters(R"(
        {
            "remeshing_max_iterations"         : 3
        })" );

        ThisRemeshingParameters.ValidateAndAssignDefaults(DefaultRemeshingParameters);

        mRemeshingMaxIterations = ThisRemeshingParameters["remeshing_max_iterations"].GetInt();
        
    
        KRATOS_CATCH("");
    }

    /**
     * Default constructor 
     * @param rModelPart: The model part of the problem
     * @param pScheme: The integration scheme
     * @param pNewLinearSolver: The linear solver employed
     * @param pNewConvergenceCriteria: The convergence criteria employed
     * @param MaxIterationNumber: The maximum number of iterations
     * @param CalculateReactions: The flag for the reaction calculation
     * @param ReformDofSetAtEachStep: The flag that allows to compute the modification of the DOF
     * @param MoveMeshFlag: The flag that allows to move the mesh
     */
    
    ResidualBasedNewtonRaphsonContactStrategyRemeshing(
        ModelPart& rModelPart,
        typename TSchemeType::Pointer pScheme,
        typename TLinearSolver::Pointer pNewLinearSolver,
        typename TConvergenceCriteriaType::Pointer pNewConvergenceCriteria,
        typename TBuilderAndSolverType::Pointer pNewBuilderAndSolver,
        unsigned int MaxIterations = 30,
        bool CalculateReactions = false,
        bool ReformDofSetAtEachStep = false,
        bool MoveMeshFlag = false,
        Parameters ThisParameters =  Parameters(R"({})"),
        Parameters ThisRemeshingParameters =  Parameters(R"({})"),
        ProcessesListType pMyProcesses = nullptr                                      
        )
        : ResidualBasedNewtonRaphsonContactStrategy<TSparseSpace, TDenseSpace, TLinearSolver>(rModelPart, pScheme, pNewLinearSolver, pNewConvergenceCriteria, pNewBuilderAndSolver, MaxIterations, CalculateReactions, ReformDofSetAtEachStep, MoveMeshFlag, ThisParameters, pMyProcesses )
        //ContactBaseType::mpMyProcesses(pMyProcesses)
    {
        KRATOS_TRY;
        /*
        ContactBaseType::mConvergenceCriteriaEchoLevel = pNewConvergenceCriteria->GetEchoLevel();
        
        Parameters DefaultParameters = Parameters(R"(
        {
            "adaptative_strategy"              : false,
            "split_factor"                     : 10.0,
            "max_number_splits"                : 3
            "remeshing_max_iterations"         : 3
        })" );

        ThisParameters.ValidateAndAssignDefaults(DefaultParameters);*/
        
        Parameters DefaultRemeshingParameters = Parameters(R"(
        {
            "remeshing_max_iterations"         : 3
        })" );

        ThisRemeshingParameters.ValidateAndAssignDefaults(DefaultRemeshingParameters);

        //mRemeshingMaxIterations = ThisParameters["remeshing_max_iterations"].GetInt();
        mRemeshingMaxIterations =3;
        KRATOS_CATCH("");
    }

    /** 
     * Destructor.
     */
    
    ~ResidualBasedNewtonRaphsonContactStrategyRemeshing()
    = default;
    
    //******************** OPERATIONS ACCESSIBLE FROM THE INPUT: ************************//
    //***********************************************************************************//
    
    /**
     * Initialization of member variables and prior operations
     */
     
    /*void Initialize() override
    {
        KRATOS_TRY;

        BaseType::Initialize();
        mFinalizeWasPerformed = false;

        KRATOS_CATCH("");
    }*/
    
    /**
     * Performs all the required operations that should be done (for each step) 
     * after solving the solution step.
     */
    
    /*void FinalizeSolutionStep() override
    {
        KRATOS_TRY;

        if (mFinalizeWasPerformed == false)
        {
            BaseType::FinalizeSolutionStep();
            
            // To avoid compute twice the FinalizeSolutionStep
            mFinalizeWasPerformed = true;
        }

        KRATOS_CATCH("");
    }*/

    /**
     * Solves the current step. This function returns true if a solution has been found, false otherwise.
     */
    
    bool SolveSolutionStep() override
    {
        bool remeshing_necessary = true;
        unsigned int remeshing_iterator = 0;
        bool is_converged;
        while(remeshing_necessary == true && remeshing_iterator++ < mRemeshingMaxIterations)
        {
            is_converged = ContactBaseType::SolveSolutionStep();

            //initialize metric
            MetricFastInit<2> MetricInit = MetricFastInit<2>(StrategyBaseType::GetModelPart());
            MetricInit.Execute();
            ComputeSPRErrorSolMetricProcess<2> ComputeMetric = ComputeSPRErrorSolMetricProcess<2>(StrategyBaseType::GetModelPart());
            double error_percentage;
            error_percentage = ComputeMetric.Execute();
            
            if(error_percentage >0.11)
            {
            
                // Remeshing
                Parameters RemeshingParameters = Parameters(R"(
                {
                    "filename"                             : "out",
                    "framework"                            : "Lagrangian",
                    "internal_variables_parameters"        :
                    {
                        "allocation_size"                      : 1000, 
                        "bucket_size"                          : 4, 
                        "search_factor"                        : 2, 
                        "interpolation_type"                   : "LST",
                        "internal_variable_interpolation_list" :[]
                    },
                    "save_external_files"              : false,
                    "max_number_of_searchs"            : 1000,
                    "echo_level"                       : 3,
                    "step_data_size"                   : 0,
                    "buffer_size"                      : 0
                })" );
                
                #ifdef INCLUDE_MMG
                    //if (mDimension == 2)
                    if (true)
                    {
                        //MmgProcess<2> MmgRemesh = MmgProcess<2>(StrategyBaseType::GetModelPart(), RemeshingParameters); 
                        //MmgRemesh.Execute();
                        ExecuteRemeshing<2> MmgRemesh = ExecuteRemeshing<2>(StrategyBaseType::GetModelPart(), RemeshingParameters);
                        MmgRemesh.Execute();
                    }
                    else
                    {
                        //!!not yet implemented
                        //MmgProcess<3> MmgRemesh = MmgProcess<3>(mThisModelPart, mThisParameters["remeshing_parameters"]); 
                        //MmgRemesh.Execute();
                    }
                #else 
                    KRATOS_ERROR << "Please compile with MMGc to use this utility" << std::endl;
                #endif

                
                //mFindNodalH.Execute();
                
                // Processes initialization
                ContactBaseType::mpMyProcesses->ExecuteInitialize();
                // Processes before the loop
                ContactBaseType::mpMyProcesses->ExecuteBeforeSolutionLoop();
                // Processes of initialize the solution step
                ContactBaseType::mpMyProcesses->ExecuteInitializeSolutionStep();
                
                // We set the model part as modified
                StrategyBaseType::GetModelPart().Set(MODIFIED, true);
            }
            else
            {
                remeshing_necessary = false;
            }
        }
        /*
//      bool is_converged = BaseType::SolveSolutionStep(); // FIXME: Requires to separate the non linear iterations 
        bool is_converged = BaseSolveSolutionStep();
        
        // Plots a warning if the maximum number of iterations is exceeded
        if ((mAdaptativeStrategy == true) && (is_converged == false))
        {
            if (mpMyProcesses == nullptr && StrategyBaseType::mEchoLevel > 0)
            {
                std::cout << "WARNING:: If you have not implemented any method to recalculate BC or loads in function of time, this strategy will be USELESS" << std::endl;
            }
        
            ProcessInfo& this_process_info = StrategyBaseType::GetModelPart().GetProcessInfo();

            const double original_delta_time = this_process_info[DELTA_TIME]; // We save the delta time to restore later
            
            unsigned int split_number = 0;
            
            // We iterate until we reach the convergence or we split more than desired
            while (is_converged == false && split_number <= mMaxNumberSplits)
            {                   
                // Expliting time step as a way to try improve the convergence
                split_number += 1;
                double aux_delta_time;
                double current_time; 
                const double aux_time = SplitTimeStep(aux_delta_time, current_time);
                
                bool inside_the_split_is_converged = true;
                unsigned int inner_iteration = 0;
                while (inside_the_split_is_converged == true && this_process_info[TIME] <= aux_time)
                {      
                    current_time += aux_delta_time;
                    inner_iteration += 1;
                    this_process_info[TIME_STEPS] += 1;
                    
                    if (inner_iteration == 1)
                    {
                        if (StrategyBaseType::MoveMeshFlag() == true)
                        {
                            UnMoveMesh();
                        }
                        
                        NodesArrayType& nodes_array = StrategyBaseType::GetModelPart().Nodes();
                        const int num_nodes = static_cast<int>(nodes_array.size());
                        
                        #pragma omp parallel for
                        for(int i = 0; i < num_nodes; i++)  
                        {
                            auto it_node = nodes_array.begin() + i;
                            
                            it_node->OverwriteSolutionStepData(1, 0);
//                             it_node->OverwriteSolutionStepData(2, 1);
                        }
                        
                        this_process_info.SetCurrentTime(current_time); // Reduces the time step
                        
                        ContactBaseType::FinalizeSolutionStep();
                    }
                    else
                    {
                        NodesArrayType& nodes_array = StrategyBaseType::GetModelPart().Nodes();
                        const int num_nodes = static_cast<int>(nodes_array.size());
                        
                        #pragma omp parallel for
                        for(int i = 0; i < num_nodes; i++)  
                        {
                            auto it_node = nodes_array.begin() + i;
                            
                            it_node->CloneSolutionStepData();
                        }
                        
                        this_process_info.CloneSolutionStepInfo();
                        this_process_info.ClearHistory(StrategyBaseType::GetModelPart().GetBufferSize());
                        this_process_info.SetAsTimeStepInfo(current_time); // Sets the new time step
                    }
                    
                    // We execute the processes before the non-linear iteration
                    if (mpMyProcesses != nullptr)
                    {
                        // TODO: Think about to add the postprocess processes
                        mpMyProcesses->ExecuteInitializeSolutionStep();
                    }
                    
                    // In order to initialize again everything
                    BaseType::mInitializeWasPerformed = false;
                    mFinalizeWasPerformed = false;
                    
                    // We repeat the solve with the new DELTA_TIME
                    ContactBaseType::Initialize();
                    ContactBaseType::InitializeSolutionStep();
                    BaseType::Predict();
                    inside_the_split_is_converged = BaseType::SolveSolutionStep();
                    ContactBaseType::FinalizeSolutionStep();
                    
                    // We execute the processes after the non-linear iteration
                    if (mpMyProcesses != nullptr)
                    {
                        // TODO: Think about to add the postprocess processes
                        mpMyProcesses->ExecuteFinalizeSolutionStep();
//                         mpMyProcesses->ExecuteBeforeOutputStep();
//                         mpMyProcesses->ExecuteAfterOutputStep();
                    }
                }
                
                if (inside_the_split_is_converged == true)
                {
                    is_converged = true;
                }
            }
            
            // Plots a warning if the maximum number of iterations and splits are exceeded
            if (is_converged == false)
            {
                ContactBaseType::MaxIterationsAndSplitsExceeded();
            }
            
            // Restoring original DELTA_TIME
            this_process_info[DELTA_TIME] = original_delta_time;
        */

        return is_converged;
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

    ///@}
    ///@name Friends
    ///@{

protected:

    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{
    
    // ADAPTATIVE STRATEGY PARAMETERS
    //bool mAdaptativeStrategy;         // If consider time split
    //bool mFinalizeWasPerformed;       // If the FinalizeSolutionStep has been already performed
    //double mSplitFactor;              // Number by one the delta time is split
    //ProcessesListType mpMyProcesses;  // The processes list
    //unsigned int mMaxNumberSplits;    // Maximum number of splits
    
    // REMESHING PARAMETERS
    //bool mAdaptiveRemeshing;          // if adaptive remeshing is activated
    int mRemeshingMaxIterations;       // max number of adaptive remeshing loops
    // OTHER PARAMETERS
    //int mConvergenceCriteriaEchoLevel; // The echo level of the convergence criteria

    ///@}
    ///@name Protected Operators
    ///@{
    
    /**
     * Solves the current step. This function returns true if a solution has been found, false otherwise.
     */
    /*
    bool BaseSolveSolutionStep()
    {
        // Pointers needed in the solution
        typename TSchemeType::Pointer pScheme = BaseType::GetScheme();
        typename TBuilderAndSolverType::Pointer pBuilderAndSolver = BaseType::GetBuilderAndSolver();

        TSystemMatrixType& A = *BaseType::mpA;
        TSystemVectorType& Dx = *BaseType::mpDx;
        TSystemVectorType& b = *BaseType::mpb;

        //initializing the parameters of the Newton-Raphson cicle
        unsigned int iteration_number = 1;
        StrategyBaseType::GetModelPart().GetProcessInfo()[NL_ITERATION_NUMBER] = iteration_number;

        bool is_converged = false;
        bool residual_is_updated = false;
        pScheme->InitializeNonLinIteration(StrategyBaseType::GetModelPart(), A, Dx, b);
        is_converged = BaseType::mpConvergenceCriteria->PreCriteria(StrategyBaseType::GetModelPart(), pBuilderAndSolver->GetDofSet(), A, Dx, b);

        //function to perform the building and the solving phase.
        if (StrategyBaseType::mRebuildLevel > 1 || StrategyBaseType::mStiffnessMatrixIsBuilt == false)
        {
            TSparseSpace::SetToZero(A);
            TSparseSpace::SetToZero(Dx);
            TSparseSpace::SetToZero(b);

            pBuilderAndSolver->BuildAndSolve(pScheme, StrategyBaseType::GetModelPart(), A, Dx, b);
        }
        else
        {
            TSparseSpace::SetToZero(Dx); //Dx=0.00;
            TSparseSpace::SetToZero(b);

            pBuilderAndSolver->BuildRHSAndSolve(pScheme, StrategyBaseType::GetModelPart(), A, Dx, b);
        }
        
        // Debugging info
        BaseType::EchoInfo(iteration_number);
        
        // Updating the results stored in the database
        UpdateDatabase(A, Dx, b, StrategyBaseType::MoveMeshFlag());

        pScheme->FinalizeNonLinIteration(StrategyBaseType::GetModelPart(), A, Dx, b);

        if (is_converged == true)
        {
            //initialisation of the convergence criteria
            BaseType::mpConvergenceCriteria->InitializeSolutionStep(StrategyBaseType::GetModelPart(), pBuilderAndSolver->GetDofSet(), A, Dx, b);

            if (BaseType::mpConvergenceCriteria->GetActualizeRHSflag() == true)
            {
                TSparseSpace::SetToZero(b);

                pBuilderAndSolver->BuildRHS(pScheme, StrategyBaseType::GetModelPart(), b);
            }

            is_converged = BaseType::mpConvergenceCriteria->PostCriteria(StrategyBaseType::GetModelPart(), pBuilderAndSolver->GetDofSet(), A, Dx, b);
        }

        // adaptive mesh refinement: 
        //executed after each iteraltion loop if the estimated error is not below a certain limit and if the max number of refinements is not reached

        if(mAdaptativeStrategy == false){
            // execute refinement loop only once
            mRemeshingMaxIterations = 1;}
        bool remeshing_necessary = true;
        unsigned int remeshing_iterator = 0;
        while (remeshing_necessary == true && remeshing_iterator++ < mRemeshingMaxIterations){

            //Iteration Cicle... performed only for NonLinearProblems
            while (is_converged == false &&
                    iteration_number++<BaseType::mMaxIterationNumber)
            {
                //setting the number of iteration
                StrategyBaseType::GetModelPart().GetProcessInfo()[NL_ITERATION_NUMBER] = iteration_number;

                pScheme->InitializeNonLinIteration(StrategyBaseType::GetModelPart(), A, Dx, b);

                // To be able to calculate the current gap and recalulate the penalty
                if (StrategyBaseType::GetModelPart().GetProcessInfo()[ADAPT_PENALTY] == true)
                {
                    TSparseSpace::SetToZero(b);
                    pBuilderAndSolver->BuildRHS(pScheme, StrategyBaseType::GetModelPart(), b);
                }
                        
                is_converged = BaseType::mpConvergenceCriteria->PreCriteria(StrategyBaseType::GetModelPart(), pBuilderAndSolver->GetDofSet(), A, Dx, b);

                //call the linear system solver to find the correction mDx for the
                //it is not called if there is no system to solve
                if (SparseSpaceType::Size(Dx) != 0)
                {
                    if (StrategyBaseType::mRebuildLevel > 1 || StrategyBaseType::mStiffnessMatrixIsBuilt == false )
                    {
                        if( BaseType::GetKeepSystemConstantDuringIterations() == false)
                        {
                            //A = 0.00;
                            TSparseSpace::SetToZero(A);
                            TSparseSpace::SetToZero(Dx);
                            TSparseSpace::SetToZero(b);

                            pBuilderAndSolver->BuildAndSolve(pScheme, StrategyBaseType::GetModelPart(), A, Dx, b);
                        }
                        else
                        {
                            TSparseSpace::SetToZero(Dx);
                            TSparseSpace::SetToZero(b);

                            pBuilderAndSolver->BuildRHSAndSolve(pScheme, StrategyBaseType::GetModelPart(), A, Dx, b);
                        }
                    }
                    else
                    {
                        TSparseSpace::SetToZero(Dx);
                        TSparseSpace::SetToZero(b);

                        pBuilderAndSolver->BuildRHSAndSolve(pScheme, StrategyBaseType::GetModelPart(), A, Dx, b);
                    }
                }
                else
                {
                    std::cout << "ATTENTION: no free DOFs!! " << std::endl;
                }

                // Debugging info
                BaseType::EchoInfo(iteration_number);
            
                // Updating the results stored in the database
                UpdateDatabase(A, Dx, b, StrategyBaseType::MoveMeshFlag());

                pScheme->FinalizeNonLinIteration(StrategyBaseType::GetModelPart(), A, Dx, b);

                residual_is_updated = false;

                if (is_converged == true)
                {

                    if (BaseType::mpConvergenceCriteria->GetActualizeRHSflag() == true)
                    {
                        TSparseSpace::SetToZero(b);

                        pBuilderAndSolver->BuildRHS(pScheme, StrategyBaseType::GetModelPart(), b);
                        residual_is_updated = true;
                        //std::cout << "mb is calculated" << std::endl;
                    }

                    is_converged = BaseType::mpConvergenceCriteria->PostCriteria(StrategyBaseType::GetModelPart(), pBuilderAndSolver->GetDofSet(), A, Dx, b);
                }
            }//end nonlinear iteration loop
            // error estimation
            //if (mDimension == 2)
            if (true)
            {
                //metric initialization
                MetricFastInit<2> MetricInit = MetricFastInit<2>(StrategyBaseType::GetModelPart());
                MetricInit.Execute();
                ComputeSPRErrorSolMetricProcess<2> ComputeMetric = ComputeSPRErrorSolMetricProcess<2>(StrategyBaseType::GetModelPart());
                ComputeMetric.Execute();
            }
            
            else
            {
                // !!! not yet implemented!!!
                // Dummy procedure for further developments
                //metric initialization
                MetricFastInit<3> MetricInit = MetricFastInit<3>(mrModelPart);
                MetricInit.Execute();
                ComputeSPRErrorSolMetricProcess<3> ComputeMetric = ComputeSPRErrorSolMetricProcess<3>(mrModelPart);
                ComputeMetric.Execute();
            }
            
            // Remeshing
            Parameters RemeshingParameters = Parameters(R"(
            {
                "filename"                             : "out",
                "framework"                            : "Eulerian",
                "internal_variables_parameters"        :
                {
                    "allocation_size"                      : 1000, 
                    "bucket_size"                          : 4, 
                    "search_factor"                        : 2, 
                    "interpolation_type"                   : "LST",
                    "internal_variable_interpolation_list" :[]
                },
            "save_external_files"              : false,
            "max_number_of_searchs"            : 1000,
            "echo_level"                       : 3,
            "step_data_size"                   : 0,
            "buffer_size"                      : 0
            })" );
            #ifdef INCLUDE_MMG
                if (mDimension == 2)
                {
                    MmgProcess<2> MmgRemesh = MmgProcess<2>(StrategyBaseType::GetModelPart(),RemeshingParameters); 
                    MmgRemesh.Execute();
                }
                else
                {
                    //!!not yet implemented
                    //MmgProcess<3> MmgRemesh = MmgProcess<3>(mThisModelPart, mThisParameters["remeshing_parameters"]); 
                    //MmgRemesh.Execute();
                }
            #else 
                KRATOS_ERROR << "Please compile with MMG to use this utility" << std::endl;
            #endif

            
            //mFindNodalH.Execute();
            
            // Processes initialization
            mpMyProcesses->ExecuteInitialize();
            // Processes before the loop
            mpMyProcesses->ExecuteBeforeSolutionLoop();
            // Processes of initialize the solution step
            mpMyProcesses->ExecuteInitializeSolutionStep();
            
            // We set the model part as modified
            StrategyBaseType::GetModelPart().Set(MODIFIED, true);
    }//end remeshing loop

        //plots a warning if the maximum number of iterations is exceeded
        if (iteration_number >= BaseType::mMaxIterationNumber && StrategyBaseType::GetModelPart().GetCommunicator().MyPID() == 0)
        {
            MaxIterationsExceeded();
        }

        //recalculate residual if needed
        //(note that some convergence criteria need it to be recalculated)
        if (residual_is_updated == false)
        {
            // NOTE:
            // The following part will be commented because it is time consuming
            // and there is no obvious reason to be here. If someone need this
            // part please notify the community via mailing list before uncommenting it.
            // Pooyan.

            //    TSparseSpace::SetToZero(mb);
            //    pBuilderAndSolver->BuildRHS(pScheme, StrategyBaseType::GetModelPart(), mb);
        }

        //calculate reactions if required
        if (BaseType::mCalculateReactionsFlag == true)
        {
            pBuilderAndSolver->CalculateReactions(pScheme, StrategyBaseType::GetModelPart(), A, Dx, b);
        }

        return is_converged;
    }*/
    
    /**
     * Performs all the required operations that should be done (for each step) 
     * before solving the solution step.
     * A member variable should be used as a flag to make sure this function is called only once per step.
     */
    /*   
    void InitializeSolutionStep() override
    {
        BaseType::InitializeSolutionStep();
        
        // TODO: Add something if necessary
    }
    
    /**
     * Here the database is updated
     */
    /*
    void UpdateDatabase(
        TSystemMatrixType& A,
        TSystemVectorType& Dx,
        TSystemVectorType& b,
        const bool MoveMesh
        ) override
    {
        BaseType::UpdateDatabase(A,Dx,b,MoveMesh);
        
        // TODO: Add something if necessary
    }
    
    /**
     * Here the time step is splitted
     */
    /*
    double SplitTimeStep(
        double& AuxDeltaTime,
        double& CurrentTime
        )
    {
        KRATOS_TRY;

        const double aux_time = StrategyBaseType::GetModelPart().GetProcessInfo()[TIME];
        AuxDeltaTime = StrategyBaseType::GetModelPart().GetProcessInfo()[DELTA_TIME];
        CurrentTime = aux_time - AuxDeltaTime;
        
        StrategyBaseType::GetModelPart().GetProcessInfo()[TIME] =   CurrentTime; // Restore time to the previous one
        AuxDeltaTime /= mSplitFactor;
        StrategyBaseType::GetModelPart().GetProcessInfo()[DELTA_TIME] = AuxDeltaTime; // Change delta time
        
        CoutSplittingTime(AuxDeltaTime);
        
        return aux_time;
        
        KRATOS_CATCH("");
    }
    
    /**
     * This method moves bak the mesh to the previous position
     */
    /*
    void UnMoveMesh()
    {
        KRATOS_TRY;

        if (StrategyBaseType::GetModelPart().NodesBegin()->SolutionStepsDataHas(DISPLACEMENT_X) == false)
        {
            KRATOS_ERROR << "It is impossible to move the mesh since the DISPLACEMENT var is not in the model_part. Either use SetMoveMeshFlag(False) or add DISPLACEMENT to the list of variables" << std::endl;
        }

        NodesArrayType& nodes_array = StrategyBaseType::GetModelPart().Nodes();
        const int num_nodes = static_cast<int>(nodes_array.size());

        #pragma omp parallel for
        for(int i = 0; i < num_nodes; i++)  
        {
            auto it_node = nodes_array.begin() + i;

            noalias(it_node->Coordinates()) = it_node->GetInitialPosition().Coordinates();
            noalias(it_node->Coordinates()) += it_node->FastGetSolutionStepValue(DISPLACEMENT, 1);
        }

        KRATOS_CATCH("");
    }
    
    /**
     * This method prints information after solving the problem
     */
    /*
    void CoutSolvingProblem()
    {
        if (mConvergenceCriteriaEchoLevel != 0)
        {
            std::cout << "STEP: " << StrategyBaseType::GetModelPart().GetProcessInfo()[TIME_STEPS] << "\t NON LINEAR ITERATION: " << StrategyBaseType::GetModelPart().GetProcessInfo()[NL_ITERATION_NUMBER] << "\t TIME: " << StrategyBaseType::GetModelPart().GetProcessInfo()[TIME] << "\t DELTA TIME: " << StrategyBaseType::GetModelPart().GetProcessInfo()[DELTA_TIME]  << std::endl;
        }
    }
    
    /**
     * This method prints information after split the increment of time
     */
    /*   
    void CoutSplittingTime(const double AuxDeltaTime)
    {
        if (mConvergenceCriteriaEchoLevel > 0 && StrategyBaseType::GetModelPart().GetCommunicator().MyPID() == 0 )
        {
            const double Time = StrategyBaseType::GetModelPart().GetProcessInfo()[TIME];
            std::cout.precision(4);
            std::cout << "|----------------------------------------------------|" << std::endl;
            #if !defined(_WIN32)
                std::cout << "|     " << BOLDFONT("Max. iter. exceeded: SPLITTING TIME STEP") << "       |" << std::endl;
                std::cout << "| " << BOLDFONT("COMING BACK TO TIME: ") << std::scientific << Time << "                    |" << std::endl;
                std::cout << "| " << BOLDFONT("      NEW TIME STEP: ") << std::scientific << AuxDeltaTime << "                    |" << std::endl;
            #else
                std::cout << "|     Max. iter. exceeded: SPLITTING TIME STEP       |" << std::endl;
                std::cout << "| COMING BACK TO TIME: " << std::scientific << Time << "                    |" << std::endl;
                std::cout << "|       NEW TIME STEP: " << std::scientific << AuxDeltaTime << "                    |" << std::endl;
            #endif
            std::cout << "|----------------------------------------------------|" << std::endl;
        }
    }
    
    /**
     * This method prints information after reach the max number of interations
     */
    /*
    void MaxIterationsExceeded() override
    {
        if (mConvergenceCriteriaEchoLevel > 0 && StrategyBaseType::GetModelPart().GetCommunicator().MyPID() == 0 )
        {
            std::cout << "|----------------------------------------------------|" << std::endl;
            #if !defined(_WIN32)
                std::cout << "|        " << BOLDFONT(FRED("ATTENTION: Max iterations exceeded")) << "          |" << std::endl;
            #else
                std::cout << "|        ATTENTION: Max iterations exceeded          |" << std::endl;
            #endif
            std::cout << "|----------------------------------------------------|" << std::endl;
        }
    }
    
    /**
     * This method prints information after reach the max number of interations and splits
     */
      /*  
    void MaxIterationsAndSplitsExceeded()
    {
        if (mConvergenceCriteriaEchoLevel > 0 && StrategyBaseType::GetModelPart().GetCommunicator().MyPID() == 0 )
        {
            std::cout << "|----------------------------------------------------|" << std::endl;
            #if !defined(_WIN32)
                std::cout << "|        " << BOLDFONT(FRED("ATTENTION: Max iterations exceeded")) << "          |" << std::endl;
                std::cout << "|        " << BOLDFONT(FRED("   Max number of splits exceeded  ")) << "          |" << std::endl;
            #else
                std::cout << "|        ATTENTION: Max iterations exceeded          |" << std::endl;
                std::cout << "|           Max number of splits exceeded            |" << std::endl;
            #endif
            std::cout << "|----------------------------------------------------|" << std::endl;
        }
    }*/

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
    ///@{

    /** 
     * Copy constructor.
     */
    
    ResidualBasedNewtonRaphsonContactStrategyRemeshing(const ResidualBasedNewtonRaphsonContactStrategyRemeshing& Other)
    {
    };

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

    ///@}
    ///@name Serialization
    ///@{

    ///@name Private Inquiry
    ///@{
    ///@}
    ///@name Un accessible methods
    ///@{
    ///@}

}; /* Class ResidualBasedNewtonRaphsonContactStrategyRemeshing */
///@}
///@name Type Definitions
///@{
///@}
///@name Input and output
///@{
///@}
}  // namespace Kratos

#endif /* KRATOS_RESIDUALBASED_NEWTON_RAPHSON_CONTACT_STRATEGY_REMESHING */
