
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <setjmp.h>
#include <assert.h>
#include "uthread.h"

#define DEBUG 0

struct uthread *active_threads[8] = {0};
int active_threads_top = 0;

struct uthread *running_uthread = NULL;

#define STACK_SIZE (1024 * 1024)

int uthread_create(struct uthread *thread, void (*func)(int), int arg) {
    thread->uthread_state = UTHREAD_RUNNING;
    thread->stack = malloc(STACK_SIZE);
    thread->tid = active_threads_top;
    active_threads[active_threads_top++] = thread;

    struct uthread *return_to = running_uthread;

    if (setjmp(running_uthread->execution_state)) {
        running_uthread = return_to;
        return 0;
    }
    running_uthread = thread;
    asm volatile ("mov %0, %%rsp\n\t" :: "g"(thread->stack + STACK_SIZE));
    func(arg);
    assert(0);
}

struct uthread thread_one = {0};

int uthread_init() {
    active_threads[active_threads_top++] = &thread_one;
    running_uthread = &thread_one;
    thread_one.uthread_state = UTHREAD_RUNNING;
    setjmp(thread_one.execution_state);
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
    if (DEBUG) printf("yield %d\n", running_uthread->tid);
    if (setjmp(running_uthread->execution_state)) {
        if (DEBUG) printf("enter %d\n", running_uthread->tid);
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
    if (DEBUG) printf("exit %d\n", running_uthread->tid);
    running_uthread->uthread_state = UTHREAD_DONE;
    uthread_yield();
}

void uthread_join(struct uthread *th) {
    if (DEBUG) printf("[%d]: join %d\n", running_uthread->tid, th->tid);
    while (th->uthread_state == UTHREAD_RUNNING) {
        uthread_yield();
    }
}

