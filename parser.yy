
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.
           
           Currently being updated by Sushant and Sanchit as a part of l
           ab course on Implementation of Programming Languages.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc
%union 
{
	int integer_value;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
    Boolean_Ast * e_Ast;
    Arithmetic_Ast* a_Ast;
    Goto_Ast * goto_ast;
    Conditional_Ast * cond_ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
    list<int> *basic_block_no;
    list<int> *goto_no;
	Procedure * procedure;
    float float_value;
    char char_value;
    Boolean_Ast::BooleanOp op;
    Return_Ast * return_ast;
    Data_Type dt;
};

%token <integer_value> INTEGER_NUMBER
%token <integer_value> BASICBLOCK
%token <float_value> FLOAT_NUMBER
%token <string_value> NAME
%token <return_ast> RETURN
%token INTEGER FLOAT DOUBLE IF ELSE GOTO ASSIGN_OP
%left NE EQ 
%left LT LE GT GE
%left '+' '-'
%left '*' '/'
%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> variable
%type <ast> constant
%type <cond_ast> if_control_block
%type <goto_ast> goto_statement
%type <ast> expression
%type <ast> logical_expression
%type <ast> atomic_expression
%type <ast> arithmetic_expression
%type <ast> unary_expression
%type <ast> type_expression
%type <dt> type_specifier

/* start symbol is named "program" */
%start program

%%	//separates the 2 sections Bison Directives and Grammar Rules										
/*** Grammar Rules ***/

program:
	declaration_statement_list procedure_name
    {       
            program_object.set_global_table(*$1);
            return_statement_used_flag = false; 
    }   
	procedure_body
    {
            program_object.set_procedure_map(*current_procedure);
            if ($1)
            $1->global_list_in_proc_map_check(get_line_number());
            delete $1;
    }
	|
	procedure_name
	{
        return_statement_used_flag = false; 
    }
    procedure_body
    {
        program_object.set_procedure_map(*current_procedure);
    }
;

procedure_name:
	NAME '(' ')'
    {
        current_procedure = new Procedure(void_data_type, *$1);
    }
;

procedure_body:
	'{' declaration_statement_list
    {
            current_procedure->set_local_list(*$2);
            delete $2;
    }
	basic_block_list '}'
	{
            if (return_statement_used_flag == false)
            {
              int line = get_line_number();
              report_error("Atleast 1 basic block should have a return statement", line);
            }
            int a = current_procedure->check_valid_goto();
            if(a!=0){
                string error_message = "bb ";
                char intStr[100];
                sprintf(intStr,"%d",a);
                string str = string(intStr);
                error_message.append(str);
                error_message.append(" doesn't exist");
                report_error(error_message,get_line_number());
            }
            current_procedure->set_basic_block_list(*$4);
            delete $4;
    }
    |
	'{' basic_block_list '}'
    {
            if (return_statement_used_flag == false)
            {
                int line = get_line_number();
                report_error("Atleast 1 basic block should have a return statement", line);
            }

            int a = current_procedure->check_valid_goto();
            if(a!=0){
                string error_message = "bb ";
                char intStr[100];
                sprintf(intStr,"%d",a);
                string str = string(intStr);
                error_message.append(str);
                error_message.append(" doesn't exist");
                report_error(error_message,get_line_number());
            }
            current_procedure->set_basic_block_list(*$2);

            delete $2;
    }
;

declaration_statement_list:
	declaration_statement
    {
        int line = get_line_number();
        program_object.variable_in_proc_map_check($1->get_variable_name(), line);

        string var_name = $1->get_variable_name();
        if (current_procedure && current_procedure->get_proc_name() == var_name)
        {
            int line = get_line_number();
            report_error("Variable name cannot be same as procedure name", line);
        }

        $$ = new Symbol_Table();
        $$->push_symbol($1);
    }
	|
	declaration_statement_list declaration_statement
    {
    
        // if declaration is local then no need to check in global list
        // if declaration is global then this list is global list

    int line = get_line_number();
    program_object.variable_in_proc_map_check($2->get_variable_name(), line);

    string var_name = $2->get_variable_name();
    if (current_procedure && current_procedure->get_proc_name() == var_name)
        {
            int line = get_line_number();
            report_error("Variable name cannot be same as procedure name", line);
        }

    if ($1 != NULL)
    {
        if($1->variable_in_symbol_list_check(var_name))
        {
            int line = get_line_number();
            report_error("Variable is declared twice", line);
        }

        $$ = $1;
    }

    else
    $$ = new Symbol_Table();

    $$->push_symbol($2);
    }
;

declaration_statement:
	type_specifier
    NAME ';'
    {
            $$ = new Symbol_Table_Entry(*$2, $1);

            delete $2;
    
    }
;

basic_block_list:
	basic_block_list basic_block
    {
        if (!$2)
        {
            int line = get_line_number();
            report_error("Basic block doesn't exist", line);
        }

        bb_strictly_increasing_order_check($$, $2->get_bb_number());

        $$ = $1;
        $$->push_back($2);
    
    }
	|
	basic_block	
    {
    
        if (!$1)
        {
            int line = get_line_number();
            report_error("Basic block doesn't exist", line);
        }

        $$ = new list<Basic_Block *>;
        $$->push_back($1);
    }
;

basic_block:
	BASICBLOCK ':' executable_statement_list
    {
    if($1 < 2){
        int line = get_line_number();
        report_error("Illegal basic block lable",line);
    }

    current_procedure->add_basic_block_no($1);
    
    if ($3 != NULL)
         $$ = new Basic_Block($1, *$3);
    else
    {
        list<Ast *> * ast_list = new list<Ast *>;
        $$ = new Basic_Block($1, *ast_list);

        }
    }
;

executable_statement_list:
	assignment_statement_list
	{
        $$ = $1;
    }
    |
	assignment_statement_list RETURN ';'
    {
            Ast * ret = new Return_Ast();

            return_statement_used_flag = true; // Current procedure has an occurrence of return statement

            if ($1 != NULL)
                $$ = $1;

            else
                 $$ = new list<Ast *>;
    
            $$->push_back(ret);
    }
    |
    assignment_statement_list if_control_block
    {
        if ($1 != NULL)
                    $$ = $1;

        else
                    $$ = new list<Ast *>;

        $$->push_back($2);
    }
    |
    assignment_statement_list goto_statement
    {
        if ($1 != NULL)
                    $$ = $1;

        else
                    $$ = new list<Ast *>;

        $$->push_back($2);
    }
;

assignment_statement_list:
    {
        $$ = NULL;
    }
	|
	assignment_statement_list assignment_statement
    {
        if ($1 == NULL)
                    $$ = new list<Ast *>;

        else
                    $$ = $1;

        $$->push_back($2);
    }
;

assignment_statement:
    variable ASSIGN_OP type_expression ';'
    {
	$$ = new Assignment_Ast($1, $3);

	int line = get_line_number();
	
    }
;

type_expression:
    expression
    {
        $$= new Type_Expression_Ast($1);
    }
    |
    '(' type_specifier ')' atomic_expression
    {
        $$=$4;
        $$= new Type_Expression_Ast($4,$2);
    
    }
;


expression:
    logical_expression
    {
        $$=$1;
    }
    |
    arithmetic_expression
    {
        $$=$1;
    }
    |
    unary_expression
    {
        $$ = $1;
    }
;


logical_expression:
    type_expression EQ type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::EQ);
    }
    |
    type_expression NE type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::NE);
    }
    |
    type_expression GT type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::GT);
    }
    |
    type_expression GE type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::GE);
    }
    |
    type_expression LT type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::LT);
    }
    |
    type_expression LE type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::LE);
    }
;

arithmetic_expression:
    type_expression '-' type_expression
    {
    
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::MINUS);
    }
    |
    type_expression '+' type_expression
    {
    
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::PLUS);
    }
    |
    type_expression '/' type_expression
    {
    
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::DIVIDE);
    }
    |
    type_expression '*' type_expression
    {
    
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::MULTIPLY);
    }
;

atomic_expression:
    variable
    {
         $$ = $1;
    }
    |
    constant
    {
         $$ = $1;
    }
    |
    '(' type_expression ')'
    {
        $$ = $2;
    }
;

unary_expression:
    '-' unary_expression
    {
        $$ = new Unary_Ast($2,1);
    }
    |
    atomic_expression
    {
        $$ = new Unary_Ast($1,0);
    }
;

if_control_block:
    IF '(' logical_expression ')' goto_statement ELSE goto_statement
    {
        $$ = new Conditional_Ast($3,$5,$7);   
    }
;

goto_statement:
    GOTO BASICBLOCK ';' 
    {
        $$ =  new Goto_Ast($2); 
        current_procedure->add_goto_no($2);
    }

;
variable:
	NAME
	{
		Symbol_Table_Entry var_table_entry;

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = program_object.get_symbol_table_entry(*$1);

		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
	}    
    
;

constant:
	INTEGER_NUMBER 
    {
	$$ = new Number_Ast<int>($1, int_data_type);
    }
    |
    FLOAT_NUMBER
    {
	$$ = new Number_Ast<float>($1, float_data_type);
    }

;

type_specifier:
    INTEGER
    {
        $$=Data_Type::int_data_type;
    } 
    |
    FLOAT
    {
        $$=Data_Type::float_data_type;
    }
    |
    DOUBLE
    {
        $$=Data_Type::double_data_type;
    }
;

