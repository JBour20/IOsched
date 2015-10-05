/*
 * Jeffrey Bour
 * queue.h defines the queue functions
 * implemented in queue.c
 */

#ifndef _QUEUE_H_INCLUDE_
#define _QUEUE_H_INCLUDE_

#include "operation.h"

typedef struct queue_obj* queue;

queue new_queue();

void free_queue(queue *pq);

int is_empty(queue q);

int size(queue q);

void move_to_front(queue q, operation o);

void enqueue(queue q, operation o);

void id_insert(queue q, operation o);

operation dequeue(queue q);

operation fifo(queue q);

operation sstf(queue q);

operation scan(queue q);

operation cscan(queue q);

operation fscan(queue q);

operation peek(queue q);

void make_empty(queue q);

void print_summary(queue q);

#endif