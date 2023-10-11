typedef int (*Comparator)(const void *, const void *);
typedef char *(*Stringizer)(const void *);

struct node;
struct binary_tree;

struct node* create_node(void*);
struct node* create_sentinel();
int is_sentinel(struct node*);
struct node* left_node(struct node*);
struct node* right_node(struct node*);
struct node* parent_node(struct node*);
const void* data(struct node*);
void change_data(struct node*, void*);
void change_parent_node(struct node*, struct node*);
void change_left_node(struct node*, struct node*);
void change_right_node(struct node*, struct node*);
int has_left_child(struct node*);
int has_right_child(struct node*);
int has_two_child(struct node*);
void free_node(struct node*);

struct binary_tree* create_binary_tree();
struct node* root(struct binary_tree*);
void change_root(struct binary_tree*, struct node*);
int is_root(struct binary_tree*, struct node*, Comparator);
int is_binary_tree_empty(struct binary_tree*);
struct node* search(struct binary_tree*, const void*, Comparator);  
int contains(struct binary_tree*, const void*, Comparator);
void change_sentinel_node(struct node*, const void*);
void insert(struct binary_tree*, const void*, Comparator);
void detach(struct binary_tree*, struct node*, struct node*, Comparator);
struct node* successor(struct node*, Comparator);
void delete(struct binary_tree*, const void*, Comparator);
int height(struct binary_tree*);
void free_tree(struct binary_tree*);

void print_tree_pre_order(struct binary_tree*);
void print_tree_in_order(struct binary_tree*);
void print_tree_post_order(struct binary_tree*);




struct node
{
    void* data;
    int is_sent;
    struct node* left;
    struct node* right;
    struct node* parent;
};

struct node* create_node(void* data)
{
    struct node* result = malloc(sizeof(struct node));
    result->data = data;
    result->is_sent = 0;
    result->left = result;
    result->right = result;
    result->parent = result;
  
    return result;
}

struct node* create_sentinel()
{
    struct node* result = malloc(sizeof(struct node));
    result->data = NULL;
    result->is_sent = 1;
    result->left = result;
    result->right = result;
    result->parent = result;
  
    return result;
}

int is_sentinel(struct node* n)
{
    return n->is_sent;  
}

struct node* left_node(struct node* n)
{
    return n->left;
}  

struct node* right_node(struct node* n)
{
    return n->right;
}  

struct node* parent_node(struct node* n)
{
    return n->parent;
}  

const void* data(struct node* n)
{
    return n->data;
}

void change_data(struct node* n, void* data)
{
    n->data = data;
}

void change_parent_node(struct node* n, struct node* parent)
{
    n->parent = parent;
} 

void change_left_node(struct node* n, struct node* left)
{
    n->left = left;
    change_parent_node(left_node(n), n);
}

void change_right_node(struct node* n, struct node* right)
{
    n->right = right;
    change_parent_node(right_node(n), n);
} 

int has_left_child(struct node* n)
{
    return !is_sentinel(left_node(n));
}

int has_right_child(struct node* n)
{
    return !is_sentinel(right_node(n));
}

int has_two_child(struct node* n)
{
    if (has_left_child(n) && has_right_child(n))
        return TRUE;
    else
        return FALSE;
}

void free_node(struct node* n)
{
    if (is_sentinel(n))
        free(n);
    else
    {
        free_node(n->left);
        free_node(n->right);
        free(n);
    }
}



struct binary_tree
{
    struct node* root;
};

struct binary_tree* create_binary_tree()
{ 
    struct binary_tree* result = malloc(sizeof(struct binary_tree));
    
    result->root = create_node(NULL);
    change_right_node(result->root, create_sentinel());
    change_left_node(result->root, create_sentinel());
  
    return result;
}

struct node* root(struct binary_tree* tree)
{
    return tree->root;  
}

void change_root(struct binary_tree* tree, struct node* root)
{
    change_left_node(tree->root, root);
}

int is_root(struct binary_tree* tree, struct node* n, Comparator compare)
{
    return compare(data(left_node(root(tree))), data(n)) == 0;
}

int is_binary_tree_empty(struct binary_tree* tree)
{
    return is_sentinel(left_node(root(tree)));
}  

struct node* search(struct binary_tree* tree, const void* x, Comparator compare)
{
    struct node* n = left_node(root(tree));
    
    while (!is_sentinel(n))
    {
        if (compare(data(n), x) == 0)             return n;
        else if (compare(data(n), x) > 0)    n = left_node(n);
        else                                 n = right_node(n);
    }
    return n;
}  

int contains(struct binary_tree* tree, const void* x, Comparator compare)
{
    return !is_sentinel(search(tree, x, compare));  
}

void change_sentinel_node(struct node* n, const void* data)
{
    n->is_sent = 0;
    change_data(n, (void*)data);
    change_left_node(n, create_sentinel());
    change_right_node(n, create_sentinel());
}  

void insert(struct binary_tree* tree, const void* x, Comparator compare)
{
    if (!contains(tree, x, compare))
        change_sentinel_node(search(tree, x, compare), x);

}  

void detach(struct binary_tree* tree, struct node* n, struct node* link, Comparator compare)
{
    struct node* child;
    
    if (has_right_child(n))
        child = right_node(n);
    else
        child = left_node(n);
 
    if (is_root(tree, n, compare))
        change_root(tree, child);
    else
    {
        if (left_node(link) == n)
            change_left_node(link, child);
        else
            change_right_node(link, child);
    }
}

struct node* successor(struct node* n, Comparator compare)
{
    if (has_right_child(n))
    {
        n = right_node(n);
        while (has_left_child(n))
            n = left_node(n);
        return n;
    }
    else
    {
        struct node* parent = parent_node(n);
        while (!is_sentinel(parent) && compare(data(n), data(left_node(parent))))
        {
            n = parent;
            parent = parent_node(parent);
        }
        return parent;
    }
}

void delete(struct binary_tree* tree, const void* x, Comparator compare)
{
    struct node* n = search(tree, x, compare);   
    if (!has_two_child(n))
        detach(tree, n, parent_node(n), compare);  
    else
    {
        struct node* s = successor(n, compare);
        struct node* p = parent_node(s);
        detach(tree, s, p, compare);
        change_data(n, (void*)data(s));
    }
}  


int _height(struct node* n)
{
    if (is_sentinel(n))
        return -1;
    
    int left = _height(left_node(n));
    int right = _height(right_node(n));

    if (left > right)
        return left + 1;
    else 
        return right + 1;
}

int height(struct binary_tree* tree)
{
    return _height(left_node(root(tree)));
}

void free_tree(struct binary_tree* tree)
{
    free_node(root(tree));
    free(tree);
}


int int_ascending(const void* a, const void* b) {
  int left = *((int*)a), right = *((int*)b);
  return left - right;
}

char* int_stringizer(const void* a) {
  int value = *((int*)a);
  char* s = calloc(16, sizeof(char));
  sprintf(s, "%d", value);
  return s;
}

void _print_tree_pre_order(struct node* n, Stringizer stringizer)
{
    if (is_sentinel(n) || data(n) == NULL)
        return;
    char* c = stringizer(data(n));
    printf("%s ", c);
    free(c); 
    _print_tree_pre_order(left_node(n), stringizer); 
    _print_tree_pre_order(right_node(n), stringizer);
}

void print_tree_pre_order(struct binary_tree* tree)
{
    if (is_binary_tree_empty(tree))
        return;
    _print_tree_pre_order(left_node(root(tree)), int_stringizer);
}

void _print_tree_in_order(struct node* n, Stringizer stringizer)
{
    if (is_sentinel(n) || data(n) == NULL)
        return;
    _print_tree_in_order(left_node(n), stringizer);
    char* c = stringizer(data(n));
    printf("%s ", c);
    free(c);  
    _print_tree_in_order(right_node(n), stringizer);
}

void print_tree_in_order(struct binary_tree* tree)
{
    if (is_binary_tree_empty(tree))
        return;
    _print_tree_in_order(left_node(root(tree)), int_stringizer);
}

void _print_tree_post_order(struct node* n, Stringizer stringizer)
{
    if (is_sentinel(n) || data(n) == NULL)
        return;
    _print_tree_post_order(left_node(n), stringizer); 
    _print_tree_post_order(right_node(n), stringizer);
    char* c = stringizer(data(n));
    printf("%s ", c);
    free(c);
}

void print_tree_post_order(struct binary_tree* tree)
{
    if (is_binary_tree_empty(tree))
        return;
    _print_tree_post_order(left_node(root(tree)), int_stringizer);
}

void _print_tree_level_order(struct node* n, int level, Stringizer stringizer)
{
    if (is_sentinel(n) || data(n) == NULL)
        return;
    if (level == 0)
    {
        char* c = stringizer(data(n));
        printf("%s ", c);
        free(c);
    }
    else
    {
        _print_tree_level_order(left_node(n), level - 1, stringizer);
        _print_tree_level_order(right_node(n), level - 1, stringizer);
    }
}

void print_tree_level_order(struct binary_tree* tree)
{
    int h = height(tree);
    for (int i = 0; i <= h; i++)
        _print_tree_level_order(left_node(root(tree)), i, int_stringizer);
}


int main()
{
    int n;
    scanf("%d", &n);

    struct binary_tree* tree = create_binary_tree();
    int values[n];
    for (int i = 0; i < n; i++) {
        int vi;
        scanf("%d", &vi);
        values[i] = vi;

        insert(tree, &values[i], int_ascending);
    }

    print_tree_pre_order(tree);
    puts("");
    print_tree_in_order(tree);
    puts("");
    print_tree_post_order(tree);
    puts("");
    print_tree_level_order(tree);
    puts("");
    
    free_tree(tree);
    
    return 0;
}