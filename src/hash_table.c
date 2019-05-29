// hash_table.c
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

/* a function ht_new_item allocates a chunk of memory the size of an ht_item
 * and saves a copy of the strings k and v in the new chunk of memory
 *
 */
static ht_item * ht_new_item(const char *k, const char* v)
{
	ht_item *i = malloc(sizeof(ht_item));
	i->key = strdup(k);
	i->value = strdup(v);
	/*strdup(s) return a pointer hich is a duplicate of the string pointed to by s
	 * The memory obtained is done dynamically using malloc */
	return i;
}
/* function ht_new initialises a new hash table.
 * size defines how many items we can strore
 * We initialise the array of items with calloc,
 * which fills the allocated memory with NULL bytes.
 * A NULL entry in the array indicates that the bucket is empty.
 */
ht_hash_table* ht_new() {
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht->size = HASH_TABLE_MAX_SIZE;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}
/*function ht_items and ht_hash_tables,
 * which free the memory we've allocated,
 * so we don't cause memory leaks.
  */
static void ht_del_item(ht_item* i) {
    free(i->key);
    free(i->value);
    free(i);
}
void ht_del_hash_table(ht_hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

/*Hash function*/
/*algorithm
 *
	function hash(string, a, num_buckets):
	hash = 0
	string_len = length(string)
	for i = 0, 1, ..., string_len:
		hash += (a ** (string_len - (i+1))) * char_code(string[i])
	hash = hash % num_buckets
	return hash
 * 1. Convert the string to a large integer
 * 2. Reduce the size of the integer to a fixed range by taking its remainder mod m
 * */
static int ht_hash(const char* s, const int a, const int m)
{
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}
/*handle collisions using a technique called open addressing with double hashing
 *
 */
static int ht_get_hash(
    const char* s, const int num_buckets, const int attempt
) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

/*INSERT
 * To insert a new key-value pair, we iterate through indexes until we find an
 *  empty bucket. We then insert the item into that bucket and increment the
 *  hash table's count attribute, to indicate a new item has been added
 *  When inserting, if we hit a deleted node, we can insert the new node
 *  into the deleted slot*/
void ht_insert(ht_hash_table *ht, const char *key, const char *value)
{
	ht_item* item = ht_new_item(key,value);
	int index = ht_get_hash(item->key, ht->size,0);
	ht_item* cur_item = ht->items[index];
	int i = 1;
	while(cur_item != NULL)
	{
		if(cur_item != &HT_DELETED_ITEM)
		{
			if(strcmp(cur_item->key, key) == 0){
				ht_del_item(cur_item);
				ht->items[index] = item;
				return;
			}
			index = ht_get_hash (item->key, ht->size,i);
			cur_item = ht->items[index];
			i++;
		}
		else
		{
		/*if cur_item == &HT_DELETED_ITEM*/
			break;
		}
		
	}
	ht->items[index] = item;
    ht->count++;
}
/* SEARCH
 * Searching is similar to inserting, but at each iteration of the while loop,
 *  we check whether the item's key matches the key we're searching for.
 *  If it does, we return the item's value. If the while loop hits a NULL bucket,
 *  we return NULL, to indicate that no value was found.
 *  When searching, we ignore and 'jump over' deleted nodes.
 */
char* ht_search(ht_hash_table * ht, const char *key){
	int index = ht_get_hash(key, ht->size,0);
	ht_item* item = ht->items[index];
	int i = 1;
	while(item != NULL){
		if(item != &HT_DELETED_ITEM)
		{
			if(strcmp(item->key, key) == 0){
				return item->value;
			}
		}
		index = ht_get_hash(key, ht->size,i);
		item = ht->items[index];
		i++;
	}
	return NULL;
}
/*DELETE
 * Removing it from the table will break that chain, and will make finding
 * items in the tail of the chain impossible. To solve this, instead of deleting
 * the item, we simply mark it as deleted*/
void ht_delete(ht_hash_table * ht, const char* key){
	int index = ht_get_hash(key, ht->size, 0);
	ht_item *item = ht->items[index];
	int i = 1;
	while(item != NULL) {
		if(item != &HT_DELETED_ITEM){
			if(strcmp(item->key,key) == 0){
				ht_del_item(item);
				ht->items[index] = &HT_DELETED_ITEM;
			}
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}
	ht->count--;
}



