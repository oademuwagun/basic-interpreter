BASIC INTERPRETER
========================================================

This project is a basic interpreter for a very simple language.

Language Rules
-------------------------------------------------------

The syntax for this language is defined by the following grammar:

<stmts> -> <stmt> | <stmt><eos> | <stmt><eos><stmts>

<stmt> -> <var> : <expr> | display<msp><var>
<eos> -> <lf> | <cr><lf>

<var> -> A | B | C | D | E
<expr> -> <term> | <term> + <term> | <term> - <term> | <term> * <term> | <term> / <term>
<msp> -> <sp> | <sp><msp>

<term> -> <var> | <const>

<const> -> <digit> | <digit><const>

<digit> -> 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0

Note:

<sp> => space character
<lf> => line feed character
<cr> => carriage return character

Also,
- At most five variables can be defined in a program.
- The symbol, ':' is the assignment operator. 
- It assigns the result of an expression to a variable.


Set Up
------------------------------------
Pre-requisites
- GCC

Instructions:
- Run the following command:
	gcc interpreter.c -std=c99 -o interpreter
	./interpreter [PROGRAM_FILE_NAME] 

	
Sample Program
-----------------------------
File content:

A:1
B:A+0
display A

Output:
1

More sample programs in the samples directory.

