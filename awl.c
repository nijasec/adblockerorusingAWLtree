#include "awl.h"

//Delete Tree
void deleteTree(struct Node* node)  
{  
    if (node == NULL) return;  
  
    /* first delete both subtrees */
    deleteTree(node->left);  
    deleteTree(node->right);  
      
    /* then delete the node */
    printf("\n Deleting node: %s",node->data);  
	free(node->data);
    free(node);  
} 
//searching for data
int iterativeSearch(struct Node* root, char *data) 
{ 
    // Traverse untill root reaches to dead end 
    while (root != NULL) { 
        // pass right subtree as new tree 
        if (strcmp(data,root->data)>0) 
            root = root->right; 
  
        // pass left subtree as new tree 
        else if (strcmp(data ,root->data)<0) 
            root = root->left; 
        else
            return 1; // if the key is found return 1 
    } 
    return 0; 
}

// A utility function to get maximum of two integers 
int max(int a, int b); 

// A utility function to get the height of the tree 
int height(struct Node *N) 
{ 
	if (N == NULL) 
		return 0; 
	return N->height; 
} 

// A utility function to get maximum of two integers 
int max(int a, int b) 
{ 
	return (a > b)? a : b; 
} 

 
/* Helper function that allocates a new node with the given key and 
	NULL left and right pointers. */
struct Node* newNode(char *data,int len) 
{ 
	struct Node* node = (struct Node*) 
						malloc(sizeof(struct Node)); 
						//printf("allocated for node\n");
						//printf("\tdata:%d--%d\n",len,strlen(data));
						//printf("\n\n");
				//	if(len>0)
					//{
						node->data=(char*)malloc(len);
					//}
			//	else{
				//	node->data=(char*)malloc(255);
					
				//}
			//	printf("done\n\n");
			//	int l=0;
		//	if(strlen(node->data)>strlen(data))
			//l=strlen(node->data);
			//else
				//l=strlen(data);
			//memset(node->data,0,l);
			memcpy(node->data,data,len);
	
	node->left = NULL; 
	node->right = NULL; 
	node->height = 1; // new node is initially added at leaf 
	return(node); 
} 

// A utility function to right rotate subtree rooted with y 
// See the diagram given above. 
struct Node *rightRotate(struct Node *y) 
{ 
	struct Node *x = y->left; 
	struct Node *T2 = x->right; 

	// Perform rotation 
	x->right = y; 
	y->left = T2; 

	// Update heights 
	y->height = max(height(y->left), height(y->right))+1; 
	x->height = max(height(x->left), height(x->right))+1; 

	// Return new root 
	return x; 
} 

// A utility function to left rotate subtree rooted with x 
// See the diagram given above. 
struct Node *leftRotate(struct Node *x) 
{ 
	struct Node *y = x->right; 
	struct Node *T2 = y->left; 

	// Perform rotation 
	y->left = x; 
	x->right = T2; 

	// Update heights 
	x->height = max(height(x->left), height(x->right))+1; 
	y->height = max(height(y->left), height(y->right))+1; 

	// Return new root 
	return y; 
} 

// Get Balance factor of node N 
int getBalance(struct Node *N) 
{ 
	if (N == NULL) 
		return 0; 
	return height(N->left) - height(N->right); 
} 

// Recursive function to insert a key in the subtree rooted 
// with node and returns the new root of the subtree. 
struct Node* insert(struct Node* node, char *data,int len) 
{ 
	struct Node *temp=NULL;
	/* 1. Perform the normal BST insertion */
	if (node == NULL) {
		temp=newNode(data,len);
		return temp;
	}
		 

	if (strcmp(data,node->data)<0) 
		node->left = insert(node->left, data,len); 
	else if (strcmp(data,node->data)>0) 
		node->right = insert(node->right, data,len); 
	else // Equal keys are not allowed in BST 
	{
		//printf("**************\n");
		if(temp!=NULL){
			free(temp->data);
			free(temp);
			
		}
		return node; 
	}
	/* 2. Update height of this ancestor node */
	node->height = 1 + max(height(node->left), 
						height(node->right)); 

	/* 3. Get the balance factor of this ancestor 
		node to check whether this node became 
		unbalanced */
	int balance = getBalance(node); 

	// If this node becomes unbalanced, then 
	// there are 4 cases 

	// Left Left Case 
	if (balance > 1 && strcmp(data,node->left->data)<0) 
		return rightRotate(node); 

	// Right Right Case 
	if (balance < -1 && strcmp(data,node->right->data)>0) 
		return leftRotate(node); 

	// Left Right Case 
	if (balance > 1 && strcmp(data,node->left->data)>0) 
	{ 
		node->left = leftRotate(node->left); 
		return rightRotate(node); 
	} 

	// Right Left Case 
	if (balance < -1 && strcmp(data,node->right->data)<0) 
	{ 
		node->right = rightRotate(node->right); 
		return leftRotate(node); 
	} 

//printf("node-%s-\n",node->data);
	/* return the (unchanged) node pointer */
	return node; 
} 
// A utility function to do inorder traversal of BST 
void inorder(struct Node *root) 
{ 
	if (root != NULL) 
	{ 
		inorder(root->left); 
		fprintf(stdout,"* %s *",root->data);
		//fflush(stdout);//
		fprintf(stdout," ");
		//printf("%s ", root->data); 
		inorder(root->right); 
		
	} 
	//fprintf(stdout,"\n");
} 
// A utility function to print preorder traversal 
// of the tree. 
// The function also prints height of every node 
void preOrder(struct Node *root) 
{ 
	if(root != NULL) 
	{ 
		
		printf("%s\n", root->data); 
		 preOrder(root->left);
		preOrder(root->right); 
	} 
} 

