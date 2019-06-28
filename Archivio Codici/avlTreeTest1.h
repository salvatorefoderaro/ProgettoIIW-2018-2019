
typedef struct node
{
    char* data;
    struct node*  left;
    struct node*  right;
    int      height;
} node;
 
 
void dispose(node* t);
node* find( int e, node *t );
node* find_min( node *t );
node* find_max( node *t );
node* insert( int data, node *t );
node* delete( int data, node *t );
void display_avl(node* t);
int get( node* n );
