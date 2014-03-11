
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

#include<string>
#include<fstream>
#include<iostream>

using namespace std;

#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"
#include"basic-block.hh"
#include"procedure.hh"
#include"program.hh"

Procedure::Procedure(Data_Type proc_return_type, string proc_name)
{
	return_type = proc_return_type;
	name = proc_name;
	defined = false;
}

Procedure::~Procedure()
{
	list<Basic_Block *>::iterator i;
	for (i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		delete (*i);
}

string Procedure::get_proc_name()
{
	return name;
}

void Procedure::set_basic_block_list(list<Basic_Block *> bb_list)
{
	basic_block_list = bb_list;
}

void Procedure::set_local_list(Symbol_Table & new_list)
{
	//cout<<"local_list set"<<endl;
	local_symbol_table = new_list;
	local_symbol_table.set_table_scope(local);
}


Data_Type Procedure::get_return_type()
{
	return return_type;
}

bool Procedure::variable_in_symbol_list_check(string variable)
{
	return local_symbol_table.variable_in_symbol_list_check(variable);
}

Symbol_Table_Entry & Procedure::get_symbol_table_entry(string variable_name)
{
	return local_symbol_table.get_symbol_table_entry(variable_name);
}

void Procedure::print_ast(ostream & file_buffer)
{
	file_buffer << PROC_SPACE << "Procedure: " << name << "\n";

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
		(*i)->print_bb(file_buffer);

	file_buffer << "\n";
}
	
Basic_Block & Procedure::get_start_basic_block()
{
	list<Basic_Block *>::iterator i;
	i = basic_block_list.begin();
	return **i;
}

Basic_Block * Procedure::get_next_bb(Basic_Block & current_bb)
{
	bool flag = false;

	list<Basic_Block *>::iterator i;
	for(i = basic_block_list.begin(); i != basic_block_list.end(); i++)
	{
		if((*i)->get_bb_number() == current_bb.get_bb_number())
		{
			flag = true;
			continue;
		}
		if (flag)
			return (*i);
	}
	
	return NULL;
}


Basic_Block * Procedure::get_bb(int block_no){
	list<Basic_Block *>::iterator i;
	int pos;
	for(i = basic_block_list.begin();i != basic_block_list.end(); i++){
		if( (*i)->get_bb_number() == block_no){
			return (*i);
		}
	}
	return NULL;
}


Eval_Result & Procedure::evaluate(ostream & file_buffer)
{
	Local_Environment & eval_env = *new Local_Environment();
	local_symbol_table.create(eval_env);
	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure << " << name << " >>\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		if(result==NULL){
			report_error("Basic Block is empty",-1);
		}
		else if(result->get_result_enum()==return_result){
			break;
		}
		else if(result->get_result_enum()==goto_result){
			current_bb = get_bb(result->get_value().res);
		}
		else{
			current_bb = get_next_bb(*current_bb);		
		}
	}

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << " <<name<<" >>\n";
	eval_env.print(file_buffer);

	return *result;
}

Eval_Result & Procedure::evaluate(Local_Environment& eval_env , ostream & file_buffer){
	
	Eval_Result * result = NULL;

	file_buffer << PROC_SPACE << "Evaluating Procedure << " << name << " >>\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (before evaluating):\n";
	eval_env.print(file_buffer);
	file_buffer << "\n";
	
	Basic_Block * current_bb = &(get_start_basic_block());
	while (current_bb)
	{
		result = &(current_bb->evaluate(eval_env, file_buffer));
		if(result==NULL){
			report_error("Basic Block is empty",-1);
		}
		else if(result->get_result_enum()==return_result){
			break;
		}
		else if(result->get_result_enum()==goto_result){
			current_bb = get_bb(result->get_value().res);
		}
		else{
			current_bb = get_next_bb(*current_bb);		
		}
	}

	file_buffer << "\n\n";
	file_buffer << LOC_VAR_SPACE << "Local Variables (after evaluating) Function: << " <<name<<" >>\n";
	eval_env.print(file_buffer);

	return *result;
}


void Procedure::add_basic_block_no(int n){
    basic_block_no.push_back(n);
}

void Procedure::add_goto_no(int n){
    goto_no.push_back(n);
}


int Procedure::check_valid_goto(){

    for (std::list<int>::iterator git=goto_no.begin(); git != goto_no.end(); ++git){
        bool flag=false;
        for (std::list<int>::iterator it=basic_block_no.begin(); it != basic_block_no.end(); ++it){
            if(*it==*git){
                flag=true;
            }
        }
        if(!flag){
            return *git;
        }
    }
    return 0;
}


void Procedure::print_nos(){
cout<<"Block Nos ";
        for (std::list<int>::iterator it=basic_block_no.begin(); it != basic_block_no.end(); ++it){
    cout<<(*it)<<" ";
        }
        cout<<endl;


cout<<"Goto Nos ";
        for (std::list<int>::iterator it=goto_no.begin(); it != goto_no.end(); ++it){
    cout<<(*it)<<" ";
        }
        cout<<endl;
        return;
}

//0-incorrect name
//1- incorrect type
//2- less arguments  in newlist
//3- more arguments in newlist
//4- all is well
void Procedure::check_parameter_list(Symbol_Table* new_table, int line){
	list<Symbol_Table_Entry *> local_list = local_symbol_table.get_variable_table();
	list<Symbol_Table_Entry *> new_list = new_table->get_variable_table();
	list<Symbol_Table_Entry *>::iterator i,j;
	int k=0;
	//list<Symbol_Table_Entry *>::iterator j;
	//cout<<"new_list"<<endl;
	//cout<<"sizes are: "<<new_list.size()<<" "<<parameter_length<<endl;
	if(new_list.size() == 0){
		// cout<<"new_list is null"<<endl;
		if(local_list.size()!=0){
			report_error("Procedure and its prototype parameter f_list length doens't match",line);
		}
		else{
			return;
		}
	}
	// for (i = local_list.begin() ; i != local_list.end(); i++){
	// 	cout<<"hello\n";
	// }

	for (i = local_list.begin(), j = new_list.begin(), k=0 ; (k < parameter_length) && (i != local_list.end()) and (j != new_list.end()) ; i++ , j++ , k++)
	{
		//cout<<local_list.size()<<endl;
		//cout<<"name: "<<(*i)->get_variable_name() <<" "<<(*j)->get_variable_name()<<endl;
		if ((*i)->get_variable_name() != (*j)->get_variable_name()){
			
			report_error("Variable name of one of the parameters of the procedre and its prototypes doesn't match",line);
		}
		else if((*i)->get_data_type() != (*j)->get_data_type())
		{
			report_error("Return type of one of the parameters of the procedure and its prototype doesn't match",line);
		}
		else{
			continue;
		}
	}
	if(k<parameter_length){
		//cout<<"local list: \n"<< k <<" "<<parameter_length<<endl;
		report_error("Procedure and its prototype parameter f_list length doens't match",line);
	}
	else if(j!= new_list.end()){
		//cout<<new_list.size()<<" "<<local_list.size()<<endl;
		//cout<<"local list: \n"<< k <<" "<<parameter_length<<endl;
		// cout<<(*j)->get_variable_name()<<endl;
		// cout<<"new list: \n";
		report_error("Procedure and its prototype parameter f_list length doens't match",line);
	}
	else{
		return;
	}

}




void Procedure::check_called_data_type(list<Ast*> params, int line){
	list<Symbol_Table_Entry *> local_list = local_symbol_table.get_variable_table();
	list<Symbol_Table_Entry *>::iterator i = local_list.begin();
	list<Ast*>::iterator j = params.begin();
	int k=0;
	for(;i!=local_list.end() && j!=params.end() && k <parameter_length ; i++, j++, k++){
		if ((*i)->get_data_type() != (*j)->get_data_type()){
			report_error("Actual and formal parameters data types are not matching",line);
		}
	}
	if(k<parameter_length){
		//cout<<"local list: \n"<< k <<" "<<parameter_length<<endl;
		report_error("Actual and formal parameter count do not match",line);
	}
	else if(j!= params.end()){
		//cout<<new_list.size()<<" "<<local_list.size()<<endl;
		//cout<<"local list: \n"<< k <<" "<<parameter_length<<endl;
		// cout<<(*j)->get_variable_name()<<endl;
		// cout<<"new list: \n";
		report_error("Actual and formal parameter count do not match",line);
	}
	else{
		return;
	}



}
void Procedure::append_local_list(Symbol_Table & new_table, int line){
	list<Symbol_Table_Entry *> new_list = new_table.get_variable_table();
	list<Symbol_Table_Entry *>::iterator i;

	for (i = new_list.begin() ; i != new_list.end() ; i++){
		if(variable_in_symbol_list_check((*i)->get_variable_name())){
			report_error("Formal parameter and local variable name cannot be same",line);
		}
		local_symbol_table.push_symbol(*i);
	}
	return;
	//cout<<"local_list appended"<<endl;
}

Symbol_Table Procedure::get_local_symbol_table(){
	return local_symbol_table;
}


void Procedure::set_parameter_length(int _a){
	parameter_length = _a;
}
int Procedure::get_parameter_length(){
	return parameter_length;
}

void  Procedure::set_defined(bool _val){
	defined = _val;
}

bool  Procedure::is_defined(){
	return defined;
}

void Procedure::set_return_type(Data_Type type){
	return_type = type;
}