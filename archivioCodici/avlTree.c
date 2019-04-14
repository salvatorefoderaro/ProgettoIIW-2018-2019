#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedListImage.c"
 
typedef struct node {
	char data[1024];
	struct node *left,*right;
	int ht;
	treeNode *imageList;
} node;
 
node *insert(node *, char*,  treeNode *imageList);
node *Delete(node *,char*);
void preorder(node *);
void inorder(node *);
int height( node *);
node *rotateright(node *);
node *rotateleft(node *);
node *RR(node *);
node *LL(node *);
node *LR(node *);
node *RL(node *);
int BF(node *);
node *searchNode(node *T, char* find, int w, int h, int quality, int colorSpace, char* address, int *size);

node *root;

/* int main(void)
{
	root=NULL;
	int x,n,i,op;
    char string[1024];
	
	do{
		printf("\n2)Insert:");
		printf("\n3)Cerca:");
		printf("\n4)Print:");
		printf("\n\nEnter Your Choice:");
		scanf("%d",&op);
		
		switch(op){	
		case 2: printf("\nEnter a data:");
				scanf("%s",string);
				treeNode *test = malloc(sizeof(treeNode));
				insert(root, string, test);
				break;
				
		case 3: printf("\nEnter a data:");
				
				scanf("%s", string);
				char *address;
				int *size;
				if (searchNode(root, string, 0, 0, 0, 0, address, size) == NULL){
					treeNode *imageListNode = createNode(string, 1024, 1024, 0, 0, size);
					root = insert(root, string, imageListNode);
					address = imageListNode->imageBuffer;
				}
				printf("\nIndirizzo è: %x\n", address);
				    // Scrittura su file
				FILE *write_ptr;
				write_ptr = fopen("test124.jpg","wb");
				fwrite(address,12576,1,write_ptr);
				break;
		
		case 4: printf("\n\nInorder sequence:\n");
				preorder(root);
				break;			
		}
	} while(op != 5);
	
	return 0;
} */

node *searchNode(node *T, char* find, int w, int h, int quality, int colorSpace, char* address, int *size){
	
	node *returnNode;
    if (T == NULL){
        return NULL;
    }
	if (strcmp(find, T->data) == 0){
		returnNode = T;
	} else if(find > T->data){
		returnNode = searchNode(T->right, find, w, h, quality, colorSpace, address, size);
	} else if (find < T->data) {
		returnNode = searchNode(T->left, find, w, h, quality, colorSpace, address, size);
	}

	if (returnNode != NULL){
		searchNodeInList(returnNode->imageList, find, w, h, quality, colorSpace, address, size); 
	} else {
		return NULL;
	}
}
 
node *insert(node *T, char* x, treeNode *imageList)
{
	if(T==NULL)
	{
		T=(node*)malloc(sizeof(node));
        strcpy(T->data, x);
		T->left=NULL;
		T->right=NULL;
		T->imageList=imageList;
	}
	else if(x > T->data){
		T->right=insert(T->right, x, imageList);
		if(BF(T)==-2)
			if(x>T->right->data)
				T=RR(T);
			else
				T=RL(T);
	} else if(x < T->data){
		T->left=insert(T->left,x, imageList);
		if(BF(T)==2)
			if(x < T->left->data)
				T=LL(T);
			else
				T=LR(T);
	}
	
	T->ht=height(T);
	return T;
}
 
node *Delete(node *T,char * x)
{
	node *p;
	
	if(T==NULL){
		return NULL;
	}
	else if(x > T->data){
			T->right=Delete(T->right,x);
			if(BF(T)==2)
				if(BF(T->left)>=0)
					T=LL(T);
				else
					T=LR(T);
		} else if(x<T->data){
				T->left=Delete(T->left,x);
				if(BF(T)==-2)	//Rebalance during windup
					if(BF(T->right)<=0)
						T=RR(T);
					else
						T=RL(T);
			}
			else {
				//data to be deleted is found
				if(T->right!=NULL){	//delete its inorder succesor
					p=T->right;
					
					while(p->left!= NULL)
						p=p->left;
					
                    strcpy(T->data, p->data);
					T->right=Delete(T->right,p->data);
					
					if(BF(T)==2)//Rebalance during windup
						if(BF(T->left)>=0)
							T=LL(T);
						else
							T=LR(T);\
				}
				else
					return(T->left);
			}
	T->ht=height(T);
	return(T);
}
 
int height(node *T)
{
	int lh,rh;
	if(T==NULL)
		return(0);
	
	if(T->left==NULL)
		lh=0;
	else
		lh=1+T->left->ht;
		
	if(T->right==NULL)
		rh=0;
	else
		rh=1+T->right->ht;
	
	if(lh>rh)
		return(lh);
	
	return(rh);
}
 
node * rotateright(node *x)
{
	node *y;
	y=x->left;
	x->left=y->right;
	y->right=x;
	x->ht=height(x);
	y->ht=height(y);
	return(y);
}
 
node * rotateleft(node *x)
{
	node *y;
	y=x->right;
	x->right=y->left;
	y->left=x;
	x->ht=height(x);
	y->ht=height(y);
	
	return(y);
}
 
node * RR(node *T)
{
	T=rotateleft(T);
	return(T);
}
 
node * LL(node *T)
{
	T=rotateright(T);
	return(T);
}
 
node * LR(node *T)
{
	T->left=rotateleft(T->left);
	T=rotateright(T);
	
	return(T);
}
 
node * RL(node *T)
{
	T->right=rotateright(T->right);
	T=rotateleft(T);
	return(T);
}
 
int BF(node *T)
{
	int lh,rh;
	if(T==NULL)
		return(0);
 
	if(T->left==NULL)
		lh=0;
	else
		lh=1+T->left->ht;
 
	if(T->right==NULL)
		rh=0;
	else
		rh=1+T->right->ht;
 
	return(lh-rh);
}
 
void preorder(node *T)
{
	if(T!=NULL)
	{
		printf("%s(Bf=%d)",T->data,BF(T));
		preorder(T->left);
		preorder(T->right);
	}
}
 
void inorder(node *T)
{
	if(T!=NULL)
	{
		inorder(T->left);
		printf("%s(Bf=%d)",T->data,BF(T));
		inorder(T->right);
	}
}