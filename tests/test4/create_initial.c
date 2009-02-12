#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AGENTS 10
#define BUFFER 100

static void put_int(int int_single, FILE * file)
{
	char buffer[BUFFER];
	
	(void) snprintf(buffer, sizeof(char)*BUFFER, "%d", int_single);
	(void) fputs(buffer, file);
}

static void put_agent_a(int int_single, FILE * file)
{
	char buffer[BUFFER];
	int int_single_2;
	
	int_single_2 = int_single + 1;
	
	(void) fputs("<xagent>\n<name>agent_a</name>\n", file);
	(void) fputs("<int_single>", file);
	put_int(int_single, file);
	(void) fputs("</int_single>\n", file);
	(void) fputs("<int_single_2>", file);
	put_int(int_single_2, file);
	(void) fputs("</int_single_2>\n", file);
	(void) fputs("</xagent>\n", file);
}

static void put_agent_b(FILE * file)
{
	(void) fputs("<xagent>\n<name>agent_b</name>\n", file);
	(void) fputs("<int_single>0</int_single>\n", file);
	(void) fputs("</xagent>\n", file);
}

int main(int argc, char ** argv)
{
	FILE * file;
	int i;
	int no_agents;
	
	if(argc < 2) no_agents = AGENTS;
	else no_agents = atoi(argv[1]);
	
	if((file = fopen("0.xml", "w"))==NULL)
	{
		printf("Error: cannot open file '0.xml' for writing\n");
		return 1;
	}
	
	(void) fputs("<states>\n<itno>0</itno>\n<environment>\n<env_int>", file);
	put_int(no_agents, file);
	(void) fputs("</env_int>\n<env_float>2.200000</env_float>\n<env_double>3.300000</env_double>\n<env_char>e</env_char>\n</environment>\n", file);
	put_agent_b(file);
	for(i = 1; i <= no_agents; i++) put_agent_a(i, file);
	(void) fputs("</states>\n", file);
	(void) fclose(file);
	
	return 0;
}
