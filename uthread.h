
#include <stdbool.h>
#include <setjmp.h>

enum uthread_state {
    UTHREAD_RUNNING = 1,
    UTHREAD_DONE,
};

struct uthread {
    volatile enum uthread_state uthread_state;
    void *stack;
    int tid;
    jmp_buf execution_state;
};

extern struct uthread *running_uthread;

#define STACK_SIZE (1024 * 1024)

int uthread_create(struct uthread *thread, void (*func)(int), int arg);
void uthread_init(void);
struct uthread *uthread_sched(void);
void uthread_yield(void);
void uthread_exit(void);
void uthread_join(struct uthread *th);

