#include <stdio.h>
#include "header.h"

int is_true(void)
{
	fprintf(stderr, "TRUE");

	return 0;
}

int is_false(void)
{
	fprintf(stderr, "FALSE");
	
	return 0;
}

