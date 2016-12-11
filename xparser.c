/**
 * \file  xparser.c
 * \code
 *     Author:       		Simon Coakley (USFD)
 *     Added to and Modified: 	David Worth (STFC)
 *				Lee Shawn Chin (STFC)
 *				Chris Greenough (STFC)
 *     Copyright (c) 2014 STFC Rutherford Appleton Laboratory/Sheffield University
 * \endcode
 * \brief Architecture for agent-based modelling based on X-machines.
 */

/** \mainpage xparser - parser tool for the FLAME framework
 *
 * \section intro_sec Introduction
 *
 * This program is the FLAME XMML parser. It accepts a model description
 * in X-agent XML and produces source code for the model in either serial
 * (default) or in parallel (via MPI libraries).
 *
 * The command line is:
 *
 * xparser [-p | -s] [XMML file]
 *
 * xparser without any arguments will produce:
 *
 * xparser: Version \<version number\>
 *
 * Usage: xparser [XMML file] [-s | -p]
 *
 * By default xparser will generate a serial program.
 *
 * The code xparser generates, although not pretty, should compile on most
 * systems that have a compiler conforming to the C99 standard.
 *
 * \section install_sec Installation
 *
 * Compile using a C compiler. A Make file is provided. xparser has been tested
 * on a large number of UNIX based systems.
 */

#include "header.h"

/** \fn int main(int argc, char * argv[])
 * \brief Main program function.
 * \param argc Argument count.
 * \param argv Pointer Pointer to Argument vector
 * \return system int
 */
int main(int argc, char * argv[])
{
    /* Variables for parsing directories */
    int lastd;
    int i;
    /* Error value */
    int rc;

    /* Variable to read in command line input */
    char inputfile[1000];
    char directory[1000];
    char filename[1000];
    char tmpl_name[1000];
    char tmpl_directory[1000];
    char *locxparser;

    /* Usage message */

    char *usage =
        "Usage: xparser [XMML file] [-s | -p] [-f]\n" \
        "\t-s\tSerial mode\n" \
        "\t-p\tParallel mode\n" \
        "\t-f\tFinal production mode\n";

    /* For reading input files */
    input_file * current_input_file;
    /* Structure to hold model data */
    model_data * modeldata;

    /* Hold modeldata data */
    xmachine * xmachines;
    xmachine_message * xmessage;
    variable * envvar;
    env_func * envfunc;
    variable * envdefine;
    variable * allvars;
    variable * constant_filter_vars;
    f_code * it_end_code;
    layer * layers;
    flame_communication * communications;
    model_datatype * datatypes;
    time_data * time_units;
    input_file * temp_input_file;

    /* Allocate memory for modeldata */
    modeldata = (model_data *)malloc(sizeof(model_data));
    /* 0=serial(default) 1=parallel 2=grid */
    modeldata->code_type = 0;
    /* 0=dgraph.dot 1=stategraph.dot */
    modeldata->depends_style = 0;
    modeldata->debug_mode = 1;

    inputfile[1]='\0';

    /* Initialise pointers */
    modeldata->name = NULL;
    modeldata->p_xmachines = &xmachines;
    xmachines = NULL;
    modeldata->p_xmessages = &xmessage;
    xmessage = NULL;
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
    modeldata->p_layers = &layers;
    layers = NULL;
    modeldata->p_communications = &communications;
    communications = NULL;
    modeldata->p_datatypes = &datatypes;
    datatypes = NULL;
    modeldata->p_time_units = &time_units;
    time_units = NULL;
    modeldata->p_files = &temp_input_file;
    temp_input_file = NULL;
    modeldata->p_constant_filter_vars = &constant_filter_vars;
    constant_filter_vars = NULL;

    printf("xparser (Version %d.%d.%d)\n", VERSIONMAJOR, VERSIONMINOR, VERSIONMICRO);

    /* Must be at least the input file name */
    if(argc < 2) {
        printf("%s",usage);
        free_modeldata(modeldata);
        return 0;
    }

    /* Location of xparser */

    locxparser = getenv("FLAME_XPARSER_DIR");

    if(!locxparser) { /* Checking for empty string */
        strcpy(tmpl_directory,FLAME_XPARSER_DIR);
        if(strcmp(tmpl_directory,".") != 0) {
            printf("\n***Warning: Environment variable FLAME_XPARSER_DIR not set - looking in %s for Templates\n",tmpl_directory);
        } else {
            printf("\n***Warning: Environment variable FLAME_XPARSER_DIR not set - looking in current directory for Templates\n");
        }
    } else {
        strcpy(tmpl_directory,locxparser);
        printf("\n***Info: Evnironment variable FLAME_XPARSER_DIR set - looking in %s for Templates\n",tmpl_directory);
    };

    /* Check directory path and make sure it ends with '/' or '\' */
    if(strchr(tmpl_directory,'/') != NULL) {

        tmpl_directory[strlen(tmpl_directory)]='/';
        tmpl_directory[strlen(tmpl_directory)+1]='\0';

    }

    /* parser command line */
    while(argc >1) {
        if(argv[1][0] == '-') {
            switch (argv[1][1]) {
            case 's':
                modeldata->code_type = 0;
                break;
            case 'p':
                modeldata->code_type = 1;
                break;
            case 'f' :
                modeldata->debug_mode = 0;
                break;
            case 't' :
                break;
            default:
                printf("xparser: Error - unknown option %s\n",argv[1]);
                printf("%s",usage);
                free_modeldata(modeldata);
                return 0;
            }
        } else {
            strcpy(inputfile,argv[1]);
            /* printf("XMML input file : %s\n",argv[1]); */
        }
        argc--;
        argv++;
    }

    if(inputfile[1] == '\0') {
        printf("xparser: Error - XMML must be specified\n");
        free_modeldata(modeldata);
        return 0;
    }

    /* Print what type of code writing */
    printf("\n");
    printf("Code type       : ");
    if(modeldata->code_type == 0) printf("Serial");
    if(modeldata->code_type == 1) printf("Parallel");
    if(modeldata->debug_mode == 1) printf(" (DEBUG)");
    printf("\n");

    /* Calculate directory to write files to */
    i = 0;
    lastd = 0;
    while(inputfile[i] != '\0') {
        /* For windows directories */
        if(inputfile[i] == '\\') lastd=i;
        /* For unix directories */
        if(inputfile[i] == '/') lastd=i;
        i++;
    }
    /* If a directory is in the path */
    if(lastd != 0) {
        strcpy(directory, inputfile);
        directory[lastd+1] = '\0';
    } else directory[0] = '\0';

    /* Calculate directory where xparser and template files are */
    i = 0;
    lastd = 0;
    while(tmpl_directory[i] != '\0') {
        /* For windows directories */
        if(tmpl_directory[i] == '\\') lastd=i;
        /* For unix directories */
        if(tmpl_directory[i] == '/') lastd=i;
        i++;
    }
    /* If a directory is in the path */
    if(lastd != 0) {
        tmpl_directory[lastd+1] = '\0';
    } else tmpl_directory[0] = '\0';

    printf("Input XMML file : %s\n", inputfile);
    printf("Model root dir  : %s\n", directory);
    printf("Template dir    : %s\n", tmpl_directory);
    printf("\n");

    current_input_file = add_input_file(modeldata->p_files);
    current_input_file->fullfilepath = copystr(inputfile);
    current_input_file->fulldirectory = copystr(directory);
    current_input_file->localdirectory = copystr("");

    /* Read model from model xml file */
    current_input_file = * modeldata->p_files;
    while(current_input_file) {
        if(current_input_file->enabled == 1)
            readModel(current_input_file, directory, modeldata);

        current_input_file = current_input_file->next;
    }
    rc = checkmodel(modeldata);
    if(rc == -1) {
        free_modeldata(modeldata);
        return 0;
    }

    /* Calculate dependency graph for model functions */
    rc = create_dependency_graph(directory, modeldata);
    if(rc == -1) {
        free_modeldata(modeldata);
        return 0;
    }

    strcpy(filename, directory);
    strcat(filename, "Makefile");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "Makefile.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "xml.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "xml.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "main.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "main.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "header.h");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "header.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "memory.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "memory.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "low_primes.h");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "low_primes.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "messageboards.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "messageboards.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "partitioning.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "partitioning.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "timing.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "timing.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "Doxyfile");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "Doxyfile.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);
    strcpy(filename, directory);
    strcat(filename, "rules.c");
    strcpy(tmpl_name, tmpl_directory);
    strcat(tmpl_name, "rules.tmpl");
    parseTemplate(filename, tmpl_name, modeldata);

    printf("\n");
    parseAgentHeaderTemplate(directory, modeldata);
    /*parseUnittest(directory, modeldata);*/
    /*parser0dtd(directory, modeldata);*/
    /*parser0xsd(directory, modeldata);*/

    free_modeldata(modeldata);

    printf("\n--- xparser finished ---\n\n");

    printf("To compile and run the generated code, you will need:\n");
    printf(" * libmboard (version %s or newer)\n", LIBMBOARD_MINVER_STR);

    /* Exit successfully by returning zero to Operating System */
    return 0;
}
