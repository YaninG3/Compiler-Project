#ifndef DATABASE_MODEL_DEF_H
#define DATABASE_MODEL_DEF_H

#include "tokens.h"


/*
structue representing one array of tokens
*/
typedef struct arrayNode{
	Token tokens_arr[100];
	int token_counter;
	struct arrayNode *next;
	struct arrayNode *prev;
}ArrayList;


void create_and_store_token(eTokenType tokenType, char *lexeme, int lineNum);
Token next_token();
Token back_token();
void deleteArrayList();
void print_token(Token token);
void printArrayList();

#endif