/* libarray.h
   routines in libarray.a
*/

extern void *array_init(int array_size, size_t array_element_size);

/* extend an array's length */
extern void *array_extend(void *array, int array_size, int array_size_inc, size_t array_element_size);
