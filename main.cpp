#include <iostream>
#include <pthread.h>
#include <unistd.h>

#define THREADS_COUNT 2
#define I_MUTEX 0
#define I_BARRIER 1

using namespace std;

struct thread_arg {
    int                 num;
    pthread_mutex_t     mutex;
    pthread_cond_t      cond;
    pthread_barrier_t   barrier;
};

static void* thread_start(void *arg)
{
    struct thread_arg *tharg = (struct thread_arg *)arg;

    cout << "in new thread N = " << tharg->num << endl;

    switch (tharg->num) {
    case 0:
        pthread_cond_wait(&tharg->cond, &tharg->mutex);
        break;

    default:
        pthread_barrier_wait(&tharg->barrier);
        break;
    }

    cout << "close thread N = " << tharg->num << endl;

//    pthread_exit((void *)((struct thread_arg *)arg)->num);
    pthread_exit((void *)tharg->num);

}

int main()
{
    pthread_t thread[THREADS_COUNT];
    struct thread_arg arg[THREADS_COUNT];
    int res;
    int *ret;

    arg[I_MUTEX].num = I_MUTEX;

    res = pthread_mutex_init(&arg[I_MUTEX].mutex, NULL);
    if (res != 0) {
        cout << "pthread_mutex_init() is fail for thread " << I_MUTEX << ", res = " << res << endl;
        return 1;
    }

    res = pthread_cond_init(&arg[I_MUTEX].cond, NULL);
    if (res != 0) {
        cout << "pthread_cond_init() is fail for thread " << I_MUTEX << ", res = " << res << endl;
        return 1;
    }

    res = pthread_create(&thread[I_MUTEX], NULL, &thread_start, &(arg[I_MUTEX]));
    if (res != 0) {
        cout << "pthread_create() is fail for thread " << I_MUTEX << ", res = " << res << endl;
        return 1;
    }

    sleep(2);

    arg[I_BARRIER].num = I_BARRIER;

    pthread_barrier_init(&arg[I_BARRIER].barrier, NULL, 2);
    if (res != 0) {
        cout << "pthread_barrier_init() is fail for thread " << I_BARRIER << ", res = " << res << endl;
        return 1;
    }

    res = pthread_create(&thread[I_BARRIER], NULL, &thread_start, &(arg[I_BARRIER]));
    if (res != 0) {
        cout << "pthread_create() is fail for thread " << I_BARRIER << ", res = " << res << endl;
        return 1;
    }

    sleep(5);

    res = pthread_cond_signal(&arg[I_MUTEX].cond);
    if (res != 0) {
        cout << "pthread_cond_signal() is fail for thread " << I_MUTEX << ", res = " << res << endl;
        return 1;
    }

    sleep(2);

    res = pthread_barrier_wait(&arg[I_BARRIER].barrier);
    if (res != 0 && res != PTHREAD_BARRIER_SERIAL_THREAD) {
        cout << "pthread_barrier_wait() is fail for thread " << I_BARRIER << ", res = " << res << endl;
        return 1;
    }

    for (int i = 0; i < THREADS_COUNT; i++) {
        res = pthread_join(thread[i], (void **)&ret);
        if (res != 0) {
            cout << "pthread_join() is fail for thread " << i << ", res = " << res << endl;
            return 1;
        }
    }

    res= pthread_mutex_destroy(&arg[I_MUTEX].mutex);
    if (res != 0) {
        cout << "pthread_mutex_destroy() is fail for thread " << I_MUTEX << ", res = " << res << endl;
        return 1;
    }

    res= pthread_cond_destroy(&arg[I_MUTEX].cond);
    if (res != 0) {
        cout << "pthread_mutex_destroy() is fail for thread " << I_MUTEX << ", res = " << res << endl;
        return 1;
    }

    res= pthread_barrier_destroy(&arg[I_BARRIER].barrier);
    if (res != 0) {
        cout << "pthread_mutex_destroy() is fail for thread " << I_BARRIER << ", res = " << res << endl;
        return 1;
    }

    return 0;
}
