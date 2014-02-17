#include "simplifynodes.h"

extern int outputStage; // This variable is located in vslc.c

Node_t* simplify_default ( Node_t *root, int depth )
{
    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            root->children[i] = node->simplify(node, depth+1);
        }
    }
    return root;
}


Node_t *simplify_types ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    if (root->n_children == 1) {
        Node_t *node = root->children[0];
        if (node != NULL) {
            root->data_type.class_name = STRDUP(node->label);
            free(root->children);
            root->n_children = 0;
        }
    }
    return root;
}


Node_t *simplify_function ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    Node_t **children = malloc(sizeof(node_t) * 2);
    int new_i = 0;
    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            node = node->simplify(node, depth+1);
            if (node->nodetype.index == VARIABLE) {
                root->label = STRDUP(node->label);
            } else if (node->nodetype.index == TYPE) {
                root->data_type = node->data_type;
            } else {
                children[new_i++] = node;
            }
        } else {
            children[new_i++] = node;
        }
    }
    free(root->children);
    root->children = children;
    root->n_children = new_i;
    return root;
}


Node_t *simplify_class( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    Node_t **children = malloc(sizeof(node_t*) * root->n_children);
    int new_i = 0;
    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            node = node->simplify(node, depth+1);

            if (node->nodetype.index == VARIABLE) {
                root->label = STRDUP(node->label);
            } else {
                children[new_i++] = node;
            }
        }
    }
    free(root->children);
    root->children = children;
    root->n_children = new_i;
    return root;
}


Node_t *simplify_declaration_statement ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            node = node->simplify(node, depth+1);
            if (node->nodetype.index == TYPE) {
                root->data_type = node->data_type;
                free(node);
            } else if (node->nodetype.index == VARIABLE) {
                root->label = STRDUP(node->label);
                free(node);
            }
        }
    }

    free(root->children);
    root->n_children = 0;
    return root;
}


Node_t *simplify_single_child ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        root->children[i] = (node != NULL) ? node->simplify(node, depth+1) : NULL;
    }
    if (root->n_children == 1) {
        Node_t *yolo = root->children[0];
        return yolo;
    } else {
        return root;
    }
}

Node_t *simplify_list_with_null ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    Node_t **children = malloc(sizeof(node_t*) * 100); // TODO: sophisticatealize
    int new_i = 0;
    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            node = node->simplify(node, depth+1);
            switch (node->nodetype.index) {
                case FUNCTION_LIST:
                case DECLARATION_LIST:
                    for (int j=0; j < node->n_children; j++) {
                        if (node->children[j] != NULL) {
                            children[new_i++] = node->children[j];
                        }
                    }
                    break;
                default:
                    children[new_i++] = node;
            }
        }
    }
    free(root->children);
    root->children = children;
    root->n_children = new_i;
    return root;
}


Node_t *simplify_list ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );
		
    Node_t **children = malloc(sizeof(node_t*) * 100); //TODO: Better number here
    int new_i = 0;
    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            node = node->simplify(node, depth+1);
            switch (node->nodetype.index) {
                case STATEMENT_LIST:
                case EXPRESSION_LIST:
                case VARIABLE_LIST:
                case CLASS_LIST:
                {
                    for (int j=0; j < node->n_children; j++) {
                        if (node->children[j] != NULL) {
                            children[new_i++] = node->children[j];
                        }
                    }
                }
                    break;
                default:
                    children[new_i++] = node;
            }
        }
    }
    free(root->children);
    root->children = children;
    root->n_children = new_i;
    return root;
}


Node_t *simplify_expression ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s (%s) \n", depth, ' ', root->nodetype.text, root->expression_type.text );

    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        if (node != NULL) {
            root->children[i] = node->simplify(node, depth+1);
        }
    }
    if (root->n_children == 1) {
        switch (root->expression_type.index) {
            case NEW_E: case UMINUS_E: case NOT_E:
                return root;
            default:
                {
                Node_t *returnNode = root->children[0];
                return returnNode;
                }
        }
    }
    return root;
}

