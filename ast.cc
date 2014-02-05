
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#include<iostream>
#include<fstream>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<"\n" << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}
/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

//////////////////////////////////////////////////////////////////////////

Expression_Ast::Expression_Ast(Ast * lhs , Ast *  rhs , BooleanOp _op){
	lhs_exp = lhs;
	rhs_exp = rhs;
	op  = _op;
	node_data_type = int_data_type;
}

Expression_Ast::~Expression_Ast(){
	delete(lhs_exp);
	delete(rhs_exp);
}
void Expression_Ast :: print_ast(ostream & file_buffer){

	file_buffer << "\n"<<AST_NODE_SPACE <<"Condition: "<<opNames[op]<<"\n";
	file_buffer << AST_NODE_SPACE << "\tLHS (";
	lhs_exp->print_ast(file_buffer);
	file_buffer << ")\n";
	file_buffer  << AST_NODE_SPACE << "\tRHS (";
	rhs_exp->print_ast(file_buffer);
	file_buffer << ")";
}

Data_Type Expression_Ast::get_data_type()
{
	return node_data_type;
}

Eval_Result & Expression_Ast:: evaluate(Local_Environment & eval_env, ostream & file_buffer){
		Eval_Result & l_res = lhs_exp->evaluate(eval_env, file_buffer);
		if (l_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on lhs of condition", NOLINE);
		}
		Eval_Result & r_res = rhs_exp->evaluate(eval_env, file_buffer);
		if (r_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on rhs of condition", NOLINE);
		}
		Eval_Result & result = *new Eval_Result_Value_Int();
		int l = l_res.get_value();
		int r = r_res.get_value();
		int temp = 0;
		switch (op) {
			case EQ :
				if(l==r){
					temp=1;
				}
				break;
			case NE :
				if(l!=r){
					temp=1;
				}
				break;
			case GT :
				if(l>r){
					temp=1;
				}
				break;
			case LT :
				if(l<r){
					temp=1;
				}
				break;
			case GE :
				if(l>=r){
					temp=1;
				}
				break;
			case LE :
				if(l<=r){
					temp=1;
				}
				break;
		}
		result.set_value(temp);
		result.set_result_enum(int_result);
        return result;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

Conditional_Ast::Conditional_Ast(Ast* _condition, Goto_Ast*  trueGoto, Goto_Ast* falseGoto){
	condition = _condition;
	true_goto = trueGoto;
	false_goto = falseGoto;
}

Conditional_Ast::~Conditional_Ast(){
	delete(condition);
	delete(true_goto);
	delete(false_goto);
	
}

void Conditional_Ast ::  print_ast(ostream & file_buffer){
	
	file_buffer << "\n" << AST_SPACE << "If_Else statement:";
    condition->print_ast(file_buffer);
	file_buffer << "\n" << AST_NODE_SPACE <<"True Successor: "<<true_goto->getBlockNo()<<"\n"; 
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->getBlockNo();
}

Eval_Result & Conditional_Ast:: evaluate(Local_Environment & eval_env, ostream & file_buffer){

        
	file_buffer <<"\n"<< AST_SPACE << "If_Else statement:";
    condition->print_ast(file_buffer);
	file_buffer << "\n"<<AST_NODE_SPACE <<"True Successor: "<<true_goto->getBlockNo()<<"\n"; 
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->getBlockNo()<<"\n";
		Eval_Result & cond_result = condition->evaluate(eval_env,file_buffer);
		if(cond_result.get_value()==1){
            file_buffer <<AST_SPACE<< "Condition True : Goto (BB " << true_goto->getBlockNo()<<")\n";
			return true_goto->evaluate(eval_env,file_buffer);
		}
        else{

            file_buffer <<AST_SPACE<< "Condition False : Goto (BB " << false_goto->getBlockNo()<<")\n";
        	return false_goto->evaluate(eval_env,file_buffer);
        }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



Goto_Ast::Goto_Ast(int bb){
	block_no=bb;
}


Goto_Ast::~Goto_Ast(){
	//nothing to delete
}

int Goto_Ast::getBlockNo(){
    return block_no;
}

Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
		
		Eval_Result & result = *new Eval_Result_Value_Goto();
		result.set_value(block_no);
        return result;
}



void Goto_Ast::print_ast(ostream & file_buffer){
	file_buffer<<"\n"<<AST_SPACE<< "Goto statement:\n "<<AST_NODE_SPACE <<"Successor: "<< block_no;
};





/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////




template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	file_buffer << "Num : " << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
    Eval_Result & result = *new Eval_Result_Value_Int();
	return result;
}

template class Number_Ast<int>;
