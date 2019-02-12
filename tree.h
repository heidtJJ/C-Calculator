#ifndef TREE_H
#define TREE_H

#define NUM 257

/* Tree type structure. */
typedef struct tree_s{
    int type; // Token type.
    int attribute; // Token attribute.
    struct tree_s* leftChild;
    struct tree_s* rightChild;    
} tree_t;

/* Constructor */
tree_t* mktree(int type, tree_t* left, tree_t* right);

/* Auxiliary */
int tree_eval(tree_t*);

#endif