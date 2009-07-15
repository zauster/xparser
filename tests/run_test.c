#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "CUnit/Basic.h"

/*
 * no agents
 * one agent, one function
 * time units
 * inclusion of model files
 */

void xparse_test_model_01(void);
void xparse_test_model_02(void);
void make_and_run_test_model_02(void);
void test_model_1(void);
void test_complicated_model(void);
void test_reading_and_writing_model_data(void);
void test_truncated_empty_xml_tags(void);
void test_code_standard(void);
void test_compile_warnings(void);
void test_4_parallel_1(void);
void test_4_parallel_2(void);
void test_4_parallel_3(void);
void test_4_parallel_4(void);
void test_input(void);
void test_debug(void);
void test_model_code_standard(int n);
void test_model_code_standard_1(void) { test_model_code_standard(1); }
void test_model_code_standard_2(void) { test_model_code_standard(2); }
void test_model_code_standard_3(void) { test_model_code_standard(3); }
void test_model_code_standard_4(void) { test_model_code_standard(4); }
void test_model_code_standard_5(void) { test_model_code_standard(5); }
void test_model_code_standard_6(void) { test_model_code_standard(6); }

/* Define tests within this suite */
CU_TestInfo test_array_1[] =
{
//    {"xparse model with no agents                   ", xparse_test_model_01       },
//    {"xparse model with one agent                   ", xparse_test_model_02       },
//    {"make and run model with one agent             ", make_and_run_test_model_02 },
//    {"test_model_1             ", test_model_1 },
	{"test complicated model                          ", test_complicated_model },
	{"test model code standard                        ", test_model_code_standard_1 },
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_2[] =
{
	{"test reading and writing model data             ", test_reading_and_writing_model_data },
	{"test model code standard                        ", test_model_code_standard_2 },
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_3[] =
{
	{"test code standard                              ", test_code_standard },
/*	{"test compile warnings                           ", test_compile_warnings },*/
	{"test truncated empty xml tags                   ", test_truncated_empty_xml_tags },
	{"test model code standard                        ", test_model_code_standard_3 },
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_4[] =
{
	{"test parallel (n 1)                             ", test_4_parallel_1 },
	{"test parallel (n 2)                             ", test_4_parallel_2 },
	{"test parallel (n 3)                             ", test_4_parallel_3 },
	{"test parallel (n 4)                             ", test_4_parallel_4 },
	{"test model code standard                        ", test_model_code_standard_4 },
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_5[] =
{
	{"test input filter/sort/random                   ", test_input },
	{"test model code standard                        ", test_model_code_standard_5 },
    CU_TEST_INFO_NULL,
};

CU_TestInfo test_array_6[] =
{
	{"test debug mode                                 ", test_debug },
	{"test model code standard                        ", test_model_code_standard_6 },
    CU_TEST_INFO_NULL,
};

char * xparser_path;

int call_external(char * command)
{
	int rc;
	int fdout, fderr;
	fpos_t posout, poserr;

	/* Flush the stream */
	fflush(stdout);
	/* Get current position in the file */
	fgetpos(stdout, &posout);
	/* duplicate an object descriptor */
	fdout = dup(fileno(stdout));
	/* associates a stream with a file */
	freopen("stdout.out", "w", stdout);

	fflush(stderr);
	fgetpos(stderr, &poserr);
	fderr = dup(fileno(stderr));
	freopen("stderr.out", "w", stderr);

	/* Call the external command now with
	 * stdout pointing to stdout.out, and
	 * stderr pointing to stderr.out */
	rc = system(command);

	/* Flush the stream */
	fflush(stdout);
	/* dupicate an object descriptor */
	dup2(fdout, fileno(stdout));
	/* Delete a descriptor */
	close(fdout);
	/* CLear stream of end-of-file and error indicators */
	clearerr(stdout);
	/* Set the current position in the file */
	fsetpos(stdout, &posout);

	fflush(stderr);
	dup2(fderr, fileno(stderr));
	close(fderr);
	clearerr(stderr);
	fsetpos(stderr, &poserr);

	return rc;
}

int call_xparser(char * xparser_command)
{
	int rc;
	char * command;

	command = (char *)malloc(strlen(xparser_path) + strlen(xparser_command) + 1);

	strcpy(command, xparser_path);
	strcat(command, xparser_command);

	rc = call_external(command);

	free(command);

	return rc;
}

int check_agent_var_num(char * path_to_xml, char * agent_name, char * var_name)
{
	int rc;
	char * command;

	command = (char *)malloc(strlen(agent_name) + strlen(var_name)*2 + strlen(path_to_xml) + 100);

	strcpy(command, "xml sel -T -t -m \"//xagent[name='");
	strcat(command, agent_name);
	strcat(command, "']\" -s A:N:- \"");
	strcat(command, var_name);
	strcat(command, "\" -v \"");
	strcat(command, var_name);
	strcat(command, "\" -n ");
	strcat(command, path_to_xml);

	rc = call_external(command);

	free(command);

	return rc;
}

void make_and_run_test_model_02(void)
{
	FILE *out;
	int n, rc;
	char buffer[1000];

	rc = call_external("make --directory=test_models/test_model_02/");
	if(rc != 0)
	{
		if((out = fopen("stderr.out", "r"))==NULL)
		{
			CU_FAIL("cannot read stderr.out");
		}
		else
		{
			while(fgets(buffer, 1000, out) != NULL)
			{
				if(strstr(buffer, "error: mboard.h: No such file or directory") != NULL)
				{
					CU_FAIL("mboard.h not found");
				}
			}

			fclose(out);
		}

		CU_FAIL_FATAL("make test_model_02");
	}
	else CU_PASS("make test_model_02");

	CU_ASSERT_EQUAL_FATAL(call_external("make --directory=test_models/test_model_02/"), 0);

	CU_ASSERT_EQUAL(call_external("test_models/test_model_02/main 1 test_models/test_model_02/its/0.xml"), 0);

	check_agent_var_num("test_models/test_model_02/its/1.xml", "agent_a", "id");

	if((out = fopen("stdout.out", "r"))==NULL)
	{
		CU_FAIL("cannot read stdout.out");
	}
	else
	{
		fscanf(out,"%d",&n);
		CU_ASSERT_EQUAL(n, 4);
		fscanf(out,"%d",&n);
		CU_ASSERT_EQUAL(n, 5);

		fclose(out);
	}

	call_external("make vclean --directory=test_models/test_model_02/");
	remove("test_models/test_model_02/its/1.xml");
	remove("test_models/test_model_02/its/log.xml");
}

void test_model_1(void)
{
	FILE *out;
	int rc;
	char buffer[1000];

	rc = call_xparser(" test1/test_model_1.xml");

	CU_ASSERT_EQUAL(rc, 0);

	rc = call_external("make --directory=test1/");
	if(rc != 0)
	{
		if((out = fopen("stderr.out", "r"))==NULL)
		{
			CU_FAIL("cannot read stderr.out");
		}
		else
		{
			while(fgets(buffer, 1000, out) != NULL)
			{
				if(strstr(buffer, "error: mboard.h: No such file or directory") != NULL)
				{
					CU_FAIL("mboard.h not found");
				}
			}

			fclose(out);
		}

		CU_FAIL_FATAL("make test_model_1");
	}
	else CU_PASS("make test_model_1");

	CU_ASSERT_EQUAL(call_external("test1/main 11 test1/0.xml -f 12"), 0);

	//check_agent_var_num("models/test_model_1/its/1.xml", "agent_a", "id");

	system("diff -q test1/expected.out stdout.out");

	//exit(0);

	/*if((out = fopen("stdout.out", "r"))==NULL)
	{
		CU_FAIL("cannot read stdout.out");
	}
	else
	{
		fscanf(out,"%d",&n);
		CU_ASSERT_EQUAL(n, 4);
		fscanf(out,"%d",&n);
		CU_ASSERT_EQUAL(n, 5);

		fclose(out);
	}*/

	rc = call_external("make vclean --directory=models/test_model_1/");
	remove("models/test_model_1/its/1.xml");
	remove("models/test_model_1/its/log.xml");
}

int compare_chars_in_files(char * filename1, char * filename2)
{
	FILE * file1;
	FILE * file2;
	char c1 = '\0';
	char c2 = '\0';
	int count = 0;
	
	if((file1 = fopen(filename1, "r"))==NULL)
	{
		CU_FAIL("cannot read filename1");
		return -1;
	}
	if((file2 = fopen(filename2, "r"))==NULL)
	{
		CU_FAIL("cannot read filename2");
		return -1;
	}
	
	while(1)
	{
		if(c1 != EOF) c1 = fgetc(file1);
		if(c2 != EOF) c2 = fgetc(file2);
		
		if(c1 != c2) count++;
		
		if(c1 == EOF && c2 == EOF) break;
	}
	
	fclose(file1);
	fclose(file2);
	
	return count;
}

void test_complicated_model(void)
{
	int rc;
	
	rc = call_external("test1/main 1 test1/0.xml");
	CU_ASSERT_EQUAL(rc, 0);
}

void test_truncated_empty_xml_tags(void)
{
	int rc;
	
	rc = call_xparser(" test3/test_truncated_empty_xml_tags.xml");
	CU_ASSERT_EQUAL(rc, 0);
}

void test_code_standard(void)
{
	int rc;
	/* Test if splint is available */
	rc = call_external("splint -weak -bufferoverflowhigh ../*.c");
	
	if(rc == 0) { CU_PASS(); }
	else
	{
		printf("\n");
		rc = system("splint -weak -bufferoverflowhigh ../*.c");
		CU_FAIL();
	}
}

/*void test_compile_warnings(void)
{
	int rc;
	
	rc = call_external("make vclean --directory=../");
	rc = call_external("make --directory=../");
	system("more stderr.out");
}*/

void test_reading_and_writing_model_data(void)
{
	int rc;
	
	rc = call_external("test2/main 1 test2/0.xml");
	CU_ASSERT_EQUAL(rc, 0);
	
	rc = compare_chars_in_files("test2/0.xml", "test2/1.xml");
	CU_ASSERT_EQUAL(rc, 1);
}

void test_input(void)
{
	int rc;
	FILE *out;
	char buffer[1000];
	
	rc = call_external("test5/main 1 test5/0.xml");
	//rc = system("test5/main 1 test5/0.xml");
	CU_ASSERT_EQUAL(rc, 0);
	
	if((out = fopen("stdout.out", "r"))==NULL)
	{
		CU_FAIL("cannot read stdout.out");
	}
	else
	{
		rc = 0;
		while(fgets(buffer, 1000, out) != NULL)
		{
			if(strstr(buffer, "FAILURE") != NULL) rc = 1;
		}
		if(rc == 0) { CU_PASS("result success"); }
		else { CU_FAIL("result fail"); }

		fclose(out);
	}
}

void test_debug(void)
{
	int rc;
	FILE *out;
	char buffer[1000];
	
	rc = call_external("test6/main 1 test6/0.xml");
	CU_ASSERT_EQUAL(rc, 0);
	
	if((out = fopen("stdout.out", "r"))==NULL)
	{
		CU_FAIL("cannot read stdout.out");
	}
	else
	{
		rc = 0;
		while(fgets(buffer, 1000, out) != NULL)
		{
			if(strstr(buffer, "Debug mode enabled") != NULL) rc = 1;
		}
		if(rc == 0) CU_FAIL("Debug mode enabled text not found");

		fclose(out);
	}
	
	if((out = fopen("stderr.out", "r"))==NULL)
	{
		CU_FAIL("cannot read stderr.out");
	}
	else
	{
		rc = 0;
		fgets(buffer, 1000, out);
		if(strstr(buffer, "ERROR: A function condition test has failed for agent type 'agent_a' leaving state 'start'") == NULL) rc = 1;
		fgets(buffer, 1000, out);
		if(strstr(buffer, "reason: there was more than one possible outgoing transition function") == NULL) rc = 1;
		fgets(buffer, 1000, out);
		if(strstr(buffer, "ERROR: A function condition test has failed for agent type 'agent_a' leaving state 'middle2'") == NULL) rc = 1;
		fgets(buffer, 1000, out);
		if(strstr(buffer, "reason: there was no possible outgoing transition function") == NULL) rc = 1;
		if(rc == 0) { CU_PASS("result success"); }
		else { CU_FAIL("result fail"); }

		fclose(out);
	}
}

void test_4_parallel(int n)
{
	FILE *out;
	int rc;
	int i;
	char buffer[1000];
	
	sprintf(buffer, "mpiexec -n %d test4/main 1 test4/0.xml -r", n);
	rc = call_external(buffer);
	CU_ASSERT_EQUAL(rc, 0);
	
	if((out = fopen("stdout.out", "r"))==NULL)
	{
		CU_FAIL("cannot read stdout.out");
	}
	else
	{
		rc = 0;
		while(fgets(buffer, 1000, out) != NULL)
		{
			if(strstr(buffer, "SUCCESS") != NULL) rc = 1;
		}
		if(rc == 1) { CU_PASS("result success"); }
		else { CU_FAIL("result fail"); }

		fclose(out);
	}
	
	/* Remove node results files */
	for(i = 0; i < n; i++)
	{
		sprintf(buffer, "test4/node%d-1.xml", i);
		remove(buffer);
	}
}

void test_4_parallel_1(void) { test_4_parallel(1); }
void test_4_parallel_2(void) { test_4_parallel(2); }
void test_4_parallel_3(void) { test_4_parallel(3); }
void test_4_parallel_4(void) { test_4_parallel(4); }

void test_model_code_standard(int n)
{
	int rc;
	char buffer[1000];
	
	sprintf(buffer, "splint -I/Users/stc/workspace/libmboard-0.2/include -weak -namechecks -bufferoverflowhigh test%d/*.c", n);
	
	/* Test if splint is available */
	rc = call_external(buffer);
	
	if(rc == 0) { CU_PASS(); }
	else
	{
		printf("\n");
		rc = system(buffer);
		CU_FAIL();
	}
}

int init_test_model(int index, int option)
{
	FILE *out;
	int rc, rc2;
	char buffer[1000];
	//printf("init_test_model_1\n");

	sprintf(buffer, " test%d/test_model_%d.xml", index, index);
	if(option == 1) strcat(buffer, " -p");
	if(option == 2) strcat(buffer, " -f");
	rc = call_xparser(buffer);
	if(rc != 0)
	{
		printf("\nSuite: test_model_%d xparsing failed", index);
		return -1;
	}
	sprintf(buffer, "make --directory=test%d/", index);
	rc = call_external(buffer);
	if(rc != 0)
	{
		if((out = fopen("stderr.out", "r"))==NULL)
		{
			printf("\nSuite: test_model_%d reading stderr.out failed", index);
			return -1;
		}
		else
		{
			while(fgets(buffer, 1000, out) != NULL)
			{
				if(strstr(buffer, "error: mboard.h: No such file or directory") != NULL)
				{
					/* Try again but with Simon's default libmboard location */
					sprintf(buffer, "make --directory=test%d/ LIBMBOARD_DIR=/Users/stc/workspace/libmboard-0.2/", index);
					rc2 = call_external(buffer);
					//rc2 = system(buffer);
					if(rc2 == 0) return 0;
					
					printf("\nSuite: test_model_%d mboard.h not found", index);
					fclose(out);
					return -1;
				}
				if(strstr(buffer, "error") != NULL)
					printf(buffer);
				if(strstr(buffer, "warning") != NULL)
					printf(buffer);
			}

			fclose(out);
		}

		printf("\nSuite: test_model_%d make failed", index);
		return -1;
	}

	return 0;
}

int clean_test_model(int index)
{
	char buffer[1000];

	sprintf(buffer, "make vclean --directory=test%d/", index);
	call_external(buffer);
	sprintf(buffer, "test%d/1.xml", index);
	remove(buffer);
	sprintf(buffer, "test%d/log.xml", index);
	remove(buffer);
	sprintf(buffer, "test%d/space_partitions.xml", index);
	remove(buffer);

	return 0;
}

int  init_test_model_1(void) { return init_test_model(1, 0);  }
int clean_test_model_1(void) { return clean_test_model(1); }
int  init_test_model_2(void) { return init_test_model(2, 0);  }
int clean_test_model_2(void) { return clean_test_model(2); }
int clean_test_model_3(void) { return clean_test_model(3); }
int  init_test_model_4(void) { return init_test_model(4, 1);  }
int clean_test_model_4(void) { return clean_test_model(4); }
int  init_test_model_5(void) { return init_test_model(5, 0);  }
int clean_test_model_5(void) { return clean_test_model(5); }
int  init_test_model_6(void) { return init_test_model(6, 0);  }
int clean_test_model_6(void) { return clean_test_model(6); }

static int clean_quit(void)
{
    remove("stdout.out");
    remove("stderr.out");

    free(xparser_path);

    CU_cleanup_registry();
    return CU_get_error();
}

int main(int argc, char ** argv)
{
	FILE * file;

    /*  */
    if(argc < 2)
    {
    	xparser_path = (char *)malloc(11);
    	strcpy(xparser_path,"../xparser");
    }
    else
    {
    	xparser_path = (char *)malloc(strlen(argv[1]) + 1);
    	strcpy(xparser_path,argv[1]);
    }

    if((file = fopen(xparser_path, "r"))==NULL)
    {
    	printf("ERROR: xparser not found\n");
    	return clean_quit();
    }
    else fclose(file);

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

	/* Register test suite */
	CU_SuiteInfo suites[] =
	{
			{"Test parsing", NULL, clean_test_model_3, test_array_3},
			{"Test_model_1", init_test_model_1, clean_test_model_1, test_array_1},
			{"Test reading and writing model data", init_test_model_2, clean_test_model_2, test_array_2},
			{"Test input filters, sort, random", init_test_model_5, clean_test_model_5, test_array_5},
			{"Test debug mode", init_test_model_6, clean_test_model_6, test_array_6},
			{"Test parallel syncs", init_test_model_4, clean_test_model_4, test_array_4},
			CU_SUITE_INFO_NULL,
    };

	if(CU_register_suites(suites) != CUE_SUCCESS) return clean_quit();

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    return clean_quit();
}
