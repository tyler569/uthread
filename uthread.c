
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <setjmp.h>

enum uthread_state {
    UTHREAD_RUNNING = 1,
    UTHREAD_DONE,
};

struct uthread {
    enum uthread_state uthread_state;
    jmp_buf execution_state;
};

void uthread_exit();
void uthread_yield();

void thread_fn(int th_num) {
    while(true) {
        int x = 100;
        /*
        fprintf(stderr, "enter a number: ");
        fflush(stderr);
        fscanf(stdin, "%d", &x);
        */
        printf("thread %d: %d\n", th_num, x);

        if (x == 100) {
            uthread_exit();
        }

        uthread_yield();
    }
}

struct uthread *active_threads[8] = {0};
int active_threads_top = 0;

struct uthread *running_uthread = NULL;

int uthread_create(struct uthread *thread, void (*func)(int), int arg) {
    thread->uthread_state = UTHREAD_RUNNING;
    active_threads[active_threads_top++] = thread;
    running_uthread = thread;
    func(arg);
    return 0;
}

int uthread_init(struct uthread *thread_one) {
    active_threads[active_threads_top++] = thread_one;
    thread_one->uthread_state = UTHREAD_RUNNING;
    running_uthread = thread_one;
    setjmp(thread_one->execution_state);
}

struct uthread *uthread_sched() {
    static int pos = 4;

    struct uthread *candidate;
    while (true) {
        pos += 1;
        if (pos > 7) {
            pos = 0;
        }
        candidate = active_threads[pos];
        if (candidate && candidate->uthread_state == UTHREAD_RUNNING) {
            break;
        }
    }
    return candidate;
}

struct uthread *volatile next = 0;

void uthread_yield() {
    if (setjmp(running_uthread->execution_state)) {
        running_uthread = next;
        return;
    }
    struct uthread *next_uthread = uthread_sched();
    if (next_uthread == running_uthread) {
        return;
    }
    next = next_uthread;
    longjmp(next_uthread->execution_state, 1);
}

void uthread_exit() {
    running_uthread->uthread_state = UTHREAD_DONE;
    uthread_yield();
}

void uthread_join(struct uthread *th) {
    while (th->uthread_state == UTHREAD_RUNNING) {
        uthread_yield();
    }
}

struct uthread thread_one = {0};

int main() {
    uthread_init(&thread_one);

    struct uthread th1, th2;
    uthread_create(&th1, thread_fn, 1);
    uthread_create(&th2, thread_fn, 2);
    uthread_join(&th1);
    uthread_join(&th2);
}

