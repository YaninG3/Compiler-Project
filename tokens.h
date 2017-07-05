#ifndef TOKEN_DEF_H
#define TOKEN_DEF_H

typedef enum eTokenType{
EOF_SYMBOL,
ID,						/* may contain letters (only lower case) and digits, must start with a letter */
NUMBER,
REL_OP,					/* comparison operations ( == , != , > , < , <= , >= ) */
BINARY_AR_OP,			/* binary arithmetic operation (addition, subtraction, multiplication, division)*/
ASSIGNMENT,				/* assignment operation shown as  =  */
SEMICOLON,				/* ; */
COLON,					/* : */
COMMA,					/* , */
OPEN_BRACKET,			/* ( */
CLOSE_BRACKET,			/* ) */
OPEN_CURLY_BRACKET,		/* { */
CLOSE_CURLY_BRACKET,	/* } */
KEYWORD_START,
KEYWORD_FINISH,
KEYWORD_REAL,                           
KEYWORD_INTEGER,
KEYWORD_WHILE,
KEYWORD_DO,
KEYWORD_OD,
KEYWORD_BLOCK,
KEYWORD_RAISE,
KEYWORD_END_BLOCK,
KEYWORD_EXCEPTION,
KEYWORD_CONST,
ERROR_TYPE
}eTokenType;

/*
this structure represent a token object
*/
typedef struct Token{
	eTokenType type;
	char *lexeme;
	int num_of_line;
}Token;

#endif