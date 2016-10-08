#include <stdio.h>

void test1();
void test2();
void test3();
void test4();

int main()
{
	int x = 5;
    if (!(x == 0 || 0 == 0)) {
		test1();
	}

	if (!(x == x)) {
		test2();
	}

	if (!(5 == 5)) {
		test3();
	}

	for (int y = 0; y < 0; y++) {
		test4();
	}

    return 0;
}

void test1()
{
    printf("test1\n");
}

void test2()
{
    printf("test2\n");
}

void test3()
{
    printf("test3\n");
}

void test4()
{
    printf("test4\n");
}
