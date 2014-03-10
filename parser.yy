
/********************************************************************************************

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
Variable has
           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


**********************************************************************************************/

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
%token INTEGER FLOAT DOUBLE VOID IF ELSE GOTO ASSIGN_OP
%left NE EQ 
%left LT LE GT GE
%left '+' '-'
%left '*' '/'
%type <symbol_table> declaration_statement_list
%type <symbol_table> parameter_list
%type <symbol_table> para_list
%type <symbol_entry> declaration_statement
%type <symbol_entry> procedure_declaration
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast_list> call_para_list
%type <ast_list> call_parameter_list
%type <ast> assignment_statement
%type <ast> function_call
%type <ast> variable
%type <ast> constant
%type <cond_ast> if_control_block
%type <goto_ast> goto_statement
%type <ast> expression
%type <ast> logical_expression
%type <ast> atomic_expression
%type <ast> arithmetic_expression
%type <ast> return_expression
%type <ast> unary_expression
%type <ast> type_expression
%type <dt> type_specifier
//%type <dt> return_type_specifier
%type <dt> VOID

/*  start symbol is named "program" */
%start program

%%	//separates the 2 sections Bison Directives and Grammar Rules										

program:
    declaration_statement_list 
    {
         program_object.set_global_table(*$1);
         return_statement_used_flag = false; 
    }
    procedure_list 
    |
    declaration_statement_list
    procedure_declaration_list 
    {
         program_object.set_global_table(*$1);
         return_statement_used_flag = false; 
    }
    procedure_list
    |
    procedure_declaration_list 
    {
    }
    procedure_list
    |
    {}
    procedure_list
;

procedure_list:
    procedure_defn
    {}
    |
    procedure_defn procedure_list
    {}
;

procedure_defn:
    procedure_name procedure_body
    {
        //cout<<" int procedure_defn\n";
        //program_object.set_procedure_map(*current_procedure);
        current_procedure->set_defined(true);
        if(return_statement_used_flag == false){
            cout<<"no return statement"<<endl;
        }
    }
;

procedure_name:
	NAME '(' parameter_list ')'
    {
        //cout<<"in procedure_name"<<endl;
        if(*$1!="main"){
          //  cout<<"procedure is not main"<<endl;
            current_procedure = program_object.get_procedure(*$1);
            if(current_procedure == NULL){
                report_error("Procedure corresponding to the name is not found", get_line_number());
            }
            else if(current_procedure->is_defined()){
                report_error("Procedure has already been defined before",get_line_number());
            }
            //cout<<" int procedure_defn\n";
        }

        else{
            //cout<<"from here only\n";
            current_procedure = new Procedure(void_data_type, *$1);
            if($3==NULL){
                current_procedure->set_local_list(* new Symbol_Table());
               // cout<<"param list is empty in declaration"<<endl;
            }
            else{
                current_procedure->set_local_list(*$3);                 
            }
           
        }
        if($3!=NULL){
            //cout<<"not null\n";
            //cout<<current_procedure->get_local_symbol_table().get_variable_table().size()<<endl;
            current_procedure->check_parameter_list($3, get_line_number());
        }
        else{
            current_procedure->check_parameter_list(new Symbol_Table(), get_line_number());
        }             
        //cout << "here1\n";
    }
;

procedure_body:
	'{' declaration_statement_list
    { 
            //cout<<"before current_procedure->append_local_list(*$2);"<<endl;
            current_procedure->append_local_list(*$2,get_line_number());
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

procedure_declaration_list:
    procedure_declaration
    {}
    |
    procedure_declaration_list procedure_declaration
    {}
;

procedure_declaration:
    type_specifier NAME '(' parameter_list ')' ';'
    { 
        if(*$2!="main"){
            $$ = new Symbol_Table_Entry(*$2, Data_Type::function_data_type);
            Procedure* proc = new Procedure($1,*$2);
            if($4==NULL){
                //cout<<"param list is empty in declaration"<<endl;
                proc->set_local_list(* new Symbol_Table());
                proc->set_parameter_length(0);
            }
            else{
                //cout<<"param list is not empty in declaration"<<endl;
                proc->set_local_list(*$4);
                proc->set_parameter_length($4->get_variable_table().size());
            }

            program_object.set_procedure_map(*proc);
        }
    }
    |
    VOID NAME '(' parameter_list ')' ';'
    {
            $$ = new Symbol_Table_Entry(*$2, Data_Type::function_data_type);
            Procedure* proc = new Procedure(Data_Type::void_data_type,*$2);
            
            if($4==NULL){ 
                proc->set_local_list(* new Symbol_Table());
                proc->set_parameter_length(0);
            }
            else{
                proc->set_local_list(*$4);
                proc->set_parameter_length($4->get_variable_table().size());
            }
            program_object.set_procedure_map(*proc);
    }
;

declaration_statement_list:
	declaration_statement
    {
        int line = get_line_number();
        //cout << $1->get_variable_name()<<endl;
        program_object.variable_in_proc_map_check($1->get_variable_name(), line);
        string var_name = $1->get_variable_name();
        //cout<<var_name<<"\n";
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
     
        /*  if declaration is local then no need to check in global list
            if declaration is global then this list is global list
        */

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

para_list:
    type_specifier NAME
    {
        $$ = new Symbol_Table();
        Symbol_Table_Entry* ste = new Symbol_Table_Entry(*$2, $1);
        $$->push_symbol(ste);
    }
    |
    para_list ',' type_specifier NAME
    {
        if ($1 != NULL)
        {
            $$ = $1;
        }

        else
            $$ = new Symbol_Table();
        if($$->variable_in_symbol_list_check(*$4)){
            report_error("Formal Parameter declared twice",get_line_number());
        }
        else{
            Symbol_Table_Entry* ste = new Symbol_Table_Entry(*$4, $3);
            $$->push_symbol(ste);
        }
    }
;

parameter_list:
    {
        //cout<<"parameter list is null:\n";
        $$ = NULL;
    }
    |
    para_list
    {
        $$ = $1;
    }
;

declaration_statement:
	type_specifier NAME ';'
    {
            $$ = new Symbol_Table_Entry(*$2, $1);
    }
;


call_para_list:
    type_expression
    {
        $$ = new list<Ast*>();
        $$->push_back($1);
    }
    |
    call_para_list ',' type_expression
    {
        if ($1 != NULL)
        {

            $$ = $1;
        }

        else
            $$ = new list<Ast*>();

        $$->push_back($3);   
    }
;

call_parameter_list:
    {
        $$ = new list<Ast*>();
    }
    |
    call_para_list
    {
        $$ = $1;
    }
;

function_call:
    NAME '(' call_parameter_list ')'
    { 
        Procedure* p = program_object.get_procedure(*$1);  
        if(p!=NULL){
            Data_Type dt = program_object.get_return_type(*$1);
            $$ = new Call_Ast(*$1,*$3, dt);

        }
        else{
            report_error("Procedure corresponding to the name is not found", get_line_number());
        }

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

    if ($3 != NULL){
        $$ = new Basic_Block($1, *$3);
    }

    else
    {
        list<Ast *> * ast_list = new list<Ast *>;
        $$ = new Basic_Block($1, *ast_list);

        }
     }
;

return_expression:
    {
        $$ = NULL;
    }
    |
    type_expression
    {
        $$ = $1;
    }
;

executable_statement_list:
	assignment_statement_list
	{
         $$ = $1;
    }
    |
	assignment_statement_list RETURN return_expression';'
    { 
             Ast * ret;
             if($3!=NULL){
                if(current_procedure->get_proc_name() == "main"){
                    if(current_procedure->get_return_type() != void_data_type){
                        if(current_procedure->get_return_type() != $3->get_data_type()){
                            report_error("Two or more types of return values", get_line_number());
                        }
                    }
                    else{
                        current_procedure->set_return_type($3->get_data_type());
                    }
                }

                if(current_procedure->get_return_type() != $3->get_data_type()){
                    report_error("Last return statement type, of procedure, and its prototype should match",get_line_number());
                }
                ret = new Return_Ast($3);

             }
            else{
                ret= new Return_Ast();
            }

            return_statement_used_flag = true;   //Current procedure has an occurrence of return statement



            if ($1 != NULL){
                $$ = $1;
            }

            else{
                $$ = new list<Ast *>;
            }
    
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
	$$->check_ast(line);
     }
    |
    function_call ';'
    {
        $$ = $1;
    }
;

type_expression:
    expression
    {
         $$= new Type_Expression_Ast($1);
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
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression NE type_expression
    { 
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::NE);
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression GT type_expression
    { 
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::GT);
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression GE type_expression
    { 
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::GE);
        int line = get_line_number();
        $$->check_ast(line);  
    }
    |
    type_expression LT type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::LT);
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression LE type_expression
    {
        $$ = new Boolean_Ast($1,$3,Boolean_Ast::BooleanOp::LE);
        int line = get_line_number();
        $$->check_ast(line); 
     }
;

arithmetic_expression:
    type_expression '-' type_expression
    {
     
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::MINUS);
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression '+' type_expression
    {
     
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::PLUS);
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression '/' type_expression
    {
     
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::DIVIDE);
        int line = get_line_number();
        $$->check_ast(line); 
     }
    |
    type_expression '*' type_expression
    {
     
        $$ = new Arithmetic_Ast($1,$3,Arithmetic_Ast::ArithOp::MULTIPLY);
        int line = get_line_number();
        $$->check_ast(line); 
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
    |
    '(' type_specifier ')' atomic_expression
    {
         $$=$4;
         $$= new Type_Expression_Ast($4,$2);
    
    }
    |
    function_call
    {
         $$ = $1;
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
