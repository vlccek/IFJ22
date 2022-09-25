//
// Created by jvlk on 18.9.22.
//
#include "queue.h"

typedef stack_type queue_t;

queue_t *queue() {
    return stackInit();
}

inline void enqueue(queue_t *queue, void *nm) {
    push(queue, nm);
}

inline void *dequeue(queue_t *queue) {
    return popBack(queue);
};

inline void *peek(queue_t *queue) {
    return &stackBottom(queue, NULL)->data;
}

inline void *front(queue_t *queue) {
    return &queue->top->data;
}

inline bool empty(queue_t *queue) {
    return sIsEmpty(queue);
}
