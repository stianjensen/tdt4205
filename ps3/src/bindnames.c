#include "bindnames.h"
extern int outputStage; // This variable is located in vslc.c
char* thisClass;

int bind_default ( node_t *root, int stackOffset)
{
    for (int i=0; i < root->n_children; i++) {
        node_t *node = root->children[i];
        if (node != NULL) {
            node->bind_names(node, 0);
        }
    }
    return 0;
}

int bind_constant ( node_t *root, int stackOffset)
{
    if (outputStage == 6)
        fprintf( stderr, "%s\n", root->nodetype.text );
    if (root->data_type.base_type == STRING_TYPE) {
        int string_index = strings_add(STRDUP(root->string_const));
        root->string_index = string_index;
    }
    return 0;
}


