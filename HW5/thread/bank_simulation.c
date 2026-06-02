#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define OPERATIONS 100000

typedef struct {
    int balance;
    pthread_mutex_t lock;
} BankAccount;

void init_account(BankAccount *acc) {
    acc->balance = 1000000;
    pthread_mutex_init(&acc->lock, NULL);
}

void deposit(BankAccount *acc, int amount) {
    pthread_mutex_lock(&acc->lock);
    acc->balance += amount;
    pthread_mutex_unlock(&acc->lock);
}

void withdraw(BankAccount *acc, int amount) {
    pthread_mutex_lock(&acc->lock);
    acc->balance -= amount;
    pthread_mutex_unlock(&acc->lock);
}

typedef struct {
    BankAccount *acc;
    int id;
} ThreadArg;

void *thread_func(void *arg) {
    ThreadArg *ta = (ThreadArg *)arg;
    BankAccount *acc = ta->acc;
    int id = ta->id;

    for (int i = 0; i < OPERATIONS; i++) {
        deposit(acc, 100);
        withdraw(acc, 100);
    }

    printf("Thread %d done\n", id);
    return NULL;
}

int main() {
    BankAccount acc;
    pthread_t t1, t2;
    ThreadArg arg1, arg2;

    init_account(&acc);

    arg1.acc = &acc;
    arg1.id = 1;
    arg2.acc = &acc;
    arg2.id = 2;

    printf("Initial balance: %d\n", acc.balance);
    printf("Starting %d deposit+withdraw per thread...\n", OPERATIONS);

    pthread_create(&t1, NULL, thread_func, &arg1);
    pthread_create(&t2, NULL, thread_func, &arg2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final balance: %d (expected: %d)\n", acc.balance, 1000000);
    printf("Difference: %d\n", acc.balance - 1000000);

    if (acc.balance == 1000000) {
        printf("PASS: Balance is correct!\n");
    } else {
        printf("FAIL: Balance is WRONG due to race condition!\n");
    }

    pthread_mutex_destroy(&acc.lock);
    return 0;
}
