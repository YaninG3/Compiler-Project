#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "tokens.h"
#include "parser.h"

#define TABLE_SIZE 1021


/*an entry to store the variable name, its datatype, and its line of declaration */
typedef struct TableEntry {
	char *name;
	int datatype;
	int line;
	int constant;
	struct TableEntry* next;
}TableEntry;

/* HashTable is a hash table built from pointers to table enteries,
	each table entry is a one way linked list */
typedef TableEntry* HashTable[TABLE_SIZE];

/* a table node represent a single Symbol Table in a stack,
	one single table node is dedicated to a single scope
	each node is linked with its "father" node */
typedef struct TableNode {
	HashTable* hash_table;
	struct TableNode* father;
}TableNode;

TableNode* cur_table_node = NULL;
/* The translation scheme uses a global variable cur_table */

/*The Symbol Table Interface was taken from Lecture 10 - Scope Checking*/


void make_table ();
/* Called when entering into a nested block in the program; creates symbol table
for this block, and links it to the table of the current scope */

void pop_table ();
/* Called when exiting from a nested block; updates the global pointer to the enclosing
block’s table (i.e. table of the exited block&#39;s &quot;father&quot;) */

TableEntry* insert (char* id_name);
/* Called when a variable declaration is processed. Inserts the variable name and
type into symbol table of the current scope, and returns pointer to the new
entry. Before insertion, checks whether the id_name already appears in the
current table (for this, uses function lookup). If yes then returns NULL ; this
indicates to an error (duplicate declaration of the same name) that should be
reported. */

TableEntry* lookup (HashTable* curr_table, char* id_name);
/* Searches for a variable name in symbol table of the current scope (only);
returns pointer to the found entry, or NULL if the name is not found in that table */

TableEntry* find (char* id_name);
/* Called when variable use is found in assignment; allows to check whether the
variable is declared. For this, performs hierarchical search of the variable
name, starting from the symbol table of the current scope; returns the found
entry pointer, or NULL (if the variable is undeclared). */

void set_id_type (TableEntry* id_entry, int datatype);
/* Called when id declaration is processed, to store its type in the symbol type. */

int get_id_type (TableEntry* id_entry);
/* Called when assignment is processed, for variables that appear in its left and
right sides. Returns variable&#39;s type (integer or real) */

void set_as_const(TableEntry* id_entry);
/*in case declaration is of const type this function will be called*/

int is_const(TableEntry* id_entry);
/*in case assigment is taking place, this function is called in order to check if it's allowed*/

int is_exception(TableEntry* id_entry);
/* 	exception name can be referenced only in a command raise; all other references to exceptions are illegal
	referencing a non-exception object in command raise is forbidden */

int is_integer(TableEntry* id_entry);
/* will return if the data type in the entry is integer opr not */

/* misc function */ 
void initHashTable(HashTable* newHashTable);
void deleteHashTable(HashTable* hash_table);
int hash(char *lexeme);

#endif