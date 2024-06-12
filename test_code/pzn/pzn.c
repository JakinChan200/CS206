// output if input is positive, negative, zero
#include <stdio.h>

main(argc,argv)
int argc;
char *argv[];
{
	if (argc != 2) {
		fprintf(stdout, "Format is \"pzn #\"\n");
		return -1;
	}

	int n;
	sscanf(argv[1], "%d", &n);
	if (n > 0) {
		fprintf(stdout, "%d is positive.\n", n);
	}
	else if (n < 0) {
		fprintf(stdout, "%d is negative.\n", n);
	}
	else {
		fprintf(stdout, "%d is zero.\n", n);
	}

	return 0;
}