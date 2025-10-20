#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include "../include/qpsort.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define MAX_ARRAY_SIZE 20000000  // Максимальный размер массива
#define INITIAL_SIZE 1000        // Начальный размер для динамического массива

size_t N_threads_max = 1; // значение по умолчанию


int main(int argc, char *argv[]) {
    // Начинаем с небольшого массива
    size_t capacity = INITIAL_SIZE;
    size_t array_size = 0;
    int *array = malloc(capacity * sizeof(int));
    if (array == NULL) {
        perror("Failed to allocate memory");
        return EXIT_FAILURE;
    }

    FILE *input = stdin;
    int close_file = 0;

    // Определяем количество потоков из аргументов командной строки
    if (argc > 1) {
        unsigned long narg = 1;
        errno = 0;
        char *endptr;
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
                // Проверяем наличие следующего аргумента
                // Если следующая позиция выходит за пределы argc, выводим ошибку
                if (i + 1 >= argc) {
                    fprintf(stderr, "Error: Missing value for %s\n", argv[i]);
                    free(array);
                    return EXIT_FAILURE;
                }
                // Сбрасываем errno перед вызовом strtoul
                errno = 0;
                // strtoul для преобразования строки в unsigned long
                // &endptr используется для проверки корректности преобразования - хранит в себе адрес первого некорректного символа
                narg = strtoul(argv[i + 1], &endptr, 10);
                // Проверяем ошибки преобразования
                if (errno != 0 || *endptr != '\0') {
                    fprintf(stderr, "Error: Invalid number of threads: %s\n", argv[i + 1]);
                    free(array);
                    return EXIT_FAILURE;
                }
                // Устанавливаем значение N_threads_max
                N_threads_max = narg;
                i++; // пропускаем следующий аргумент, так как он уже обработан
            } else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file") == 0) {
                // Обработка файла
                if (i + 1 >= argc) {
                    fprintf(stderr, "Error: Missing value for %s\n", argv[i]);
                    free(array);
                    return EXIT_FAILURE;
                }
                input = fopen(argv[i + 1], "r");
                if (input == NULL) {
                    perror("Failed to open input file");
                    free(array);
                    return EXIT_FAILURE;
                }
                close_file = 1;
                i++; // пропускаем следующий аргумент
            }
        }
        // Проверяем корректность значения N_threads_max
        if (N_threads_max == 0 || N_threads_max > 128) {
            fprintf(stderr, "Error: Number of threads must be between 1 and 128\n");
            free(array);
            return EXIT_FAILURE;
        }
    } 
    // Если аргументы не заданы, используем значение по умолчанию
    else {
        N_threads_max = 1; // значение по умолчанию
    }

    // Заполняем массив значениями из файла или stdin (читаем до конца файла)
    int value;
    while (fscanf(input, "%d", &value) == 1) {
        // Если массив заполнен, увеличиваем его размер
        if (array_size >= capacity) {
            capacity *= 2;
            if (capacity > MAX_ARRAY_SIZE) {
                fprintf(stderr, "Error: Array size exceeds maximum (%d)\n", MAX_ARRAY_SIZE);
                free(array);
                return EXIT_FAILURE;
            }
            int *temp = realloc(array, capacity * sizeof(int));
            if (temp == NULL) {
                perror("Failed to reallocate memory");
                free(array);
                return EXIT_FAILURE;
            }
            array = temp;
        }
        array[array_size++] = value;
    }

    if (array_size == 0) {
        fprintf(stderr, "Error: No data to sort\n");
        free(array);
        if (close_file) fclose(input);
        return EXIT_FAILURE;
    }

    if (close_file) fclose(input);

    fprintf(stderr, "Прочитано элементов: %zu\n", array_size);
    
    // Сортируем массив с помощью быстрой сортировки
    // Время замеряется внутри qpsort()
    qpsort(array, 0, array_size - 1);

    // Печатаем элементы отсортированного массива в файл
    FILE *output_file = fopen("sorted_output.txt", "w");
    if (output_file == NULL) {
        perror("Failed to open output file");
        free(array);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < array_size; i++) {
        fprintf(output_file, "%d\n", array[i]);
    }

    fclose(output_file);
    free(array);
    return EXIT_SUCCESS;
}