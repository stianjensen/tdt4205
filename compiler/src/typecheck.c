#include "typecheck.h"
extern int outputStage;

data_type_t wrap_base_type(base_data_type_t);
void typecheck_children(node_t *root);

void type_error(node_t* root){
	fprintf(stderr, "Type error at:\n");
	if(root != NULL){
		fprintf(stderr,"%s", root->nodetype.text);
		if(root->nodetype.index == EXPRESSION){
			fprintf(stderr," (%s)", root->expression_type.text);
		}
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"Exiting\n");
	exit(-1);
}

int equal_types(data_type_t a, data_type_t b)
{
    return (a.base_type == b.base_type);
}

data_type_t typecheck_default(node_t* root)
{
    typecheck_children(root);
    return root->data_type;
}

data_type_t typecheck_expression(node_t* root)
{
	if(outputStage == 10)
		fprintf( stderr, "Type checking expression %s\n", root->expression_type.text);

    switch (root->expression_type.index) {
        case ADD_E:
        case SUB_E:
        case MUL_E:
        case DIV_E:
            {
                data_type_t type1 = root->children[0]->typecheck(root->children[0]);
                data_type_t type2 = root->children[1]->typecheck(root->children[1]);
                if (type1.base_type == INT_TYPE && type2.base_type == INT_TYPE) {
                    root->data_type = type1;
                    return wrap_base_type(INT_TYPE);
                }
                if (type1.base_type == FLOAT_TYPE && type2.base_type == FLOAT_TYPE) {
                    root->data_type = type1;
                    return wrap_base_type(FLOAT_TYPE);
                }
                type_error(root);
            }
            break;
        case LESS_E:
        case GREATER_E:
        case GEQUAL_E:
        case LEQUAL_E:
            {
                data_type_t type1 = root->children[0]->typecheck(root->children[0]);
                data_type_t type2 = root->children[1]->typecheck(root->children[1]);
                if (type1.base_type == INT_TYPE && type2.base_type == INT_TYPE) {
                    root->data_type = wrap_base_type(BOOL_TYPE);
                    return wrap_base_type(BOOL_TYPE);
                }
                if (type1.base_type == FLOAT_TYPE && type2.base_type == FLOAT_TYPE) {
                    root->data_type = wrap_base_type(BOOL_TYPE);
                    return wrap_base_type(BOOL_TYPE);
                }
                type_error(root);
            }
            break;
        case EQUAL_E:
        case NEQUAL_E:
            {
                data_type_t type1 = root->children[0]->typecheck(root->children[0]);
                data_type_t type2 = root->children[1]->typecheck(root->children[1]);
                if (type1.base_type == INT_TYPE && type2.base_type == INT_TYPE) {
                    root->data_type = wrap_base_type(BOOL_TYPE);
                    return wrap_base_type(BOOL_TYPE);
                }
                if (type1.base_type == FLOAT_TYPE && type2.base_type == FLOAT_TYPE) {
                    root->data_type = wrap_base_type(BOOL_TYPE);
                    return wrap_base_type(BOOL_TYPE);
                }
                if (type1.base_type == BOOL_TYPE && type2.base_type == BOOL_TYPE) {
                    root->data_type = wrap_base_type(BOOL_TYPE);
                    return wrap_base_type(BOOL_TYPE);
                }
                type_error(root);
            }
            break;
        case UMINUS_E:
            {
                data_type_t type = root->children[0]->typecheck(root->children[0]);

                if (type.base_type == INT_TYPE || type.base_type == FLOAT_TYPE) {
                    root->data_type = type;
                    return type;
                }
                type_error(root);
            }
            break;
        case NOT_E:
            {
                data_type_t type = root->children[0]->typecheck(root->children[0]);
                if (type.base_type == BOOL_TYPE) {
                    root->data_type = type;
                    return type;
                }
                type_error(root);
            }
            break;
        case AND_E:
        case OR_E:
            {
                data_type_t type1 = root->children[0]->typecheck(root->children[0]);
                data_type_t type2 = root->children[1]->typecheck(root->children[1]);
                if (type1.base_type == BOOL_TYPE && type2.base_type == BOOL_TYPE) {
                    root->data_type = type1;
                    return wrap_base_type(BOOL_TYPE);
                }
                type_error(root);
            }
            break;
        case FUNC_CALL_E:
        case METH_CALL_E:
            {
                function_symbol_t *function_symbol = root->function_entry;
                node_t *argument_list = root->children[root->n_children-1];
                if (argument_list != NULL) {
                    if (function_symbol->nArguments != argument_list->n_children) {
                        type_error(root);
                    }
                    for (int i=0; i < argument_list->n_children; i++) {
                        data_type_t param_type = argument_list->children[i]->data_type;
                        if (param_type.base_type == NO_TYPE) {
                            param_type = argument_list->children[i]->typecheck(argument_list->children[i]);
                        }
                        if (!equal_types(
                                    function_symbol->argument_types[i],
                                    param_type
                                    )) {
                            type_error(root);
                        }
                    }
                } else if (function_symbol->nArguments != 0) {
                    type_error(root);
                }

                // Set return type on root node
                // so it can be checked easily from print statements
                root->data_type = root->function_entry->return_type;

                return function_symbol->return_type;
            }
            break;
        case CLASS_FIELD_E:
            {
                data_type_t right = root->children[1]->typecheck(root->children[1]);
                root->children[1]->data_type = right;
                root->data_type = right;
                return right;
            }
            break;
        case NEW_E:
            {
                return wrap_base_type(CLASS_TYPE);
            }
            break;
        default:
            typecheck_children(root);
            return root->data_type;
    }
}

data_type_t typecheck_variable(node_t* root){
    if (root->entry != NULL) {
        root->data_type = root->entry->type;
        return root->entry->type;
    }
    return wrap_base_type(NO_TYPE);
}

data_type_t typecheck_assignment(node_t* root)
{
    data_type_t left  = root->children[0]->typecheck(root->children[0]);
    data_type_t right = root->children[1]->typecheck(root->children[1]);

    if (equal_types(left, right)) {
        root->data_type = left;
        return left;
    }
    type_error(root);
}

data_type_t wrap_base_type(base_data_type_t base_type) {
    return (data_type_t){.base_type = base_type};
}

void typecheck_children(node_t *root) {
    for (int i=0; i < root->n_children; i++) {
        node_t *node = root->children[i];
        if (node != NULL) {
            node->typecheck(node);
        }
    }
}
