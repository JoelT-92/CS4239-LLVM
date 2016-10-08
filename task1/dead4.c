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
