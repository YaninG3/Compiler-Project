/* 1. Declarations/definition part contains internal to lex definitions of text replacements */
%{
/* C declarations  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "databasemodel.h"
#include "databasemodel.c"
#include "tokens.h"
#include "parser.h"
#include "parser.c"
#include "symboltable.h"
#include "symboltable.c"

int line = 1;
int lexErrors = 0;
%}
ID			[a-z][a-z0-9]*
DIGIT			[0-9]
INTEGER			0|[1-9]{DIGIT}*
REAL			{INTEGER}+("."{DIGIT}+)?
NUMBER			{INTEGER}|{REAL}
REL_OP			"<"|">"|"=="|"!="|"<="|">="
BINARY_AR_OP		"+"|"/"|"-"|"*"
ASSIGNMENT		"="
SEMICOLON		[;]
COLON			[:]
COMMA			[,]
OPEN_BRACKET		[(]
CLOSE_BRACKET		[)]
OPEN_CURLY_BRACKET	[{]
CLOSE_CURLY_BRACKET	[}]
KEYWORD_START		"start"
KEYWORD_FINISH		"finish"
KEYWORD_REAL		"real"
KEYWORD_INTEGER		"integer"
KEYWORD_WHILE		"while"
KEYWORD_DO		"do"
KEYWORD_OD		"od"
KEYWORD_BLOCK		"block"
KEYWORD_RAISE		"raise"
KEYWORD_END_BLOCK	"end_block"
KEYWORD_EXCEPTION	"exception"
KEYWORD_CONST		"const"
%%/* 2. Translation rules part -  is built from regular expressions and action to be performed when regular expression is matched. */
{NUMBER}		{create_and_store_token(NUMBER, yytext, line);return 1;}
{REL_OP}		{create_and_store_token(REL_OP, yytext, line);return 1;}
{BINARY_AR_OP}		{create_and_store_token(BINARY_AR_OP, yytext, line);return 1;}
{ASSIGNMENT}		{create_and_store_token(ASSIGNMENT, yytext, line);return 1;}
{SEMICOLON}		{create_and_store_token(SEMICOLON, yytext, line);return 1;}
{COLON}			{create_and_store_token(COLON, yytext, line);return 1;}
{COMMA}			{create_and_store_token(COMMA, yytext, line);return 1;}
{OPEN_BRACKET}		{create_and_store_token(OPEN_BRACKET, yytext, line);return 1;}
{CLOSE_BRACKET}		{create_and_store_token(CLOSE_BRACKET, yytext, line);return 1;}
{OPEN_CURLY_BRACKET}	{create_and_store_token(OPEN_CURLY_BRACKET, yytext, line);return 1;}
{CLOSE_CURLY_BRACKET}	{create_and_store_token(CLOSE_CURLY_BRACKET, yytext, line);return 1;}
{KEYWORD_START}		{create_and_store_token(KEYWORD_START, yytext, line);return 1;}
{KEYWORD_FINISH}	{create_and_store_token(KEYWORD_FINISH, yytext, line);return 1;}
{KEYWORD_REAL}		{create_and_store_token(KEYWORD_REAL, yytext, line);return 1;}
{KEYWORD_INTEGER}	{create_and_store_token(KEYWORD_INTEGER, yytext, line);return 1;}
{KEYWORD_WHILE}		{create_and_store_token(KEYWORD_WHILE, yytext, line);return 1;}
{KEYWORD_DO}		{create_and_store_token(KEYWORD_DO, yytext, line);return 1;}
{KEYWORD_OD}		{create_and_store_token(KEYWORD_OD, yytext, line);return 1;}
{KEYWORD_BLOCK}		{create_and_store_token(KEYWORD_BLOCK, yytext, line);return 1;}
{KEYWORD_RAISE}		{create_and_store_token(KEYWORD_RAISE, yytext, line);return 1;}
{KEYWORD_END_BLOCK}	{create_and_store_token(KEYWORD_END_BLOCK, yytext, line);return 1;}
{KEYWORD_EXCEPTION}	{create_and_store_token(KEYWORD_EXCEPTION, yytext, line);return 1;}
{KEYWORD_CONST}		{create_and_store_token(KEYWORD_CONST, yytext, line);return 1;}
{ID}			{create_and_store_token(ID, yytext, line);return 1;}
[ \t]+			/* eat up whitespace */
<<EOF>>			{printf("\nLEX: ---- End of File ----, line #%d\n",line); yyterminate(); return 0;}
[\n]			{line++;}
.			{printf("\nLEX: Lexical Error. lexeme '%s' is unknown (line #%d)\n",yytext,line); lexErrors++; next_token(); return 1;} //if an undentified token found, get the next token
%%
/* 3. User Subroutines */

int yywrap()
{
	return 1;
}

void main(int argc, char* argv[])
{
	//read an input text file, if missing than read from the c onsole
	if(argv[1])
		yyin = fopen(argv[1], "r");
	else
		yyin=stdin;
	yyout = stdout;
	
	//initiate the parser
	parse();
	
	//close the text file
	if(argv[1]){
		fclose(yyin);
	}

	//delete the database
	deleteArrayList();

	//error report
	if(lexErrors>0 || semantic_errors>0 || miss_match>0){
		printf("\nLexical Erros:	%d",lexErrors);
		printf("\nSyntax Erros:	%d",miss_match);
		printf("\nSemantic Erros:	%d",semantic_errors);
	}
	else{
		printf("\nCode is Good");
	}
	//intended for stop the console from closing itself
	getchar();
} 
