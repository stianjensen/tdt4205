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
                free(node->label);
                free(node);
            } else if (node->nodetype.index == TYPE) {
                root->data_type = node->data_type;
                free(node);
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
        Node_t *node = root->children[0];
        free(root->children);
        free(root);
        return node;
    } else {
        return root;
    }
}

Node_t *simplify_list_with_null ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    if (root->children[0] == NULL) {
        Node_t **children = malloc(sizeof(node_t*));
        Node_t *node = root->children[1];
        children[0] = node->simplify(node, depth+1);
        free(root->children);
        root->children = children;
        root->n_children = 1;
        return root;
    }

    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        root->children[i] = node->simplify(node, depth+1);
    }

    Node_t *node = root->children[0];
    int size = node->n_children + 1;
    Node_t **children = malloc(sizeof(node_t*) * size);

    for (int i=0; i < node->n_children; i++) {
        children[i] = node->children[i];
    }
    children[size-1] = root->children[1];

    free(root->children);
    root->children = children;
    root->n_children = size;
    return root;
}


Node_t *simplify_list ( Node_t *root, int depth )
{
	if(outputStage == 4)
		fprintf ( stderr, "%*cSimplify %s \n", depth, ' ', root->nodetype.text );

    if (root->n_children == 1) {
        node_t *node = root->children[0];
        root->children[0] = node->simplify(node, depth+1);
        return root;
    }

    for (int i=0; i < root->n_children; i++) {
        Node_t *node = root->children[i];
        root->children[i] = node->simplify(node, depth+1);
    }

    Node_t *node = root->children[0];
    int size = 1 + node->n_children;
    Node_t **children = malloc(sizeof(node_t*) * size);
    for (int i=0; i < node->n_children; i++) {
        children[i] = node->children[i];
    }
    children[size-1] = root->children[1];

    free(root->children);
    root->children = children;
    root->n_children = size;

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

