#include <err.h>
#include <stdlib.h>
#include "shared_queue.h"

shared_queue *shared_queue_new()
{
    struct shared_queue *sq = (struct shared_queue *)malloc(sizeof(shared_queue));
    if (sq == NULL) {
        errx(-1, "malloc()");
    }
    if (sem_init(&(sq->lock), 0, 1) == -1) {
        errx(EXIT_FAILURE, "sem_init(lock)");
    }
    if (sem_init(&(sq->size), 0, 0) == -1) {
        errx(EXIT_FAILURE, "sem_init(size)");
    }
    sq->queue = NULL;
    return sq;
}

void shared_queue_push(shared_queue *sq, int val)
{
    if (sem_wait(&(sq->lock)) == -1) errx(EXIT_FAILURE, "sem_wait(lock)");
    sq->queue = queue_push(sq->queue, val);
    if (sem_post(&(sq->size)) == -1) errx(EXIT_FAILURE, "sem_post(size)");
    if (sem_post(&(sq->lock)) == -1) errx(EXIT_FAILURE, "sem_post(lock)");
}

int shared_queue_pop(shared_queue *sq)
{
    if (sem_wait(&(sq->size)) == -1) errx(EXIT_FAILURE, "sem_wait(size)");
    if (sem_wait(&(sq->lock)) == -1) errx(EXIT_FAILURE, "sem_wait(lock)");
    int val; sq->queue = queue_pop(sq->queue, &val);
    if (sem_post(&(sq->lock)) == -1) errx(EXIT_FAILURE, "sem_post(lock)");
    return val;
}

void shared_queue_destroy(shared_queue *sq)
{
    if (sem_destroy(&(sq->lock)) == -1) {
        errx(EXIT_FAILURE, "sem_destroy(lock)");
    }
    if (sem_destroy(&(sq->size)) == -1) {
        errx(EXIT_FAILURE, "sem_destroy(size)");
    }
    queue_empty(&(sq->queue));
    free(sq);
}