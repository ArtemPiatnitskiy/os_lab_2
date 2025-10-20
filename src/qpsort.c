
#include "../include/qpsort.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>


// Минимальный размер подмассива для параллелизации
// Если подмассив меньше этого значения, не создаём новый поток
#define PARALLEL_THRESHOLD 10000

pthread_mutex_t thread_count_mutex = PTHREAD_MUTEX_INITIALIZER;
size_t active_threads = 0;
// Переменная для отслеживания максимального количества одновременных потоков
size_t max_threads_reached = 0;



void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp; 
}


int partition(int *array, int low, int hight) {
    int pivot = array[hight];
    int i = low - 1;
    
    for (int j = low; j < hight; j++) {  
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }

    swap(&array[i + 1], &array[hight]);
    return i + 1;
}


void *quicksort_recursive(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    // Базовый случай - если подмассив пустой или из одного элемента
    if (data->low >= data->high) {
        free(data);
        return NULL;
    }

    // Разделение массива
    int pi = partition(data->array, data->low, data->high);

    // Вычисляем размер подмассива
    int size = data->high - data->low + 1;
    
    // Создание данных для левого и правого подмассивов
    // Проверяем можно ли создать новый поток
    // Условия: 1) есть свободные потоки, 2) подмассив достаточно большой
    pthread_mutex_lock(&thread_count_mutex); // блокируем мьютекс перед проверкой и изменением active_threads
    // Переменная (вернёт 0 или 1) для проверки возможности создания потока
    int can_create_thread = (active_threads < N_threads_max) && (size >= PARALLEL_THRESHOLD);
    if (can_create_thread) {
        active_threads++;
        // Обновляем максимальное количество одновременных потоков
        if (active_threads > max_threads_reached) {
            max_threads_reached = active_threads;
        }
    }
    pthread_mutex_unlock(&thread_count_mutex); // разблокируем мьютекс

    if (can_create_thread) {
        // Создаем поток для левого подмассива
        // Создаем структуру данных для левого подмассива (Динамическое выделение памяти для данных в новом потоке)
        thread_data_t *left_data = malloc(sizeof(thread_data_t));
        // Инициализируем структуру данных для левого подмассива
        left_data->array = data->array;
        left_data->low = data->low;
        left_data->high = pi - 1;

        pthread_t thread;
        pthread_create(&thread, NULL, quicksort_recursive, left_data);

        // Правую часть обрабатываем сами
        data->low = pi + 1;
        quicksort_recursive(data);


        // Ожидаем завершения потока
        pthread_join(thread, NULL);

        // Уменьшаем счетчик активных потоков
        pthread_mutex_lock(&thread_count_mutex);
        active_threads--;
        pthread_mutex_unlock(&thread_count_mutex);

        return NULL;
    }
    else {
        // Если создание потока невозможно, сортируем оба подмассива в текущем потоке
        // Левый подмассив
        thread_data_t *left_data = malloc(sizeof(thread_data_t));
        left_data->array = data->array;
        left_data->low = data->low;
        left_data->high = pi - 1;

        quicksort_recursive(left_data);

        // Правый подмассив
        data->low = pi + 1;
        quicksort_recursive(data);

        return NULL;
    }
}



void qpsort(int *array, int low, int hight) {
    // Засекаем время начала сортировки
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Создаем структуру данных для передачи в рекурсивную функцию
    thread_data_t *data = malloc(sizeof(thread_data_t));
    // Инициализируем структуру данных
    data->array = array;
    data->low = low;
    data->high = hight;

    // Запускаем рекурсивную функцию быстрой сортировки
    quicksort_recursive(data);
    
    // Засекаем время окончания сортировки
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // Вычисляем затраченное время
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    // Выводим статистику
    fprintf(stderr, "\n===Максимальное количество одновременных потоков: %zu ===\n", max_threads_reached);
    fprintf(stderr, "===Время сортировки: %.3f секунд ===\n", elapsed);
}