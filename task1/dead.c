//Authors : A0133662J CHEW YUNG CHUNG, A0125473H TAN QIU HAO, JOEL, A0126332R JULIANA SENG
//Email(respectively) : A0133662@U.NUS.EDU, A0125473@U.NUS.EDU, A0126332@U.NUS.EDU
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int complex_function();
void maybe_dead_function();
void function_always_called();
void dead_function();
int dead2_complex_function();
void dead2_maybe_dead_function();
void dead2_function_always_called();
void dead2_dead_function();

void (*f)(void);

int main(void) {
    if (complex_function()) {
        maybe_dead_function();
    }
    function_always_called();
    if (0) {
        dead_function();
    }

    if (dead2_complex_function()) {
        dead2_maybe_dead_function();
    }
    dead2_function_always_called();
    
    if (0) {
        dead2_dead_function();
    }
    
    f = &dead_function;
    f();
}

int complex_function() {
    printf("complex_function\n");
    time_t t;
    srand((unsigned) time(&t));
    return (rand() % 2);

}

void maybe_dead_function() {
    printf("maybe_dead_function\n");
}

void function_always_called() {
    printf("function_always_called\n");
}


void dead_function() {
    printf("dead_function\n");
}
