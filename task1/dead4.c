//Authors : A0133662J CHEW YUNG CHUNG, A0125473H TAN QIU HAO, JOEL, A0126332R JULIANA SENG
//Email(respectively) : A0133662@U.NUS.EDU, A0125473@U.NUS.EDU, A0126332@U.NUS.EDU
void foo1();
void foo2();

int main() {
 return 0;
}

void foo1() {
 foo2();
}

void foo2() {
 foo1();
}
