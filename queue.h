//
// Created by jvlk on 20.9.22.
//
#include "stack.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>


#ifndef STACK_QUEUE_H
#define STACK_QUEUE_H

typedef stack_type queue_t;

queue_t *queue();

inline void enqueue(queue_t *queue, void *nm);

inline void *dequeue(queue_t *queue);

inline void *peek(queue_t *queue);

inline void *front(queue_t *queue);

inline bool empty(queue_t *queue);

#endif //STACK_QUEUE_H
