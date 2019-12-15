
#include <stdio.h>
#include "uthread.h"
/*
void thread_fn(int th_num) {
    while(true) {
        int x = 100;
        fprintf(stderr, "enter a number: ");
        fflush(stderr);
        fscanf(stdin, "%d", &x);
        printf("thread %d: %d\n", th_num, x);

        if (x == 100) {
            uthread_exit();
        }

        uthread_yield();
    }
}

int main() {
    uthread_init();

    struct uthread th1, th2;
    uthread_create(&th1, thread_fn, 1);
    uthread_create(&th2, thread_fn, 2);
    uthread_join(&th1);
    uthread_join(&th2);
}
*/

void ab(int ab) {
    for (int i=0; i<1000; i++) {
        printf("%c", ab);
        uthread_yield();
    }
    printf("\n");
    uthread_exit();
}

int main() {
    uthread_init();

    struct uthread a, b;

    printf("making a\n");
    uthread_create(&a, ab, 'a');
    printf("making b\n");
    uthread_create(&b, ab, 'b');
    printf("make b\n");

    printf("joining on a\n");
    uthread_join(&a);
    printf("joining on b\n");
    uthread_join(&b);

    printf("done, goodbye!\n");
}

