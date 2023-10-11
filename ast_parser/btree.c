typedef unsigned char bool;

typedef struct tree_node {
	int key_array[(3*2)-1];
	struct tree_node *child_array[(3*2)];
	unsigned int key_index;
	bool leaf;
} node_t;

typedef struct {
	node_t *node_pointer;
	int key;
	bool found;
	unsigned int depth;
} result_t;

typedef struct {
	node_t *root;
	unsigned short order;
	bool lock;
} btree_t;

static int BTreeGetLeftMax(node_t *T);
static int BTreeGetRightMin(node_t *T);

static node_t *create_node()
{
	int i;
	node_t *new_node = (node_t *)malloc(sizeof(node_t));

	if(!new_node){
		printf("Out of memory");
		exit(0);
	}

	
	for(i = 0;i < (3*2)-1; i++){
		new_node->key_array[i] = 0;
	}

	
	for(i = 0;i < (3*2); i++){
		new_node->child_array[i] = NULL;
	}

	new_node->key_index = 0;
	new_node->leaf = 1;

	return new_node;
}

btree_t *create_btree()
{
	btree_t *new_root = (btree_t *)malloc(sizeof(btree_t));

	if(!new_root){
		return NULL;
	}

	node_t *head = create_node();

	if(!head){
		return NULL;
	}

	new_root->order = 3;
	new_root->root = head;
	new_root->lock = 0;

	return new_root;
}

static result_t *get_resultset()
{
	result_t *ret = (result_t *)malloc(sizeof(result_t));

	if(!ret){
		printf("ERROR! Out of memory.");
		exit(0);
	}

	ret->node_pointer = NULL;
	ret->key = 0;
	ret->found = 0;
	ret->depth = 0;

	return ret;
}

void print_node(node_t *n)
{
	int i, q;

	printf(">>-----0x%x-----\n", n);
	printf("  Index: %d\n", n->key_index);
	printf("   Leaf: ");
	if(n->leaf){
		printf("1\n");
	}else{
		printf("0\n");
	}

	printf("  Array:");
	for(i = 0; i < n->key_index; i++){
		printf(" [%d : %d]", i, n->key_array[i]);
	}

	printf("\n  Child:");
	if(n->leaf){
		printf(" NONE");
	}else{
		for(q = 0; q < (3*2); q++){
			printf(" [%d : %x]", q, n->child_array[q]);
		}
	}

	printf("\n<<------------------\n");
}


result_t *search(int key, node_t *node)
{
	print_node(node);
	int i = 0;

	while((i < node->key_index) && (key > node->key_array[i])){
		
		i++;
	}



	
	if(i == 6){
		i--;
	}

	
	if((i <= node->key_index) && (key == node->key_array[i])){
		result_t *result = get_resultset();
		result->node_pointer = node;
		result->key = i;
		result->found = 1;
		return result;
	}

	
	if(node->leaf){
		result_t *result = get_resultset();
		result->node_pointer = node;
		result->found = 0;
		return result;
	}else{
		result_t *result = get_resultset();
		return search(key, node->child_array[i]);
	}
}

static void split_child(node_t *parent_node, int i, node_t *child_array)
{
	int j;

	
	node_t *new_node = create_node();
	new_node->leaf = child_array->leaf;
	new_node->key_index = 3-1;

	
	for(j = 0;j < 3-1;j++){
		new_node->key_array[j] = child_array->key_array[3+j];
	}

	
	
	if(child_array->leaf == 0){
		for(j = 0;j < 3;j++){
			new_node->child_array[j] = child_array->child_array[3+j];
		}
	}
	child_array->key_index = 3-1;

	
	for(j = parent_node->key_index;j>=i;j--){
		parent_node->child_array[j+1] = parent_node->child_array[j];
	}

	
	parent_node->child_array[i] = new_node;

	
	for(j = parent_node->key_index;j>=i;j--){
		parent_node->key_array[j] = parent_node->key_array[j-1];
	}

	
	parent_node->key_array[i-1] = child_array->key_array[3-1];

	
	parent_node->key_index++;
}

static void insert_nonfull(node_t *n, int key){
	int i = n->key_index;
	
	if(n->leaf){
		
		while(i>=1 && key<n->key_array[i-1]){
			n->key_array[i] = n->key_array[i-1];
			i--;
		}
		n->key_array[i] = key;
		n->key_index++;
	}else{
		
		while(i>=1 && key<n->key_array[i-1]){
			i--;
		}
		
		if(n->child_array[i]->key_index == (3*2)-1){
			split_child(n, i+1, n->child_array[i]);
			if(key > n->key_array[i]){
				i++;
			}
		}
		
		insert_nonfull(n->child_array[i], key);
	}
}

node_t *insert(int key, btree_t *b)
{
	if(!b->lock){
		node_t *root = b->root;
		if(root->key_index == (3*2)-1){ 
			node_t *newNode = create_node();
			b->root = newNode; 
			newNode->leaf = 0;
			newNode->key_index = 0;
			newNode->child_array[0] = root;
			split_child(newNode, 1, root);
			insert_nonfull(newNode, key); 
		}else{ 
			insert_nonfull(b->root, key);
		}
	}else{
		printf("Tree is locked\n");
	}

	return b->root;
}

static void merge_children(node_t *root, int index, node_t *child1, node_t *child2){
	child1->key_index = (3*2)-1;
	int i;
	
	for(i=3;i<(3*2)-1;i++)
		child1->key_array[i] = child2->key_array[i-3];
	child1->key_array[3-1] = root->key_array[index]; 
	
	
	if(0 == child2->leaf){
		for(i=3;i<(3*2);i++)
			child1->child_array[i] = child2->child_array[i-3];
	}
	
	for(i=index+1;i<root->key_index;i++){
		root->key_array[i-1] = root->key_array[i];
		root->child_array[i] = root->child_array[i+1];
	}
	root->key_index--;
	free(child2);
}

static void BTreeBorrowFromLeft(node_t *root, int index, node_t *leftPtr, node_t *curPtr){
	curPtr->key_index++;
	int i;
	for(i=curPtr->key_index-1;i>0;i--)
		curPtr->key_array[i] = curPtr->key_array[i-1];
	curPtr->key_array[0] = root->key_array[index];
	root->key_array[index] = leftPtr->key_array[leftPtr->key_index-1];
	if(0 == leftPtr->leaf)
		for(i=curPtr->key_index;i>0;i--)
			curPtr->child_array[i] = curPtr->child_array[i-1];
	curPtr->child_array[0] = leftPtr->child_array[leftPtr->key_index];
	leftPtr->key_index--;
}

static void BTreeBorrowFromRight(node_t *root, int index, node_t *rightPtr, node_t *curPtr){
	curPtr->key_index++;
	curPtr->key_array[curPtr->key_index-1] = root->key_array[index];
	root->key_array[index] = rightPtr->key_array[0];
	int i;
	for(i=0;i<rightPtr->key_index-1;i++)
		rightPtr->key_array[i] = rightPtr->key_array[i+1];
	if(0 == rightPtr->leaf){
		curPtr->child_array[curPtr->key_index] = rightPtr->child_array[0];
		for(i=0;i<rightPtr->key_index;i++)
			rightPtr->child_array[i] = rightPtr->child_array[i+1];
	}
	rightPtr->key_index--;
}

static void BTreeDeleteNoNone(int X, node_t *root){
	int i;
	
	if(1 == root->leaf){
		i=0;
		while( (i<root->key_index) && (X>root->key_array[i])) 
			i++;
		
		if(X == root->key_array[i]){
			for(;i<root->key_index-1;i++)
				root->key_array[i] = root->key_array[i+1];
			root->key_index--;
		}
		else{
			printf("Node not found.\n");
			return ;
		}
	}
	else{  
		i = 0;
		node_t *prePtr = NULL, *nexPtr = NULL;
		
		while( (i<root->key_index) && (X>root->key_array[i]) )
			i++;
		if( (i<root->key_index) && (X == root->key_array[i]) ){ 
			prePtr = root->child_array[i];
			nexPtr = root->child_array[i+1];
			
			if(prePtr->key_index > 3-1){
				int aPrecursor = BTreeGetLeftMax(prePtr);
				root->key_array[i] = aPrecursor;
				
				BTreeDeleteNoNone(aPrecursor,prePtr);
			}
			else
			if(nexPtr->key_index > 3-1){
			
				int aSuccessor = BTreeGetRightMin(nexPtr);
				root->key_array[i] = aSuccessor;
				BTreeDeleteNoNone(aSuccessor,nexPtr);
			}
			else{
			
				merge_children(root,i,prePtr,nexPtr);
				BTreeDeleteNoNone(X,prePtr);
			}
		}
		else{ 
			prePtr = root->child_array[i];
			node_t *leftBro = NULL;
			if(i<root->key_index)
				nexPtr = root->child_array[i+1];
			if(i>0)
				leftBro = root->child_array[i-1];
			
			if(3-1 == prePtr->key_index){
			
				if( (leftBro != NULL) && (leftBro->key_index > 3-1))
					BTreeBorrowFromLeft(root,i-1,leftBro,prePtr);
				else 
				if( (nexPtr != NULL) && (nexPtr->key_index > 3-1))
					BTreeBorrowFromRight(root,i,nexPtr,prePtr);
				
				else if(leftBro != NULL){
					
					merge_children(root,i-1,leftBro,prePtr);
					prePtr = leftBro;
				}
				else 
					merge_children(root,i,prePtr,nexPtr);
			}
			
			BTreeDeleteNoNone(X,prePtr);
		}
	}
}

static int BTreeGetLeftMax(node_t *T){
	if(0 == T->leaf){
		return BTreeGetLeftMax(T->child_array[T->key_index]);
	}else{
		return T->key_array[T->key_index-1];
	}
}

static int BTreeGetRightMin(node_t *T){
	if(0 == T->leaf){
		return BTreeGetRightMin(T->child_array[0]);
	}else{
		return T->key_array[0];
	}
}

node_t *delete(int key, btree_t *b)
{
	if(!b->lock){
		
		
		if(b->root->key_index == 1){
			node_t *child1 = b->root->child_array[0];
			node_t *child2 = b->root->child_array[1];
			if((!child1) && (!child2)){
				if((3-1 == child1->key_index) && (3-1 == child2->key_index)){
					
					merge_children(b->root, 0, child1, child2);
					free(b->root);
					BTreeDeleteNoNone(key, child1);
					return child1;
				}
			}
		}
		BTreeDeleteNoNone(key, b->root);
	}else{
		printf("Tree is locked\n");
	}
	return b->root;
}

void tree_unlock(btree_t *r)
{
	r->lock = 0;
}

bool tree_lock(btree_t *r)
{
	if(r->lock){
		return 0;
	}	
	r->lock = 1;

	return 1;
}



void console(btree_t *b)
{
	int number;
	char name[256];

	printf("BTree> ");
	scanf("%s", name);
	
	if(!strcmp("add", name)){
		scanf("%d", &number);
		if(number){
			b->root = insert(number, b);
		}
		printf("Insert %d\n", number);
	}else if(!strcmp("del", name)){
		scanf("%d", &number);
		if(number){
			b->root = delete(number, b);
		}
		printf("Delete %d\n", number);
	}else if(!strcmp("search", name)){
		scanf("%d", &number);
		if(number){
			result_t *rs = search(number, b->root);
			if(rs->found){
				printf("Found\n");
				print_node(rs->node_pointer);
			}else{
				printf("Not found\n");
			}
		}
	}else if(!strcmp("tree", name)){
		printf("  Order: %d\n", b->order);
		printf("   Lock: ", b->lock);
		if(b->lock){
			printf("1\n");
		}else{
			printf("0\n");
		}
		print_node(b->root);
	}else if(!strcmp("lock", name)){
		tree_lock(b);
	}else if(!strcmp("unlock", name)){
		tree_unlock(b);
	}else if(!strcmp("exit", name)){
		exit(0);
	}else if(!strcmp("quit", name)){
		exit(0);
	}else{
		printf("Unknown [%s]\n", name);
	}

	return;
}

int main(int argc, char *argv[])
{
	int test[] = {1, -11, 12, 13, 15, 16, 17, 18, 19, 20, 25, 28, 29, 31,
				 35, 36, 39, 41, 42, 45, 55, 58, 59, 61, 67, 71, 73, 74,
				 76, 80, 81, 82, 83, 88, 89, 99, 83, 91, 93, 94, 95, 98};
	int test2[] = {-23, -234, -24, -3, -38, -82, -49, -72, -84, -27, -22,
				   -35, -9, -29, -374, -84, -24 , -92, -83, -372, -756};
	int todelete[] = {15, 19};

	btree_t *main = create_btree();
	int i;

	for(i=0; i<sizeof(test)/sizeof(int); i++){
		main->root = insert(test[i], main);
	}

	for(i=0; i<sizeof(test2)/sizeof(int); i++){
		main->root = insert(test2[i], main);
	}

	for(i=0; i<sizeof(todelete)/sizeof(int); i++){
		main->root = delete(todelete[i], main);
	}

	
	for(;;){
		console(main);
	}

	return 0;
}