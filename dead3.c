void function1();
void function2();
int function3(int);
char* function4();
void function5();
void declareOnly1();
int declareOnly2();


int main() {
  int mainInt1 = 1;
  int mainInt2 = 2;  
  
  for (int i=0; i < 5; ++i) {
	  int a = i;
  }
  
  while (1) {
	  function1();
	  break;
  }
  
  function2();
  
  if (mainInt1 == 1) {
	  int input = 1;
	  function3(input);
  }
  return 0;
}

void randomUndeclared();

void function1() {
	int inFunc1 = 1;
	function2();
}

void function2() {
	function1();
	function5();
}

int function3(int input) {
	if (input == 1) function1();
	else function2();
	
	return -1;
}

char* function4() {
	function1();
	char* inFunc4 = "hello";
	return inFunc4;
}

void function5() {
	return;
}