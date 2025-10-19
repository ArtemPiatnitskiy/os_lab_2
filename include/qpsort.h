#ifndef QPSORT_H
#define QPSORT_H

#include <stddef.h>

// Видимость переменной N_threads_max из main.c
extern size_t N_threads_max;

typedef struct thread_data_t {
    int *array;
    int low;
    int high;
} thread_data_t;


void qpsort(int *array, int low, int high);

#endif // QPSORT_H