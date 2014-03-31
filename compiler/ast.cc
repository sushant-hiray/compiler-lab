
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
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"icode.hh"
#include"reg-alloc.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	stringstream msg;
	msg << "No print_value() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	stringstream msg;
	msg << "No get_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	stringstream msg;
	msg << "No set_value_of_evaluation() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	lhs = temp_lhs;
	rhs = temp_rhs;

	ast_num_child = binary_arity;
	node_data_type = void_data_type;

	lineno = line;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, 
		"Assignment statement data type not compatible");
}

void Assignment_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Asgn:";

	file_buffer << "\n" << AST_NODE_SPACE << "LHS (";
	lhs->print(file_buffer);
	file_buffer << ")";

	file_buffer << "\n" << AST_NODE_SPACE << "RHS (";
	rhs->print(file_buffer);
	file_buffer << ")";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	CHECK_INPUT_AND_ABORT(result.is_variable_defined(), "Variable should be defined to be on rhs of Assignment_Ast", lineno);

	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}

Code_For_Ast & Assignment_Ast::compile()
{
	/* 
		An assignment x = y where y is a variable is 
		compiled as a combination of load and store statements:
		(load) R <- y 
		(store) x <- R
		If y is a constant, the statement is compiled as:
		(imm_Load) R <- y 
		(store) x <- R
		where imm_Load denotes the load immediate operation.
	*/

	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();
	load_register->set_used_for_expr_result(true);

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	load_register->reset_use_for_expr_result();
	Code_For_Ast * assign_stmt;
	
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}

Code_For_Ast & Assignment_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	//cout<<"Assign"<<endl;
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null");

	if(typeid(*rhs)!=typeid(Boolean_Ast)){
		lra.optimize_lra(mc_2m, lhs, rhs);
	}

	Code_For_Ast load_stmt = rhs->compile_and_optimize_ast(lra);
	Register_Descriptor * result_register = load_stmt.get_reg();
	Code_For_Ast store_stmt = lhs->create_store_stmt(result_register);

	// lra.optimize_lra(mc_2m, lhs, rhs);
	if(typeid(*rhs)==typeid(Boolean_Ast)){

		Symbol_Table_Entry* destination_symbol_entry = &(lhs->get_symbol_entry());
		Register_Descriptor* destination_register = destination_symbol_entry->get_register(); 
		if (destination_register){
			destination_symbol_entry->free_register(destination_register); 
		}

		destination_symbol_entry->update_register(result_register);
	}

	list<Icode_Stmt *> ic_list;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);

	result_register->reset_use_for_expr_result();

	return *assign_stmt;
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{
	variable_symbol_entry = &var_entry;

	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");

	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
	lineno = line;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry->get_data_type();
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	return *variable_symbol_entry;
}

void Name_Ast::print(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_symbol_entry->get_variable_name();
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	Eval_Result * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result * glob_var_val = interpreter_global_table.get_variable_value(variable_name);

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		CHECK_INVARIANT(loc_var_val->get_result_enum() == int_result, "Result type can only be int");
		file_buffer << loc_var_val->get_int_value() << "\n";
	}

	else
	{
		CHECK_INVARIANT(glob_var_val->get_result_enum() == int_result, 
			"Result type can only be int and float");

		if (glob_var_val == NULL)
			file_buffer << "0\n";
		else
			file_buffer << glob_var_val->get_int_value() << "\n";
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	string variable_name = variable_symbol_entry->get_variable_name();

	if (eval_env.does_variable_exist(variable_name))
	{
		CHECK_INPUT_AND_ABORT((eval_env.is_variable_defined(variable_name) == true), 
					"Variable should be defined before its use", lineno);

		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	CHECK_INPUT_AND_ABORT((interpreter_global_table.is_variable_defined(variable_name) == true), 
				"Variable should be defined before its use", lineno);

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result * i;
	string variable_name = variable_symbol_entry->get_variable_name();

	if (variable_symbol_entry->get_data_type() == int_data_type)
		i = new Eval_Result_Value_Int();
	else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

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
    else
		CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Type of a name can be int/float only", lineno);

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

Code_For_Ast & Name_Ast::compile()
{
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(load, opd, register_opd);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	CHECK_INVARIANT((store_register != NULL), "Store register cannot be null");

	Ics_Opd * register_opd = new Register_Addr_Opd(store_register);
	Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

	Icode_Stmt * store_stmt = new Move_IC_Stmt(store, register_opd, opd);

	if (command_options.is_do_lra_selected() == false)
		variable_symbol_entry->free_register(store_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(store_stmt);

	Code_For_Ast & name_code = *new Code_For_Ast(ic_list, store_register);

	return name_code;
}

Code_For_Ast & Name_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;;

	bool load_needed = lra.is_load_needed();

	Register_Descriptor * result_register = lra.get_register();
	CHECK_INVARIANT((result_register != NULL), "Register cannot be null");
	Ics_Opd * register_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * load_stmt = NULL;
	if (load_needed)
	{
		Ics_Opd * opd = new Mem_Addr_Opd(*variable_symbol_entry);

		load_stmt = new Move_IC_Stmt(load, opd, register_opd);
			
		ic_list.push_back(load_stmt);
	}

	Code_For_Ast & load_code = *new Code_For_Ast(ic_list, result_register);

	return load_code;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

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
void Boolean_Ast :: print(ostream & file_buffer){

	file_buffer << "\n"<<AST_NODE_SPACE <<"Condition: "<<opNames[op]<<"\n";
	file_buffer << COND_NODE_SPACE << "LHS (";
	lhs_exp->print(file_buffer);
	file_buffer << ")\n";
	file_buffer  << COND_NODE_SPACE << "RHS (";
	rhs_exp->print(file_buffer);
	file_buffer << ")";
}

Data_Type Boolean_Ast::get_data_type()
{
	return node_data_type;
}

Eval_Result & Boolean_Ast:: evaluate(Local_Environment & eval_env, ostream & file_buffer){
		Eval_Result & l_res = lhs_exp->evaluate(eval_env, file_buffer);
		if (l_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on lhs of condition", NOLINE);
		}
		Eval_Result & r_res = rhs_exp->evaluate(eval_env, file_buffer);
		if (r_res.is_variable_defined() == false){
			report_error("Variable should be defined to be on rhs of condition", NOLINE);
		}
		Eval_Result & result = *new Eval_Result_Value_Int();
		int l = l_res.get_int_value();
		int r = r_res.get_int_value();
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
        Result r1;
        r1.no = 1;   //boolean EXP IS OF INT
        r1.res = temp;
        result.set_value(r1);
		result.set_result_enum(int_result);
        return result;
}

Code_For_Ast & Boolean_Ast::compile(){
	CHECK_INVARIANT((lhs_exp != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs_exp != NULL), "Rhs cannot be null");

	Code_For_Ast & lhs_stmt = lhs_exp->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->set_used_for_expr_result(true);
	
	Code_For_Ast & rhs_stmt = rhs_exp->compile();
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->set_used_for_expr_result(true);
	//cout<<lhs_register->get_name()<< " "<< rhs_register->get_name()<<endl;
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	result_register->set_used_for_expr_result(true);

	Ics_Opd* lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd* rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd* res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * comp_stmt;

	switch (op) {
		case EQ :
			comp_stmt = new Compare_IC_Stmt(seq, lhs_opd, rhs_opd, res_opd);
			break;
		case NE :
			comp_stmt = new Compare_IC_Stmt(sne, lhs_opd, rhs_opd, res_opd);
			break;
		case GT :
			comp_stmt = new Compare_IC_Stmt(sgt, lhs_opd, rhs_opd, res_opd);
			break;
		case LT :
			comp_stmt = new Compare_IC_Stmt(slt, lhs_opd, rhs_opd, res_opd);
			break;
		case GE :
			comp_stmt = new Compare_IC_Stmt(sge, lhs_opd, rhs_opd, res_opd);
			break;
		case LE :
			comp_stmt = new Compare_IC_Stmt(sle, lhs_opd, rhs_opd, res_opd);
			break;

		default:
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Intermediate code format not supported");
			break;
	}


	list<Icode_Stmt *> ic_list;
	
	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	ic_list.push_back(comp_stmt);
	Code_For_Ast * assign_stmt;
	
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);



	lhs_register->reset_use_for_expr_result();
	rhs_register->reset_use_for_expr_result();

	Code_For_Ast & comp_code = *new Code_For_Ast(ic_list, result_register);
	return comp_code;
}

Code_For_Ast & Boolean_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lhs_exp != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs_exp != NULL), "Rhs cannot be null");

	Lra_Outcome* lra_lhs = new Lra_Outcome();
	Lra_Outcome* lra_rhs = new Lra_Outcome();

	if(typeid(*lhs_exp) != typeid(Boolean_Ast)){
		//lhs_exp->print(cout);
		lra_lhs->optimize_lra(mc_2r,NULL,lhs_exp);
	}


	Code_For_Ast & lhs_stmt = lhs_exp->compile_and_optimize_ast(*lra_lhs);
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->set_used_for_expr_result(true);
	
	if(typeid(*rhs_exp) != typeid(Boolean_Ast)){
		//rhs_exp->print(cout);
		lra_rhs->optimize_lra(mc_2r,NULL,rhs_exp);
	}

	Code_For_Ast & rhs_stmt = rhs_exp->compile_and_optimize_ast(*lra_rhs);
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->set_used_for_expr_result(true);


	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	result_register->set_used_for_expr_result(true);

	Ics_Opd* lhs_opd = new Register_Addr_Opd(lhs_register);
	Ics_Opd* rhs_opd = new Register_Addr_Opd(rhs_register);
	Ics_Opd* res_opd = new Register_Addr_Opd(result_register);

	Icode_Stmt * comp_stmt;

	switch (op) {
		case EQ :
			comp_stmt = new Compare_IC_Stmt(seq, lhs_opd, rhs_opd, res_opd);
			break;
		case NE :
			comp_stmt = new Compare_IC_Stmt(sne, lhs_opd, rhs_opd, res_opd);
			break;
		case GT :
			comp_stmt = new Compare_IC_Stmt(sgt, lhs_opd, rhs_opd, res_opd);
			break;
		case LT :
			comp_stmt = new Compare_IC_Stmt(slt, lhs_opd, rhs_opd, res_opd);
			break;
		case GE :
			comp_stmt = new Compare_IC_Stmt(sge, lhs_opd, rhs_opd, res_opd);
			break;
		case LE :
			comp_stmt = new Compare_IC_Stmt(sle, lhs_opd, rhs_opd, res_opd);
			break;

		default:
			CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, "Intermediate code format not supported");
			break;
	}


	list<Icode_Stmt *> ic_list;
	
	if (lhs_stmt.get_icode_list().empty() == false)
		ic_list = lhs_stmt.get_icode_list();

	if (rhs_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), rhs_stmt.get_icode_list());

	ic_list.push_back(comp_stmt);
	Code_For_Ast * assign_stmt;
	
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, result_register);



	lhs_register->reset_use_for_expr_result();
	rhs_register->reset_use_for_expr_result();

	Code_For_Ast & comp_code = *new Code_For_Ast(ic_list, result_register);
	return comp_code;

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



Data_Type Arithmetic_Ast::get_data_type()
{
    return node_data_type;
}


void Arithmetic_Ast::print(ostream & file_buffer){

    file_buffer << "\n"<<AST_NODE_SPACE <<"Arith: "<<arithOp[op]<<"\n";
    file_buffer << COND_NODE_SPACE << "LHS (";
    lhs_exp->print(file_buffer);
    file_buffer << ")\n";
    file_buffer  << COND_NODE_SPACE << "RHS (";
    rhs_exp->print(file_buffer);
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
                //  resullt.set_value(l_res.get_value() / (float)r_res.get_value());
                // }
                // else if(l_res.get_data_type()==double_data_type){
                //  resullt.set_value(l_res.get_value() / (double)r_res.get_value());
                // }
                // else if(l_res.get_data_type()==itn_data_type){
                //  resullt.set_value(l_res.get_value() / r_res.get_value());
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



Code_For_Ast & Arithmetic_Ast::compile(){
	CHECK_INVARIANT((lhs_exp != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs_exp != NULL), "Rhs cannot be null");

	Code_For_Ast & lhs_stmt = lhs_exp->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->set_used_for_expr_result(true);
	
	Code_For_Ast & rhs_stmt = rhs_exp->compile();
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->set_used_for_expr_result(true);
    
    return lhs_stmt;
}



Code_For_Ast & Arithmetic_Ast::compile_and_optimize_ast(Lra_Outcome & lra){
	CHECK_INVARIANT((lhs_exp != NULL), "Lhs cannot be null");
	CHECK_INVARIANT((rhs_exp != NULL), "Rhs cannot be null");

	Code_For_Ast & lhs_stmt = lhs_exp->compile();
	Register_Descriptor * lhs_register = lhs_stmt.get_reg();
	lhs_register->set_used_for_expr_result(true);
	
	Code_For_Ast & rhs_stmt = rhs_exp->compile();
	Register_Descriptor * rhs_register = rhs_stmt.get_reg();
	rhs_register->set_used_for_expr_result(true);

    return lhs_stmt;
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

void Conditional_Ast::print(ostream & file_buffer){
	
	file_buffer << "\n" << AST_SPACE << "If_Else statement:";
    condition->print(file_buffer);
	file_buffer << "\n" << AST_NODE_SPACE <<"True Successor: "<<true_goto->getBlockNo()<<"\n"; 
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->getBlockNo();
}

Eval_Result & Conditional_Ast:: evaluate(Local_Environment & eval_env, ostream & file_buffer){

        
	file_buffer <<"\n"<< AST_SPACE << "If_Else statement:";
    condition->print(file_buffer);
	Eval_Result & cond_result = condition->evaluate(eval_env,file_buffer);
	file_buffer << "\n"<<AST_NODE_SPACE <<"True Successor: "<<true_goto->getBlockNo()<<"\n"; 
	file_buffer << AST_NODE_SPACE << "False Successor: "<<false_goto->getBlockNo()<<"\n";
		if(cond_result.get_int_value()==1){
            file_buffer <<AST_SPACE<< "Condition True : Goto (BB " << true_goto->getBlockNo()<<")\n";
            Eval_Result & result = *new Eval_Result_Value_Goto();
            Result r;
            r.no=4;
            r.res = true_goto->getBlockNo();
            result.set_value(r);
			return result;
		}
        else{

            file_buffer <<AST_SPACE<< "Condition False : Goto (BB " << false_goto->getBlockNo()<<")\n";
            Eval_Result & result = *new Eval_Result_Value_Goto();
            Result r;
            r.no=4;
            r.res = false_goto->getBlockNo();
            result.set_value(r);
			return result;
        }
}



Code_For_Ast & Conditional_Ast::compile()
{

	CHECK_INVARIANT((condition != NULL), "Condition cannot be null");
	
	Code_For_Ast & cond_stmt = condition->compile();
	Register_Descriptor * cond_register = cond_stmt.get_reg();
	cond_register->set_used_for_expr_result(true);
	Ics_Opd* opd1 = new Register_Addr_Opd(cond_register);

	Register_Descriptor * zero_register = new Register_Descriptor(zero, "zero", int_num, fixed_reg);
	Ics_Opd* opd2 = new Register_Addr_Opd(zero_register);

	int true_num = true_goto->getBlockNo();
	int false_num = false_goto->getBlockNo();

	// cout<<"true goto: "<<true_num<<" false goto: "<<false_num<<endl;

	Ics_Opd* opd3 = new Label_Addr_Opd(true_num);
	Ics_Opd* opd4 = new Label_Addr_Opd(false_num);

	Icode_Stmt* true_stmt = new Branch_IC_Stmt(bne, opd1, opd2, opd3);
	Icode_Stmt* false_stmt = new Branch_IC_Stmt(j, opd4, NULL, NULL);



	list<Icode_Stmt *> ic_list;
	
	if (cond_stmt.get_icode_list().empty() == false)
		ic_list = cond_stmt.get_icode_list();

	ic_list.push_back(true_stmt);
	ic_list.push_back(false_stmt);
	cond_register->reset_use_for_expr_result();

	Code_For_Ast & comp_code = *new Code_For_Ast(ic_list, NULL);
	return comp_code;
}

Code_For_Ast & Conditional_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((condition != NULL), "Condition cannot be null");
	
	Code_For_Ast & cond_stmt = condition->compile_and_optimize_ast(lra);
	Register_Descriptor * cond_register = cond_stmt.get_reg();
	cond_register->set_used_for_expr_result(true);
	Ics_Opd* opd1 = new Register_Addr_Opd(cond_register);

	Register_Descriptor * zero_register = new Register_Descriptor(zero, "zero", int_num, fixed_reg);
	Ics_Opd* opd2 = new Register_Addr_Opd(zero_register);

	int true_num = true_goto->getBlockNo();
	int false_num = false_goto->getBlockNo();

	// cout<<"true goto: "<<true_num<<" false goto: "<<false_num<<endl;

	Ics_Opd* opd3 = new Label_Addr_Opd(true_num);
	Ics_Opd* opd4 = new Label_Addr_Opd(false_num);

	Icode_Stmt* true_stmt = new Branch_IC_Stmt(bne, opd1, opd2, opd3);
	Icode_Stmt* false_stmt = new Branch_IC_Stmt(j, opd4, NULL, NULL);



	list<Icode_Stmt *> ic_list;
	
	if (cond_stmt.get_icode_list().empty() == false)
		ic_list = cond_stmt.get_icode_list();

	ic_list.push_back(true_stmt);
	ic_list.push_back(false_stmt);
	cond_register->reset_use_for_expr_result();

	Code_For_Ast & comp_code = *new Code_For_Ast(ic_list, NULL);
	return comp_code;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////



Goto_Ast::Goto_Ast(int bb){
	// cout << bb<<endl;
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



void Goto_Ast::print(ostream & file_buffer){
	file_buffer<<"\n"<<AST_SPACE<< "Goto statement:\n"<<AST_NODE_SPACE <<"Successor: "<< block_no;
};



Code_For_Ast & Goto_Ast::compile()
{
	Ics_Opd* opd = new Label_Addr_Opd(block_no);
	Icode_Stmt* goto_stmt = new Branch_IC_Stmt(j, opd, NULL, NULL);
	
	list<Icode_Stmt *> ic_list;
	ic_list.push_back(goto_stmt);

	Code_For_Ast & comp_code = *new Code_For_Ast(ic_list, NULL);
	return comp_code;
}

Code_For_Ast & Goto_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Ics_Opd* opd = new Label_Addr_Opd(block_no);
	Icode_Stmt* goto_stmt = new Branch_IC_Stmt(j, opd, NULL, NULL);
	
	list<Icode_Stmt *> ic_list;
	ic_list.push_back(goto_stmt);

	Code_For_Ast & comp_code = *new Code_For_Ast(ic_list, NULL);
	return comp_code;
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	constant = number;
	node_data_type = constant_data_type;

	ast_num_child = zero_arity;

	lineno = line;
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
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	file_buffer << std::fixed;
	file_buffer << std::setprecision(2);

	file_buffer << "Num : " << constant;
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

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Register_Descriptor * result_register = machine_dscr_object.get_new_register();
	CHECK_INVARIANT((result_register != NULL), "Result register cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(result_register);
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, result_register);
	return num_code;
}

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile_and_optimize_ast(Lra_Outcome & lra)
{
	CHECK_INVARIANT((lra.get_register() != NULL), "Register assigned through optimize_lra cannot be null");
	Ics_Opd * load_register = new Register_Addr_Opd(lra.get_register());
	Ics_Opd * opd = new Const_Opd<int>(constant);

	Icode_Stmt * load_stmt = new Move_IC_Stmt(imm_load, opd, load_register);

	list<Icode_Stmt *> ic_list;
	ic_list.push_back(load_stmt);

	Code_For_Ast & num_code = *new Code_For_Ast(ic_list, lra.get_register());

	return num_code;
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast(int line)
{
	lineno = line;
	node_data_type = void_data_type;
	ast_num_child = zero_arity;
}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print(ostream & file_buffer)
{
	file_buffer << "\n" << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Int();
	return result;
}

Code_For_Ast & Return_Ast::compile()
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

Code_For_Ast & Return_Ast::compile_and_optimize_ast(Lra_Outcome & lra)
{
	Code_For_Ast & ret_code = *new Code_For_Ast();
	return ret_code;
}

template class Number_Ast<int>;
