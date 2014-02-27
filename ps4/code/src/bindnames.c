#include "bindnames.h"
extern int outputStage; // This variable is located in vslc.c
char* thisClass;

int b_d(node_t* root, int stack_offset);
int b_c(node_t* root, int stack_offset);
void bind_children(node_t* root, int stackOffset);

int bind_default ( node_t *root, int stackOffset)
{
	return b_d(root,stackOffset);
}


int bind_function ( node_t *root, int stackOffset)
{

	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION: Start: %s\n", root->label);

    scope_add();
    
    bind_children(root, stackOffset);

    scope_remove();

	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION: End\n");

    return 0;
}

int bind_declaration_list ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf ( stderr, "DECLARATION_LIST: Start\n");

    bind_children(root, stackOffset);

	if(outputStage == 6)
		fprintf ( stderr, "DECLARATION_LIST: End\n");

    return 0;
}

int bind_class ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf(stderr, "CLASS: Start: %s\n", root->children[0]->label);

	

	if(outputStage == 6)
			fprintf(stderr, "CLASS: End\n");

    return 0;
}

function_symbol_t* create_function_symbol(node_t* function_node)
{
    function_symbol_t *symbol = malloc(sizeof(function_symbol_t));
    symbol->label = function_node->label;
    symbol->return_type = function_node->data_type;
    if (function_node->children[0] == NULL) {
        symbol->nArguments = 0;
    } else {
        node_t *function_arguments = function_node->children[0];
        symbol->nArguments = function_arguments->n_children;
        symbol->argument_types = malloc(sizeof(data_type_t) * symbol->nArguments);
        for (int i=0; i < function_arguments->n_children; i++) {
            symbol->argument_types[i] = function_arguments->children[i]->data_type;
        }
    }
    return symbol;
}

int bind_function_list ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION_LIST: Start\n");

    scope_add();
    for (int i=0; i < root->n_children; i++) {
        // add function to func symbol table
        node_t *function_node = root->children[i];
        if (function_node != NULL) {
            function_symbol_t *function_symbol = create_function_symbol(function_node);
            function_add(function_node->label, function_symbol);
        }
    }

    bind_children(root, stackOffset);
	
    scope_remove();

	if(outputStage == 6)
		fprintf ( stderr, "FUNCTION_LIST: End\n");

    return 0;
}

int bind_constant ( node_t *root, int stackOffset)
{
	return b_c(root, stackOffset);
}


symbol_t* create_symbol(node_t* declaration_node, int stackOffset)
{
    symbol_t *symbol = malloc(sizeof(symbol_t));
    symbol->stack_offset = stackOffset;
    symbol->label = declaration_node->label;
    symbol->type = declaration_node->data_type;

    return symbol;
}

int bind_declaration ( node_t *root, int stackOffset)
{

	if(outputStage == 6)
		fprintf(stderr, "DECLARATION: parameter/variable : '%s', offset: %d\n", root->label, stackOffset);

    symbol_t* symbol = create_symbol(root, stackOffset);
    symbol_insert(symbol->label, symbol);

    return 0;
}

int bind_variable ( node_t *root, int stackOffset)
{
	if(outputStage == 6)
		fprintf ( stderr, "VARIABLE: access: %s\n", root->label);

    symbol_t *symbol = symbol_get(root->label);
    root->entry = symbol;

    return 0;
}

int bind_expression( node_t* root, int stackOffset)
{
	if(outputStage == 6)
		fprintf( stderr, "EXPRESSION: Start: %s\n", root->expression_type.text);

    if (root->expression_type.index == FUNC_CALL_E) {
        node_t *func_name_node = root->children[0];
        function_symbol_t *function_symbol = function_get(func_name_node->label);
        root->function_entry = function_symbol;
    } else {
        bind_children(root, stackOffset);
    }

	if(outputStage == 6)
		fprintf( stderr, "EXPRESSION: End\n");

    return 0;
}

void bind_children( node_t* root, int stackOffset)
{
    for (int i=0; i < root->n_children; i++) {
        node_t *node = root->children[i];
        if (node != NULL) {
            node->bind_names(node, stackOffset);
        }
    }
}
