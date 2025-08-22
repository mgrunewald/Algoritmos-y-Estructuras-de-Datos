#include "tp2.h"
#include <stdlib.h>
#include <stdbool.h>

struct node;
typedef struct node node_t;

struct node {
    void* value;
    node_t* next;
    node_t* prev;
};

struct list {
    node_t* head;
    node_t* tail;
    size_t size;
};

struct list_iter {
    list_t* list;
    node_t* curr;
};


/**
 * Establishes a node as the head of a list.
 */
void head_node(node_t *node, list_t *list) { 
    list -> head = node;
    node -> prev = NULL;
}

/**
 * Establishes a node as the tail of a list.
 */
void tail_node(node_t *node, list_t *list){
    list -> tail = node;
    node -> next = NULL;
}

list_t *list_new(){
    list_t* new_list = (list_t*)malloc(sizeof(list_t));
    if (!new_list) { return NULL; }
    new_list -> head = NULL;
    new_list -> tail = NULL;
    new_list -> size = 0;
    return new_list;
}

size_t list_length(const list_t *list){
    return list -> size;
}

bool list_is_empty(const list_t *list){
    return (list_length(list) == 0); 
}

/**
 * Creates a new node with the given value and locates it as the head or tail of a list, depending on which function is using it.
 * If the list is empty, the new node will become both the head and the tail.
 * It returns a pointer to the newly created node, or NULL if memory allocation fails.
 */
node_t *create_and_locate_node(list_t *list, void *value, bool is_head) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) { return NULL; }
    new_node -> value = value;

    if (list_is_empty(list)) {
        head_node(new_node, list);
        tail_node(new_node, list); }
    else {
        if (is_head) {
            new_node -> next = list -> head;
            list -> head-> prev = new_node;
            head_node(new_node, list); }
        else {
            new_node -> prev = list -> tail;
            list -> tail -> next = new_node;
            tail_node(new_node, list); }
    }
    list -> size++;
    return new_node;
}

bool list_insert_head(list_t *list, void *value){
    node_t *new_node = create_and_locate_node(list, value, true);
    return new_node != NULL;
}

bool list_insert_tail(list_t *list, void *value){
    node_t *new_node = create_and_locate_node(list, value, false);
    return new_node != NULL;
}

void *list_peek_head(const list_t *list){
    if (list_is_empty(list)) { return NULL; } 
    return list -> head -> value;  
}

void *list_peek_tail(const list_t *list){
    if (list_is_empty(list)) { return NULL; }
    return list -> tail -> value;
}

/**
 * Removes and returns a node from the head or tail of a list, depending on which function is using it.
 * The end that will be removed and returned is specified by whether 'is_head' is true or not. 
 * If the list is empty, it returns NULL.
 * The function returns the value stored in the removed node, or NULL if the list is empty.
 */
void *list_pop_node(list_t *list, bool is_head) { 
    if (list_is_empty(list)) { return NULL; }
    void *popped_node_value;
    node_t *temp;
    if (is_head) {
        popped_node_value = list_peek_head(list);
        temp = list -> head;
        list -> head = list -> head -> next; }
    else {
        popped_node_value = list -> tail -> value;
        temp = list -> tail;
        list -> tail = list -> tail -> prev; }
    free(temp);
    list -> size--;
    return popped_node_value;
}

void *list_pop_head(list_t *list){  
    return list_pop_node(list, true);
}

void *list_pop_tail(list_t *list){
    return list_pop_node(list, false);
}

void list_destroy(list_t *list, void destroy_value(void *)){
    while (!list_is_empty(list)) {
        void* value_im_destroying = list_pop_head(list);
        if (destroy_value) { destroy_value(value_im_destroying); } }
    free(list);
}

/**
 * Creates a new iterator for a given list, starting at either the head or tail, depending on which function is using it, specified by
 * the value of the boolean 'is_head'.
 * The function returns a pointer to the newly created iterator, or NULL if memory allocation fails.
 */
list_iter_t *list_create_iter(list_t *list, bool is_head) {
    list_iter_t *iterator = malloc(sizeof(list_iter_t));
    if (!iterator) { return NULL; }
    iterator -> list = list;
    if (is_head) {
        iterator -> curr = list -> head; }
    else { iterator -> curr = list -> tail; }
    return iterator;
}

list_iter_t *list_iter_create_head(list_t *list){
    return list_create_iter(list, true);
}

list_iter_t *list_iter_create_tail(list_t *list){
    return list_create_iter(list, false);
}

bool list_iter_forward(list_iter_t *iter){
    if (iter -> curr == iter -> list -> tail) { return false; } 
    iter -> curr = iter -> curr -> next;
    return true; 
}

bool list_iter_backward(list_iter_t *iter){
    if (iter -> curr == iter -> list -> head) { return false; }
    iter -> curr = iter -> curr -> prev;
    return true;
}

void *list_iter_peek_current(const list_iter_t *iter){
    if (list_is_empty(iter -> list)) { return NULL; }
    return iter -> curr -> value; 
}

bool list_iter_at_last(const list_iter_t *iter){
    if (iter -> curr != iter -> list -> tail) { return false; }
    return true;
}

bool list_iter_at_first(const list_iter_t *iter){
    return (iter -> curr == iter -> list -> head);
}

void list_iter_destroy(list_iter_t *iter){
    free (iter); 
}

/**
 * Inserts a new node with the given value either before or after the current iterator position, 
 * depending on the boolean value of 'insert_after'.
 * If the list is empty, the new node is inserted as the head.
 * The function returns true if the insertion was successful, false if memory allocation fails.
 */
bool list_insert_iter(list_iter_t *iter, void *value, bool insert_after) { 
    if (list_is_empty(iter->list)) {
        bool inserted_correctly = list_insert_head(iter->list, value);
        iter -> curr = iter -> list -> head;
        return inserted_correctly; }

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (!new_node) { return false; }
    new_node -> value = value;

    if (insert_after) {
        new_node -> prev = iter -> curr;
        new_node -> next = iter -> curr -> next;

        if (!iter -> curr -> next) {
            iter -> list -> tail = new_node; } 
        else {
            iter ->curr->next->prev = new_node; }

        iter -> curr -> next = new_node; }
    else { 
        new_node -> prev = iter -> curr -> prev;
        new_node -> next = iter -> curr;

        if (!iter -> curr -> prev) { iter -> list -> head = new_node; }
        else { iter -> curr -> prev -> next = new_node; }

        iter -> curr -> prev = new_node; }
    iter -> list -> size++;
    return true;
}

bool list_iter_insert_after(list_iter_t *iter, void *value){
    return list_insert_iter(iter, value, true);
}

bool list_iter_insert_before(list_iter_t *iter, void *value){
    return list_insert_iter(iter, value, false);
}

void *list_iter_delete(list_iter_t *iter){
   if (list_is_empty(iter -> list)) { return NULL; } 
    
    node_t *temp_node = iter -> curr;
    void *deleted_node_value = temp_node -> value;

    if (temp_node -> next) { 
        temp_node -> next -> prev = temp_node -> prev; }
    else { iter -> list -> tail = temp_node -> prev; }

    if (temp_node -> prev) { 
        temp_node -> prev -> next = temp_node -> next; } 
    else { iter -> list -> head = temp_node -> next; }

    iter -> curr = (temp_node -> next) ? temp_node -> next : temp_node -> prev;

    free(temp_node);
    iter -> list -> size--;
    return deleted_node_value;
}