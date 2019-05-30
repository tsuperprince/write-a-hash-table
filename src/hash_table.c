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
static ht_hash_table* ht_new_sized(const int base_size)
{
	ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
	ht->base_size = base_size;

	ht->size = next_prime(ht->base_size);
	ht->count = 0;
	ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
	return ht;
}
/* function ht_new initialises a new hash table.
 * size defines how many items we can strore
 * We initialise the array of items with calloc,
 * which fills the allocated memory with NULL bytes.
 * A NULL entry in the array indicates that the bucket is empty.
 */
ht_hash_table* ht_new() {
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
	/*
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));

    ht->size = HT_INITIAL_BASE_SIZE;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
    */
}
/*RESIZE
 * In our resize function, we check to make sure we're not attempting to reduce
 *  the size of the hash table below its minimum. We then initialise a new hash table
 *  with the desired size. All non NULL or deleted items are inserted into the new hash table.
 *   We then swap the attributes of the new and old hash tables before deleting the old*/
static void ht_resize(ht_hash_table* ht, const int base_size){
	if(base_size < HT_INITIAL_BASE_SIZE){
		return;
	}
	ht_hash_table* new_ht = ht_new_sized(base_size);
	for(int i = 0; i < ht->size; i++){
		ht_item* item = ht->items[i];
		if(item != NULL && item != &HT_DELETED_ITEM) {
			ht_insert(new_ht, item->key, item->value);
		}
	}
	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;
	/*to delete new_ht, we give it ht's size and items*/
	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	ht_del_hash_table(new_ht);
}
/*
 * To simplify resizing,
 * we define two small functions for resizing up and down.
 */
static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}
static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
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
	const int load = ht->count * 100 / ht->size;
	if(load > 70){
		ht_resize_up(ht);
	}
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
	const int load = ht->count * 100 / ht-size;
	if(load < 10){
		ht_resize_down(ht);
	}
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



