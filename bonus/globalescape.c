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