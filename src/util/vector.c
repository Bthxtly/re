/*
 * create new, push, free functions for TYPE
 * usage:
 *   #define TYPE int
 *   #include <this_file>
 */

#ifndef TYPE
#error "define TYPE first"
#else

#define CAT1(a, b) a##b
#define CAT(a, b) CAT1(a, b)
#define APPEND_TYPE(a) CAT(CAT(a, _), TYPE)
#define TYPE_NAME APPEND_TYPE(Vector)

#include <stddef.h>
#include <stdlib.h>

typedef struct TYPE_NAME {
  TYPE *data;
  size_t size;
  size_t capacity;
} TYPE_NAME;

/* create a new vector */
TYPE_NAME *APPEND_TYPE(new_vector)() {
  TYPE_NAME *vec = (TYPE_NAME *)malloc(sizeof(TYPE_NAME));
  if (!vec)
    return NULL;
  vec->size = 0;
  vec->capacity = 4;
  vec->data = (TYPE *)malloc(vec->capacity * sizeof(TYPE));
  if (!vec->data) {
    free(vec);
    return NULL;
  }
  return vec;
}

/* push a value to the vector */
int APPEND_TYPE(push_vector)(TYPE_NAME *vec, TYPE value) {
  if (vec->size >= vec->capacity) {
    size_t new_capacity = vec->capacity * 2;
    TYPE *new_data = (TYPE *)realloc(vec->data, new_capacity * sizeof(TYPE));
    if (!new_data)
      return -1;
    vec->data = new_data;
    vec->capacity = new_capacity;
  }
  vec->data[vec->size++] = value;
  return 0;
}

/* free the vector */
void APPEND_TYPE(free_vector)(TYPE_NAME *vec) {
  if (vec) {
    free(vec->data);
    free(vec);
  }
}

#undef TYPE
#endif /* ifndef TYPE */
