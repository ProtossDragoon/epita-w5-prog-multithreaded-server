#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "shared_queue.h"

// Number of threads.
const size_t THREAD_COUNT = 8;

// Number of iterations for a thread.
const size_t THREAD_ITER = 3;

// Mutex used to lock the display for the push and pop operations.
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Data structure passed to the threads.
struct thread_data
{
    // Thread ID.
    unsigned id;

    // Pointer to the shared queue.
    shared_queue* shared_queue;
};

// Function executed by the threads.
void* worker(void* arg)
{
    // Gets the structure passed as an argument.
    struct thread_data* data = arg;

    // Gets the shared queue.
    shared_queue* sq = data->shared_queue;

    // Gets the thread ID.
    unsigned id = data->id;

    for (size_t i = 0; i < THREAD_ITER; i++)
    {
        // Gets a random number between 0 and 99.
        int x = random() % 100;

        // Pushes a value onto the queue.
        // Uses a mutex for the display.
        pthread_mutex_lock(&mutex);
        printf("%u: Pushes %d\n", id, x);
        shared_queue_push(sq, x);
        pthread_mutex_unlock(&mutex);

        // Sleeps for a few microseconds.
        // (Uses the random value.)
        usleep(x);

        // Pops the value off the queue.
        // Uses a mutex for the display.
        pthread_mutex_lock(&mutex);
        x = shared_queue_pop(sq);
        printf("%u: Pops %d\n", id, x);
        pthread_mutex_unlock(&mutex);
    }

    // Terminates the thread.
    pthread_exit(NULL);
}

int main()
{
    // Creates an array to store the thread system IDs.
    pthread_t sys_ids[THREAD_COUNT];

    // Creates an array of thread_data structures
    // (one structure per thread).
    struct thread_data data[THREAD_COUNT];

    // Creates a new shared queue.
    shared_queue* sq = shared_queue_new();

    // Creates all threads.
    for (size_t i = 0; i < THREAD_COUNT; i++)
    {
        // Initializes the data structure passed to the thread.
        data[i].id = i;
        data[i].shared_queue = sq;

        // Creates the thread.
        int e = pthread_create(sys_ids + i, NULL, worker, data + i);
        if (e != 0)
        {
            errno = e;
            err(EXIT_FAILURE, "Could not create the thread %zu.", i);
        }
    }

    // Waits for the threads.
    for (size_t i = 0; i < THREAD_COUNT; i++)
    {
        int e = pthread_join(sys_ids[i], NULL);
        if (e != 0)
            errx(EXIT_FAILURE, "Could not join the thread %zu.", i);
    }

    // Destroys the mutex and the shared queue.
    pthread_mutex_destroy(&mutex);
    shared_queue_destroy(sq);

    // Terminates the process.
    return EXIT_SUCCESS;
}
