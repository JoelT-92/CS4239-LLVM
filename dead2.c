#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int dead2_complex_function();
void dead2_maybe_dead_function();
void dead2_function_always_called();
void dead2_dead_function();

int dead2_complex_function() {
	printf("dead2_complex_function\n");
	time_t t;
	srand((unsigned) time(&t));
	return (rand() % 2);

}

void dead2_maybe_dead_function() {
	printf("dead2_maybe_dead_function\n");
}

void dead2_function_always_called() {
	printf("dead2_function_always_called\n");
}


void dead2_dead_function() {
	printf("dead2_dead_function\n");
}
