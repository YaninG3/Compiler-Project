#include "databasemodel.h"

ArrayList* getArrayList(Token tkn);
ArrayList* crrnt_arrlst = NULL;
int back_token_flag = 0;

/*
	create_and_store_token() recieves a token properties and storing them as an element
	inside the array of tokens
	each array of tokens is actualy a node in a 2 directiîal linked list
	soon as the first token is recieved, a node with array of 100 places for tokens is created.
	in case there's a need for more token places (more than 100) a new node of token array is created and linked to the privious node,
*/
void create_and_store_token(eTokenType tokenType, char *lexeme, int lineNum){
	Token local_token;
	local_token.type = tokenType;
	local_token.lexeme = (char*)malloc(sizeof(char)*(yyleng+1));
	strcpy(local_token.lexeme,lexeme);
	local_token.num_of_line = lineNum;

	/* case this is the first token that been read, will create the first token array node */
	if (crrnt_arrlst==NULL){	
		crrnt_arrlst=getArrayList(local_token);
	}
	else{
		/*case the current array of tokens is not full*/
		if(crrnt_arrlst->token_counter <= 99){
			crrnt_arrlst->tokens_arr[crrnt_arrlst->token_counter] = local_token;
			crrnt_arrlst->token_counter++;
		}
		else{
			/*case the current array of tokens is full*/
			ArrayList* temp = crrnt_arrlst;
			ArrayList* nextArrlst = getArrayList(local_token);
			temp->next = nextArrlst;
			crrnt_arrlst = nextArrlst;
			crrnt_arrlst->prev = temp;
			crrnt_arrlst->token_counter = 1;
		}
	}
	printf("\nDATABASE MODEL: Token Stored [lexeme: '%s', type: %d, line #%d]\n",local_token.lexeme,local_token.type,local_token.num_of_line);
}

/*
	next_token() function is designed trigger the yylex() so it will read the next token from the input text
	by using the rules in the 'lex.yy.c', than it retrives a Token object and return it
	in case a back_token() function was called right before the next_token(), the function will NOT call for yylex().
	instead it will pull the Token object from the data store
*/
Token next_token(){
	Token token;
	if(back_token_flag){
		back_token_flag = 0;
		token = crrnt_arrlst->tokens_arr[crrnt_arrlst->token_counter-1];
	}
	else{
		if(yylex()){
			//case yylex() returned 1
			token = crrnt_arrlst->tokens_arr[crrnt_arrlst->token_counter-1];
		}
		else{
			//case yylex() returned 0, meaning its either the EOF, or unrecognized token
			token.type = -1;
			token.lexeme = NULL;
			token.num_of_line = -1;
		}
	}
	return token;
}

/*
	the back_token function is designed to pull the Token object that was stored erlier (right before the last token was stored)
	it will also mark the back_token_flag in order to alert the next_token function that a call for yylex() is not needed in the next step
*/
Token back_token(){
	Token token;
	back_token_flag = 1;
	token = crrnt_arrlst->tokens_arr[crrnt_arrlst->token_counter-2];
	return token;
}

/* will return a pointer to new arrary list of tokens*/
ArrayList* getArrayList(Token tkn){
	
	ArrayList *newArrayList=(ArrayList*)malloc(sizeof(ArrayList));
	newArrayList->tokens_arr[0]=tkn;
	newArrayList->token_counter=1;
	newArrayList->next=NULL;
	newArrayList->prev=NULL;
	printf("\nDATABASE MODEL: new database node of 100 tokens was created\n");
	return newArrayList;
}

/* delete  */
void deleteArrayList(){
	int i;
	ArrayList* tmp_arrlis;
	if(crrnt_arrlst!=NULL)
	{
		while(crrnt_arrlst->prev)
		{
			//get to the first node
			crrnt_arrlst = crrnt_arrlst->prev;
		}
		//cycle through the list of nodes until the last
		while(crrnt_arrlst != NULL)
		{
			while(i < crrnt_arrlst->token_counter)
			{
				//deleteing token
				free(crrnt_arrlst->tokens_arr[i].lexeme);
				i++;
			}
			tmp_arrlis = crrnt_arrlst;
			crrnt_arrlst = crrnt_arrlst->next;
			//deleting node
			free(tmp_arrlis);
		}
	}
	printf("\nDATABASE MODEL: database of tokens was deleted\n");
}


/* this function is for sake of debuging */
void printArrayList(){
	int i;
	if(crrnt_arrlst==NULL)
	{
		printf("no token arrays\n");
	}
	while(crrnt_arrlst->prev)
	{
		crrnt_arrlst=crrnt_arrlst->prev;
	}
	while(crrnt_arrlst != NULL)
	{
		printf("\narray token counter = %d\n", crrnt_arrlst->token_counter);
		i=0;
		while(i < crrnt_arrlst->token_counter)
		{
			printf("type: %d,  lexeme: %s, line: %d\n",crrnt_arrlst->tokens_arr[i].type, crrnt_arrlst->tokens_arr[i].lexeme, crrnt_arrlst->tokens_arr[i].num_of_line);
			i++;
		}
		crrnt_arrlst = crrnt_arrlst->next;
	}
}

/* this function is for sake of debuging */
void print_token(Token token){
	printf("type: %d,  lexeme: %s, line: %d\n",token.type, token.lexeme, token.num_of_line);
}