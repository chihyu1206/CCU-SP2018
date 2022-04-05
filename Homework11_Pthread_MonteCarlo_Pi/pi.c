#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#define N 16
// define global variables
int nthread;
long total_loop_cnt;
double global_hit = 0.0;
pthread_mutex_t mutex;
int idx = 0;


void thread(void *rand_buffer) {
    double px = 0.0, py = 0.0, rand_d = 0.0;
    long local_loop_cnt = total_loop_cnt / nthread;
    long local_hit = 0;

    for (long cnt = 0; cnt < local_loop_cnt; ++cnt) {
        drand48_r(rand_buffer, &rand_d);
        px = rand_d;
        drand48_r(rand_buffer, &rand_d);
        py = rand_d;
        if ((px * px + py * py) <= 1.0)
            local_hit++;
    }
    pthread_mutex_lock(&mutex);
    global_hit += local_hit;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
    pthread_t id[N];
    struct drand48_data *rand_buffer[N];
    double pi = 0.0, rand_d = 0.0;

    total_loop_cnt = atol(argv[1]);
    nthread = atoi(argv[2]);
    assert(nthread <= N);

    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < nthread; ++i) {
        rand_buffer[i] = aligned_alloc(64, sizeof(struct drand48_data));
        srand48_r(rand(), rand_buffer[i]);
        drand48_r(rand_buffer[i], &rand_d);
        pthread_create(&id[i], NULL, (void *)thread, rand_buffer[i]);
    }

    for (int i = 0; i < nthread; ++i)
        pthread_join(id[i], NULL);
    
    pi = (double)(global_hit / total_loop_cnt) * 4.0;
    printf("PI = %.8lf\n", pi);
    return 0;
}
