//Authors : A0133662J CHEW YUNG CHUNG, A0125473H TAN QIU HAO, JOEL, A0126332R JULIANA SENG
//Email(respectively) : A0133662@U.NUS.EDU, A0125473@U.NUS.EDU, A0126332@U.NUS.EDU
int* function1();
int* function2();
char* function3();
int* function4();
char *init_array(void);
char *escape_local(void);
int functionReturnNumber();

int main()  {
	int localIntArr[5] = {1,2,3,4,5};
	
	function1();
	function2();
	char* str = function3();
	printf("%s", str);
	
	return 0;
}

int* function1() {
	int* localIntPtr;
	char* localCharPtr;
	int localIntArr[5] = {1,2,3,4,5};
	
	return localIntArr;
}

int* function2() {
	int a[10];
	return a;
}

char* function3() {
	char * function3String = "Hello";
	return function3String;
}

int* function4() {
	return function1();
}

char *init_array(void) {
	char array[10];
	char **p;
	*p = array;
	int nicePants = 5;
	return *p;
}

char *escape_local() {
	char local_char = 'a';
	char *p = &local_char;
	return p;
}

int functionReturnNumber() {
	return 1;
}