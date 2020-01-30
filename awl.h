#include <stdio.h> 
#include <stdlib.h>
#include <string.h>  
// An AVL tree node 
struct Node 
{ 
	char *data; 
	struct Node *left; 
	struct Node *right; 
	int height; 
};

struct Node* deleteNode(struct Node* root, char* key) ;
int iterativeSearch(struct Node* root, char *data);
struct Node* insert(struct Node* node, char *data,int len) ;
// A utility function to get maximum of two integers 
int max(int a, int b); 
// A utility function to get the height of the tree 
int height(struct Node *N) ;
void inorder(struct Node *root);
void deleteTree(struct Node* node) ;