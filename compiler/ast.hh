
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

#ifndef AST_HH
#define AST_HH

#include<string>
#include<iostream>
#include<iomanip>
#include<typeinfo>
#include<list>

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define AST_SUB_NODE_SPACE "               "
#define COND_NODE_SPACE "               "

using namespace std;

class Ast;
static const char* opNames[] = {"EQ","NE","GT","LT","GE","LE" };  
static const char* arithOp[] = {"MINUS","PLUS","MULT","DIV"};  

class Ast
{
protected:
	typedef enum
	{
		zero_arity = 0,
		unary_arity = 1,
		binary_arity = 2
	}Ast_Arity;

	Data_Type node_data_type;
	Ast_Arity ast_num_child;

	int lineno;

public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast();
	virtual Symbol_Table_Entry & get_symbol_entry();

	virtual void print(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;

	virtual Code_For_Ast & compile() = 0;
	virtual Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra) = 0;
	virtual Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};

class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line);
	~Assignment_Ast();

	bool check_ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Name_Ast:public Ast
{
	Symbol_Table_Entry * variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line);
	~Name_Ast();

	Data_Type get_data_type();
	Symbol_Table_Entry & get_symbol_entry();

	void print(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
	Code_For_Ast & create_store_stmt(Register_Descriptor * store_register);
};



class Type_Expression_Ast:public Ast{
    private:
        Ast* type_exp;

    public:
        Type_Expression_Ast(Ast*, Data_Type);
        Type_Expression_Ast(Ast*);
        ~Type_Expression_Ast();
        Data_Type get_data_type();
        void print(ostream & file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
        Code_For_Ast & compile();
        Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Unary_Ast:public Ast{
    private:
        Ast* atomic_exp;
        bool minus;

    public:
        Unary_Ast(Ast*, bool);
        ~Unary_Ast();
        Data_Type get_data_type();
        void print(ostream & file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_enc , ostream & file_buffer);
        Code_For_Ast & compile();
        Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Boolean_Ast:public Ast
{
    public:
        enum BooleanOp{
            EQ = 0,
            NE,
            GT,
            LT,
            GE,
            LE
        };

    private:
        Ast* lhs_exp;
        Ast* rhs_exp;
        BooleanOp op;

    public:
        Boolean_Ast(Ast * lhs_exp , Ast * rhs_exp , BooleanOp op);
        ~Boolean_Ast();
        Data_Type get_data_type();
        bool check_ast();
        void print(ostream & file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
        Code_For_Ast & compile();
		Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);

};







class Arithmetic_Ast:public Ast
{
    public:
        enum ArithOp{
            MINUS,
            PLUS,
            MULTIPLY,
            DIVIDE
        };

    private:
        Ast* lhs_exp;
        Ast* rhs_exp;
        ArithOp op;

    public:
        Arithmetic_Ast(Ast* lhs_exp , Ast* rhs_exp , ArithOp op);
        ~Arithmetic_Ast();
        bool check_ast();
        Data_Type get_data_type();
        void print(ostream & file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_enc , ostream & file_buffer);
        Code_For_Ast & compile();
		Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};





class Goto_Ast:public Ast
{
    private:
        int block_no;
    public:
        Goto_Ast(int bb_no);
        ~Goto_Ast();
        void print(ostream &file_buffer);
        int getBlockNo();
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
        Code_For_Ast & compile();
		Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);

};








class Conditional_Ast:public Ast
{
    private:
        Ast* condition;
        Goto_Ast* true_goto;
        Goto_Ast* false_goto;

    public:
        Conditional_Ast(Ast* condition,Goto_Ast* trueGoto, Goto_Ast* falseGoto);
        ~Conditional_Ast();
        void print(ostream &file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
        Code_For_Ast & compile();
		Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};







template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type, int line);
	~Number_Ast();

	Data_Type get_data_type();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

class Return_Ast:public Ast
{

public:
	Return_Ast(int line);
	~Return_Ast();

	void print(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

	Code_For_Ast & compile();
	Code_For_Ast & compile_and_optimize_ast(Lra_Outcome & lra);
};

#endif
