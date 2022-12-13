#include "ringbuffer.h"
#include "zmalloc.h"

ringbuffer* ringbufferNew() {
    return (ringbuffer*)zcalloc(sizeof(ringbuffer));
}

/* Offer an object to ringbuffer.
 * If ringbuffer is full 0 is returned, otherwise 1. */
int ringbufferOffer(ringbuffer* rb, void *o) {
    while (1) {
        unsigned long head;
        atomicGetWithSync(rb->read_index.value, head);
        unsigned long tail;
        atomicGetWithSync(rb->write_index.value, tail);
        ssize_t size = (tail - head);
        if (size >= RING_BUFFER_SIZE) {
            return 0;
        }
        if (atomicCompareExchangeWeak(rb->write_index.value, tail, tail + 1)) {
            size_t index = (size_t) (tail & RING_BUFFER_MASK);
            atomicSetWithSync(rb->buf[index], o);
            return 1;
        }
    }
}

/* Take an object from ringbuffer.
 * If ringbuffer is empty NULL is returned, otherwise the object. */
void* ringbufferTake(ringbuffer* rb) {
    while (1) {
        unsigned long head;
        atomicGetWithSync(rb->read_index.value, head);
        unsigned long tail;
        atomicGetWithSync(rb->write_index.value, tail);
        ssize_t size = (tail - head);
        if (size == 0) {
            return NULL;
        }
        size_t index = (size_t) (head & RING_BUFFER_MASK);
        void *o;
        atomicGetWithSync(rb->buf[index], o);
        if (!o) {
            // not published yet
            return NULL;
        }
        if (atomicCompareExchangeWeak(rb->read_index.value, head, head + 1)) {
            atomicSetWithSync(rb->buf[index], NULL);
            return o;
        }
    }
}