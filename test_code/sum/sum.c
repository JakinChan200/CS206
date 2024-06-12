#include <stdio.h>

// only sum positive numbers
main(argc,argv)
int argc;
char *argv[];
{
	if (argc < 2) {
		fprintf(stdout, "Usage: ./sum # # ...");
		return -1;
	}
	int sum = 0, temp;

	for(int i = 1; i < argc; i++){
		sscanf(argv[1], "%d", &temp);
		if (temp > 0)
			sum += temp;
	}

	fprintf(stdout, "%d\n",sum);

	return 0;
}