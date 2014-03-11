
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
#include <iomanip> 
using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"
#include"symbol-table.hh"
#include"procedure.hh"
#include"program.hh"

#include"ast.hh"
class Procedure;

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

bool Assignment_Ast::check_ast(int line){
	Data_Type l = lhs->get_data_type();
	Data_Type r = rhs->get_data_type();

	if(l == float_data_type || l == double_data_type){
		if(r == float_data_type || r == double_data_type){
			
			node_data_type = lhs->get_data_type();
			return true;
		} 
	}

	else if(l==int_data_type and r==int_data_type){
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
	Eval_Result & res = rhs->evaluate(eval_env, file_buffer);
	Result r;

	if (res.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	if(node_data_type == int_data_type){

		r.no=1;
		r.res = (int) res.get_value().res;
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(r);
		result.set_result_enum(int_result);
		lhs->set_value_of_evaluation(eval_env, result);
		print_ast(file_buffer);
		lhs->print_value(eval_env, file_buffer);
		return result;
	}
	else if(node_data_type == float_data_type){
		r.no=2;
		r.res = (float) res.get_value().res;
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value(r);
		result.set_result_enum(float_result);
		lhs->set_value_of_evaluation(eval_env, result);
		print_ast(file_buffer);
		lhs->print_value(eval_env, file_buffer);
		return result;
	}
		
	else if(node_data_type == double_data_type){
		r.no=3;
		r.res = (double) res.get_value().res;
		Eval_Result & result = *new Eval_Result_Value_Double();
		result.set_value(r);
		result.set_result_enum(double_result);
		lhs->set_value_of_evaluation(eval_env, result);
		print_ast(file_buffer);
		lhs->print_value(eval_env, file_buffer);
		return result;
	}

	// Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	// result.set_value(r);
	// lhs->set_value_of_evaluation(eval_env, result);

	// // Print the result

	// print_ast(file_buffer);
	// lhs->print_value(eval_env, file_buffer);

	// return result;
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

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << (int)(loc_var_val->get_value().res) << "\n";
		else if (loc_var_val->get_result_enum() == float_result)
			file_buffer << (float)(loc_var_val->get_value().res) << "\n";
		else if (loc_var_val->get_result_enum() == double_result)
			file_buffer << (double)(loc_var_val->get_value().res) << "\n";
		else
			report_internal_error("Result type can only be int float or double");
	}

	else
	{
		//cout<<glob_var_val->get_result_enum()<<endl;
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << (int)(glob_var_val->get_value().res) << "\n";
		}
		else if (glob_var_val->get_result_enum() == float_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << (float)(glob_var_val->get_value().res) << "\n";
		}
		else if (glob_var_val->get_result_enum() == double_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << (double)(glob_var_val->get_value().res) << "\n";
		}
		else
			report_internal_error("Result type can only be int float or double");
	}
	file_buffer << "\n";
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
		i->set_result_enum(int_result);
	 	i->set_value(result.get_value());
	 	//cout<<i->get_value().no<<endl;
	}
	else if (result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
		i->set_result_enum(float_result);
	 	i->set_value(result.get_value());
	}

	else if (result.get_result_enum() == double_result)
	{
		i = new Eval_Result_Value_Double();
		i->set_result_enum(double_result);
	 	i->set_value(result.get_value());
	}

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else{
		interpreter_global_table.put_variable_value(*i, variable_name);
		//cout<<"aa rha hai\n";
	}
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

//////////////////////////////////////////////////////////////////////////

Type_Expression_Ast::Type_Expression_Ast(Ast* e, Data_Type d){
	type_exp = e;
	node_data_type = d;
}

Type_Expression_Ast::Type_Expression_Ast(Ast* e){
	type_exp = e;
	node_data_type = e->get_data_type();
}


Type_Expression_Ast::~Type_Expression_Ast(){
	delete(type_exp);
}

void Type_Expression_Ast::print_ast(ostream & file_buffer){
	type_exp->print_ast(file_buffer);
}

Data_Type Type_Expression_Ast::get_data_type(){
	return node_data_type;
}

Eval_Result & Type_Expression_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
	Eval_Result & res = type_exp->evaluate(eval_env, file_buffer);
	Result r;
	if(node_data_type == int_data_type){
		r.no=1;
		r.res = (int) res.get_value().res;
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(r);
		result.set_result_enum(int_result);
		return result;
	}
	else if(node_data_type == float_data_type){
		r.no=2;
		r.res = (float) res.get_value().res;
		Eval_Result & result = *new Eval_Result_Value_Float();
		result.set_value(r);
		result.set_result_enum(float_result);
		return result;
	}
		
	else if(node_data_type == double_data_type){
		r.no=3;
		r.res = (double) res.get_value().res;
		Eval_Result & result = *new Eval_Result_Value_Double();
		result.set_value(r);
		result.set_result_enum(double_result);
		return result;
	}
}


///////////////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////////////

Call_Ast::Call_Ast(string & name,list<Ast*> _parlist,Data_Type _return_type){
	fn_name = name;
	par_list = _parlist;
	node_data_type = _return_type;
}

Call_Ast::~Call_Ast(){
}

void Call_Ast::print_ast(ostream & file_buffer){
	file_buffer << "\n" << AST_SPACE << "FN CALL: " << fn_name <<"(";
	list<Ast*>::iterator it = par_list.begin();
	for(; it!= par_list.end();it++){
		file_buffer<< "\n" << AST_NODE_SPACE;
		(*it)->print_ast(file_buffer);
	}
	file_buffer << ")";
}

Data_Type Call_Ast::get_data_type(){
	return node_data_type;
}

Eval_Result & Call_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
	
	Procedure* p = program_object.get_procedure(fn_name);
	list<Symbol_Table_Entry *> var_table = (p->get_local_symbol_table()).get_variable_table();
	list<Ast*>::iterator it = par_list.begin();
	list<Symbol_Table_Entry *>::iterator sym_it = var_table.begin();

	Local_Environment interpreter_local_table;
	// cout<<"before create\n";
	(p->get_local_symbol_table()).create(interpreter_local_table);
	// cout<<"after create\n";
	// cout<<"/////////////////////////////////////////////////////////\n";
	// // interpreter_local_table.is_variable_defined(name);

	// interpreter_local_table.print(file_buffer);


	// cout<<"/////////////////////////////////////////////////////////\n";

	// if(var_table.size() != p->get_parameter_length()){
	// 	cout<<fn_name<<": parameter length not equal"<<endl;
	// }
	// cout<<"param length: "<<p->get_parameter_length()<<endl;
	for(int i=0;it!=par_list.end() && i< p->get_parameter_length() ;it++,i++,sym_it++){
		Eval_Result & result = (*it)->evaluate(eval_env, file_buffer);
		string var_name = (*sym_it)->get_variable_name();
		// cout<<var_name<<" defined or not: "<<interpreter_local_table.is_variable_defined(var_name)<<endl;
		Eval_Result_Value* i ; 

		if (result.get_result_enum() == int_result){
		
			i = new Eval_Result_Value_Int();
			i->set_result_enum(int_result);
		 	i->set_value(result.get_value());
		 	//cout<<i->get_value().no<<endl;
		}
		else if (result.get_result_enum() == float_result)
		{
			i = new Eval_Result_Value_Float();
			i->set_result_enum(float_result);
		 	i->set_value(result.get_value());
		}

		else if (result.get_result_enum() == double_result)
		{
			i = new Eval_Result_Value_Double();
			i->set_result_enum(double_result);
		 	i->set_value(result.get_value());
		}

		if (interpreter_local_table.does_variable_exist(var_name))
			interpreter_local_table.put_variable_value(*i, var_name);
		else{
			interpreter_global_table.put_variable_value(*i, var_name);
			//cout<<"aa rha hai\n";
		}
	}


	//evvironment created

	Eval_Result& call_res =  p->evaluate(interpreter_local_table, file_buffer);
	return call_res;
}


///////////////////////////////////////////////////////////////////////////

Boolean_Ast::Boolean_Ast(Ast * lhs , Ast *  rhs , BooleanOp _op){
	lhs_exp = lhs;
	rhs_exp = rhs;
	op  = _op;
	node_data_type = int_data_type;
}

Boolean_Ast::~Boolean_Ast(){
	delete(lhs_exp);
	delete(rhs_exp);
}

bool Boolean_Ast::check_ast(int line){
	Data_Type l = lhs_exp->get_data_type();
	Data_Type r = rhs_exp->get_data_type();

	if(l==r){
		//node_data_type = l;
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Boolean_Ast::print_ast(ostream & file_buffer){

	file_buffer << "\n"<<AST_NODE_SPACE <<"Condition: "<<boolOp[op]<<"\n";
	file_buffer << COND_NODE_SPACE << "LHS (";
	lhs_exp->print_ast(file_buffer);
	file_buffer << ")\n";
	file_buffer  << COND_NODE_SPACE << "RHS (";
	rhs_exp->print_ast(file_buffer);
	file_buffer << ")";
}

Data_Type Boolean_Ast::get_data_type()
{
	return node_data_type;
}

Eval_Result & Boolean_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
		Eval_Result & l_res = lhs_exp->evaluate(eval_env, file_buffer);
		if (l_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on lhs of condition", NOLINE);
		}
		Eval_Result & r_res = rhs_exp->evaluate(eval_env, file_buffer);
		if (r_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on rhs of condition", NOLINE);
		}
		Eval_Result & result = *new Eval_Result_Value_Int();
		Result r;
		r.no = 1;	//boolean EXP IS OF INT
		int temp = 0;
		switch (op) {
			case EQ :
				if(l_res.get_value().res == r_res.get_value().res){
					temp=1;
				}
				break;
			case NE :
				if(l_res.get_value().res != r_res.get_value().res){
					temp=1;
				}
				break;
			case GT :
				if(l_res.get_value().res > r_res.get_value().res){
					temp=1;
				}
				break;
			case LT :
				if(l_res.get_value().res < r_res.get_value().res){
					temp=1;
				}
				break;
			case GE :
				if(l_res.get_value().res >= r_res.get_value().res){
					temp=1;
				}
				break;
			case LE :
				if(l_res.get_value().res <= r_res.get_value().res){
					temp=1;
				}
				break;
		}
		r.res = temp;
		result.set_value(r);
		result.set_result_enum(int_result);
        return result;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Arithmetic_Ast::Arithmetic_Ast(Ast* _lhs, Ast* _rhs, ArithOp _op){
    lhs_exp = _lhs;
    rhs_exp = _rhs;
    op = _op;
}

Arithmetic_Ast::~Arithmetic_Ast(){
    delete(lhs_exp);
    delete(rhs_exp);
}

bool Arithmetic_Ast::check_ast(int line){
	Data_Type l = lhs_exp->get_data_type();
	Data_Type r = rhs_exp->get_data_type();

	if(l==r){
		node_data_type = l;
		return true;
	}
	else if((l==float_data_type && r==double_data_type) || (l== double_data_type && r==float_data_type)){
		node_data_type = double_data_type;
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}


Data_Type Arithmetic_Ast::get_data_type()
{
	return node_data_type;
}


void Arithmetic_Ast::print_ast(ostream & file_buffer){

	file_buffer << "\n"<<AST_NODE_SPACE <<"Arith: "<<arithOp[op]<<"\n";
	file_buffer << COND_NODE_SPACE << "LHS (";
	lhs_exp->print_ast(file_buffer);
	file_buffer << ")\n";
	file_buffer  << COND_NODE_SPACE << "RHS (";
	rhs_exp->print_ast(file_buffer);
	file_buffer << ")";
}


Eval_Result & Arithmetic_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
       
        Eval_Result & l_res = lhs_exp->evaluate(eval_env, file_buffer);
		if (l_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on lhs of condition", NOLINE);
		}
		Eval_Result & r_res = rhs_exp->evaluate(eval_env, file_buffer);
		if (r_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on rhs of condition", NOLINE);
		}

		Result r;
		r.no = l_res.get_value().no;

		switch (op) {
			case MINUS :
				r.res = l_res.get_value().res - r_res.get_value().res;
				break;
			case PLUS :
				r.res = l_res.get_value().res + r_res.get_value().res;
				break;
			case DIVIDE :
				if(r_res.get_value().res == 0){
					report_error("Divide by 0",NOLINE);
				}
				r.res = l_res.get_value().res / r_res.get_value().res;
				// if(l_res.get_data_type()==float_data_type){
				// 	resullt.set_value(l_res.get_value() / (float)r_res.get_value());
				// }
				// else if(l_res.get_data_type()==double_data_type){
				// 	resullt.set_value(l_res.get_value() / (double)r_res.get_value());
				// }
				// else if(l_res.get_data_type()==itn_data_type){
				// 	resullt.set_value(l_res.get_value() / r_res.get_value());
				// }
				break;
			case MULTIPLY :
				r.res = l_res.get_value().res * r_res.get_value().res;
				break;
		}
		
		if(l_res.get_result_enum() == int_result){
			Eval_Result& result = *new Eval_Result_Value_Int();
			result.set_value(r);
			result.set_result_enum(l_res.get_result_enum());
        	return result;
		}
		else if(l_res.get_result_enum() == double_result){
			Eval_Result& result = *new Eval_Result_Value_Double();
			result.set_value(r);
			result.set_result_enum(l_res.get_result_enum());
        	return result;
		}
		else if(l_res.get_result_enum() == float_result){
			Eval_Result& result = *new Eval_Result_Value_Float();
			result.set_value(r);
			result.set_result_enum(l_res.get_result_enum());
	        return result;
		}
		
}

/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

Unary_Ast::Unary_Ast(Ast* ast,bool m){
    atomic_exp = ast;
    node_data_type = ast->get_data_type();
    minus = m;
}

Unary_Ast::~Unary_Ast(){
    delete(atomic_exp);
}

Data_Type Unary_Ast::get_data_type()
{
	return node_data_type;
}


void Unary_Ast::print_ast(ostream & file_buffer){
	if(minus){
		file_buffer << "\n"<<AST_NODE_SPACE <<"Arith: UMINUS"<<"\n";
		file_buffer << COND_NODE_SPACE << "LHS (";
		atomic_exp->print_ast(file_buffer);
		file_buffer << ")";
	}
	else{
		atomic_exp->print_ast(file_buffer);
	}
}


Eval_Result & Unary_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer){
    
    Result r;
    Eval_Result & res = atomic_exp->evaluate(eval_env,file_buffer);

    int factor = (minus)? -1 : 1;
    //cout << "factor is: "<<factor << endl;
	if(node_data_type == int_data_type){
			Eval_Result& result = *new Eval_Result_Value_Int();
			r.no = 1;
			r.res = (int)((res.get_value().res) * factor);
			result.set_value(r);
			result.set_result_enum(int_result);
        	return result;
		}
		else if(node_data_type == float_data_type){
			Eval_Result& result = *new Eval_Result_Value_Float();
			r.no = 2;
			r.res = (float)((res.get_value().res) * factor);
			result.set_value(r);
			result.set_result_enum(float_result);
        	return result;
		}
		else if(node_data_type == double_data_type){
			Eval_Result& result = *new Eval_Result_Value_Double();
			r.no = 3;
			r.res = (double)((res.get_value().res) * factor);
			result.set_value(r);
			result.set_result_enum(double_result);
        	return result;
		}
}




/////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////




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
	Eval_Result & cond_result = condition->evaluate(eval_env,file_buffer);
	file_buffer << "\n"<<AST_NODE_SPACE <<"True Successor: "<<true_goto->getBlockNo()<<"\n"; 
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->getBlockNo()<<"\n";

	Result r;
	r.no=4;

		if(cond_result.get_value().res==1){
            file_buffer <<AST_SPACE<< "Condition True : Goto (BB " << true_goto->getBlockNo()<<")\n";
            Eval_Result & result = *new Eval_Result_Value_Goto();
            r.res = true_goto->getBlockNo();
            result.set_value(r);
			return result;
		}
        else{

            file_buffer <<AST_SPACE<< "Condition False : Goto (BB " << false_goto->getBlockNo()<<")\n";
            Eval_Result & result = *new Eval_Result_Value_Goto();
            r.res = false_goto->getBlockNo();
            result.set_value(r);
			return result;
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
        file_buffer<<"\n"<<AST_SPACE<< "Goto statement:\n"<<AST_NODE_SPACE <<"Successor: "<< block_no;
        file_buffer<< "\n" <<AST_SPACE<< "GOTO (BB "<< block_no <<")\n";
		Eval_Result & result = *new Eval_Result_Value_Goto();
		Result r;
		r.no=4;
		r.res = block_no;
		result.set_value(r);
        return result;
}



void Goto_Ast::print_ast(ostream & file_buffer){
	file_buffer<<"\n"<<AST_SPACE<< "Goto statement:\n"<<AST_NODE_SPACE <<"Successor: "<< block_no;
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
	file_buffer << "Num : " << std::fixed << std::setprecision(2) << constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Result r;
	r.res = constant;
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		r.no=1;
		result.set_value(r);
		result.set_result_enum(int_result);
		return result;
	}
	else if (node_data_type == float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		r.no=2;
		result.set_value(r);
		result.set_result_enum(float_result);
		return result;
	}
	else if (node_data_type == double_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Double();
		r.no=3;
		result.set_value(r);
		result.set_result_enum(double_result);
		return result;
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{
	node_data_type = return_data_type;
	ret_exp = NULL;
}

Return_Ast::Return_Ast(Ast* _ret)
{
	ret_exp = _ret;
	node_data_type = return_data_type;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	if(ret_exp==NULL){
		file_buffer << "\n" << AST_SPACE << "RETURN <NOTHING>\n";
	}
	else{
		file_buffer << "\n" << AST_SPACE << "RETURN ";
		ret_exp->print_ast(file_buffer);
		file_buffer << "\n";	
	}
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	if(ret_exp==NULL){
		file_buffer << "\n" << AST_SPACE << "RETURN <NOTHING>\n";
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_result_enum(return_result);
	}
	else{
		file_buffer << "\n" << AST_SPACE << "RETURN ";
		Eval_Result& result = ret_exp->evaluate(eval_env,file_buffer);
		ret_exp->print_ast(file_buffer);
		file_buffer <<"\n";
		Eval_Result_Value* i ; 
		Eval_Result_Value* ret_res;
		if (result.get_result_enum() == int_result){
		
			i = new Eval_Result_Value_Int();
			i->set_result_enum(return_result);
		 	i->set_value(result.get_value());

		 	ret_res = new Eval_Result_Value_Int();
			ret_res->set_result_enum(return_result);
		 	ret_res->set_value(result.get_value());
		 	//cout<<i->get_value().no<<endl;
		}
		else if (result.get_result_enum() == float_result)
		{
			i = new Eval_Result_Value_Float();
			i->set_result_enum(return_result);
		 	i->set_value(result.get_value());

		 	ret_res = new Eval_Result_Value_Float();
			ret_res->set_result_enum(return_result);
		 	ret_res->set_value(result.get_value());
		}

		else if (result.get_result_enum() == double_result)
		{
			i = new Eval_Result_Value_Double();
			i->set_result_enum(return_result);
		 	i->set_value(result.get_value());

		 	ret_res = new Eval_Result_Value_Double();
			ret_res->set_result_enum(return_result);
		 	ret_res->set_value(result.get_value());
		}
		else if (result.get_result_enum() == return_result)
		{
			i = new Eval_Result_Value_Double();
			i->set_result_enum(return_result);
		 	i->set_value(result.get_value());

		 	ret_res = new Eval_Result_Value_Double();
			ret_res->set_result_enum(return_result);
		 	ret_res->set_value(result.get_value());
		}

		eval_env.put_variable_value(*ret_res, "return");
		return *i;

	}


}

template class Number_Ast<int>;
template class Number_Ast<float>;
template class Number_Ast<double>;
