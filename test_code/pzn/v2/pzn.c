// output if input is positive, negative, zero
#include <stdio.h>

main(argc,argv)
int argc;
char *argv[];
{
	if (argc != 2) {
		fprintf(stdout, "Format is \"pzn #\"\n");
	}

	int n;
	sscanf(argv[1], "%d", &n);
	if (n < 0) {			// should be n > 0
		fprintf(stdout, "%d is positive.\n", n);
	}
	else if (n > 0) {		// should be n < 0
		fprintf(stdout, "%d is negative.\n", n);
	}
	else {				// affected by above
		fprintf(stdout, "%d is zero.\n", n);
	}

	return 0;
}