#include "symboltable.h"

/* make_table() Called when entering into a nested block in the program; creates symbol table
for this block, and links it to the table of the current scope */
void make_table(){
	// making new hash table
	HashTable* newHashTable = (HashTable*)malloc(sizeof(HashTable));;
	// making new node to store the hash table
	TableNode* newTableNode = (TableNode*)malloc(sizeof(TableNode));;
	// initializing new hash table
	initHashTable(newHashTable);
	// storing new hash table in the new table node
	newTableNode->hash_table = newHashTable;

	//in case this is not the first scope symbol table
	if(cur_table_node != NULL){
		//link the new node to the previous "active" node
		newTableNode->father = cur_table_node;
	}
	else{
		/* the first node have no "father" */
		newTableNode->father = NULL;
	}
	// assign the new created node to be the current active node (top of the stack)
	cur_table_node = newTableNode;
	printf("\n---- New Symbol table node created ----");
}

/* Initiating NULL pointers to TableEntries in the new hash table */
void initHashTable(HashTable* newHashTable)
{
	int i;
	for(i=0; i<TABLE_SIZE; i++)
		(*newHashTable)[i] = NULL;
}

/* Called when exiting from a nested block; updates the global pointer to the enclosing
block’s table (i.e. table of the exited block&#39;s &quot;father&quot;) */
void pop_table (){
	if(cur_table_node){
		//assign the current table node to a temporary pointer
		TableNode* node_to_delete = cur_table_node;
		//assign the father table node to be the active table node
		cur_table_node = cur_table_node->father;
		//delete the hash table inside the node
		deleteHashTable(node_to_delete->hash_table);
		//delete the node
		free(node_to_delete);
		printf("\n---- Top Symbol table node was droped ----");
	}
}

/* function to delete a hash table (free up space) */
void deleteHashTable(HashTable* hash_table){
	TableEntry* current_entry = NULL;
	TableEntry* entery_for_delete = NULL;
	int i;
	
	for(i=0; i<TABLE_SIZE; i++)
	{
		current_entry = (*hash_table)[i];
		while(current_entry){
			entery_for_delete = current_entry;
			current_entry = current_entry->next;
			if(entery_for_delete->name){
				free(entery_for_delete->name);
			}
			free(entery_for_delete);
		}
	}
	free(hash_table);
}

/* Insert() is called when a variable declaration is processed. Inserts the variable name and
type into symbol table of the current scope, and returns pointer to the new
entry. Before insertion, checks whether the id_name already appears in the
current table (for this, uses function lookup). If yes then returns NULL ; this
indicates to an error (duplicate declaration of the same name) that should be
reported. */
TableEntry* insert (char* id_name){
	printf("\nSYMBOL TABLE: Trying to insert the ID name '%s' to this scope table", id_name);
	//checks whether such id name is already exists
	TableEntry* same_id_name_entry = lookup(cur_table_node->hash_table, id_name);
	if(same_id_name_entry){
		//if such id name already exists return null
		printf("\nSYMBOL TABLE: ID name '%s' already exists in this scope", id_name);
		return NULL;
	}
	else{		//case the id name does not exist
		//generate hash key
		int hashed_key = hash(id_name);
		//find the entry by the hash key inside the symbol table of the current scope node
		HashTable* curr_table = cur_table_node->hash_table;
		TableEntry* hashTableEntry = (*curr_table)[hashed_key];
		//makes room for the new entry
		TableEntry* newEntry = (TableEntry*)malloc(sizeof(TableEntry));
		//make room for the id name char* and copy the recieved id name to the entry
		newEntry->name = (char*)malloc((strlen(id_name) + 1) * sizeof(char));
		strcpy(newEntry->name,id_name);
		//insert default value for the line attr
		newEntry->line = 0;
		newEntry->constant = 0;

		//in case the hash entry is not yet inhabbited
		if (hashTableEntry == NULL){
			newEntry->next = NULL;
			//assign the new entry record as the first in that hash entry
			(*curr_table)[hashed_key] = newEntry;
			printf("\nSYMBOL TABLE: the id name '%s' was inserted to a new place (%d) in the scope table", id_name, hashed_key);
		}
		//in case the entry retrieved is already inhabbited
		else{
			//link new entry to the old entry in the same hash
			newEntry->next = hashTableEntry;
			//and insert to the head of the hash entry
			(*curr_table)[hashed_key] = newEntry;
			printf("\nSYMBOL TABLE: the id name '%s' was inserted to the scope table in the top of existing entry (%d)", id_name, hashed_key);
		}
		return newEntry;
	}
}

/* lookuo() searches for a variable name in symbol table of the current scope (only);
returns pointer to the found entry, or NULL if the name is not found in that table */
TableEntry* lookup (HashTable* curr_table, char* id_name){
	printf("\nSYMBOL TABLE: Trying to find the id_name '%s' in the current scope", id_name);
	int hashed_key = hash(id_name);
	//points to the head of the entry that matched the retrieved hash key
	TableEntry* curr_entry = (*curr_table)[hashed_key];
	//cycle thrugh the linked enteries
	while(curr_entry != NULL){
		//compare id name to the entry's id name
		printf("\nSYMBOL TABLE: looking in hash entry %d: '%s' == '%s' ?", hashed_key, curr_entry->name, id_name);
		if( strcmp(curr_entry->name, id_name) == 0){
			//if found, than return the pointer to that entry
			printf("\nSYMBOL TABLE: the id name '%s' was found in this scope table, in entry [%d]", id_name, hashed_key);
			return curr_entry;
		}
		//cycle through
		curr_entry = curr_entry->next;
	}
	//if none was found return NULL
	printf("\nSYMBOL TABLE: the id name '%s' was not found in this scope table", id_name);
	return NULL;
}

/* find() is called when variable use is found in assignment; allows to check whether the
variable is declared. For this, performs hierarchical search of the variable
name, starting from the symbol table of the current scope; returns the found
entry pointer, or NULL (if the variable is undeclared). */
TableEntry* find (char* id_name){
	printf("\nSYMBOL TABLE: Trying to find '%s' in any of the scopes", id_name);
	//get a pointer to the current node table of this scope
	TableNode* curr_node = cur_table_node;
	HashTable* curr_hash_table;
	TableEntry* entry;
	//cycle through the scopes nodes
	while (curr_node != NULL){
		//take out the hash table
		curr_hash_table = curr_node->hash_table;
		//and looks the id name in there
		entry = lookup(curr_hash_table,id_name);
		if(entry != NULL){
			//case found return the entry
			printf("\nSYMBOL TABLE: ID name '%s' found", id_name);
				return entry;
		}
		//cycle through
		curr_node = curr_node->father;
	}
	//case none was found return NULL
	printf("\nSYMBOL TABLE: ID name '%s' wasn't found in any of the scopes", id_name);
	return NULL;
}

/* set_id_type() is called when id declaration is processed, to store its type in the symbol type. */
void set_id_type (TableEntry* id_entry, int id_type){
	if(id_entry){
		printf("\nSYMBOL TABLE: set the id type to: %d", id_type);
		id_entry->datatype = id_type;
	}
	else{
		printf("\nSYMBOL TABLE: error! entry pointer in NULL (set_id_type)");
	}
}

/* get_id_type() is called when assignment is processed, for variables that appear in its left and
right sides. Returns variable's type (integer or real) */
int get_id_type (TableEntry* id_entry){
	if(id_entry){
		return id_entry->datatype;
	}
	printf("\nSYMBOL TABLE: error! entry pointer in NULL (get_id_type)");
	return -1;
}

void setLine(TableEntry *id_entry, int line)
{
	if(id_entry){
		id_entry->line = line;
	}
	else{
		printf("\nSYMBOL TABLE: error! entry pointer in NULL (setLine)");
	}
}

int getLine(TableEntry *id_entry)
{
	if(id_entry){
		return id_entry->line;
	}
	printf("\nSYMBOL TABLE: error! entry pointer in NULL (getLine)");
	return -1;
}

/*in case declaration is of const type this function will be called*/
void set_as_const(TableEntry* id_entry){
	if(id_entry){
		id_entry->constant = 1;
	}
	else{
		printf("\nSYMBOL TABLE: error! entry pointer in NULL (set_as_const)");
	}
}

/*in case assigment is taking place, this function is called in order to check if it's allowed*/
int is_const(TableEntry* id_entry){
	if(id_entry){
		return id_entry->constant;
	}
	printf("\nSYMBOL TABLE: error! entry pointer in NULL (is_const)");
	return -1;
}
/* 	exception name can be referenced only in a command raise; all other references to exceptions are illegal
	referencing a non-exception object in command raise is forbidden */
int is_exception(TableEntry* id_entry){
	if(id_entry){
		int datatype = get_id_type(id_entry);
		if(datatype == KEYWORD_EXCEPTION){
			return 1;
		}
		else{
			return 0;
		}
	}
	printf("\nSYMBOL TABLE: error! entry pointer in NULL (is_exception)");
	return -1;
}

int is_integer(TableEntry* id_entry){
	if(id_entry){
		int datatype = get_id_type(id_entry);
		if(datatype == KEYWORD_INTEGER){
			return 1;
		}
		else{
			return 0;
		}
	}
	printf("\nSYMBOL TABLE: error! entry pointer in NULL (is_integer)");
	return -1;
}



/* hash function based on the lexeme string */
int hash(char *lexeme) {
	/* hash function (McKenzie, Harris and Bell (1990)) */
	/* h_0 = 0 */
	/* h_i = k * h_i_minus_1 + c_i for 1 =< i <= n */
	/* H = BITS(h_n, 30) MOD N */
	/* for k = 613, N = 1008 */

	const int k = 100;


	int i = 0;
	int n = strlen(lexeme);
	uint64_t h = 0;

	for(; i < n; i++) {
		h = k * h + lexeme[i];
	}

	/* BITS(h_n, 30) function (get 30 rightmost bits of h) */
	h = h & 0x3FFFFFFF;
	
	/* H = BITS(h_n, 30) MOD N */
	return h % TABLE_SIZE;
}