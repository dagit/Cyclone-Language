// Derived from:

/*
 * simple hashtable map: Cstring -> Int
 */

#include <stdio.h>
#include <string.h>
using Stdio {
using String {

#define ht_num_primes 28

static unsigned long ht_prime_list[ht_num_primes] = {
    53ul,         97ul,         193ul,       389ul,       769ul,
    1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
    49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
    1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
    50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
    1610612741ul, 3221225473ul, 4294967291ul
};

struct ht_node {
    char ?key;
    int val;
    struct ht_node *next;
};

struct ht_ht {
    int size;
    struct ht_node *?tbl;
    int iter_index;
    struct ht_node *iter_next;
    int items;
#ifdef HT_DEBUG
    int collisions;
#endif /* HT_DEBUG */
};

inline int ht_val(struct ht_node @node) {
    return(node->val);
}

inline char ?ht_key(struct ht_node @node) {
    return(node->key);
}

inline int ht_hashcode(struct ht_ht @ht, const char ?`r key) {
    unsigned long val = 0;
    for (; *key; ++key) val = 5 * val + *key;
    return (int)(val % ht->size);
}

//ANNOYING to have to declare `r
struct ht_node @ht_node_create(char ?`r key) {
    char ?newkey;
    if ((newkey = strdup(key)) == null) {
	perror("strdup newkey");
	exit(1);
    }
    struct ht_node @node =
      new ht_node {
        .key = newkey,
        .val = 0,
        .next = (struct ht_node *)null
      };
    return(node);
}

struct ht_ht @ht_create(int size) {
    int i = 0;
    while (ht_prime_list[i] < size) { i++; }
    struct ht_ht @ht =
      new ht_ht{
        .size = ht_prime_list[i],
        .tbl = new {for i<ht_prime_list[i]: null},
        .iter_index = 0,
        .iter_next = null,
        .items = 0
      };
#ifdef HT_DEBUG
    ht->collisions = 0;
#endif /* HT_DEBUG */
    return(ht);
}

void ht_destroy(struct ht_ht @ht) {
    struct ht_node *cur, *next;
    int i;
#ifdef HT_DEBUG
    int chain_len;
    int max_chain_len = 0;
    int density = 0;
    fprintf(stderr, " HT: size            %d\n", ht->size);
    fprintf(stderr, " HT: items           %d\n", ht->items);
    fprintf(stderr, " HT: collisions      %d\n", ht->collisions);
#endif /* HT_DEBUG */
    for (i=0; i<ht->size; i++) {
	next = ht->tbl[i];
#ifdef HT_DEBUG
	if (next) {
	    density++;
	}
	chain_len = 0;
#endif /* HT_DEBUG */
	while (next) {
	    cur = next;
	    next = next->next;
//	    free(cur->key);
//	    free(cur);
#ifdef HT_DEBUG
	    chain_len++;
#endif /* HT_DEBUG */
	}
#ifdef HT_DEBUG
	if (chain_len > max_chain_len)
	    max_chain_len = chain_len;
#endif /* HT_DEBUG */
    }
//    free(ht->tbl);
//    free(ht);
#ifdef HT_DEBUG
    fprintf(stderr, " HT: density         %d\n", density);
    fprintf(stderr, " HT: max chain len   %d\n", max_chain_len);
#endif /* HT_DEBUG */
}

//ANNOYING to have to declare `r
inline struct ht_node *ht_find(struct ht_ht @ht, const char ?`r key) {
    int hash_code = ht_hashcode(ht, key);
    struct ht_node *node = ht->tbl[hash_code];
    while (node) {
	if (strcmp(key, node->key) == 0) return(node);
	node = node->next;
    }
    return((struct ht_node *)null);
}

//ANNOYING to have to declare `r
inline struct ht_node @ht_find_new(struct ht_ht @ht, char ?`r key) {
    int hash_code = ht_hashcode(ht, key);
    struct ht_node *prev = null, *node = ht->tbl[hash_code];
    while (node) {
	if (strcmp(key, node->key) == 0) return(node);
	prev = node;
	node = node->next;
#ifdef HT_DEBUG
	ht->collisions++;
#endif /* HT_DEBUG */
    }
    ht->items++;
    let newnode = ht_node_create(key);
    if (prev) {
	return(prev->next = newnode);
    } else {
        ht->tbl[hash_code] = newnode;
	return(newnode);
    }
}

/*
 *  Hash Table iterator data/functions
 */
inline struct ht_node *ht_next(struct ht_ht @ht) {
    unsigned long index;
    struct ht_node *node = ht->iter_next;
    if (node) {
	ht->iter_next = node->next;
	return(node);
    } else {
	while (ht->iter_index < ht->size) {
	    index = ht->iter_index++;
	    if (ht->tbl[index]) {
		ht->iter_next = ht->tbl[index]->next;
		return(ht->tbl[index]);
	    }
	}
    }
    return((struct ht_node *)null);
}

inline struct ht_node *ht_first(struct ht_ht @ht) {
    ht->iter_index = 0;
    ht->iter_next = (struct ht_node *)null;
    return(ht_next(ht));
}

inline int ht_count(struct ht_ht @ht) {
    return(ht->items);
}

}}
