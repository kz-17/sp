#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 20

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    int count;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
} Queue;

void init_queue(Queue *q) {
    q->in = 0;
    q->out = 0;
    q->count = 0;
    sem_init(&q->empty, 0, BUFFER_SIZE);
    sem_init(&q->full, 0, 0);
    pthread_mutex_init(&q->mutex, NULL);
}

void produce(Queue *q, int item) {
    sem_wait(&q->empty);
    pthread_mutex_lock(&q->mutex);

    q->buffer[q->in] = item;
    q->in = (q->in + 1) % BUFFER_SIZE;
    q->count++;
    printf("Produced: %d  (buffer: %d/%d)\n", item, q->count, BUFFER_SIZE);

    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->full);
}

int consume(Queue *q) {
    int item;

    sem_wait(&q->full);
    pthread_mutex_lock(&q->mutex);

    item = q->buffer[q->out];
    q->out = (q->out + 1) % BUFFER_SIZE;
    q->count--;
    printf("Consumed: %d (buffer: %d/%d)\n", item, q->count, BUFFER_SIZE);

    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->empty);

    return item;
}

typedef struct {
    Queue *q;
    int id;
} ProducerArg;

void *producer(void *arg) {
    ProducerArg *pa = (ProducerArg *)arg;
    Queue *q = pa->q;
    int id = pa->id;

    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = i + id * 1000;
        produce(q, item);
        usleep((rand() % 500) * 1000);
    }

    printf("Producer %d finished\n", id);
    return NULL;
}

void *consumer(void *arg) {
    ProducerArg *ca = (ProducerArg *)arg;
    Queue *q = ca->q;
    int id = ca->id;

    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = consume(q);
        usleep((rand() % 800) * 1000);
    }

    printf("Consumer %d finished\n", id);
    return NULL;
}

int main() {
    Queue q;
    pthread_t producers[2], consumers[2];
    ProducerArg p_args[2], c_args[2];

    srand(time(NULL));
    init_queue(&q);

    printf("=== Producer-Consumer Problem ===\n");
    printf("Buffer size: %d, Items per thread: %d\n\n", BUFFER_SIZE, NUM_ITEMS);

    for (int i = 0; i < 2; i++) {
        p_args[i].q = &q;
        p_args[i].id = i + 1;
        pthread_create(&producers[i], NULL, producer, &p_args[i]);
    }

    for (int i = 0; i < 2; i++) {
        c_args[i].q = &q;
        c_args[i].id = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &c_args[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\nAll done! Total produced: %d, Total consumed: %d\n",
           2 * NUM_ITEMS, 2 * NUM_ITEMS);

    sem_destroy(&q.empty);
    sem_destroy(&q.full);
    pthread_mutex_destroy(&q.mutex);

    return 0;
}
