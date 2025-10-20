#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/qpsort.h"

size_t N_threads_max = 16; // значение по умолчанию

int main() {
    int *array = malloc(1000000 * sizeof(int));
    if (array == NULL) {
        perror("Failed to allocate memory");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 1000000; i++) {
        array[i] = rand() % 1000000; // Заполняем массив случайными числами
    }

    qpsort(array, 0, 999999);

    free(array);
    return 0;
}