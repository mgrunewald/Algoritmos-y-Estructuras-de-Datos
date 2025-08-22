#define _XOPEN_SOURCE 700 

#include "tp3.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


struct dictionary;
struct node;
typedef struct node node_t;
typedef struct dictionary dictionary_t;
typedef void (*destroy_f)(void *);

#define TABLE_CAPACITY 16
#define LOAD_FACTOR_THRESHOLD 0.75

struct node {
  const char *key;
  void *value;
  struct node *next;
};

typedef void (*destroy_f)(void *);

struct dictionary {
  node_t **table;
  int size;
  int capacity;
  destroy_f destroy;
};

uint64_t fnv1a_64(const char *key) {
    uint64_t hash = 14695981039346656037ULL;
    while (*key) {
        hash ^= (uint64_t)(*key);
        hash *= 1099511628211ULL;
        key++;
    }
    return hash;
}

int hash_function(const char* key, int capacity) {
  uint64_t hash = fnv1a_64(key);
  return (int)(hash % capacity);
}

dictionary_t *dictionary_create(destroy_f destroy) {
  dictionary_t *dict = (dictionary_t *)malloc(sizeof(dictionary_t));
  if (!dict) { return NULL; }

  dict->table = (node_t **)calloc(TABLE_CAPACITY,sizeof(node_t *));
  if(!dict->table){
    free(dict);
    return NULL;}
  dict->size = 0;
  dict->capacity = TABLE_CAPACITY;
  dict->destroy = destroy;
  return dict;
};

bool needs_rehash(dictionary_t *dictionary) {
  float size = (float)dictionary->size; 
  float capacity = (float)dictionary->capacity;
  return (size / capacity) > LOAD_FACTOR_THRESHOLD;
}


bool rehash(dictionary_t *dictionary) {
  int new_capacity = 2 * dictionary->capacity;
  node_t **new_table = (node_t **)malloc(new_capacity * sizeof(node_t *));
  if (!new_table) { return false; }
  for (int i = 0; i < new_capacity; i++) { new_table[i] = NULL; }
  for (int i = 0; i < dictionary->capacity; i++) {
    node_t *current = dictionary->table[i];
    while (current) {
      int new_index = hash_function(current->key, new_capacity);
      node_t *next = current -> next; 
      current->next = new_table[new_index];
      new_table[new_index] = current;
      current = next;
    }
  }
  
  free(dictionary->table);
  dictionary -> table = new_table;
  dictionary->capacity = new_capacity;
  return true;
}

node_t *get_node (dictionary_t *dictionary, const char *key){
  int index = hash_function(key, dictionary->capacity);
  node_t *head = dictionary->table[index];

  while (head) {
    if (strcmp(head->key, key) == 0) {
      return head; }
    head = head->next;
  }
  return NULL; 

}

bool dictionary_put(dictionary_t *dictionary, const char *key, void *value) {
  node_t *node = get_node(dictionary, key);
  if (node){
    free(node->value);
    node -> value = value;
    return true;
  }
  int index = hash_function(key, dictionary->capacity);
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  if (!new_node) { return false; }

  char* new_key = strdup(key);
  if (!new_key){
    free(new_node);
    return false;
  }

  new_node->key = new_key;
  new_node->value = value;
  new_node->next = dictionary->table[index];
  dictionary->table[index] = new_node; 
  dictionary->size++;

  if (needs_rehash(dictionary)) { rehash(dictionary); }

  return true;
}

void *dictionary_get(dictionary_t *dictionary, const char *key, bool *err) {
  if (!dictionary || !key || key[0] == '\0') {
    *err = true;
    return NULL; 
  }
  int index = hash_function(key, dictionary->capacity);
  node_t *head = dictionary->table[index];

  while (head) {
    if (strcmp(head->key, key) == 0) {
      *err = false;
      return head->value; }
    head = head->next;
  }
  *err = true; 
  return NULL; 
};

bool dictionary_delete(dictionary_t *dictionary, const char *key) {
  int index = hash_function(key, dictionary->capacity);
  node_t *current = dictionary->table[index];
  node_t *prev = NULL;
  if (!current) { return false; }
  while(current) {
    if (strcmp(current->key, key) == 0) {
    dictionary->size--;
    if (!prev) {dictionary->table[index] = current -> next;
    } else {
      prev -> next = current -> next;
    }
    free((char *)current -> key);
    if (dictionary -> destroy) {
    dictionary -> destroy(current -> value);}
    free(current);
    return true; 
  }
  prev = current;
  current = current -> next;
  }
  return false;
};

void *dictionary_pop(dictionary_t *dictionary, const char *key, bool *err) {
  int index = hash_function(key, dictionary->capacity);
  node_t *current = dictionary->table[index];
  node_t *prev = NULL;

  while(current) {
    if (strcmp(current->key, key) == 0) {
      dictionary->size--;
      if (!prev) {dictionary->table[index] = current -> next;
      } else { prev -> next = current -> next; }
      free((char*)current -> key);
      void *returning = current -> value;
      free(current);
      *err = false;
      return returning; 
  }
  prev = current;
  current = current -> next;
  }
  *err = true;
  return NULL;
};

bool dictionary_contains(dictionary_t *dictionary, const char *key) {
  int index = hash_function(key, dictionary->capacity);
  node_t *head = dictionary->table[index];

  while (head) {
    if (strcmp(head->key, key) == 0) { return true; }
    head = head->next;
  }
  return false;
};


size_t dictionary_size(dictionary_t *dictionary) { 
  return dictionary -> size;
};  

void dictionary_destroy(dictionary_t *dictionary) {
  for (int i = 0; i < dictionary->capacity; i++) {
    node_t *current = dictionary->table[i];
    while (current) {
      node_t *next = current->next;
      free((void *)current->key); 
      if (dictionary->destroy) { dictionary->destroy(current->value); }
      free(current);
      current = next;
    }
  }
  free(dictionary->table);
  free(dictionary);
}
