#include "typecheck.h"
extern int outputStage;

data_type_t td(node_t* root);
data_type_t te(node_t* root);
data_type_t tv(node_t* root);

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
	return td(root);
}

data_type_t typecheck_expression(node_t* root)
{
	data_type_t toReturn;

	if(outputStage == 10)
		fprintf( stderr, "Type checking expression %s\n", root->expression_type.text);

	toReturn = te(root);
	
	//Insert additional checking here

    if (root->expression_type.index == FUNC_CALL_E || root->expression_type.index == METH_CALL_E) {
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
        toReturn = function_symbol->return_type;
    }
    return toReturn;
}

data_type_t typecheck_variable(node_t* root){
	return tv(root);
}

data_type_t typecheck_assignment(node_t* root)
{
    data_type_t left  = root->children[0]->typecheck(root->children[0]);
    data_type_t right = root->children[1]->typecheck(root->children[1]);

    if (equal_types(left, right)) {
        return left;
    }   
    type_error(root);
}
