#include "typecheck.h"
extern int outputStage;

data_type_t wrap_base_type(base_data_type_t);

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
                    return wrap_base_type(INT_TYPE);
                }
                if (type1.base_type == FLOAT_TYPE && type2.base_type == FLOAT_TYPE) {
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
                    return wrap_base_type(BOOL_TYPE);
                }
                if (type1.base_type == FLOAT_TYPE && type2.base_type == FLOAT_TYPE) {
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
                    return wrap_base_type(BOOL_TYPE);
                }
                if (type1.base_type == FLOAT_TYPE && type2.base_type == FLOAT_TYPE) {
                    return wrap_base_type(BOOL_TYPE);
                }
                if (type1.base_type == BOOL_TYPE && type2.base_type == BOOL_TYPE) {
                    return wrap_base_type(BOOL_TYPE);
                }
                type_error(root);
            }
            break;
        case UMINUS_E:
            {
                data_type_t type = root->children[0]->typecheck(root->children[0]);

                if (type.base_type == INT_TYPE || type.base_type == FLOAT_TYPE) {
                    return type;
                }
                type_error(root);
            }
            break;
        case NOT_E:
            {
                data_type_t type = root->children[0]->typecheck(root->children[0]);
                if (type.base_type == BOOL_TYPE) {
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
                    return wrap_base_type(BOOL_TYPE);
                }
                type_error(root);
            }
            break;
        default:
            typecheck_children(root);
            return root->data_type;
    }
}

data_type_t typecheck_variable(node_t* root)
{
    if (root->entry != NULL) {
        return root->entry->type;
    }
    return wrap_base_type(NO_TYPE);
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
