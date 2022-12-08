/**
 * @file queue.h
 * @author Jakub Vlk (xvlkja07@stud.fit.vutbr.cz)
 * @brief Queue
 * Implementation IFJ22 compiler
 */
#include "stack.h"
#include <stdlib.h>
#include <stdbool.h>


#ifndef STACK_QUEUE_H
#define STACK_QUEUE_H

typedef genericStack queue_t;

queue_t *queue();

inline void enqueue(queue_t *queue, void *nm);

inline void *dequeue(queue_t *queue);

inline void *peek(queue_t *queue);

inline void *front(queue_t *queue);

inline bool empty(queue_t *queue);

#endif //STACK_QUEUE_H
