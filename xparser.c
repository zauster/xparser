/**
 * \file  xparser.c
 * \code
 *     Author:       Simon Coakley
 *     Copyright (c) 2007 The University of Sheffield.
 *     License:      n/a
 * \endcode
 * \brief Architecture for agent-based modelling based on X-machines.
 */

/** \mainpage Xparser - parser tool for the X-agents framework
 *
 * \section intro_sec Introduction
 *
 * This program accepts a model description in X-agent XML and produces source code for the model in either serial (default) or in parallel (via MPI libraries).
 *
 * \section install_sec Installation
 *
 * Compile using a C compiler.
 * E.g. gcc
 *
 * \section changelog
 *
 * 0.11.0 Can handle arrays in agent memory in serial (not parallel)
 */

#include "header.h"

/** \fn int main(int argc, char ** argv)
 * \brief Main program function.
 * \param argc Argument count.
 * \param argv Pointer Pointer to Argument vector
 * \return system int
 */
int main(int argc, char ** argv)
{
	/* Variables for parsing directories */
	int lastd = 0;
	int i;
	
	/* Variable to read in command line input */
	char buffer[10];
	char inputfile[100];
	char directory[100];
	char filename[100];
	char templatename[100];
	
	model_data * modeldata;
	
	/* Hold model data */
	xmachine * xmachines;
	/*xmachine_memory * xmemory;*/
	xmachine_message * xmessage;
	/*xmachine_state * xstates;*/
	/*xmachine_function * xfunctions;*/
	variable * envvar;
	env_func * envfunc;
	variable * envdefine;
	variable * allvars;
	f_code * it_end_code;
	communication_layer * communication_layers;
	model_datatype * datatypes;
	
	modeldata = (model_data *)malloc(sizeof(model_data));
	/* Variable for code type */
	/* 0=serial(default) 1=parallel 2=grid */
	modeldata->code_type = 0;
	
	/* Initialise pointers */
	modeldata->p_xmachines = &xmachines;
	xmachines = NULL;
	/*modeldata->p_xmemory = &xmemory;*/
	/*xmemory = NULL;*/
	modeldata->p_xmessages = &xmessage;
	xmessage = NULL;
	/*modeldata->p_xstates = &xstates;*/
	/*xstates = NULL;*/
	/*modeldata->p_xfunctions = &xfunctions;*/
	/*xfunctions = NULL;*/
	modeldata->p_envvars = &envvar;
	envvar = NULL;
	modeldata->p_envfuncs = &envfunc;
	envfunc = NULL;
	modeldata->p_envdefines = &envdefine;
	envdefine = NULL;
	modeldata->p_allvars = &allvars;
	allvars = NULL;
	modeldata->p_it_end_code = &it_end_code;
	it_end_code = NULL;
	modeldata->p_com_layers = &communication_layers;
	communication_layers = NULL;
	modeldata->p_datatypes = &datatypes;
	datatypes = NULL;
	
	printf("FLAME xparser %d.%d.%d\n", VERSIONMAJOR, VERSIONMINOR, VERSIONMICRO);
	printf("Copyright (C) 2007 The University of Sheffield. \n");
	printf("UoS License: See <https://trac.flame.ac.uk/wiki/License>\n");
	
	/* If file not defined then ask for it */
	if(argc < 2)
	{
		printf("Usage: xparser [file] [-sp]\n");
		return 0;
	}
	else
	{
		strcpy(inputfile, argv[1]);
	}
	
	/* Calculate directory to write files to */
	i = 0;
	while(inputfile[i] != '\0')
	{
	/* For windows directories */
		if(inputfile[i] == '\\') lastd=i;
	/* For unix directories */
		if(inputfile[i] == '/') lastd=i;
		i++;
	}
	/* If a directory is in the path */
	if(lastd != 0)
	{
		strcpy(directory, inputfile);
		directory[lastd+1] = '\0';
	}
	else directory[0] = '\0';
	
	printf("inputfile: %s\n", inputfile);
	printf("directory: %s\n", directory);
	
	/* Read model from model xml file */
	readModel(inputfile, directory, modeldata);
	
	/* Calculate dependency graph for model functions */
	create_dependency_graph(directory, modeldata);
	
	i = 0;
	if(argc > 2) 
	{
		strcpy(buffer, argv[2]);
		
		while(buffer[i] != '\0')
		{
			if(buffer[i] == '-') modeldata->code_type = 9;
			if(buffer[i] == 's') if(modeldata->code_type == 9) modeldata->code_type = 0;
			if(buffer[i] == 'p') if(modeldata->code_type == 9) modeldata->code_type = 1;
			if(buffer[i] == 'g') if(modeldata->code_type == 9) modeldata->code_type = 2;
			if(buffer[i] == 'q') if(modeldata->code_type == 9) modeldata->code_type = 3;
			
			i++;
		}
	}
	
	/* Print what type of code writing */
	if(modeldata->code_type == 0) printf("code-type   : Serial\n");
	if(modeldata->code_type == 1) printf("code-type   : Parallel\n");
	if(modeldata->code_type == 2)
	{
		printf("code-type   : Grid\n*** not implemented, need more funding ***\n");
		exit(1);
	}
	if(modeldata->code_type == 3)
	{
		printf("code-type   : Quantum\n*** not implemented, LOTS more funding needed ***\n");
		exit(1);
	}
	
	strcpy(filename, directory); strcat(filename, "Makefile"); strcpy(templatename, "Makefile.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "xml.c"); strcpy(templatename, "xml.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "main.c"); strcpy(templatename, "main.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "header.h"); strcpy(templatename, "header.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "memory.c"); strcpy(templatename, "memory.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "low_primes.h"); strcpy(templatename, "low_primes.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "messageboards.c"); strcpy(templatename, "messageboards.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "partitioning.c"); strcpy(templatename, "partitioning.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "propagate_messages.c"); strcpy(templatename, "propagate_messages.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "propagate_agents.c"); strcpy(templatename, "propagate_agents.tmpl");
	parseTemplate(filename, templatename, modeldata);
	strcpy(filename, directory); strcat(filename, "Doxyfile"); strcpy(templatename, "Doxyfile.tmpl");
	parseAgentHeaderTemplate(directory, modeldata);
	
	freexmachines(modeldata->p_xmachines);
	freexmessages(modeldata->p_xmessages);
	freeenvfunc(modeldata->p_envfuncs);
	freevariables(modeldata->p_envvars);
	freevariables(modeldata->p_envdefines);
	freecommunication_layers(modeldata->p_com_layers);
	freedatatypes(modeldata->p_datatypes);
	free(modeldata);
	
	/* Exit successfully by returning zero to Operating System */
	return 0;
}
