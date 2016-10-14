//Authors : A0133662J CHEW YUNG CHUNG, A0125473H TAN QIU HAO, JOEL, A0126332R JULIANA SENG
//Email(respectively) : A0133662@U.NUS.EDU, A0125473@U.NUS.EDU, A0126332@U.NUS.EDU
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
int dead2_complex_function();
void dead2_maybe_dead_function();
void dead2_function_always_called();
void dead2_dead_function();
*/

void dead2_dead_function() {
	printf("dead2_dead_function\n");
}

int dead2_complex_function() {
	printf("dead2_complex_function\n");
	time_t t;
	srand((unsigned) time(&t));

	dead2_dead_function();
	
	return (rand() % 2);

}

void dead2_maybe_dead_function() {
	printf("dead2_maybe_dead_function\n");
	dead2_complex_function();
}

void dead2_function_always_called() {
	printf("dead2_function_always_called\n");
	dead2_maybe_dead_function();
}

