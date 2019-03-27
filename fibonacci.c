/* C program to implement Fibonacci heap tree */

#include<stdio.h>
#include<math.h>
#include<malloc.h>
#define NIL 0
 int nNodes;



struct fheap_node
{
struct fheap_node *parent;
struct fheap_node *left;
struct fheap_node *right;
struct fheap_node *child;
int degree;
int mark;
int key; };
struct fheap_node *min,*min1;

/* creating fibonacci heap */

void create_fib()
{
min->parent=NIL;
min->key=NIL;
min->left=NIL;
min->right=NIL;
min->child=NIL;
nNodes=0;
}

/* inserting in fibonacci heap */

void Finsert(int val)
{
struct fheap_node *fheap;
fheap=malloc(sizeof(struct fheap_node));
fheap->key=val;
fheap->parent=NIL;
fheap->left=NIL;
fheap->right=NIL;
fheap->child=NIL;
if(min->key!=NIL)
{
fheap->right=min;
fheap->left=min->left;
(min)->left=fheap;
(fheap->left)->right=fheap;
if(val<min->key)
min=fheap; }
else
{
min=fheap;
min->left=min;
min->right=min;
}
nNodes++;
}

/* Displaying Fibonacci heap*/

void display(struct node *min1)
{
struct fheap_node *q,*chil;
if(min==NIL)
{
printf("\n Fibonacci heap is empty");
return;
}
q=min;
printf("\n Fibonacci heap display");
do
{
printf("\t%d ",q->key);
if(q->child!=NIL)
{
display(q->child);
}
q=q->right;
}
while(q!=min);
}

/* finding minimum key in heap */

void findmin()
{
printf("\nminimum is %d: ",min->key);
}
int main ()
{
int option,n,i,m;
clrscr();
min=NIL;
while(1)
{printf("\nmenu\n");
printf("1:create fibonacci heap\n");
printf("2:insert in fibonacci heap\n");
printf("3: find min in fibonacci heap \n");
printf("4:display\n");
printf("5: exit \n");
scanf ("%d",&option);
switch(option)
{
case 1 :create_fib();
break;
case 2: printf("\nenter the element= ");
scanf("%d",&n);
Finsert(n);
break;
case 3: findmin();
break;
case 4: display(min1);
break;
case 5 :exit(1);
default: printf("\nwrong choice... try again \n ");
  }
 }
} 