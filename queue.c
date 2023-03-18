#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

queue* queue_push(queue* start, int val)
{
    struct queue *new = (struct queue*) malloc(sizeof(struct queue));
    if (new == NULL) {
        errx(-1, "malloc()");
    }
    new->val = val;
    if (start == NULL) {
        new->next = new;
    }
    else {
        new->next = start->next;
        start->next = new;
    }
    return new;
}

queue* queue_pop(queue* start, int *pval)
{
    if (start == NULL) {
	    return NULL;
    }
    queue *old = start->next;
    int is_only_one_elem_in_q = (start == old)? 1: 0;
    *pval = old->val;
    start->next = old->next;
    free(old);
    if (is_only_one_elem_in_q) {
        return NULL;
    }
    else {
        return start;
    }
}

void queue_empty(queue** pstart)
{
    int _val;
    do {
        *pstart = queue_pop(*pstart, &_val);
    } while (*pstart != NULL);
}

