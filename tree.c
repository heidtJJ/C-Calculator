#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tree.h"

/* Constructor */
tree_t* mktree(int type, tree_t* left, tree_t* right){
    tree_t* root = (tree_t*) malloc(sizeof(tree_t));
    assert(root != NULL);// No memory can be allocated.
    root->type = type;
    root->leftChild = left;
    root->rightChild = right;
    return root;
}

/* Auxiliary */
int tree_eval(tree_t* root){
    assert(root != NULL);
    
    int leftValue;
    int rightValue;
    switch(root->type){
        case '+':
            leftValue = tree_eval(root->leftChild);
            rightValue = tree_eval(root->rightChild);
            return leftValue + rightValue;
        case '-':
            leftValue = tree_eval(root->leftChild);
            if(root->rightChild){
                rightValue = tree_eval(root->rightChild);
                return leftValue - rightValue;
            }
            else 
                return -leftValue;  
        case '*':
            leftValue = tree_eval(root->leftChild);
            rightValue = tree_eval(root->rightChild);
            return leftValue * rightValue;
        case '/':
            leftValue = tree_eval(root->leftChild);
            rightValue = tree_eval(root->rightChild);
            return leftValue / rightValue;
        case NUM:
            return root->attribute;
        default:
            break;
    }

    return -1000000;
}