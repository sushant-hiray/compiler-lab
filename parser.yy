
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
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
    float float_value;
    char char_value;
};

%token <integer_value> INTEGER_NUMBER
%token <string_value> NAME
%token BASICBLOCK RETURN INTEGER IF ELSE GOTO 

/* start symbol is named "program" */
%start program

%%	//separates the 2 sections Bison Directives and Grammar Rules										
/*** Grammar Rules ***/

program:
	declaration_statement_list procedure_name
	procedure_body
    {}
	|
	procedure_name
	procedure_body
    {}
;

procedure_name:
	NAME '(' ')'
    {}
;

procedure_body:
	'{' declaration_statement_list
	basic_block_list '}'
	{}
    |
	'{' basic_block_list '}'
    {}
;

declaration_statement_list:
	declaration_statement
    {}
	|
	declaration_statement_list declaration_statement
    {}
;

declaration_statement:
	type_specifier
    NAME ';'
    {}
;

basic_block_list:
	basic_block_list basic_block
    {}
	|
	basic_block	
    {}
;

basic_block:
	BASICBLOCK ':' executable_statement_list
    {}
;

executable_statement_list:
	assignment_statement_list
	{}
    |
	assignment_statement_list RETURN ';'
    {}
    |
    assignment_statement_list if_control_block
    {}
    |
    assignment_statement_list goto_statement
    {}
;

assignment_statement_list:
	|
	assignment_statement_list assignment_statement
    {}
;

assignment_statement:
    variable '=' expression ';'
    {}
;

expression:
    logical_expression
    {}
;

boolean_op:
    '=' '=' 
    {}
    | 
    '!' '='
    {}
    |
    '>' 
    {}
    |
    '<' 
    {}
    |
    '>' '='
    {}
    |
    '<' '='
    {}
;

logical_expression:
    expression boolean_op atomic_expression
    {}
    |
    atomic_expression
    {}
;

atomic_expression:
    variable
    {}
    |
    constant
    {}
;


if_control_block:
    IF '(' logical_expression ')' goto_statement ELSE goto_statement
    {}
;

goto_statement:
    GOTO BASICBLOCK ';' 
    {}

;
variable:
	NAME
    {}
;

constant:
	INTEGER_NUMBER 
    {}
;

type_specifier:
    INTEGER
    {}
;

