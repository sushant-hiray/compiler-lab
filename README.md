compiler-lab
============

Incremental development of interpreter and compiler according to: [Course Website](http://www.cse.iitb.ac.in/~uday/cfglp/)

Details about level 0 implementation:

 cfglp : A CFG Language Processor
--------------------------------


About:
-----

Implemented by Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
Khedker (http://www.cse.iitb.ac.in/~uday)  for the courses
cs302+cs306: Language  Processors (theory and lab)  at IIT
Bombay.

Release  date Jan  15, 2013.  Copyrights reserved  by Uday
Khedker. This implemenation has been made available purely
for academic purposes without any warranty of any kind.


------------------------------------------------------------------------ 


Details of CFGLP (Control Flow Graph Language Processor) tool can be
found at http://www.cse.iitb.ac.in/~uday/cfglp

This directory (either level-0-32bits or level-0-64bits) contains 
the following:
- Source files for cfglp (.cc, .ll, .yy and .hh files)
- cfgpp32/cfglpp64 executable
- plugin32.so/plugin64.so shared object
- test_files directory
- valid .c files
- invalid .ecfg (erroneous cfg) files
- README
- Makefile.cfg
- Makefile.cfglp
- USER-MANUAL

------------------------------------------------------------------------ 


Details of Level 1 Interpretation:

This level extends level 0 by supporting intraprocedural control flow statements and evaluation of conditions in the following ways:

- Conditional expression e1?e2:e3, while, do-while, for, if, and if-else statements may exists in the C source program.
switch and goto statements are not present in the C program.
- The boolean conditions controlling the control flow consist of the six comparison operators (<, <=, >, >=, ==, and !=) and three logical operators  (&&, ||, and !).
- The values used for comparison are only integer values.
- The comparison expressions can also appear on the right hand sides of assignment statements.


------------------------------------------------------------------------
Details of Level 2 Interpretation:

- Apart from integers, single precision floating point types (introduced by the type specifier float) are supported.
- Expressions appearing in the right hand side of an assignment or in comparisons may contain the following arithmetic operators:  +, -, *, /, and unary minus. Note that the expressions do not have any side effects.

