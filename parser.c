#include "parser.h"

typedef struct Var {
	int datatype;
	char* lexeme;
}Var;

/* declare new variable for storing the type */
Var TYPE;
Var DEFINITION;
Var DEFINITION_B;
Var EXPRESSION;
Var EXPRESSION_B;
Var ID_LIST;
FILE* report;
int miss_match = 0;
int semantic_errors = 0;
TableEntry* id_entry;
/*
parse() function is starting the parsing process
it's opening a report file which will be used to write the different rules
that's been used in the way.
initaly it calls the program function that is the first rule
*/
void parse(){
	report = fopen("parser_report.txt","w");
	if(report == NULL){
		printf("\nCould not locate 'parser_report.txt'.\n");
	}
	program();
	if(miss_match){
		printf("\nParser errors occurrd: %d\n", miss_match);
	}
	if(semantic_errors){
		printf("\nTotal of semantic errors: %d\n", semantic_errors);
	}
	if(report){
		fclose(report);
	} 
}

/*
	match function is designed to check whether the recieved token type is
	the same as next token type that read from the input file
	in case it deasn't, it will mark with miss_match flag and also print an appropiate message
*/
Token match(int expected_token_type){
	fprintf(report, "\nExpected token type: %d, ", expected_token_type);
	Token t = next_token();
	if(expected_token_type != t.type){
		fprintf(report, "Next token type = %d, miss match in line %d, lexeme: '%s'", t.type, t.num_of_line, t.lexeme);
		miss_match++;
	}
	else{
		fprintf(report, "Next token type = %d (lexeme: '%s') matched!", t.type, t.lexeme);
	}
	return t;
}

/* 
	The following function are the parser rules.
	they were designed to report each rule in the way, and also report about errors
*/

/* rule 1:	PROGRAM -> start DEFINITIONS { COMMANDS } finish */
void program(){
	fprintf(report, "\nProcessing rule 1");
	match(KEYWORD_START);
	make_table();			//creates the very first symbol table
	definitions();
	match(OPEN_CURLY_BRACKET);
	commands();
	match(CLOSE_CURLY_BRACKET);
	match(KEYWORD_FINISH);
	pop_table();			//deletes the initial symbol table
}

/* rule 2: DEFINITIONS -> DEFINITION  DEFINITIONS_B */
void definitions(){
	fprintf(report, "\nProcessing rule 2");
	definition();
	definitions_b();
}

/*
	rule 3: DEFINITIONS_B -> ; DEFINITIONS
	rule 4: DEFINITIONS_B -> epsilon
*/
void definitions_b(){
	Token t = next_token();
	switch (t.type)
	{
	case SEMICOLON:
		fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 3", t.type, t.lexeme);
		definitions();
		break;
	default:
		fprintf(report, "\nProcessing rule 4 as default");
		back_token();
	}
}

/*
	rule 5: DEFINITION -> id  :  exception
	rule 6: DEFINITION -> TYPE id DEFINITION_B
*/
void definition(){
	int data_type;
	Token t = next_token();
	switch(t.type)
	{
		case ID:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 5", t.type, t.lexeme);
			//insert the new id name and receive a pointer to its location
			id_entry = insert(t.lexeme);
			//in case the returned pointer is empty because this id was already declared
			if(id_entry == NULL){ error(DUPLICATED_DECLARATION_ERROR, t.num_of_line, t.lexeme); } 
			match(COLON);
			match(KEYWORD_EXCEPTION);
			//set the id entry datatype to exception
			set_id_type (id_entry, KEYWORD_EXCEPTION);
			break;
		default:
			fprintf(report, "\nProcessing rule 6 as default");
			back_token();
			type();
			t = match(ID);
			id_entry = insert(t.lexeme);
			if(id_entry == NULL){ error(DUPLICATED_DECLARATION_ERROR, t.num_of_line, t.lexeme); }
			else{
				set_id_type (id_entry, ID_LIST.datatype);
				DEFINITION_B.lexeme = t.lexeme;
			}
			DEFINITION_B.datatype = TYPE.datatype;
			definition_b();
	}
}

/*
	rule 7: DEFINITION_B -> ID_LIST
	rule 8: DEFINITION_B -> const  =  number
*/
void definition_b(){
	Token t = next_token();
	switch(t.type)
	{
		case KEYWORD_CONST:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 8", t.type, t.lexeme);
			//setting the id that was inherited to be const
			id_entry = lookup(cur_table_node->hash_table,DEFINITION_B.lexeme);
			set_as_const(id_entry);
			match(ASSIGNMENT);
			match(NUMBER);
			break;
		default:
			fprintf(report, "\nProcessing rule 7 as default");
			back_token();
			ID_LIST.datatype = DEFINITION_B.datatype;
			id_list();
	}
}

/*
	rule 9:  TYPE -> integer
	rule 10: TYPE -> real
*/
void type(){
	Token t = next_token();
	switch(t.type)
	{
		case KEYWORD_INTEGER:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 9", t.type, t.lexeme);
			TYPE.datatype = KEYWORD_INTEGER;
			break;
		case KEYWORD_REAL:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 10", t.type, t.lexeme);
			TYPE.datatype = KEYWORD_REAL;
			break;
		default:
			fprintf(report, "\nNext token type = %d, miss match in line %d, lexeme: '%s' (should be Integer type or Real type)", t.type, t.num_of_line, t.lexeme);
			back_token();
			miss_match++;
	}
}


/*
	rule 11: ID_LIST -> epsilon
	rule 12: ID_LIST -> , id ID_LIST
*/
void id_list(){
	Token t = next_token();
	switch(t.type)
	{
		case COMMA:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 12", t.type, t.lexeme);
			t = match(ID);
			id_entry = insert(t.lexeme);
			if(id_entry == NULL){ error(DUPLICATED_DECLARATION_ERROR, t.num_of_line, t.lexeme); }
			else{ set_id_type (id_entry, ID_LIST.datatype); }
			//ID_LIST_R.datatype = ID_LIST_L.datatype 
			id_list();
			break;
		default:
			fprintf(report, "\nProcessing rule 11 as default");
			back_token();
	}
}

/*
	rule 13: COMMANDS -> COMMAND  COMMANDS_B
*/
void commands(){
	fprintf(report, "\nProcessing rule 13");
	command();
	commands_b();
}

/*
	rule 14: COMMANDS_B -> epsilon
	rule 15: COMMANDS_B -> ; COMMANDS
*/
void commands_b(){
	Token t = next_token();
	switch(t.type)
	{
		case SEMICOLON:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 15", t.type, t.lexeme);
			commands();
			break;
		default:
			fprintf(report, "\nProcessing rule 14 as default");
			back_token();
	}
}

/*
	rule 16: COMMAND  -> id = EXPRESSION 
	rule 17: COMMAND  -> while CONDITION do COMMANDS od
	rule 18: COMMAND  -> raise id  
	rule 19: COMMAND  -> block DEFINITIONS { COMMANDS } end_block
*/
void command(){
	Token t = next_token();
	switch(t.type)
	{
		case ID:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 16", t.type, t.lexeme);
			match(ASSIGNMENT);
			id_entry = find(t.lexeme);
			//VARIABLE_NOT_DECLARED_ERROR
			if ( id_entry == NULL) { error(VARIABLE_NOT_DECLARED_ERROR, t.num_of_line, t.lexeme); }
			//ASSIGNMENT_TO_CONSTANT_ERROR
			else if ( is_const(id_entry)) { error(ASSIGNMENT_TO_CONSTANT_ERROR, t.num_of_line, t.lexeme); }
			//EXCEPTION_NAME_REFERENCED_ERROR
			else if ( is_exception(id_entry) ){ error(EXCEPTION_NAME_REFERENCED_ERROR, t.num_of_line, t.lexeme); } 
			expression();
			//TYPE_CONSISTENCY_ERROR - case integer is on the left and real is on the right of the assignment
			if(id_entry){
				if ( is_integer(id_entry) && EXPRESSION.datatype == KEYWORD_REAL ){ error(TYPE_CONSISTENCY_ERROR, t.num_of_line, t.lexeme);}
			}
			break;

		case KEYWORD_WHILE:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 17", t.type, t.lexeme);
			condition();
			match(KEYWORD_DO);
			commands();
			match(KEYWORD_OD);
			break;

		case KEYWORD_RAISE:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 18", t.type, t.lexeme);
			t = match(ID);
			id_entry = find(t.lexeme);
			// in case trying to include non-exception in raise command
			if( id_entry == NULL){error(VARIABLE_NOT_DECLARED_ERROR, t.num_of_line, t.lexeme);}
			else if( !is_exception(id_entry) ){ error(NON_EXCEPTION_IN_RAISE_ERROR, t.num_of_line, t.lexeme);}
			break;

		case KEYWORD_BLOCK:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 19", t.type, t.lexeme);
			//start a new symbol table for this block, which will be the current active table
			make_table();
			definitions();
			match(OPEN_CURLY_BRACKET);
			commands();
			match(CLOSE_CURLY_BRACKET);
			match(KEYWORD_END_BLOCK);
			//pops the newest symbol table, its father will be the active symbol table now 
			pop_table();
			break;

		default:
			fprintf(report, "\nNext token type = %d, miss match in line %d, lexeme: '%s' (should be: ID / while / raise / block)", t.type, t.num_of_line, t.lexeme);
			back_token();
			miss_match++;
	}
}

/*
	rule 20: EXPRESSION -> number
	rule 21: EXPRESSION -> id EXPRESSION_B
*/
void expression(){
	Token t = next_token();
	switch(t.type)
	{
		case NUMBER:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 20", t.type, t.lexeme);
			if ( is_real_number(t.lexeme) ){
				EXPRESSION.datatype = KEYWORD_REAL; }
			else{
				EXPRESSION.datatype = KEYWORD_INTEGER; }
			break;

		case ID:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 21", t.type, t.lexeme);
			id_entry = find(t.lexeme);
			if(id_entry == NULL){
				EXPRESSION.datatype = ERROR_TYPE;
				error(VARIABLE_NOT_DECLARED_ERROR, t.num_of_line, t.lexeme);
			}
			expression_b();
			/*	either left and right sides are both of the same type
				or the left side is real and the right side is integer (the opposite is forbidden)		*/
			if (EXPRESSION_B.datatype == ERROR_TYPE){
				EXPRESSION.datatype = ERROR_TYPE;}
			else if (is_integer(id_entry) && EXPRESSION_B.datatype == KEYWORD_INTEGER){
				EXPRESSION.datatype = KEYWORD_INTEGER; }
			else{
				EXPRESSION.datatype = KEYWORD_REAL;}
			break;

		default:
			fprintf(report, "\nNext token type = %d, miss match in line %d, lexeme: '%s' (should be NUMBER or ID)", t.type, t.num_of_line, t.lexeme);
			back_token();
			miss_match++;
	}
}

/*
	rule 22: EXPRESSION_B -> epsilon
	rule 23: EXPRESSION_B -> binary_ar_op  EXPRESSION
*/
void expression_b(){
	Token t = next_token();
	switch(t.type)
	{
		case BINARY_AR_OP:
			fprintf(report, "\nNext token type: %d (lexeme: '%s'), Processing rule 23", t.type, t.lexeme);
			expression();
			EXPRESSION_B.datatype = EXPRESSION.datatype;
			break;

		default:
			fprintf(report, "\nProcessing rule 22 as default");
			// since this is epsilon situation, integer was picked as the datatype of EXPRESSION_B
			// since if real is in the left it will stay real,
			// and if integer is on the left it will stay integer
			EXPRESSION_B.datatype = KEYWORD_INTEGER;
			back_token();
	}
}

/*
	rule 24: CONDITION -> ( id  rel_op  id ) 
*/
void condition(){
	Token t;
	fprintf(report, "\nProcessing rule 24");
	match(OPEN_BRACKET);
	t = match(ID);
	id_entry = find(t.lexeme);
	//case lexeme wasn't found in any symbol table
	if(id_entry == NULL ){error(VARIABLE_NOT_DECLARED_ERROR, t.num_of_line, t.lexeme);}
	//if id is of the type exeption report an error
	else if(is_exception(id_entry)){ error(EXCEPTION_NAME_REFERENCED_ERROR, t.num_of_line, t.lexeme);}
	match(REL_OP);
	t = match(ID);
	id_entry = find(t.lexeme);
	//case lexeme wasn't found in any symbol table
	if(id_entry == NULL ){error(VARIABLE_NOT_DECLARED_ERROR, t.num_of_line, t.lexeme);}
	//if id is of the type exeption report an error
	else if(is_exception(id_entry)){ error(EXCEPTION_NAME_REFERENCED_ERROR, t.num_of_line, t.lexeme);}
	match(CLOSE_BRACKET);
}

/* 
	Error handling:
	Each time a semantic error is discovered,
	the program sends an appropriate error message
	that clearly explains what is wrong and where the error occurred (line number)

	DUPLICATED_DECLARATION_ERROR - duplicated declaration of the same name within same scope is forbidden
	VARIABLE_NOT_DECLARED_ERROR - variable is used without being declared
	ASSIGNMENT_TO_CONSTANT_ERROR - assignments to constants are forbidden
	EXCEPTION_NAME_REFERENCED_ERROR - exception name can be referenced only in a command raise; all other references to exceptions are illegal
	TYPE_CONSISTENCY_ERROR - left side integer and right side is real is forbidden
	NON_EXCEPTION_IN_RAISE_ERROR - referencing a non-exception object in command raise is forbidden

*/

void error(int errorType, int line, char* lexeme){
	fprintf(report,"\n---Semantic Error: detected in line %d, lexeme '%s'---", line, lexeme);
	semantic_errors++;
	switch(errorType){
	case DUPLICATED_DECLARATION_ERROR:
		fprintf(report,"\n---duplicated declaration of the same name within same scope is forbidden)---");
		break;
	case VARIABLE_NOT_DECLARED_ERROR:
		fprintf(report,"\n---variable is used without being declared---");
		break;
	case ASSIGNMENT_TO_CONSTANT_ERROR:
		fprintf(report,"\n---assignments to constants are forbidden---");
		break;
	case EXCEPTION_NAME_REFERENCED_ERROR:
		fprintf(report,"\n---exception name can be referenced only in a command raise; all other references to exceptions are illegal---");
		break;
	case TYPE_CONSISTENCY_ERROR:
		fprintf(report,"\n---left side integer and right side is real is forbidden---");
		break;
	case NON_EXCEPTION_IN_RAISE_ERROR:
		fprintf(report,"\n---referencing a non-exception object in command raise is forbidden---");
		break;
	default:
		break;
	}
}

int is_real_number(char* number){
	char* c = strchr(number, '.');
	if (c)
		return 1;
	else
		return 0;
}