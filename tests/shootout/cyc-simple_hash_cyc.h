/*
 * simple hashtable map: Cstring -> Int
 */

#include <stdio.h>
#include <string.h>

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

static inline int ht_val(struct ht_node @node) {
    return(node->val);
}

static inline char ?ht_key(struct ht_node @node) {
    return(node->key);
}

static inline int ht_hashcode(struct ht_ht @ht, const char ?`r key) {
    unsigned long val = 0;
    unsigned i = 0, sz = numelts(key);
    char c;
    for (; i < sz; ++i) {
      c = key[i];
      if (c == 0) break;
      val = 5 * val + c;
    }
    return (val % ht->size);
}

static inline mstring_t mydup(buffer_t src) {
  unsigned sz = numelts(src);
  mstring_t res = (char ?)calloc(sz+1,sizeof(char));
   memcpy(res,src,sz);
  return res;
}

struct ht_node @ht_node_create(char ? key) {
    char ?newkey;
    struct ht_node *node;
    if ((node = (struct ht_node *)malloc(sizeof(struct ht_node))) == 0) {
	perror("malloc ht_node");
	exit(1);
    }
    if ((newkey = (char ?)mydup(key)) == NULL) {
	perror("strdup newkey");
	exit(1);
    }
    node->key = newkey;
    node->val = 0;
    node->next = (struct ht_node *)NULL;
    return(node);
}

struct ht_ht @ht_create(int size) {
    int i = 0;
    struct ht_ht *ht = (struct ht_ht *)malloc(sizeof(struct ht_ht));
    while (ht_prime_list[i] < size) { i++; }
    ht->size = ht_prime_list[i];
    ht->tbl = new {for i<ht_prime_list[i]: NULL};
    ht->iter_index = 0;
    ht->iter_next = 0;
    ht->items = 0;
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
	    free(cur->key);
	    free(cur);
#ifdef HT_DEBUG
	    chain_len++;
#endif /* HT_DEBUG */
	}
#ifdef HT_DEBUG
	if (chain_len > max_chain_len)
	    max_chain_len = chain_len;
#endif /* HT_DEBUG */
    }
    free(ht->tbl);
    free(ht);
#ifdef HT_DEBUG
    fprintf(stderr, " HT: density         %d\n", density);
    fprintf(stderr, " HT: max chain len   %d\n", max_chain_len);
#endif /* HT_DEBUG */
}

static inline int mystrcmp(const char ? s1, const char ? s2) {
  unsigned sz1 = numelts(s1), sz2 = numelts(s2);
  for (unsigned i = 0; i < sz1 && i < sz2; i++) {
    char c1 = s1[i];
    char c2 = s2[i];
    if (c1 != c2) return 1;
    if (c1 == 0) return 0;
  }
  return 0;
}

inline struct ht_node *ht_find(struct ht_ht @ht, const char ? key) {
    int hash_code = ht_hashcode(ht, key);
    struct ht_node *node = ht->tbl[hash_code];
    while (node) {
	if (mystrcmp(key, node->key) == 0) return(node);
	node = node->next;
    }
    return((struct ht_node *)NULL);
}

inline struct ht_node @ht_find_new(struct ht_ht @ht, char ? key) {
    int hash_code = ht_hashcode(ht, key);
    struct ht_node *prev = 0, *node = ht->tbl[hash_code];
    while (node) {
	if (mystrcmp(key, node->key) == 0) return(node);
	prev = node;
	node = node->next;
#ifdef HT_DEBUG
	ht->collisions++;
#endif /* HT_DEBUG */
    }
    ht->items++;
    if (prev) {
	return(prev->next = ht_node_create(key));
    } else {
	return(ht->tbl[hash_code] = ht_node_create(key));
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
    return((struct ht_node *)NULL);
}

inline struct ht_node *ht_first(struct ht_ht @ht) {
    ht->iter_index = 0;
    ht->iter_next = (struct ht_node *)NULL;
    return(ht_next(ht));
}

inline int ht_count(struct ht_ht @ht) {
    return(ht->items);
}
