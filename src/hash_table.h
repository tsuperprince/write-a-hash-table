//hash_table.h

/*define const parameter*/
#define HASH_TABLE_MAX_SIZE 53;
/*key-value pairs (items) will each be stored in a struct*/
typedef struct {
	char * key;
	char * value;
}ht_item;

/*hash table stores an array of pointers to items,
 * and some details about its size and how full it is:*/
typedef struct {
	int size;
	int count;
	ht_item ** items;
} ht_hash_table;

ht_hash_table* ht_new();
static ht_item * ht_new_item(const char *k, const char* v);
static void ht_del_item(ht_item* i)
void ht_del_hash_table(ht_hash_table* ht) ;
static int ht_hash(const char* s, const int a, const int m);
static int ht_get_hash(const char* s, const int num_buckets, const int attempt);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key);
static ht_item HT_DELETED_ITEM = {NULL, NULL};
