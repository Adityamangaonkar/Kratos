// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:     BSD License
//           license: structural_mechanics_application/license.txt
//
//  Main authors:    Martin Fusseder, https://github.com/MFusseder 
//


#include "cr_beam_adjoint_element_3D2N.hpp"
#include "structural_mechanics_application_variables.h"
#include "custom_response_functions/response_utilities/response_data.h"
#include "includes/checks.h"


namespace Kratos
{

    CrBeamAdjointElement3D2N::CrBeamAdjointElement3D2N(IndexType NewId,
        GeometryType::Pointer pGeometry)
        : CrBeamElementLinear3D2N(NewId, pGeometry)
    {
    }

    CrBeamAdjointElement3D2N::CrBeamAdjointElement3D2N(IndexType NewId,
        GeometryType::Pointer pGeometry,
        PropertiesType::Pointer pProperties)
        : CrBeamElementLinear3D2N(NewId, pGeometry, pProperties)
    {
    }

    Element::Pointer CrBeamAdjointElement3D2N::Create(IndexType NewId,
        NodesArrayType const& rThisNodes,
        PropertiesType::Pointer pProperties) const
    {
        const GeometryType& rGeom = this->GetGeometry();
        return Kratos::make_shared<CrBeamAdjointElement3D2N>(
            NewId, rGeom.Create(rThisNodes), pProperties);
    }

    Element::Pointer CrBeamAdjointElement3D2N::Create(IndexType NewId,
            GeometryType::Pointer pGeom,
            PropertiesType::Pointer pProperties) const
    {
        return Kratos::make_shared<CrBeamAdjointElement3D2N>(NewId, pGeom, pProperties);
    }

    CrBeamAdjointElement3D2N::~CrBeamAdjointElement3D2N() {}

    void CrBeamAdjointElement3D2N::EquationIdVector(EquationIdVectorType& rResult,
        ProcessInfo& rCurrentProcessInfo) {

        const int number_of_nodes = this->GetGeometry().PointsNumber();
        const int dimension = this->GetGeometry().WorkingSpaceDimension();
        const unsigned int local_size = number_of_nodes * dimension * 2;

        if (rResult.size() != local_size) rResult.resize(local_size);

        for (int i = 0; i < number_of_nodes; ++i)
        {
            int index = i * number_of_nodes * dimension;
            rResult[index] = this->GetGeometry()[i].GetDof(ADJOINT_DISPLACEMENT_X)
                .EquationId();
            rResult[index + 1] = this->GetGeometry()[i].GetDof(ADJOINT_DISPLACEMENT_Y)
                .EquationId();
            rResult[index + 2] = this->GetGeometry()[i].GetDof(ADJOINT_DISPLACEMENT_Z)
                .EquationId();

            rResult[index + 3] = this->GetGeometry()[i].GetDof(ADJOINT_ROTATION_X)
                .EquationId();
            rResult[index + 4] = this->GetGeometry()[i].GetDof(ADJOINT_ROTATION_Y)
                .EquationId();
            rResult[index + 5] = this->GetGeometry()[i].GetDof(ADJOINT_ROTATION_Z)
                .EquationId();
        }

    }

    void CrBeamAdjointElement3D2N::GetDofList(DofsVectorType& rElementalDofList,
        ProcessInfo& rCurrentProcessInfo) {

        const int number_of_nodes = this->GetGeometry().PointsNumber();
        const int dimension = this->GetGeometry().WorkingSpaceDimension();
        const unsigned int local_size = number_of_nodes * dimension * 2;

        if (rElementalDofList.size() != local_size) {
            rElementalDofList.resize(local_size);
        }

        for (int i = 0; i < number_of_nodes; ++i)
        {
            int index = i * number_of_nodes * dimension;
            rElementalDofList[index] = this->GetGeometry()[i]
                .pGetDof(ADJOINT_DISPLACEMENT_X);
            rElementalDofList[index + 1] = this->GetGeometry()[i]
                .pGetDof(ADJOINT_DISPLACEMENT_Y);
            rElementalDofList[index + 2] = this->GetGeometry()[i]
                .pGetDof(ADJOINT_DISPLACEMENT_Z);

            rElementalDofList[index + 3] = this->GetGeometry()[i]
                .pGetDof(ADJOINT_ROTATION_X);
            rElementalDofList[index + 4] = this->GetGeometry()[i]
                .pGetDof(ADJOINT_ROTATION_Y);
            rElementalDofList[index + 5] = this->GetGeometry()[i]
                .pGetDof(ADJOINT_ROTATION_Z);
        }
    }

    double CrBeamAdjointElement3D2N::GetDisturbanceMeasureCorrectionFactor(const Variable<double>& rDesignVariable)
    {
        KRATOS_TRY;

        if ( this->GetProperties().Has(rDesignVariable) )
        {
            const double variable_value = this->GetProperties()[rDesignVariable];
            return variable_value;
        }
        else
            return 1.0;

        KRATOS_CATCH("")
    }

    double CrBeamAdjointElement3D2N::GetDisturbanceMeasureCorrectionFactor(const Variable<array_1d<double,3>>& rDesignVariable)
    {
        KRATOS_TRY;

        if(rDesignVariable == SHAPE_SENSITIVITY)
        {
            double dx = this->GetGeometry()[1].X0() - this->GetGeometry()[0].X0();
            double dy = this->GetGeometry()[1].Y0() - this->GetGeometry()[0].Y0();
            double dz = this->GetGeometry()[1].Z0() - this->GetGeometry()[0].Z0();
            double L = std::sqrt(dx*dx + dy*dy + dz*dz);
            return L;
        }
        else
            return 1.0;

        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::CalculateSensitivityMatrix(const Variable<double>& rDesignVariable, Matrix& rOutput,
                                            const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;
        // define working variables
        Vector RHS_undist;
        Vector RHS_dist;
        ProcessInfo copy_of_process_info = rCurrentProcessInfo;

        // Compute RHS before disturbing
        this->CalculateRightHandSide(RHS_undist, copy_of_process_info);

        rOutput.resize(1,RHS_undist.size());

        // Get disturbance measure
        double delta = this->GetValue(DISTURBANCE_MEASURE);
        double correction_factor = this->GetDisturbanceMeasureCorrectionFactor(rDesignVariable);
        delta *= correction_factor;

        if ( this->GetProperties().Has(rDesignVariable) )
        {
            // Save properties and its pointer
            Properties& r_global_property = this->GetProperties();
            Properties::Pointer p_global_properties = this->pGetProperties();

            // Create new property and assign it to the element
            Properties::Pointer p_local_property(new Properties(r_global_property));
            this->SetProperties(p_local_property);

            // Disturb the design variable
            const double current_property_value = this->GetProperties()[rDesignVariable];
            p_local_property->SetValue(rDesignVariable, (current_property_value + delta));

            // Compute RHS after disturbance
            this->CalculateRightHandSide(RHS_dist, copy_of_process_info);

            rOutput.resize(1,RHS_dist.size());

            // Compute derivative of RHS w.r.t. design variable with finite differences
            noalias(RHS_dist) -= RHS_undist;
            RHS_dist /= delta;
            for(unsigned int i = 0; i < RHS_dist.size(); i++)
                rOutput(0, i) = RHS_dist[i];

            // Give element original properties back
            this->SetProperties(p_global_properties);
        }
        else
        {
            rOutput.clear();
        }

        KRATOS_CATCH("")

    }

    void CrBeamAdjointElement3D2N::CalculateSensitivityMatrix(const Variable<array_1d<double,3>>& rDesignVariable, Matrix& rOutput,
                                            const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;

        // define working variables
        Vector RHS_undist;
        Vector RHS_dist;
        ProcessInfo copy_of_process_info = rCurrentProcessInfo;

        // Get disturbance measure
        double delta = this->GetValue(DISTURBANCE_MEASURE);
        double correction_factor = this->GetDisturbanceMeasureCorrectionFactor(rDesignVariable);
        delta *= correction_factor;

        if(rDesignVariable == SHAPE_SENSITIVITY)
        {
            const int number_of_nodes = GetGeometry().PointsNumber();
            const unsigned int dimension = rCurrentProcessInfo.GetValue(DOMAIN_SIZE);
            const int local_size = number_of_nodes * dimension * 2;
            
            rOutput.resize(dimension * number_of_nodes, local_size);

            // compute RHS before disturbing
            this->CalculateRightHandSide(RHS_undist, copy_of_process_info);

            int index = 0;
            //TODO: look that this works also for parallel computing
            for(auto& node_i : this->GetGeometry())
            {
                for(std::size_t coord_dir_i = 0; coord_dir_i < dimension; coord_dir_i++)
                {
                    // disturb the design variable
                    node_i.GetInitialPosition()[coord_dir_i] += delta;

                    // compute RHS after disturbance
                    this->CalculateRightHandSide(RHS_dist, copy_of_process_info);

                    //compute derivative of RHS w.r.t. design variable with finite differences
                    noalias(RHS_dist) -= RHS_undist;
                    RHS_dist /= delta;
                    for(unsigned int i = 0; i < RHS_dist.size(); i++)
                        rOutput( (coord_dir_i + index*dimension), i) = RHS_dist[i]; 

                    // Reset pertubed vector
                    RHS_dist = Vector(0);

                    // undisturb the design variable
                    node_i.GetInitialPosition()[coord_dir_i] -= delta;
                }
                index++;
            }// end loop over element nodes
        }
        else
            KRATOS_ERROR << "Unsupported design variable!" << std::endl;

        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::Calculate(const Variable<Vector >& rVariable,
                           Vector& rOutput,
                           const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;

        if(rVariable == STRESS_ON_GP || rVariable == STRESS_ON_NODE)
        {
            TracedStressType traced_stress_type = static_cast<TracedStressType>(this->GetValue(TRACED_STRESS_TYPE));

            std::vector< array_1d<double, 3 > > stress_vector;
            int direction_1 = 0;
            bool stress_is_moment = true;
  
            switch (traced_stress_type)  
            { 
                case MX:
                {
                    direction_1 = 0; 
                    break;
                }
                case MY:
                {
                    direction_1 = 1; 
                    break; 
                }
                case MZ:
                {
                    direction_1 = 2; 
                    break; 
                }
                case FX:
                {
                    direction_1 = 0; 
                    stress_is_moment = false;
                    break;
                }	
                case FY:
                {
                    direction_1 = 1; 
                    stress_is_moment = false;
                    break;
                }
                case FZ:
                {
                    direction_1 = 2; 
                    stress_is_moment = false;
                    break;
                }
                default:
                    KRATOS_ERROR << "Invalid stress type! Stress type not supported for this element!" << std::endl;  
            }

            if(stress_is_moment)
                CrBeamElementLinear3D2N::CalculateOnIntegrationPoints(MOMENT, stress_vector, rCurrentProcessInfo);
            else
                CrBeamElementLinear3D2N::CalculateOnIntegrationPoints(FORCE, stress_vector, rCurrentProcessInfo);
    
            if(rVariable == STRESS_ON_GP)
            {
                const unsigned int&  GP_num = GetGeometry().IntegrationPointsNumber(Kratos::GeometryData::GI_GAUSS_3);

                rOutput.resize(GP_num);
                for(unsigned int i = 0; i < GP_num ; i++)
                {
                    rOutput(i) = stress_vector[i][direction_1];
                }
            }
            else if(rVariable == STRESS_ON_NODE)
            {
                rOutput.resize(2);
                rOutput(0) = 2 * stress_vector[0][direction_1] - stress_vector[1][direction_1];
                rOutput(1) = 2 * stress_vector[2][direction_1] - stress_vector[1][direction_1];
            }
        }
        else
        {
            rOutput.resize(3);
            rOutput.clear();
        }

        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::Calculate(const Variable<Matrix >& rVariable,
                           Matrix& rOutput,
                           const ProcessInfo& rCurrentProcessInfo)
    {
           KRATOS_TRY;


        if(rVariable == STRESS_DISP_DERIV_ON_GP)
        {
               this->CalculateStressDisplacementDerivative(STRESS_ON_GP, rOutput, rCurrentProcessInfo);
        }
        else if(rVariable == STRESS_DISP_DERIV_ON_NODE)
        {
            this->CalculateStressDisplacementDerivative(STRESS_ON_NODE, rOutput, rCurrentProcessInfo);
        }
        else if(rVariable == STRESS_DESIGN_DERIVATIVE_ON_GP)
        {
            const std::string design_varible_name = this->GetValue( DESIGN_VARIABLE_NAME );

            if (KratosComponents<Variable<double>>::Has(design_varible_name) == true)
            {
                const Variable<double>& r_variable =
                    KratosComponents<Variable<double>>::Get(design_varible_name);
                this->CalculateStressDesignVariableDerivative(r_variable, STRESS_ON_GP, rOutput, rCurrentProcessInfo);
            }
            else if (KratosComponents<Variable<array_1d<double, 3>>>::Has(design_varible_name) == true)
            {
                const Variable<array_1d<double, 3>>& r_variable =
                    KratosComponents<Variable<array_1d<double, 3>>>::Get(design_varible_name);
                this->CalculateStressDesignVariableDerivative(r_variable, STRESS_ON_GP, rOutput, rCurrentProcessInfo);
            }
        }
        else if(rVariable == STRESS_DESIGN_DERIVATIVE_ON_NODE)
        {
            std::string design_varible_name = this->GetValue( DESIGN_VARIABLE_NAME );

            if (KratosComponents<Variable<double>>::Has(design_varible_name) == true)
            {
                const Variable<double>& r_variable =
                    KratosComponents<Variable<double>>::Get(design_varible_name);
                this->CalculateStressDesignVariableDerivative(r_variable, STRESS_ON_NODE, rOutput, rCurrentProcessInfo);
            }
            else if (KratosComponents<Variable<array_1d<double, 3>>>::Has(design_varible_name) == true)
            {
                const Variable<array_1d<double, 3>>& r_variable =
                    KratosComponents<Variable<array_1d<double, 3>>>::Get(design_varible_name);
                this->CalculateStressDesignVariableDerivative(r_variable, STRESS_ON_NODE, rOutput, rCurrentProcessInfo);
            }
        }
           else
        {
            rOutput.clear();
        }

        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::CalculateStressDisplacementDerivative(const Variable<Vector>& rStressVariable,
                                    Matrix& rOutput, const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;

        const int num_nodes = this->GetGeometry().PointsNumber();
        const int dimension = this->GetGeometry().WorkingSpaceDimension();
        const int num_dofs = num_nodes * dimension * 2;
        ProcessInfo copy_process_info = rCurrentProcessInfo;

        Vector initial_state_variables;
        initial_state_variables.resize(num_dofs);
        Vector stress_derivatives_vector;

        Vector dummy;
        this->Calculate(rStressVariable, dummy, rCurrentProcessInfo);
        rOutput.resize(num_dofs, dummy.size() );
        rOutput.clear();
        
        // Built vector of variables containing the DOF-variables of the primal problem 
        std::vector<VariableComponent<VectorComponentAdaptor<array_1d<double, 3>>>> primal_solution_variable_list;
        primal_solution_variable_list.push_back(DISPLACEMENT_X);       
        primal_solution_variable_list.push_back(DISPLACEMENT_Y);       
        primal_solution_variable_list.push_back(DISPLACEMENT_Z);       
        primal_solution_variable_list.push_back(ROTATION_X);       
        primal_solution_variable_list.push_back(ROTATION_Y);       
        primal_solution_variable_list.push_back(ROTATION_Z);       

        for (int i = 0; i < num_nodes; i++) 
        {	
            int index = i * dimension * 2;
            for(unsigned int j = 0; j < primal_solution_variable_list.size(); j++)
            {
                initial_state_variables[index + j] = this->GetGeometry()[i].FastGetSolutionStepValue(primal_solution_variable_list[j]);
                this->GetGeometry()[i].FastGetSolutionStepValue(primal_solution_variable_list[j]) = 0.0;
            }
        }

        for (int i = 0; i < num_nodes; i++) 
        {	
            int index = i * dimension * 2;
            for(unsigned int j = 0; j < primal_solution_variable_list.size(); j++)
            {
                this->GetGeometry()[i].FastGetSolutionStepValue(primal_solution_variable_list[j]) = 1.0;
                
                this->Calculate(rStressVariable, stress_derivatives_vector, rCurrentProcessInfo);
                
                for(unsigned int k = 0; k < stress_derivatives_vector.size(); k++)
                    rOutput(index+j, k) = stress_derivatives_vector[k];
                
                stress_derivatives_vector.clear();
                
                this->GetGeometry()[i].FastGetSolutionStepValue(primal_solution_variable_list[j]) = 0.0;
            }
        }
       
        for (int i = 0; i < num_nodes; i++) 
        {	
            int index = i * dimension * 2;
            for(unsigned int j = 0; j < primal_solution_variable_list.size(); j++)
                this->GetGeometry()[i].FastGetSolutionStepValue(primal_solution_variable_list[j]) = initial_state_variables[index + j];
        }

        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::CalculateStressDesignVariableDerivative(const Variable<double>& rDesignVariable,
                                        const Variable<Vector>& rStressVariable, Matrix& rOutput,
                                            const ProcessInfo& rCurrentProcessInfo)
    {
         KRATOS_TRY;

        // Define working variables
        Vector stress_vector_undist;
        Vector stress_vector_dist;
        ProcessInfo copy_process_info = rCurrentProcessInfo;

        // Get disturbance measure
        double delta= this->GetValue(DISTURBANCE_MEASURE);
        double correction_factor = this->GetDisturbanceMeasureCorrectionFactor(rDesignVariable);
        delta *= correction_factor;

        // Compute stress before disturbance
        this->Calculate(rStressVariable, stress_vector_undist, rCurrentProcessInfo);

        const int stress_vector_size = stress_vector_undist.size();
        rOutput.resize(1, stress_vector_size);

        if( this->GetProperties().Has(rDesignVariable) )
        {
            // Save properties and its pointer
            Properties& r_global_property = this->GetProperties();
            Properties::Pointer p_global_properties = this->pGetProperties();

            // Create new property and assign it to the element
            Properties::Pointer p_local_property(new Properties(r_global_property));
            this->SetProperties(p_local_property);

            // Disturb the design variable
            const double current_property_value = this->GetProperties()[rDesignVariable];
            p_local_property->SetValue(rDesignVariable, (current_property_value + delta));

            // Compute stress on GP after disturbance
            this->Calculate(rStressVariable, stress_vector_dist, rCurrentProcessInfo);

            // Compute derivative of stress w.r.t. design variable with finite differences
            noalias(stress_vector_dist)  -= stress_vector_undist;
            stress_vector_dist  /= delta;

            for(int j = 0; j < stress_vector_size; j++)
                rOutput(0, j) = stress_vector_dist[j];

            // Give element original properties back
            this->SetProperties(p_global_properties);
        }
        else
            rOutput.clear();

        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::CalculateStressDesignVariableDerivative(const Variable<array_1d<double,3>>& rDesignVariable,
                                            const Variable<Vector>& rStressVariable,
                                            Matrix& rOutput, const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;

        // define working variables
        Vector stress_vector_undist;
        Vector stress_vector_dist;
        ProcessInfo copy_process_info = rCurrentProcessInfo;

        // Get disturbance measure
        double delta= this->GetValue(DISTURBANCE_MEASURE);
        double correction_factor = this->GetDisturbanceMeasureCorrectionFactor(rDesignVariable);
        delta *= correction_factor;

        if(rDesignVariable == SHAPE_SENSITIVITY)
        {
            const int number_of_nodes = GetGeometry().PointsNumber();
            const unsigned int dimension = rCurrentProcessInfo.GetValue(DOMAIN_SIZE);

            // Compute stress on GP before disturbance
            this->Calculate(rStressVariable, stress_vector_undist, rCurrentProcessInfo);

            const int stress_vector_size = stress_vector_undist.size();

            rOutput.resize(dimension * number_of_nodes, stress_vector_size);

            int index = 0;
            //TODO: look that this works also for parallel computing
            for(auto& node_i : this->GetGeometry())
            {
                for(std::size_t coord_dir_i = 0; coord_dir_i < dimension; coord_dir_i++)
                {
                    // Disturb the design variable
                    node_i.GetInitialPosition()[coord_dir_i] += delta;

                    // Compute stress on GP after disturbance
                    this->Calculate(rStressVariable, stress_vector_dist, rCurrentProcessInfo);

                    // Compute derivative of stress w.r.t. design variable with finite differences
                    noalias(stress_vector_dist)  -= stress_vector_undist;
                    stress_vector_dist  /= delta;

                    for(int i = 0; i < stress_vector_size; i++)
                        rOutput( (coord_dir_i + index*dimension), i) = stress_vector_dist[i];

                    // Reset pertubed vector
                    stress_vector_dist = Vector(0);

                    // Undisturb the design variable
                    node_i.GetInitialPosition()[coord_dir_i] -= delta;
                }
                index++;
            }// end loop over element nodes
        }
        else
            KRATOS_ERROR << "Unsupported design variable!" << std::endl;

        KRATOS_CATCH("")
    }


    void CrBeamAdjointElement3D2N::CalculateOnIntegrationPoints(const Variable<double>& rVariable,
                          std::vector<double>& rOutput,
                          const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;

        // Resize Output
        const unsigned int&  write_points_number = GetGeometry()
            .IntegrationPointsNumber(GetIntegrationMethod());
        if (rOutput.size() != write_points_number)
            rOutput.resize(write_points_number);

        if(this->Has(rVariable))
        {
            // Get result value for output
            double output_value = this->GetValue(rVariable);

            // Write scalar result value on all Gauss-Points
            for(unsigned int i = 0; i < write_points_number; ++i)
                rOutput[i] = output_value;
        }
        else if (rVariable == TRUSS_ELONGATION_MODE)
        {
            const double elongation = CalculateFirstOrderElongation();

            // Write scalar result value on all Gauss-Points
            for(unsigned int i = 0; i < write_points_number; ++i)
                rOutput[i] = elongation;
        }
        else
            KRATOS_ERROR << "Unsupported output variable." << std::endl;


        KRATOS_CATCH("")

    }

    void CrBeamAdjointElement3D2N::CalculateOnIntegrationPoints(
			const Variable<array_1d<double, 3 > >& rVariable,
			std::vector< array_1d<double, 3 > >& rOutput,
			const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;
        
        const unsigned int &write_points_number =
        GetGeometry().IntegrationPointsNumber(GetIntegrationMethod());
        if (rOutput.size() != write_points_number) 
            rOutput.resize(write_points_number);

        double length = this->CalculateReferenceLength();     

        // rOutput[GP 1,2,3][x,y,z]
        if(rVariable == BEAM_BENDING_MODES) 
        {
            const double numerical_limit = std::numeric_limits<double>::epsilon();
            Vector rot_node_1 = this->GetGeometry()[0].FastGetSolutionStepValue(ADJOINT_ROTATION, 0);
            Vector rot_node_2 = this->GetGeometry()[1].FastGetSolutionStepValue(ADJOINT_ROTATION, 0);

            Vector delta_x = ZeroVector(msDimension);

            BoundedVector<double, msLocalSize> current_nodal_position = this->GetCurrentNodalPosition();

            for (unsigned int i = 0; i < msDimension; ++i)
                delta_x[i] = current_nodal_position[msDimension + i] - current_nodal_position[i];

            // Calculate slopes between deformed node 1 and 2 in x-, y- and z-direction
            Vector slopes = ZeroVector(msDimension);
            if (delta_x[1] > numerical_limit)
                slopes[0] = delta_x[2]/delta_x[1]; 
            if (delta_x[0] > numerical_limit)  
                slopes[1]  = -delta_x[2]/delta_x[0];
            if (delta_x[1] > numerical_limit)     
                slopes[2]  = -delta_x[0]/delta_x[1];

            // Calculate differences between rotation of nodes and slopes
            Vector diff_phi_node_1 = ZeroVector(msDimension);
            diff_phi_node_1 = rot_node_1 - slopes; 
            Vector diff_phi_node_2 = ZeroVector(msDimension);
            diff_phi_node_2 = rot_node_2 - slopes;

            // Define deformation mode of bending around x-axis
            Vector def_mode_bending_x = ZeroVector(2);
            def_mode_bending_x[0] = diff_phi_node_1[0];
            def_mode_bending_x[1] = diff_phi_node_2[0];
            // Define deformation mode of bending around y-axis
            Vector def_mode_bending_y = ZeroVector(2);
            def_mode_bending_y[0] = diff_phi_node_1[1];
            def_mode_bending_y[1] = diff_phi_node_2[1];
            // Define deformation mode of bending around z-axis
            Vector def_mode_bending_z = ZeroVector(2);
            def_mode_bending_z[0] = diff_phi_node_1[2];
            def_mode_bending_z[1] = diff_phi_node_2[2];

            // Hard coded correction TODO: look for a smart solution!
            if(Has(TRACED_STRESS_TYPE))
                def_mode_bending_y[0] += 1.0;

            Vector x_mode_internal = CalculateBendingDeformationModesOnGP(def_mode_bending_x);
            Vector y_mode_internal = CalculateBendingDeformationModesOnGP(def_mode_bending_y);
            Vector z_mode_internal = CalculateBendingDeformationModesOnGP(def_mode_bending_z);

            // Write output on GP
            if(rOutput.size() == x_mode_internal.size())
            {
                for(unsigned int  j = 0; j < rOutput.size(); j++)
                  rOutput[j][0] =  x_mode_internal[j];
            }
            if(rOutput.size() == y_mode_internal.size())
            {
                for(unsigned int  j = 0; j < rOutput.size(); j++)
                  rOutput[j][1] =  y_mode_internal[j];
            }
            if(rOutput.size() == z_mode_internal.size())
            {
                for(unsigned int  j = 0; j < rOutput.size(); j++)
                  rOutput[j][2] =  z_mode_internal[j];
            }

    
            //Verify results+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++   
            //Compute Sensitivities with deformation modes
            Matrix pseudo_load_I22;
            CalculateSensitivityMatrix(I22, pseudo_load_I22, rCurrentProcessInfo);
            double sensitivity_wrt_I22 =  pseudo_load_I22(0,4)*def_mode_bending_y[0] + pseudo_load_I22(0,10)*def_mode_bending_y[1];

            double reference_I22_sensitivity = this->GetValue(I22_SENSITIVITY);

            // check if relative deviation < 8%
            KRATOS_ERROR_IF(std::abs((-reference_I22_sensitivity+sensitivity_wrt_I22)/reference_I22_sensitivity) > 0.08)
                << "Wrong sensitivity computation" << std::endl;
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++      
    
        }
        else if (rVariable == PSEUDO_LOAD_1) 
        {
            Matrix sensitivity_matrix; 
            this->CalculateSensitivityMatrix(CROSS_AREA, sensitivity_matrix, rCurrentProcessInfo);   
            double value_left = sensitivity_matrix(0,0);
            double function_prefactor = 6 * value_left / length;

            rOutput[0][0] = function_prefactor * 0.75 - function_prefactor * 0.25;
            rOutput[1][0] = function_prefactor * 0.50 - function_prefactor * 0.50;
            rOutput[2][0] = function_prefactor * 0.25 - function_prefactor * 0.75;

            rOutput[0][1] = 0.0;
            rOutput[1][1] = 0.0;
            rOutput[2][1] = 0.0;

            rOutput[0][2] = 0.0;
            rOutput[1][2] = 0.0;
            rOutput[2][2] = 0.0;
            
            // Check continuous pseudo-load by computing the the L_2-product between the influence function and pseudo-load******
            Vector boundary_values_pseudo_load;
            boundary_values_pseudo_load.resize(2);
            boundary_values_pseudo_load[0] = function_prefactor;
            boundary_values_pseudo_load[1] = -1.0 * function_prefactor;

            unsigned int NumNodes = this->GetGeometry().PointsNumber();

            unsigned int num_gauss_points = this->GetGeometry().IntegrationPointsNumber(GeometryData::GI_GAUSS_2);
            Vector DetJ = ZeroVector(num_gauss_points);
            Vector integration_weigths;
            integration_weigths.resize(num_gauss_points);

            this->GetGeometry().DeterminantOfJacobian(DetJ,GeometryData::GI_GAUSS_2); 

            const Matrix NContainer = this->GetGeometry().ShapeFunctionsValues(GeometryData::GI_GAUSS_2);

            const auto& IntegrationPoints = this->GetGeometry().IntegrationPoints(GeometryData::GI_GAUSS_2);

            double analytic_sensitivity = 0.0;

            for (unsigned int g = 0; g < num_gauss_points; ++g)
            {
                const Kratos::Vector& N = row(NContainer,g);
                const double GaussWeight = DetJ[g] * IntegrationPoints[g].Weight();

                for (unsigned int i=0; i<NumNodes; ++i)
                {
                    for (unsigned int j=0; j<NumNodes; ++j)
                    {
                        analytic_sensitivity += GaussWeight * N[i] * N[j] *
                        this->GetGeometry()[i].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_X) * boundary_values_pseudo_load[j];
                    }

                }
            }

            KRATOS_ERROR_IF(std::abs(analytic_sensitivity - this->GetValue(CROSS_AREA_SENSITIVITY) > 1e-12 ))
                << "Analytic scalarproduct is unequal to the corresponding discrete one!" << std::endl;
            //************************************************

        }

        KRATOS_CATCH("")
    }

	void CrBeamAdjointElement3D2N::GetValueOnIntegrationPoints(
			const Variable<array_1d<double, 3 > >& rVariable,
			std::vector< array_1d<double, 3 > >& rOutput,
			const ProcessInfo& rCurrentProcessInfo) 
    {
        KRATOS_TRY;
        this->CalculateOnIntegrationPoints(rVariable, rOutput, rCurrentProcessInfo);
        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::GetValueOnIntegrationPoints(const Variable<double>& rVariable,
                         std::vector<double>& rValues,
                         const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY;
        this->CalculateOnIntegrationPoints(rVariable, rValues, rCurrentProcessInfo);
        KRATOS_CATCH("")
    }

    void CrBeamAdjointElement3D2N::GetValuesVector(Vector& rValues, int Step) {

        KRATOS_TRY
            const int number_of_nodes = this->GetGeometry().PointsNumber();
        const int dimension = this->GetGeometry().WorkingSpaceDimension();
        const unsigned int element_size = number_of_nodes * dimension * 2;

        if (rValues.size() != element_size) rValues.resize(element_size, false);

        for (int i = 0; i < number_of_nodes; ++i)
        {
            int index = i * dimension * 2;
            rValues[index] = this->GetGeometry()[i]
                .FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_X, Step);
            rValues[index + 1] = this->GetGeometry()[i]
                .FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Y, Step);
            rValues[index + 2] = this->GetGeometry()[i]
                .FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Z, Step);

            rValues[index + 3] = this->GetGeometry()[i]
                .FastGetSolutionStepValue(ADJOINT_ROTATION_X, Step);
            rValues[index + 4] = this->GetGeometry()[i]
                .FastGetSolutionStepValue(ADJOINT_ROTATION_Y, Step);
            rValues[index + 5] = this->GetGeometry()[i]
                .FastGetSolutionStepValue(ADJOINT_ROTATION_Z, Step);
        }
        KRATOS_CATCH("")
    }

    int CrBeamAdjointElement3D2N::Check(const ProcessInfo& rCurrentProcessInfo)
    {
        KRATOS_TRY

        KRATOS_ERROR_IF(GetGeometry().WorkingSpaceDimension() != 3 || GetGeometry().size() != 2)
        << "The beam element works only in 3D and with 2 noded elements" << "" << std::endl;

        // verify that the variables are correctly initialized
        KRATOS_CHECK_VARIABLE_KEY(VELOCITY);
        KRATOS_CHECK_VARIABLE_KEY(DISPLACEMENT);
        KRATOS_CHECK_VARIABLE_KEY(ACCELERATION);
        KRATOS_CHECK_VARIABLE_KEY(DENSITY);
        KRATOS_CHECK_VARIABLE_KEY(CROSS_AREA);
        KRATOS_CHECK_VARIABLE_KEY(ADJOINT_DISPLACEMENT);
        KRATOS_CHECK_VARIABLE_KEY(ADJOINT_ROTATION);

        // check properties
        KRATOS_ERROR_IF(this->GetProperties().Has(CROSS_AREA) == false || this->GetProperties()[CROSS_AREA] == 0)
        << "CROSS_AREA not provided for this element" << this->Id() << std::endl;

        KRATOS_ERROR_IF(this->GetProperties().Has(YOUNG_MODULUS) == false || this->GetProperties()[YOUNG_MODULUS] == 0)
        << "YOUNG_MODULUS not provided for this element" << this->Id() << std::endl;

        KRATOS_ERROR_IF_NOT( this->GetProperties().Has(DENSITY) )
        << "DENSITY not provided for this element" << this->Id() << std::endl;

        KRATOS_ERROR_IF_NOT( this->GetProperties().Has(POISSON_RATIO) )
        << "POISSON_RATIO not provided for this element" << this->Id() << std::endl;

        KRATOS_ERROR_IF_NOT( this->GetProperties().Has(TORSIONAL_INERTIA) )
        << "TORSIONAL_INERTIA not provided for this element" << this->Id() << std::endl;
    
        KRATOS_ERROR_IF_NOT( this->GetProperties().Has(I22) )
        << "I22 not provided for this element" << this->Id() << std::endl;

        KRATOS_ERROR_IF_NOT( this->GetProperties().Has(I33) )
        << "I33 not provided for this element" << this->Id() << std::endl;

        // Check dofs
        GeometryType& r_geom = GetGeometry();
        for (unsigned int i = 0; i < r_geom.size(); i++)
        {
            auto& r_node = r_geom[i];

            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(DISPLACEMENT, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ROTATION, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ADJOINT_DISPLACEMENT, r_node);
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ADJOINT_ROTATION, r_node);

            KRATOS_CHECK_DOF_IN_NODE(ADJOINT_DISPLACEMENT_X, r_node);
            KRATOS_CHECK_DOF_IN_NODE(ADJOINT_DISPLACEMENT_Y, r_node);
            KRATOS_CHECK_DOF_IN_NODE(ADJOINT_DISPLACEMENT_Z, r_node);
            KRATOS_CHECK_DOF_IN_NODE(ADJOINT_ROTATION_X, r_node);
            KRATOS_CHECK_DOF_IN_NODE(ADJOINT_ROTATION_Y, r_node);
            KRATOS_CHECK_DOF_IN_NODE(ADJOINT_ROTATION_Z, r_node);
        }

        return 0;

        KRATOS_CATCH("")
    }

    CrBeamAdjointElement3D2N::IntegrationMethod CrBeamAdjointElement3D2N::GetIntegrationMethod() const 
    {
        // do this to have 5GP as an output in GID
        return Kratos::GeometryData::GI_GAUSS_5;
    }

    void CrBeamAdjointElement3D2N::save(Serializer& rSerializer) const
    {
        KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, CrBeamElementLinear3D2N);
    }

    void CrBeamAdjointElement3D2N::load(Serializer& rSerializer)
    {
        KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, CrBeamElementLinear3D2N);
    }

    BoundedVector<double, CrBeamAdjointElement3D2N::msLocalSize>
    CrBeamAdjointElement3D2N::GetCurrentNodalPosition() 
    {
        BoundedVector<double, msLocalSize> current_nodal_position = ZeroVector(msLocalSize);
        for (unsigned int i = 0; i < this->GetGeometry().PointsNumber(); ++i) 
        {
            int index = i * msDimension;
            current_nodal_position[index] =
                this->GetGeometry()[i].X0() +
                this->GetGeometry()[i].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_X, 0);
            current_nodal_position[index + 1] =
                this->GetGeometry()[i].Y0() +
                this->GetGeometry()[i].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Y, 0);
            current_nodal_position[index + 2] =
                this->GetGeometry()[i].Z0() +
                this->GetGeometry()[i].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Z, 0);   
        }

        return current_nodal_position;
    }

    double CrBeamAdjointElement3D2N::CalculateCurrentLength() 
    {
        KRATOS_TRY;
        const double du =
            this->GetGeometry()[1].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_X) -
            this->GetGeometry()[0].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_X);
        const double dv =
            this->GetGeometry()[1].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Y) -
            this->GetGeometry()[0].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Y);
        const double dw =
            this->GetGeometry()[1].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Z) -
            this->GetGeometry()[0].FastGetSolutionStepValue(ADJOINT_DISPLACEMENT_Z);
        const double dx = this->GetGeometry()[1].X0() - this->GetGeometry()[0].X0();
        const double dy = this->GetGeometry()[1].Y0() - this->GetGeometry()[0].Y0();
        const double dz = this->GetGeometry()[1].Z0() - this->GetGeometry()[0].Z0();
        const double l = std::sqrt((du + dx) * (du + dx) + (dv + dy) * (dv + dy) +
                                   (dw + dz) * (dw + dz));
        return l;
        KRATOS_CATCH("")
    } 

    Vector CrBeamAdjointElement3D2N::CalculateBendingDeformationModesOnGP(const Vector &DiscreteBendingDefMode)
    {
        KRATOS_TRY;

        const unsigned int &write_points_number = GetGeometry().IntegrationPointsNumber(GetIntegrationMethod());
        Vector bending_mode = ZeroVector(write_points_number);
        const double length = this->CalculateReferenceLength();     
        const double step = 1.0 / (write_points_number + 1.0);
        double current_position = 0.0;

        for(unsigned int i = 0; i < write_points_number; i++)
        {
            current_position += step ;
            bending_mode[i] = length * DiscreteBendingDefMode[0]*
                (-current_position + 2*current_position*current_position - current_position*current_position*current_position) +
                length * DiscreteBendingDefMode[1]*
                (current_position*current_position - current_position*current_position*current_position);  
        }

        return bending_mode;

        KRATOS_CATCH("")
    }

    double CrBeamAdjointElement3D2N::CalculateFirstOrderElongation()
    {
        KRATOS_TRY;

        const double numerical_limit = std::numeric_limits<double>::epsilon();
        const double L = this->CalculateReferenceLength();
     
        Vector undeformed_vector = ZeroVector(msDimension);
        undeformed_vector[0] = this->GetGeometry()[1].X0() - this->GetGeometry()[0].X0(); 
        undeformed_vector[1] = this->GetGeometry()[1].Y0() - this->GetGeometry()[0].Y0(); 
        undeformed_vector[2] = this->GetGeometry()[1].Z0() - this->GetGeometry()[0].Z0(); 
        
        Vector deformed_vector = ZeroVector(msDimension);
        BoundedVector<double, msLocalSize> current_nodal_position = this->GetCurrentNodalPosition();
        for (unsigned int i = 0; i < msDimension; ++i)
            deformed_vector[i] = current_nodal_position[msDimension + i] - current_nodal_position[i];

        // project deformed vector on the undeformed one  
        double length_relation = inner_prod(deformed_vector, undeformed_vector) / 
                                    inner_prod(undeformed_vector, undeformed_vector);

        double elongation = 0.0;
        if(std::abs(length_relation - 1) > numerical_limit)
            elongation = L * (length_relation - 1.0);
    
        return elongation;

        KRATOS_CATCH("")
    }

    //DEFORMATION MODES copied from cr beam *******************************************************************************
    /*BoundedVector<double, CrBeamAdjointElement3D2N::msLocalSize>
    CrBeamAdjointElement3D2N::CalculateDeformationModes() 
    {
        KRATOS_TRY;

        Vector bisectrix;
        Vector vector_difference;

        this->GetBisectrixAndVectorDiffernece(bisectrix, vector_difference);

        BoundedVector<double, msLocalSize> deformation_modes_total_v =
            ZeroVector(msLocalSize);
        const double L = this->CalculateReferenceLength();
        const double l = this->CalculateCurrentLength();

        Vector phi_s = CalculateSymmetricDeformationMode(vector_difference);
        Vector phi_a = CalculateAntiSymmetricDeformationMode(bisectrix);

        deformation_modes_total_v[3] = l - L;
        for (int i = 0; i < 3; ++i)
          deformation_modes_total_v[i] = phi_s[i];
        for (int i = 0; i < 2; ++i)
          deformation_modes_total_v[i + 4] = phi_a[i + 1];

        return deformation_modes_total_v;

        KRATOS_CATCH("")
    }

    Vector CrBeamAdjointElement3D2N::CalculateSymmetricDeformationMode(const Vector &VectorDifference) 
    {
        Vector phi_s = ZeroVector(msDimension);
        phi_s = prod(Matrix(trans(this->mLocalRotationMatrix)), VectorDifference);
        phi_s *= 4.00;
        
        return phi_s;
    }

    Vector CrBeamAdjointElement3D2N::CalculateAntiSymmetricDeformationMode(const Vector &Bisectrix) 
    {
        Vector phi_a = ZeroVector(msDimension);
      
        Vector rotated_nx0 = ZeroVector(msDimension);
        
        for (unsigned int i = 0; i < msDimension; ++i)
            rotated_nx0[i] = this->mLocalRotationMatrix(i, 0);

        Vector temp_vector = ZeroVector(msDimension);
        MathUtils<double>::CrossProduct(temp_vector, rotated_nx0, Bisectrix);
        phi_a = prod(Matrix(trans(this->mLocalRotationMatrix)), temp_vector);
        phi_a *= 4.00;
        
        return phi_a;
    }

    void CrBeamAdjointElement3D2N::GetBisectrixAndVectorDiffernece(Vector &Bisectrix, Vector &VectorDifference) 
    {
        KRATOS_TRY
        const double numerical_limit = std::numeric_limits<double>::epsilon();
        BoundedVector<double, msDimension> d_phi_a = ZeroVector(msDimension);
        BoundedVector<double, msDimension> d_phi_b = ZeroVector(msDimension);
        Vector increment_deformation;
        this->GetValuesVector(increment_deformation, 0); // in the linear case the increment is equal to the state 

        for (unsigned int i = 0; i < msDimension; ++i) 
        {
            d_phi_a[i] = increment_deformation[i + 3];
            d_phi_b[i] = increment_deformation[i + 9];
        }

        // calculating quaternions
        Vector drA_vec = ZeroVector(msDimension);
        Vector drB_vec = ZeroVector(msDimension);
        double drA_sca, drB_sca;

        drA_vec = 0.50 * d_phi_a;
        drB_vec = 0.50 * d_phi_b;

        drA_sca = 0.00;
        drB_sca = 0.00;
        for (unsigned int i = 0; i < msDimension; ++i) 
        {
            drA_sca += drA_vec[i] * drA_vec[i];
            drB_sca += drB_vec[i] * drB_vec[i];
        }
        drA_sca = 1.00 - drA_sca;
        drB_sca = 1.00 - drB_sca;

        drA_sca = std::sqrt(drA_sca);
        drB_sca = std::sqrt(drB_sca);

        Vector temp_vector = ZeroVector(msDimension);
        double temp_scalar = 0.00;

        Vector mQuaternionVEC_A = ZeroVector(msDimension);
		Vector mQuaternionVEC_B = ZeroVector(msDimension);
		double mQuaternionSCA_A = 1.00;
		double mQuaternionSCA_B = 1.00;

        // Node A
        temp_vector = mQuaternionVEC_A;
        temp_scalar = mQuaternionSCA_A;

        mQuaternionSCA_A = drA_sca * temp_scalar;
        for (unsigned int i = 0; i < msDimension; ++i) 
            mQuaternionSCA_A -= drA_vec[i] * temp_vector[i];
    
        mQuaternionVEC_A = drA_sca * temp_vector;
        mQuaternionVEC_A += temp_scalar * drA_vec;
        mQuaternionVEC_A +=
        MathUtils<double>::CrossProduct(drA_vec, temp_vector);

        // Node B
        temp_vector = mQuaternionVEC_B;
        temp_scalar = mQuaternionSCA_B;

        mQuaternionSCA_B = drB_sca * temp_scalar;
        for (unsigned int i = 0; i < msDimension; ++i) 
            mQuaternionSCA_B -= drB_vec[i] * temp_vector[i];
  

        mQuaternionVEC_B = drB_sca * temp_vector;
        mQuaternionVEC_B += temp_scalar * drB_vec;
        mQuaternionVEC_B += MathUtils<double>::CrossProduct(drB_vec, temp_vector);

        // scalar part of difference quaternion
        double scalar_diff;
        scalar_diff = (mQuaternionSCA_A + mQuaternionSCA_B) * (mQuaternionSCA_A + mQuaternionSCA_B);

        temp_vector = mQuaternionVEC_A + mQuaternionVEC_B;
        scalar_diff += MathUtils<double>::Norm(temp_vector) *
                       MathUtils<double>::Norm(temp_vector);

        scalar_diff = 0.50 * std::sqrt(scalar_diff);

        // mean rotation quaternion
        double mean_rotation_scalar;
        mean_rotation_scalar = (mQuaternionSCA_A + mQuaternionSCA_B) * 0.50;
        mean_rotation_scalar = mean_rotation_scalar / scalar_diff;

        BoundedVector<double, msDimension> mean_rotation_vector =
            ZeroVector(msDimension);
        mean_rotation_vector =
            (mQuaternionVEC_A + mQuaternionVEC_B) * 0.50;
        mean_rotation_vector = mean_rotation_vector / scalar_diff;

        // vector part of difference quaternion
        VectorDifference = mQuaternionSCA_A * mQuaternionVEC_B;
        VectorDifference -= mQuaternionSCA_B * mQuaternionVEC_A;
        VectorDifference += MathUtils<double>::CrossProduct(mQuaternionVEC_A,
                                                      mQuaternionVEC_B);

        VectorDifference = 0.50 * VectorDifference / scalar_diff;

        // rotate inital element basis
        const double r0 = mean_rotation_scalar;
        const double r1 = mean_rotation_vector[0];
        const double r2 = mean_rotation_vector[1];
        const double r3 = mean_rotation_vector[2];

        BoundedMatrix<double, msElementSize, msElementSize>
            reference_transformation = this->CalculateInitialLocalCS();
        Vector rotated_nx0 = ZeroVector(msDimension);
        Vector rotated_ny0 = ZeroVector(msDimension);
        Vector rotated_nz0 = ZeroVector(msDimension);
        for (SizeType i = 0; i < msDimension; ++i) 
        {
            rotated_nx0[i] = reference_transformation(i, 0);
            rotated_ny0[i] = reference_transformation(i, 1);
            rotated_nz0[i] = reference_transformation(i, 2);
        }

        Quaternion<double> q(r0, r1, r2, r3);
        q.RotateVector3(rotated_nx0);
        q.RotateVector3(rotated_ny0);
        q.RotateVector3(rotated_nz0);

        BoundedMatrix<double, msDimension, msDimension> rotated_coordinate_system =
            ZeroMatrix(msDimension, msDimension);
        for (unsigned int i = 0; i < msDimension; ++i) 
        {
            rotated_coordinate_system(i, 0) = rotated_nx0[i];
            rotated_coordinate_system(i, 1) = rotated_ny0[i];
            rotated_coordinate_system(i, 2) = rotated_nz0[i];
        }

        // rotate basis to element axis + redefine R
        Bisectrix = ZeroVector(msDimension);
        Vector delta_x = ZeroVector(msDimension);
        double vector_norm;

        BoundedVector<double, msLocalSize> current_nodal_position = this->GetCurrentNodalPosition();

        for (unsigned int i = 0; i < msDimension; ++i)
            delta_x[i] = current_nodal_position[msDimension + i] - current_nodal_position[i];

        vector_norm = MathUtils<double>::Norm(delta_x);
        if (vector_norm > numerical_limit)
            delta_x /= vector_norm;

        Bisectrix = rotated_nx0 + delta_x;
        vector_norm = MathUtils<double>::Norm(Bisectrix);
        if (vector_norm > numerical_limit)
            Bisectrix /= vector_norm;

        BoundedMatrix<double, msDimension, msDimension> n_xyz = ZeroMatrix(msDimension);
        for (unsigned int i = 0; i < msDimension; ++i) 
        {
            n_xyz(i, 0) = -rotated_coordinate_system(i, 0);
            n_xyz(i, 1) = rotated_coordinate_system(i, 1);
            n_xyz(i, 2) = rotated_coordinate_system(i, 2);
        }

        BoundedMatrix<double, msDimension, msDimension> Identity =
            ZeroMatrix(msDimension);
        for (unsigned int i = 0; i < msDimension; ++i)
          Identity(i, i) = 1.0;
        Identity -= 2.0 * outer_prod(Bisectrix, Bisectrix);
        n_xyz = prod(Identity, n_xyz);

        this->mLocalRotationMatrix = n_xyz;
      
        KRATOS_CATCH("")
    }*/
               

} // namespace Kratos.


