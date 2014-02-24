
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

#define AST_SPACE "         "
#define AST_NODE_SPACE "            "
#define COND_NODE_SPACE "               "

using namespace std;


static const char* boolOp[] = {"EQ","NE","GT","LT","GE","LE" };    
static const char* arithOp[] = {"MINUS","PLUS","MULT","DIV"};  
class Ast;

class Ast
{
protected:
	Data_Type node_data_type;
public:
	Ast();
	~Ast();

	virtual Data_Type get_data_type();
	virtual bool check_ast(int line);

	virtual void print_ast(ostream & file_buffer) = 0;
	virtual void print_value(Local_Environment & eval_env, ostream & file_buffer);

	virtual Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	virtual void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	virtual Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer) = 0;
};






class Assignment_Ast:public Ast
{
	Ast * lhs;
	Ast * rhs;

public:
	Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs);
	~Assignment_Ast();

	Data_Type get_data_type();
	bool check_ast(int line);

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};







class Name_Ast:public Ast
{
	string variable_name;
	Symbol_Table_Entry variable_symbol_entry;

public:
	Name_Ast(string & name, Symbol_Table_Entry & var_entry);
	~Name_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	void print_value(Local_Environment & eval_env, ostream & file_buffer);
	Eval_Result & get_value_of_evaluation(Local_Environment & eval_env);
	void set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result);
	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Type_Expression_Ast:public Ast{
    private:
        Ast* type_exp;

    public:
        Type_Expression_Ast(Ast*, Data_Type);
        Type_Expression_Ast(Ast*);
        ~Type_Expression_Ast();
        Data_Type get_data_type();
        void print_ast(ostream & file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
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
        void print_ast(ostream & file_buffer);
        bool check_ast(int line);
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

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
        Data_Type get_data_type();
        void print_ast(ostream & file_buffer);
        bool check_ast(int line);
        Eval_Result & evaluate(Local_Environment & eval_enc , ostream & file_buffer);
};

class Unary_Ast:public Ast{
    private:
        Ast* atomic_exp;
        bool minus;

    public:
        Unary_Ast(Ast*, bool);
        ~Unary_Ast();
        Data_Type get_data_type();
        void print_ast(ostream & file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_enc , ostream & file_buffer);
};

class Goto_Ast:public Ast
{
    private:
        int block_no;
    public:
        Goto_Ast(int bb_no);
        ~Goto_Ast();
        void print_ast(ostream &file_buffer);
        int getBlockNo();
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);

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
        void print_ast(ostream &file_buffer);
        Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};













template <class T>
class Number_Ast:public Ast
{
	T constant;

public:
	Number_Ast(T number, Data_Type constant_data_type);
	~Number_Ast();

	Data_Type get_data_type();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

class Return_Ast:public Ast
{

public:
	Return_Ast();
	~Return_Ast();

	void print_ast(ostream & file_buffer);

	Eval_Result & evaluate(Local_Environment & eval_env, ostream & file_buffer);
};

#endif
