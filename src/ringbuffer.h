#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "util.h"

#define RING_BUFFER_SIZE 16
#define RING_BUFFER_MASK (RING_BUFFER_SIZE-1)

typedef struct {
    threads_pending read_index;
    threads_pending write_index;
    void *buf[RING_BUFFER_SIZE];
} ringbuffer;

ringbuffer* ringbufferNew();
int ringbufferOffer(ringbuffer* rb, void *o);
void* ringbufferTake(ringbuffer* rb);

#endif /* RINGBUFFER_H */