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
    
    for (int i=0; i < root->n_children; i++) {
        node_t *node = root->children[i];
        if (node != NULL) {
            if (node->nodetype.index == VARIABLE_LIST) {
                bind_children(node, stackOffset + 8 + node->n_children * 4);
            } else {
                bind_children(node, 0);
            }
        }
    }

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

    // Allocate memory for the entry
    class_symbol_t* class_symbol = malloc(sizeof(class_symbol_t));
    // Initialize the private tables
    class_symbol->symbols = ght_create(8);
    class_symbol->functions = ght_create(8);
    class_symbol->size = 0;
    class_add(root->label, class_symbol);

    thisClass = root->label;
	
    node_t *declaration_list = root->children[0];
    if (declaration_list != NULL) {
        class_symbol->size = declaration_list->n_children;

        for (int i=0; i < declaration_list->n_children; i++) {
            symbol_t *declaration_symbol = create_symbol(declaration_list->children[i], i*4);
            class_insert_field(root->label, declaration_symbol->label, declaration_symbol);
        }
    }

    node_t *function_list = root->children[1];
    if (function_list != NULL) {
        for (int i=0; i < function_list->n_children; i++) {
            function_symbol_t *function_symbol = create_function_symbol(
                function_list->children[i]
            );
            class_insert_method(root->label, function_symbol->label, function_symbol);
        }

        for (int i=0; i < function_list->n_children; i++) {
            node_t *function_node = function_list->children[i];
            function_node->bind_names(function_node, 4);
        }
    }
    
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

    switch (root->expression_type.index) {
        case FUNC_CALL_E:
            {
                node_t *func_name_node = root->children[0];
                node_t *argument_list = root->children[1];
                if (argument_list != NULL) {
                    bind_children(argument_list, stackOffset);
                }

                function_symbol_t *function_symbol = function_get(func_name_node->label);
                root->function_entry = function_symbol;
            }
            break;
        case CLASS_FIELD_E:
            {
                node_t *class_name_node = root->children[0];
                class_name_node->bind_names(class_name_node, stackOffset);

                node_t *field_name_node = root->children[1];

                symbol_t *symbol = class_get_symbol(
                        class_name_node->entry->type.class_name,
                        field_name_node->label
                        );

                field_name_node->entry = symbol;
                root->entry = symbol;
            }
            break;
        case METH_CALL_E:
            {
                node_t *class_name_node = root->children[0];
                class_name_node->bind_names(class_name_node, stackOffset);

                node_t *meth_name_node = root->children[1];

                node_t *argument_list = root->children[2];
                if (argument_list != NULL) {
                    bind_children(argument_list, stackOffset);
                }

                function_symbol_t *function_symbol = class_get_method(
                        class_name_node->entry->type.class_name,
                        meth_name_node->label
                        );

                root->function_entry = function_symbol;
            }
            break;
        case NEW_E:
            {
                node_t *class_name_node = root->children[0];
                class_symbol_t *class_symbol = class_get(class_name_node->data_type.class_name);
                root->class_entry = class_symbol;
            }
            break;
        case THIS_E:
            {
                symbol_t *symbol = malloc(sizeof(symbol_t));
                symbol->type = (data_type_t) {.base_type = CLASS_TYPE, .class_name = thisClass};
                symbol->stack_offset = 8;

                root->entry = symbol;
            }
            break;
        default:
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
            if (node->nodetype.index == DECLARATION_STATEMENT) {
                stackOffset -= 4;
            }
            node->bind_names(node, stackOffset);
        }
    }
}
