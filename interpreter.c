#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int variables[5];
int charClass;
char lexeme[100];
char prevLine[100];
char line[100];
char nextChar;
int lexLen;
int lineLen;
int token;
int nextToken;
FILE *in_fp, *fopen();

int lineNo = 1;

/*Function declarations */

void getChar(void);
int lex(void);
void addChar(void);
int lookup(char);

/*Recursive descent functions declarations */
void script(void);
void stmts(void);
void eos(void);
void stmt(void);
int expr(void);
int term(void);
char var(void);
int constant(void);
void msp(void);
int digit(void);

/*Recursive descent helper functions declarations */
void error(const char*);
int hash(char);
void addToLine(char);
void startNewLine(void);

/*Character Classes*/
#define LETTER 0
#define DIGIT 1
#define DISPLAY 2
#define UNKNOWN 99


/*Token Codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define DISP_OP 25
#define L_FEED 26
#define C_RETURN 27
#define SPACE 28


/*main driver. Opens the script file and executes it. */

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Expects an argument. Please run again with one.\n");
	}

	else{

		/* Marks all variables as empty */
		for(int i =0; i<5; i++){
			variables[i] = -1;
		}

		/* If the file fails to open, exit with an error */
		if((in_fp = fopen(argv[1], "r")) == NULL)
			{error("Cannot open file");}

		/* else */
		else {
			//Get the first character in the file
			getChar();

			// Get the next token 
			lex();
			
			// Begin parsing and executing the script
			do{
				script();
			} while(nextToken != EOF);  
		} 

		//Close the file
		fclose(in_fp);
	}
	return 0;
}

//*************************************************************************************

/* addChar - a function to add the next character to the current lexeme */

void addChar(){

	/*As long as the maximum length of a lexeme will not be exceeded, 
	add the next character to the lexeme.
	The maximum length for a lexeme is 99. 
	The next cell in the lexeme array is reserved for the null character */ 

	if(lexLen <= 98){
		lexeme[lexLen++] = nextChar;
		lexeme[lexLen] = 0;
	}
	else
		{error("Lexeme is too long");}
}


//********************************************************************************

/* lookup - a function to lookup operators or special characters (newline, carriage return, and space),
add the character to the current lexeme and then return the token for these characters */

int lookup(char ch){
	switch(ch) {
		case ':':
			addChar();
			nextToken = ASSIGN_OP;
			break;
		case '+':
			addChar();
			nextToken = ADD_OP;
			break;

		case '-':
			addChar();
			nextToken = SUB_OP;
			break;

		case '*':
			addChar();
			nextToken = MULT_OP;
			break;

		case '/':
			addChar();
			nextToken = DIV_OP;
			break;

		//If the character is a newline, then start a new line.
		case '\n':
			startNewLine();
			addChar();
			nextToken = L_FEED;
			break;

		case '\r':
			addChar();
			nextToken = C_RETURN;
			break;

		case ' ':
			addChar();
			nextToken = SPACE;
			break;

		/*If the character is not among the language-defined operators and special characters
		(space, carriage return, and newline), exit with an error */
		default:
			error("Invalid expression");
			break;
	}

	return nextToken;
}

//*************************************************************************************

/* getChar - a function to get the next character of input and determine its character class
and mark it as part of the current line if it is not the newline or carriage return */

void getChar(){
	if((nextChar = fgetc(in_fp)) != EOF){

		//Mark the character retrieved as part of the current line if not the newline or carriage return
		if(nextChar != '\r' && nextChar != '\n'){
			addToLine(nextChar);
		}
		if(nextChar == 'A' || nextChar == 'B' || nextChar == 'C' || nextChar == 'D' || nextChar == 'E'){
			charClass = LETTER;
		}
		else if(nextChar == 'd'){
			charClass = DISPLAY;
		}
		else if (isdigit(nextChar)) {
				charClass = DIGIT;
		}
		else {
			charClass = UNKNOWN;
		}
	}
	else
		{charClass = EOF;}

}

//************************************************************************************
/* lex - a simple lexical analyzer 
It moves to the next lexeme. It then adds the current character to the current lexeme.

*/
int lex(){
	lexLen = 0;
	switch(charClass) {
		/*Parse identifiers */
		case LETTER:
			addChar();
			getChar();
			nextToken = IDENT;
			break;
		/*
		Add the current character which should be 'd' to the current lexeme.
		Ensure the following 6 characters match 'isplay' else exit with an error.			
		*/
		case DISPLAY:
			addChar();
			getChar(); 
			nextToken = DISP_OP;
			
			//Ensure the following 6 characters match 'isplay'

			char crosscheck[] = "isplay";
			for(int i = 0; i < 6; i++){
				if(nextChar != crosscheck[i]){
					error("Invalid lexeme");
					break;
				}
				addChar();
				getChar();
			}
			break;

		/*Parse integer literals */
		case DIGIT:
			addChar();
			getChar();
			nextToken = INT_LIT;
			break;

		/*operators */
		case UNKNOWN:
			lookup(nextChar);
			getChar();
			break;

		/* EOF */
		case EOF:
			nextToken = EOF;
			lexeme[0] = 'E';
			lexeme[1] = '0';
			lexeme[2] = 'F';
			lexeme[3] = 0;
			break;

	} /*End of Switch */

	//printf("Next Token is : %d, Next Lexeme is %s\n", nextToken, lexeme);
	return nextToken;

} /* End of function lex */

//******************************************************************************
//******************************************************************************

/*Recursive Descent Function Declarations */

//*****************************************************************************

/*
Parses and executes the strings in the language generated by the rule:
<script> -> <stmts>
*/

void script(){
	//Parse and execute the statements in the script
	stmts();
}

//******************************************************************************
/*
Parses the strings in the language generated by the rule:
<stmts> -> <stmt> | <stmt><eos> | <stmt><eos><stmts>
*/
void stmts(){
	//Parse the first statement
	stmt();

	//If there are still more statements
	if(nextToken == L_FEED || nextToken == C_RETURN){
		//Parse the end of statement characters (newline and carriage return)
		eos();
		//If there are still more statements, parse the next statements
		if(nextToken != EOF){
			stmts();
		}
	}
}

//*******************************************************************************
/*
Parses the strings in the language generated by the rule:
<eos> -> <lf> | <cr><lf>
*/
void eos(){
	/*
	Determine which RHS and then get the next token.
	If the characters do not match the rule for <eos>, then exit with an error. 
	*/
	if(nextToken == L_FEED){
		lex();
	}
	else if(nextToken == C_RETURN){
		lex(); 
		if(nextToken == L_FEED){
			lex();
		}
		else{
			error("Invalid expression");
		}
	}
	else{
		error("Invalid expression");
	}
}

//*********************************************************************************
/*
Parses the strings in the language generated by the rule:
<stmt> -> <var> : <expr> | display<msp><var>
And then executes the statement
*/
void stmt(){
	//Determine the type of statement.
	//If it is an assignment statement
	if(nextToken == IDENT){
		//Get the identifier variable. 
		char identifier = var();
		int identifierIndex = hash(identifier);

		if(nextToken == ASSIGN_OP){
			lex();
			//Parse and execute the expression in the statement
			int exprResult = expr();

			//Store the value in the variable identified by the identifier
			variables[identifierIndex] = exprResult;
		}
		//If the statement does not match the rule for an assignment, then exit with an error
		else{
			error("Invalid expression");
		}
	}
	//Else if it is a display statement
	else if(nextToken == DISP_OP){
		lex();

		//Parse the multiple spaces between the display keyword and the identifier
		msp();

		/* 
		Get the identifier. If its corresponding variable has no value, then exit with an erro
		else print the value.
		*/
		if(nextToken == IDENT){
			char identifier = lexeme[lexLen-1];
			int identifierVal = hash(identifier);
			if(variables[identifierVal] == -1){
				error("No value for variable");
			}
			else{
				printf("%c is %d\n", identifier, variables[identifierVal]);
			}
			lex();
		}
		//If the statement does not match the rule for a display statement, then exit with an error
		else{
			error("Invalid expression");
		}
	}
	//If the statement does not match the rule for a statement, then exit with an error.
	else{
		error("Invalid expression");
	}

}

//*********************************************************************************
/*
Parses the strings in the language generated by the rule:
<expr> -> <term> | <term> + <term> | <term> - <term> | <term> * <term> | <term> / <term>

Returns the value of the expression represented the string.
*/
int expr(){
	//Parse the first term in the expression and retrieve its value
	int firstTerm = term();
	int val = firstTerm;
	int secondTerm = 0;
	int operation = -1;

	//If the expression includes an operator
	if(nextToken == ADD_OP || nextToken == SUB_OP || nextToken == MULT_OP || nextToken == DIV_OP){
		operation = nextToken;
		lex();
		secondTerm = term();
		//Then retrieve the second term and execute the appropriate operation
		if(operation == ADD_OP){
			val = firstTerm + secondTerm;
		}
		else if(operation == SUB_OP){
			val = firstTerm - secondTerm;
		}
		else if(operation == MULT_OP){
			val = firstTerm * secondTerm;
		}
		else if(operation == DIV_OP){
			//If the second term is zero and the operation is division, then exit with an error
			if(secondTerm == 0){
				error("Invalid operation: Division by zero");
			}
			val = firstTerm / secondTerm; 
		}
	}
	//Return the result of the expression
	return val;
}


//*********************************************************************************
/*
Parses the strings in the language generated by the rule:
<term> -> <var> | <const>

Returns the value of the term represented by the string
*/
int term(){
	int val = 0;

	//If the term is an identifier
	if(nextToken == IDENT){

		/*Get the value of the variable refereneced by the identifier.
		If the variable has no value, exit with an error */

		char identifier = var();
		int identVal = hash(identifier);
		if(variables[identVal] == -1){
			error("No value for variable");
		}
		else{
			val = variables[identVal];
		}	
	}
	//If the term is an integer, get the integer represented by the term.
	else if(nextToken == INT_LIT){
		val = constant();
	}
	//Else if the term does not match the rule, exit with an error.
	else{
		error("Invalid expression");
	}

	//Return the value
	return val;

}

//********************************************************************************
/*
Parses the strings in the language generated by the rule:
<var> -> A | B | C | D | E

Returns the identifier referenced by the string
*/

char var(){
	char val = '\0';
	//Get the identifier
	if(nextToken == IDENT){
		val = lexeme[lexLen-1];
		lex();
	}
	//If the character retrieved does not match the rule for <var>, exit with an error
	else{
		error("Invalid expression");
	}

	//Return the identifier
	return val;
}

//********************************************************************************
/*
Parses the strings in the language generated by the rule:
<const> -> <digit> | <digit><const>

Return the integer represented by this string
*/
int constant(){
	int val = 0;

	//Get all the digits in the integer and calculate its value
	if(nextToken == INT_LIT){
		int firstDigit = digit();
		int noOfDigits = 1;
		while(nextToken == INT_LIT){
			val = val*10 + digit();
			noOfDigits++;
		}
		val = firstDigit*pow(10, noOfDigits-1) + val;
	}

	//If the string does not match the rule, exit with an error.
	else {
		error("Invalid expression");
	}

	//Return the integer
	return val;
}

//********************************************************************************
/*
Parses the strings in the language generated by the rule:
<msp> -> <sp> | <sp><msp>
*/

void msp(){
	//Get the first space character and keep getting space characters as long as the next character is a space character
	if(nextToken == SPACE){
		lex();
		while(nextToken == SPACE){
			lex();
		}
	}

	//If the string does not match the rule for <msp>, exit with an error
	else{
		error("Invalid expression");
	}
}

//********************************************************************************
/*
Parses the strings in the language generated by the rule:
<digit> -> 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0
*/
int digit(){
	int digitVal = 0;

	//If the current character represents a digit, get the digit represented
	if(nextToken == INT_LIT){
		digitVal = atoi(&lexeme[lexLen-1]);
		lex();
	}

	//If the string does not match the rule for <msp>, exit with an error
	else {
		error("Invalid expression");
	}

	//Return the digit
	return digitVal;
}

//**********************************************************************************
/*
Exits the program with an error message specified by the parameter
*/

void error(const char* s){

	//If at a new line, meaning that the error occurred at the end of the previous line
	if(nextToken == L_FEED || nextToken == C_RETURN){
		//Indicate the previous line as the location of the error
		printf("Error at line %d.\n", lineNo-1);
		printf("  \"%s\"\n", prevLine);
	}

	//Else add all the characters left in the current statement to the current line
	//Indicate the current line as the location of the error
	else{
		while(nextChar != EOF && nextChar != '\n' && nextChar != '\r'){
			getChar();
		}
		printf("Error at line %d.\n", lineNo);
		printf("  \"%s\"\n", line);
	}

	//Close the file
	fclose(in_fp);

	//Print the error message
	printf("  %s.\n", s);

	//Exit
	exit(0);
}

//**********************************************************************************

/*Returns the value of a identifier which is used to index the value of the variable it references.*/

int hash(char s){
	return (int)s - 65;
}

//**********************************************************************************

/*
Adds a character to the current line. Exits with an error if the current line will be exceed its 
maximum length (maximum length is 99. The last cell is reserved for the null character).
*/

void addToLine(char c){
	if(lineLen <= 98){
		line[lineLen++] = c;
		line[lineLen] = 0;
	}
	else{
		error("Line too long");
	}
}

//**********************************************************************************

/*
Marks the current line as the previous line and then move to the enxt a new line.
*/
void startNewLine(){
	strcpy(prevLine, line);
	lineLen = 0;
	line[lineLen] = 0;
	lineNo++;
}
