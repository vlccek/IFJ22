//
// Created by jvlk on 18.9.22.
//
#include "queue.h"

typedef genericStack queue_t;

queue_t *queue() {
    return gStackInit();
}

inline void enqueue(queue_t *queue, void *nm) {
    gStackPush(queue, nm);
}

inline void *dequeue(queue_t *queue) {
    return gStackPopBack(queue);
};

inline void *peek(queue_t *queue) {
    return &gStackBottom(queue, NULL)->data;
}

inline void *front(queue_t *queue) {
    return &queue->top->data;
}

inline bool empty(queue_t *queue) {
    return gStackIsEmpty(queue);
}
