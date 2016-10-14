//Authors : A0133662J CHEW YUNG CHUNG, A0125473H TAN QIU HAO, JOEL, A0126332R JULIANA SENG
//Email(respectively) : A0133662@U.NUS.EDU, A0125473@U.NUS.EDU, A0126332@U.NUS.EDU
char *globalptr;

void escape_local(void);

int main() {
	escape_local();
	return 0;
}

void escape_local() {
	char local_char = 'a';
	globalptr = &local_char;
}