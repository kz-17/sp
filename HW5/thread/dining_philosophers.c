#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define NUM_MEALS 5

typedef enum { THINKING, HUNGRY, EATING } PhilosopherState;

PhilosopherState state[NUM_PHILOSOPHERS];
pthread_mutex_t mutex;
pthread_cond_t cond[NUM_PHILOSOPHERS];
int philosopher_meals[NUM_PHILOSOPHERS];

int left(int i) {
    return (i + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS;
}

int right(int i) {
    return (i + 1) % NUM_PHILOSOPHERS;
}

void test(int i) {
    if (state[i] == HUNGRY &&
        state[left(i)] != EATING &&
        state[right(i)] != EATING) {

        state[i] = EATING;
        pthread_cond_signal(&cond[i]);
    }
}

void pickup(int i) {
    pthread_mutex_lock(&mutex);

    state[i] = HUNGRY;
    printf("Philosopher %d is HUNGRY\n", i);
    test(i);

    while (state[i] != EATING) {
        pthread_cond_wait(&cond[i], &mutex);
    }

    printf("Philosopher %d starts EATING (meal %d)\n",
           i, philosopher_meals[i] + 1);

    pthread_mutex_unlock(&mutex);
}

void putdown(int i) {
    pthread_mutex_lock(&mutex);

    philosopher_meals[i]++;
    printf("Philosopher %d finishes EATING (total: %d meals)\n",
           i, philosopher_meals[i]);

    state[i] = THINKING;
    test(left(i));
    test(right(i));

    pthread_mutex_unlock(&mutex);
}

void think(int i) {
    printf("Philosopher %d is THINKING\n", i);
    usleep((rand() % 1000) * 1000);
}

void eat(int i) {
    usleep((rand() % 500) * 1000);
}

void *philosopher(void *arg) {
    int id = *(int *)arg;

    for (int i = 0; i < NUM_MEALS; i++) {
        think(id);
        pickup(id);
        eat(id);
        putdown(id);
    }

    printf("Philosopher %d is FULL and leaves the table\n", id);
    return NULL;
}

int main() {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        state[i] = THINKING;
        philosopher_meals[i] = 0;
        pthread_cond_init(&cond[i], NULL);
    }

    printf("=== Dining Philosophers Problem ===\n");
    printf("%d philosophers, %d meals each\n\n", NUM_PHILOSOPHERS, NUM_MEALS);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }

    printf("\nAll philosophers have finished eating!\n");

    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_cond_destroy(&cond[i]);
    }

    return 0;
}
