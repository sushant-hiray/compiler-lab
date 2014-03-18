
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented by Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors (theory and lab)  at IIT
           Bombay.

           Release  date Jan  15, 2013.  Copyrights reserved  by Uday
           Khedker. This implemenation has been made available purely
           for academic purposes without any warranty of any kind.

           A  doxygen   generated  documentation  can  be   found  at
           http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <string>

using namespace std;

#include "common-classes.hh"

#include "user-options.hh"
#include "error-display.hh"

void report_error(string error_message, int line)
{
	string file_name = command_options.get_file_name();

	stringstream message;
	if (line > NOLINE)
		message << file_name << " : line " << line << " :: error : " << error_message;
	else
		message << file_name << " :: cfglp error : " << error_message;
	print_error(message.str(), NOTEXIT);
	exit(0);
}

void print_error(string error_message, int exit_flag)
{
	cerr << error_message << "\n";

	if (command_options.is_do_eval_selected())
	{
		exit(0);
	}

	if (exit_flag)
		exit(0);
}

void check_invariant_underlying_function(bool condition, string error_message)
{
	if (!condition)
	{
		cerr << "\ncfglp internal error: " << error_message << "\n";

		command_options.remove_files();
		exit(1);
	}
}

bool global_error_status = false;

void report_violation_of_condition(bool condition, string error_message, int lineno)
{
	string file_name = command_options.get_file_name();
	if (!condition)
	{
		cerr <<  "cfglp error: File: " << file_name << ", Line: " << lineno << ": " << error_message << "\n";
		global_error_status = true;
	}
} 

void report_violation_and_abort(bool condition, string error_message, int lineno)
{
	string file_name = command_options.get_file_name();
	if (!condition)
	{
		cerr <<  "cfglp error: File: " << file_name << ", Line: " << lineno << ": " << error_message << "\n";
		exit(1);
	}
} 

bool error_status()
{
	return global_error_status;
}
