#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include "symboltable.h"

typedef enum eErrorType{
	DUPLICATED_DECLARATION_ERROR,
	VARIABLE_NOT_DECLARED_ERROR,
	ASSIGNMENT_TO_CONSTANT_ERROR,
	EXCEPTION_NAME_REFERENCED_ERROR,
	TYPE_CONSISTENCY_ERROR,
	NON_EXCEPTION_IN_RAISE_ERROR
}eErrorType;

void parse();
Token match(int expected_token_type);
/*
	The below function are the variables in grammar that being cycled through in order to check syntax errors
*/
void program();
void definitions();
void definitions_b();
void definition();
void definition_b();
void type();
void id_list();
void commands();
void commands_b();
void command();
void expression();
void expression_b();
void condition();

void error(int errorType, int line, char* lexeme);

#endif