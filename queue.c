//
// Created by jvlk on 18.9.22.
//
#include "queue.h"

typedef stack_type queue_t;

queue_t *queue() {
    return stack();
}

inline void enqueue(queue_t *queue, typeOfStoredData nm) {
    push(queue, nm);
}

inline typeOfStoredData dequeue(queue_t *queue) {
    return popBack(queue);
};

inline typeOfStoredData *peek(queue_t *queue) {
    return &stackBottom(queue, NULL)->data;
}

inline typeOfStoredData *front(queue_t *queue) {
    return &queue->top->data;
}

inline bool empty(queue_t *queue) {
    return sIsEmpty(queue);
}
