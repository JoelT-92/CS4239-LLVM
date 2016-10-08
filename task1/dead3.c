#include <stdlib.h>

void function1();
void function2();
int function3(int);
char* function4();
void function5();
void declareOnly1();
int declareOnly2();

void (*globalFuncPtrVoid) (void);
int (*globalFuncPtrInt) (int);
char* (*globalFuncPtrChar) (void);
void (*globalNonsensePtr) (void);

typedef struct {
  char *(*p)();
  int n;
} apple;

int main() {
  int mainInt1 = 1;
  int mainInt2 = 2;  
  
  for (int i=0; i < 5; ++i) {
    int a = i;
  }
  
  apple b;
  b.p = &function4;
  b.p();
  
  while (1) {
    function1();
    break;
  }
  
  function2();
  
  if (mainInt1 == 1) {
    int input = 1;
    function3(input);
  }
  
  void (*localFuncPtrVoid)(void);
  int (*localFuncPtrInt)(int) = &function3;
  void (*localFuncPtrNonsense)(void);
  
  localFuncPtrNonsense = &function1;
  localFuncPtrVoid = localFuncPtrNonsense;
  localFuncPtrNonsense = (void*)0x12345;
  
  localFuncPtrVoid();
  localFuncPtrInt(9999);
  localFuncPtrNonsense();
  
  globalFuncPtrVoid = &function5;
  globalNonsensePtr = (void*)0xfffff;
  
  globalFuncPtrVoid();
  globalFuncPtrInt(9999);
  globalNonsensePtr();
  
  apple *x = malloc(sizeof(apple));
  x->p = &function4;
  x->p();
  
  return 0;
}

void randomUndeclared();

void function1() {
  int (*localFuncPtrVoid)(int) = &function3;
  
  localFuncPtrVoid(123);
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
