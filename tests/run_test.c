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

/* Define tests within this suite */
CU_TestInfo test_array[] =
{
//    {"xparse model with no agents                   ", xparse_test_model_01       },
//    {"xparse model with one agent                   ", xparse_test_model_02       },
//    {"make and run model with one agent             ", make_and_run_test_model_02 },
    {"test_model_1             ", test_model_1 },
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

void xparse_test_model_01(void)
{
	int rc;
	
	rc = call_xparser(" test_models/test_model_01/test_model_01.xml");
	
	CU_ASSERT_EQUAL(rc, 0);
}

void xparse_test_model_02(void)
{
	int rc;
	
	rc = call_xparser(" test_models/test_model_02/test_model_02.xml");
	
	CU_ASSERT_EQUAL(rc, 0);
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
	int n, rc;
	char buffer[1000];
	
	rc = call_xparser(" models/test_model_1/test_model_1.xml");
		
	CU_ASSERT_EQUAL(rc, 0);
	
	rc = call_external("make --directory=models/test_model_1/");
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
	
	CU_ASSERT_EQUAL_FATAL(call_external("make --directory=models/test_model_1/"), 0);
	
	CU_ASSERT_EQUAL(call_external("models/test_model_1/main 11 models/test_model_1/its/0.xml -f 12"), 0);
	
	//check_agent_var_num("models/test_model_1/its/1.xml", "agent_a", "id");
	
	system("diff -q models/test_model_1/expected.out stdout.out");
	
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
	
	call_external("make vclean --directory=models/test_model_1/");
	remove("models/test_model_1/its/1.xml");
	remove("models/test_model_1/its/log.xml");
}

static int clean_quit(void)
{
    //remove("stdout.out");
    //remove("stderr.out");
        
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
			{"Test Models", NULL, NULL, test_array},
			CU_SUITE_INFO_NULL,
    };
    
	if(CU_register_suites(suites) != CUE_SUCCESS) return clean_quit();
	
    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    return clean_quit();
}
