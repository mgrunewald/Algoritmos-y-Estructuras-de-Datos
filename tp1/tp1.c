#include "tp1.h"
#include <stdlib.h>

bool is_prime(int x){
    if ( x <= 1) { return false; }
    for (int i = 2; i < x; i++ ) {
        if ( x % i == 0) { return false; } }
    return true;
}

int storage_capacity(float d, float v){
    if (d <= 0 || v > d || v <= 0 ) { return 0; }
    float amount_products_that_fit_with_decimals = ( d / v);
    int capacity = (int)amount_products_that_fit_with_decimals;
    return capacity;
}

void swap(int *x, int *y) {
    int x0 = *x;
    *x = *y;
    *y = x0;
    return;
}

int array_max(const int *array, int length) {
    if (!array || length <= 0) { return 0; } //dado a que se tiene que devolver un int, el NULL se representa como un 0.
    int max = array[0];
    for (int i = 1; i < length; i++) {
        if (array[i] > max) { max = array[i]; }
    }
    return max;
}

void array_map(int *array, int length, int f(int)) {
    if (!array || length <= 0 || !f) { return; }
        for (int i = 0; i < length ; i++) {
            array[i] = f(array[i]); }
}

int *copy_array(const int *array, int length) {
    if (!array || length <= 0) { return NULL; }
    int *my_copy = (int *)malloc(length * sizeof(int));
    if (!my_copy) {
        free(my_copy);
        return NULL;}
    for (int i = 0; i < length; i++) {
        my_copy[i] = array[i]; }
    return my_copy;
}

int **copy_array_of_arrays(const int **array_of_arrays, const int *array_lenghts, int array_amount){
        if (!array_of_arrays || !array_lenghts || array_amount <= 0 ) { return NULL; }
        int **my_copy_of_arrays = (int **)malloc(array_amount * sizeof(int *));

        if (!my_copy_of_arrays) { return NULL; }
        
        for (int i = 0; i < array_amount; i++) {
            my_copy_of_arrays[i] = copy_array(array_of_arrays[i], array_lenghts[i]);
        }
    
    return my_copy_of_arrays;
}

void free_array_of_arrays(int **array_of_arrays, int *array_lenghts, int array_amount){
    for (int i = 0; i < array_amount; i++) { 
        free(array_of_arrays[i]); }
    free(array_of_arrays);
    free(array_lenghts);
}

void bubble_sort(int *array, int length){
    if (!array || length <= 1) { return; }
    for ( int i = 0; i < length - 1; i++) {
        for (int j = 0; j < length - i - 1; j++) {
            if (array[j] > array[j + 1]) { swap(&array[j], &array[j + 1]); } }
    }
    return;
}

bool array_equal(const int *array1, int length1, const int *array2, int length2){
    if (!array1 && !array2) { return true; }
    if (length1 != length2 || !array1 || !array2 ) { return false; }
    for ( int i = 0; i < length1; i++) 
        if ( array1[i] != array2[i]) { return false; }
    return true;
}

bool integer_anagrams(const int *array1, int length1,
                      const int *array2, int length2){
    if (!array1 || !array2 || length1 != length2) { return false; }
    int *my_copy_array1 = copy_array(array1, length1);
    int *my_copy_array2 = copy_array(array2, length2);
    bubble_sort(my_copy_array1, length1);
    bubble_sort(my_copy_array2, length2);  
    bool they_are_anagrams = array_equal(my_copy_array1, length1, my_copy_array2, length2);
    free(my_copy_array1);
    free(my_copy_array2);
    return they_are_anagrams;
}