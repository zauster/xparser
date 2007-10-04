#include "header.h"

/** \fn parseTemplate(char * filename, char * templatename, model_data * modeldata)
 * \brief Combines the model data with a template to produce a simulation source code file.
 * \param filename The output file name.
 * \param templatename The template file.
 * \param modeldata The model data.
 */
void parseTemplate(char * filename, char * templatename, model_data * modeldata)
{
	/* Pointer to output file */
	FILE *file;
	/* Pointer to template file */
	FILE *tmplfile;
	char c = ' ';
	int tmplcode = 0;
	char chartag[100][100];
	char tagbuffer[100];
	char lastloop[100];
	int lastiftag = 0;
	int looppos[100];
	char data[100];
	int numtag = 0;
	int exitforeach;
	int pos;
	int pos1;
	int i;
	int write = 1;
	int lastwrite = 1;
	int loopwrite[100];
	int loopwritepos = 0;
	int log = 0;
	int var_count;
	int message_count;
	int xagent_count;
	int found;
	char_array * buffer = init_char_array();
	char_array * buffer3 = init_char_array();
	char_array * filebuffer = init_char_array();
	char * previous_name;
	int inallvar = 0;
	int inxagentvar = 0;
	int indatatypevar = 0;

	/* pointers to model datatypes */
	xmachine * current_xmachine;
	xmachine_message * current_message;
	xmachine_function * current_function;
	variable * current_variable;
	f_code * current_code;
	communication_layer * current_com_layer;
	layer * current_layer;
	env_func * current_envfunc;
	variable * allvar;
	variable * current_envvar;
	variable * current_datatypevariable = NULL;
	model_datatype * current_datatype = NULL;
	
	/* Open the output file */
	printf("writing file: %s\t", filename);
	file = fopen(filename, "w");
	/* Open the template file */
	printf("using template: %s\n", templatename);
	tmplfile = fopen(templatename, "r");

	/* Read the template file into memory */
	while (c != (char)EOF)
	{
		c = (char)fgetc(tmplfile);
		if (c != (char)EOF)
			add_char(filebuffer, c);
	}
	/* Record the position reached in the template */
	pos = 0;
	/* While still parsing the template */
	while (pos < filebuffer->size)
	{
		/* C is the next character in the template */
		c = filebuffer->array[pos];
		/* If in a template code tag */
		if (tmplcode == 1)
		{
			/* If end of tag */
			if (c == '>')
			{
				add_char(buffer, '>');
				/* Handle tags */
				if (strcmp(buffer->array, "<?if use_arrays?>") == 0)
				{
					if (log)
						printf("start :%d\tif use_arrays\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (modeldata->agents_include_array_variables == 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if serial?>") == 0)
				{
					if (log)
						printf("start :%d\tif serial\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (modeldata->code_type != 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if parallel?>") == 0)
				{
					if (log)
						printf("start :%d\tif parallel\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (modeldata->code_type != 1)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if dynamic_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif dynamic_array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (indatatypevar)
					{
						if (current_datatypevariable->arraylength != -1)
							write = 0;
					}
					else if (inallvar)
					{
						if (allvar->arraylength != -1)
							write = 0;
					}
					else if (inxagentvar)
						if (current_variable->arraylength != -1)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if static_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif static_array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (allvar->arraylength < 1)
							write = 0;
					if (inxagentvar)
						if (current_variable->arraylength < 1)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if not_static_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif static_array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (allvar->arraylength > 0)
							write = 0;
					if (inxagentvar)
						if (current_variable->arraylength > 0)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if not_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif not_array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (allvar->arraylength != 0)
							write = 0;
					if (inxagentvar)
						if (current_variable->arraylength != 0)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if array?>") == 0)
				{
					if (log)
						printf("start :%d\tif array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (allvar->arraylength == 0)
							write = 0;
					if (inxagentvar)
						if (current_variable->arraylength == 0)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if modeldatatype?>") == 0)
				{
					if (log)
						printf("start :%d\tif modeldatatype\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
					{
						if (allvar->ismodeldatatype == 0) write = 0;
					}
					if (inxagentvar)
					{
						if (current_variable->ismodeldatatype == 0) write = 0;
					}
				}
				else if (strcmp(buffer->array, "<?if not_modeldatatype?>") == 0)
				{
					if (log)
						printf("start :%d\tif not_modeldatatype\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (allvar->ismodeldatatype == 1)
							write = 0;
					if (inxagentvar)
						if (current_variable->ismodeldatatype == 1)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if char?>") == 0)
				{
					if (log)
						printf("start :%d\tif not_char\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(lastloop, "foreach datatypevar") == 0)
					{
						if (indatatypevar)
							if (strcmp(current_datatypevariable->type, "char") != 0 && strcmp(current_datatypevariable->type, "char_array") != 0)
								write = 0;
					}
					else
					{
						if (inallvar)
							if (strcmp(allvar->type, "char") != 0 && strcmp(allvar->type, "char_array") != 0)
								write = 0;
						if (inxagentvar)
							if (strcmp(current_variable->type, "char") != 0 && strcmp(current_variable->type, "char_array") != 0)
								write = 0;
					}
				}
				else if (strcmp(buffer->array, "<?if not_char?>") == 0)
				{
					if (log)
						printf("start :%d\tif not_char\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(lastloop, "foreach datatypevar") == 0)
					{
						if (indatatypevar)
							if (strcmp(current_datatypevariable->type, "char") == 0 || strcmp(current_datatypevariable->type, "char_array") == 0)
								write = 0;
					}
					else
					{
						if (inallvar)
							if (strcmp(allvar->type, "char") == 0 || strcmp(allvar->type, "char_array") == 0)
								write = 0;
						if (inxagentvar)
							if (strcmp(current_variable->type, "char") == 0 || strcmp(current_variable->type, "char_array") == 0)
								write = 0;
					}
				}
				else if (strcmp(buffer->array, "<?if char_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif char_array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (strcmp(allvar->type, "char_array") != 0)
							write = 0;
					if (inxagentvar)
						if (strcmp(current_variable->type, "char_array") != 0)
							write = 0;
					if (indatatypevar)
						if (strcmp(current_datatypevariable->type, "char_array") != 0)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if notchar_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif notchar_array\twrite:%d\n", numtag, write);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (inallvar)
						if (strcmp(allvar->type, "char_array") == 0)
							write = 0;
					if (inxagentvar)
						if (strcmp(current_variable->type, "char_array") == 0)
							write = 0;
					if (indatatypevar)
						if (strcmp(current_datatypevariable->type, "char_array") == 0)
							write = 0;
				}
				else if (strcmp(buffer->array, "<?if int_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif int_array\twrite:%d\n", numtag, write);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(allvar->type, "int_array") != 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if float_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif float_array\twrite:%d\n", numtag, write);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(allvar->type, "float_array") != 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if double_array?>") == 0)
				{
					if (log)
						printf("start :%d\tif double_array\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(allvar->type, "double_array") != 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if first?>") == 0)
				{
					if (log)
						printf("start :%d\tif first\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(lastloop, "foreach xagent") == 0)
					if (current_xmachine != * modeldata->p_xmachines)
						write = 0;
					if (strcmp(lastloop, "foreach message") == 0)
					if (current_message != * modeldata->p_xmessages)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if notfirst?>") == 0)
				{
					if (log)
						printf("start :%d\tif notfirst\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(lastloop, "foreach xagent") == 0)
					if (current_xmachine == * modeldata->p_xmachines)
						write = 0;
					if (strcmp(lastloop, "foreach message") == 0)
					if (current_message == * modeldata->p_xmessages)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if notlast?>") == 0)
				{
					if (log)
						printf("start :%d\tif notlast\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(lastloop, "foreach xagent") == 0)
						if (current_xmachine->next == NULL)
							write = 0;
					if (strcmp(lastloop, "foreach xagentvar") == 0)
						if (current_variable->next == NULL)
							write = 0;
					if (strcmp(lastloop, "foreach message") == 0)
						if (current_message->next == NULL)
							write = 0;
					if (strcmp(lastloop, "foreach messagevar") == 0)
						if (current_variable->next == NULL)
							write = 0;
					if(strcmp(lastloop, "foreach datatypevar") == 0)
					{
						if(current_datatypevariable != NULL)
						{
							if (current_datatypevariable->next == NULL) write = 0;
						}
						else write = 0;
					}
				}
				else if (strcmp(buffer->array, "<?if last?>") == 0)
				{
					if (log)
						printf("start :%d\tif last\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(strcmp(lastloop, "foreach datatypevar") == 0)
					{
						if(current_datatypevariable != NULL)
						{
							if (current_datatypevariable->next != NULL) write = 0;
						}
						else write = 0;
					}
				}
				else if (strcmp(buffer->array, "<?if allvar_in_agent?>") == 0)
				{
					if (log)
						printf("start :%d\tif allvar_in_agent\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					found = 0;
					current_variable = current_xmachine->memory->vars;
					while (current_variable)
					{
						if (strcmp(current_variable->name, allvar->name) == 0)
							found = 1;

						current_variable = current_variable->next;
					}
					if (found == 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if use_zvar?>") == 0)
				{
					if (log)
						printf("start :%d\tif use_zvar\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(current_xmachine->zvar, "0.0") == 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?if no_zvar?>") == 0)
				{
					if (log)
						printf("start :%d\tif no_zvar\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(current_xmachine->zvar, "0.0") != 0)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?end if?>") == 0)
				{
					/* Look at last tag */
					numtag--;
					/* If different then exit */
					if (strcmp("if", &chartag[numtag][0]) != 0)
					{
						printf("error with template tag nest\n");
						printf("tag to close: %s\n", &chartag[numtag][0]);
						exit(0);
					}

					if (log)
						printf("finish:%d\tif", numtag);
					/* if cooresponds with first if*/
					if (log)
						printf("\tnumtag:%d\tlastiftag:%d\twrite:%d\tpos:%d\n", numtag, lastiftag, write, pos);
					if (numtag == lastiftag)
					{
						write = 1;
						lastwrite = 1;
						lastiftag = -1;
					}
				}
				else if (strcmp(buffer->array, "<?end foreach?>") == 0)
				{
					exitforeach = 0;
					/* Look at last tag */
					numtag--;
					
					/* Check for next element in current loop */
					if (strcmp("foreach functionfiles", &chartag[numtag][0]) == 0)
					{
						if (current_envfunc != NULL)
							current_envfunc = current_envfunc->next;
						if (current_envfunc == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
						}
					}
					else if (strcmp("foreach allvar", &chartag[numtag][0]) == 0)
					{
						if (allvar != NULL)
						{
							allvar = allvar->next;
						}
						if (allvar == NULL)
						{
							exitforeach = 1;
							inallvar = 0;
						}
						else
						{
							current_datatype = allvar->datatype;
							pos = looppos[numtag];
							numtag++;
						}
					}
					else if (strcmp("foreach envvar", &chartag[numtag][0]) == 0)
					{
						if (current_envvar != NULL)
							current_envvar = current_envvar->next;
						if (current_envvar == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
						}
					}
					else if (strcmp("foreach xagent", &chartag[numtag][0]) == 0)
					{
						previous_name = current_xmachine->name;
						
						if (current_xmachine != NULL)
							current_xmachine = current_xmachine->next;
						if (current_xmachine == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							xagent_count++;
						}
					}
					else if (strcmp("foreach xagentvar", &chartag[numtag][0]) == 0)
					{
						if (current_variable != NULL)
						{
							current_variable = current_variable->next;
							if(current_variable != NULL) current_datatype = current_variable->datatype;
						}
						if (current_variable == NULL)
						{
							exitforeach = 1;
							inxagentvar = 0;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach message", &chartag[numtag][0]) == 0)
					{
						previous_name = current_message->name;
						
						if (current_message != NULL)
							current_message = current_message->next;
						if (current_message == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							message_count++;
						}
					}
					else if (strcmp("foreach messagevar", &chartag[numtag][0]) == 0)
					{
						if (current_variable != NULL)
							current_variable = current_variable->next;
						if (current_variable == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach comlayer", &chartag[numtag][0]) == 0)
					{
						if (current_com_layer != NULL)
							current_com_layer = current_com_layer->next;
						if (current_com_layer == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach funclayer", &chartag[numtag][0]) == 0)
					{
						if (current_layer != NULL)
							current_layer = current_layer->next;
						if (current_layer == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach function", &chartag[numtag][0]) == 0)
					{
						if (current_function != NULL)
							current_function = current_function->next;
						if (current_function == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach enditfunc", &chartag[numtag][0]) == 0)
					{
						if (current_code != NULL)
							current_code = current_code->next;
						if (current_code == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach datatype", &chartag[numtag][0]) == 0)
					{
						if (current_datatype != NULL)
							current_datatype = current_datatype->next;
						if (current_datatype == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach datatypevar", &chartag[numtag][0]) == 0)
					{
						if (current_datatypevariable != NULL)
							current_datatypevariable = current_datatypevariable->next;
						if (current_datatypevariable == NULL)
						{
							exitforeach = 1;
							indatatypevar = 0;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					/* If different then exit */
					if (exitforeach)
					{
						strcpy(tagbuffer, &chartag[numtag][0]);
						tagbuffer[7] = '\0';
						if (strcmp("foreach", tagbuffer) != 0)
						{
							printf("error with template tag nest\n");
							printf("tag to close: %s\n", &chartag[numtag][0]);
							exit(0);
						}
						
						if(loopwritepos > 0)
						{
							loopwritepos--;
							write = loopwrite[loopwritepos];
						}
						else write = lastwrite;
						
						/* find last loop in chartag */
						if (log)
							printf("**** exit loop = %s\n", lastloop);
						strcpy(lastloop, "");
						for (i = 0; i < numtag; i++)
						{
							strcpy(tagbuffer, &chartag[i][0]);
							tagbuffer[7] = '\0';
							if (strcmp("foreach", tagbuffer) == 0)
							{
								strcpy(lastloop, &chartag[i][0]);
								if (log)
									printf("**** found last loop = %s\n", lastloop);
							}
						}
					}
				}
				else if (strcmp(buffer->array, "<?foreach functionfiles?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach functionfiles\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach functionfiles");
					strcpy(lastloop, "foreach functionfiles");
					looppos[numtag] = pos;
					numtag++;
					lastwrite = write;

					current_envfunc = * modeldata->p_envfuncs;
					if (current_envfunc == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach allvar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach allvar\tpos: %d\n", numtag, looppos[numtag]);
					strcpy(&chartag[numtag][0], "foreach allvar");
					strcpy(lastloop, "foreach allvar");
					looppos[numtag] = pos;
					numtag++;
					lastwrite = write;
					inallvar = 1;
					allvar = * modeldata->p_allvars;
					if(allvar != NULL) current_datatype = allvar->datatype;
				}
				else if (strcmp(buffer->array, "<?foreach envvar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach envvar\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach envvar");
					strcpy(lastloop, "foreach envvar");
					looppos[numtag] = pos;
					numtag++;
					lastwrite = write;

					current_envvar = * modeldata->p_envvars;
					if (current_envvar == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach xagent?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach xagent\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach xagent");
					strcpy(lastloop, "foreach xagent");
					looppos[numtag] = pos;
					numtag++;
					xagent_count = 0;
					lastwrite = write;
					previous_name = NULL;

					current_xmachine = * modeldata->p_xmachines;
					if (current_xmachine == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach xagentvar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach xagentvar\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach xagentvar");
					strcpy(lastloop, "foreach xagentvar");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					lastwrite = write;
					inxagentvar = 1;
					current_variable = current_xmachine->memory->vars;
					if(current_variable != NULL) current_datatype = current_variable->datatype;
					if (current_variable == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach message?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach message\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach message");
					strcpy(lastloop, "foreach message");
					looppos[numtag] = pos;
					numtag++;
					message_count = 0;
					lastwrite = write;
					previous_name = NULL;

					current_message = * modeldata->p_xmessages;
					if (current_message == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach messagevar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach messagevar\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach messagevar");
					strcpy(lastloop, "foreach messagevar");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					lastwrite = write;

					current_variable = current_message->vars;
					if (current_variable == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach comlayer?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach comlayer\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach comlayer");
					strcpy(lastloop, "foreach comlayer");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					lastwrite = write;

					current_com_layer = * modeldata->p_com_layers;
					if (current_com_layer == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach funclayer?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach funclayer\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach funclayer");
					strcpy(lastloop, "foreach funclayer");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					lastwrite = write;

					current_layer = current_com_layer->layers;
					if (current_layer == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach function?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach function\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach function");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					lastwrite = write;

					if (strcmp(lastloop, "foreach funclayer") == 0)
					{
						current_function = current_layer->functions;
						if (current_function == NULL)
							write = 0;
					}
					if (strcmp(lastloop, "foreach xagent") == 0)
					{
						current_function = current_xmachine->functions;
						if (current_function == NULL)
							write = 0;
					}

					strcpy(lastloop, "foreach function");
				}
				else if (strcmp(buffer->array, "<?foreach enditfunc?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach enditfunc\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach enditfunc");
					strcpy(lastloop, "foreach enditfunc");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					lastwrite = write;

					current_code = * modeldata->p_it_end_code;
					if (current_code == NULL)
						write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach datatype?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach datatype\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach datatype");
					strcpy(lastloop, "foreach datatype");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					loopwrite[loopwritepos] = write;
					loopwritepos++;
					
					current_datatype = * modeldata->p_datatypes;
					if (current_datatype == NULL) write = 0;
				}
				else if (strcmp(buffer->array, "<?foreach datatypevar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach datatypevar\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach datatypevar");
					strcpy(lastloop, "foreach datatypevar");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					loopwrite[loopwritepos] = write;
					loopwritepos++;
					
					indatatypevar = 1;
					current_datatypevariable = NULL;
					if(current_datatype != NULL) current_datatypevariable = current_datatype->vars;
					if (current_datatypevariable == NULL)
					{
						write = 0;
						indatatypevar = 0;
					}
				}
				else
				{
					if(write) fputs("<", file);
					pos = pos1;
				}
				tmplcode = 0;
				reset_char_array(buffer);
			}
			else if (c == '<' || c == '\n')
			{
				if(write) fputs("<", file);
				pos = pos1;
				tmplcode = 0;
			}
			else
			{
				add_char(buffer, c);
			}
		}
		else
		{
			/* If possible start of template variable */
			if (c == '$' && write == 1)
			{
				reset_char_array(buffer3);
				pos1 = pos;

				while (strcmp(buffer3->array, "$number_messagesplusone") != 0 && strcmp(buffer3->array, "$number_xagentsplusone") != 0 && strcmp(buffer3->array, "$model_name") != 0&& pos <= (pos1 + 24))
				{
					add_char(buffer3, c);
					pos++;
					c = filebuffer->array[pos];
				}
				if (strcmp(buffer3->array, "$model_name") == 0) fputs(modeldata->name, file);
				else
				if (strcmp(buffer3->array, "$number_messagesplusone") == 0)
				{
					sprintf(data, "%i", modeldata->number_messages + 1);
					fputs(data, file);
				}
				else if (strcmp(buffer3->array, "$number_xagentsplusone") == 0)
				{
					sprintf(data, "%i", modeldata->number_xmachines + 1);
					fputs(data, file);
				}
				else
				{
					reset_char_array(buffer3);
					pos = pos1;
					c = '$';
				}

				if (strcmp("foreach allvar", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$default_value") != 0 && strcmp(buffer3->array, "$c_type") != 0 && strcmp(buffer3->array, "$notarraytype") != 0 && strcmp(buffer3->array, "$arraylength") != 0 && pos <= (pos1 + 14))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(allvar->name, file);
					else if (strcmp(buffer3->array, "$type") == 0)
						fputs(allvar->type, file);
					else if (strcmp(buffer3->array, "$arraylength") == 0)
					{
						sprintf(data, "%i", allvar->arraylength);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$default_value") == 0)
						fputs(allvar->defaultvalue, file);
					else if (strcmp(buffer3->array, "$c_type") == 0)
						fputs(allvar->c_type, file);
					else if (strcmp(buffer3->array, "$notarraytype") == 0)
						fputs(allvar->typenotarray, file);
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach envvar", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$default_value") != 0 && strcmp(buffer3->array, "$c_type") != 0 && pos <= (pos1 + 14))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_envvar->name, file);
					else if (strcmp(buffer3->array, "$type") == 0)
						fputs(current_envvar->type, file);
					else if (strcmp(buffer3->array, "$default_value") == 0)
						fputs(current_envvar->defaultvalue, file);
					else if (strcmp(buffer3->array, "$c_type") == 0)
						fputs(current_envvar->c_type, file);
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach functionfiles", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$filepath") != 0 && pos <= (pos1 + 9))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$filepath") == 0)
						fputs(current_envfunc->filepath, file);
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach xagent", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$rangevar") != 0 && strcmp(buffer3->array, "$idvar") != 0 && strcmp(buffer3->array, "$xvar") != 0 && strcmp(buffer3->array, "$yvar") != 0 && strcmp(buffer3->array, "$zvar") != 0 && strcmp(buffer3->array, "$var_number") != 0 && strcmp(buffer3->array, "$allvar_name") != 0 && strcmp(buffer3->array, "$xagentcountplusone") != 0 && strcmp(buffer3->array, "$xagent_count") != 0 && strcmp(buffer3->array, "$previous_name") != 0 && pos <= (pos1 + 22))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_xmachine->name, file);
					else if (strcmp(buffer3->array, "$rangevar") == 0)
						fputs(current_xmachine->rangevar, file);
					else if (strcmp(buffer3->array, "$idvar") == 0)
						fputs(current_xmachine->idvar, file);
					else if (strcmp(buffer3->array, "$xvar") == 0)
						fputs(current_xmachine->xvar, file);
					else if (strcmp(buffer3->array, "$yvar") == 0)
						fputs(current_xmachine->yvar, file);
					else if (strcmp(buffer3->array, "$zvar") == 0)
						fputs(current_xmachine->zvar, file);
					else if (strcmp(buffer3->array, "$allvar_name") == 0)
						fputs(allvar->name, file);
					else if (strcmp(buffer3->array, "$var_number") == 0)
					{
						sprintf(data, "%i", current_xmachine->var_number);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$xagentcountplusone") == 0)
					{
						sprintf(data, "%i", xagent_count + 1);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$xagent_count") == 0)
					{
						sprintf(data, "%i", xagent_count);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$previous_name") == 0)
					{
						if(previous_name != NULL)
						{
							sprintf(data, "%s", previous_name);
							fputs(data, file);
						}
					}
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach xagentvar", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$agent_name") != 0 && strcmp(buffer3->array, "$c_type") != 0 && strcmp(buffer3->array, "$mpi_type") != 0 && strcmp(buffer3->array, "$arraylength") != 0 && strcmp(buffer3->array, "$var_count") != 0 && pos <= (pos1 + 14))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_variable->name, file);
					else if (strcmp(buffer3->array, "$type") == 0)
						fputs(current_variable->type, file);
					else if (strcmp(buffer3->array, "$agent_name") == 0)
						fputs(current_xmachine->name, file);
					else if (strcmp(buffer3->array, "$c_type") == 0)
						fputs(current_variable->c_type, file);
					else if (strcmp(buffer3->array, "$mpi_type") == 0)
						fputs(current_variable->mpi_type, file);
					else if (strcmp(buffer3->array, "$arraylength") == 0)
					{
						sprintf(data, "%i", current_variable->arraylength);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$var_count") == 0)
					{
						sprintf(data, "%i", var_count);
						fputs(data, file);
					}
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach message", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$var_number") != 0 && strcmp(buffer3->array, "$message_countplusone") != 0 && strcmp(buffer3->array, "$previous_name") != 0 && strcmp(buffer3->array, "$capsname") != 0 && pos <= (pos1 + 22))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_message->name, file);
					else if (strcmp(buffer3->array, "$capsname") == 0)
					{
						strcpy(data, current_message->name);
						for(i = 0; i < strlen(data); i++) data[i] = (data[i] >= 'a' && data[i] <= 'z')?('A' + data[i] -'a'):data[i];
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$var_number") == 0)
					{
						sprintf(data, "%i", current_message->var_number);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$message_countplusone") == 0)
					{
						sprintf(data, "%i", message_count + 1);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$previous_name") == 0)
					{
						if(previous_name != NULL)
						{
							sprintf(data, "%s", previous_name);
							fputs(data, file);
						}
					}
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach messagevar", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$messagename") != 0 && strcmp(buffer3->array, "$c_type") != 0 && strcmp(buffer3->array, "$mpi_type") != 0 && strcmp(buffer3->array, "$arraylength") != 0 && strcmp(buffer3->array, "$var_count") != 0 && pos <= (pos1 + 14))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_variable->name, file);
					else if (strcmp(buffer3->array, "$type") == 0)
						fputs(current_variable->type, file);
					else if (strcmp(buffer3->array, "$messagename") == 0)
						fputs(current_message->name, file);
					else if (strcmp(buffer3->array, "$c_type") == 0)
						fputs(current_variable->c_type, file);
					else if (strcmp(buffer3->array, "$mpi_type") == 0)
						fputs(current_variable->mpi_type, file);
					else if (strcmp(buffer3->array, "$arraylength") == 0)
					{
						sprintf(data, "%i", current_variable->arraylength);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$var_count") == 0)
					{
						sprintf(data, "%i", var_count);
						fputs(data, file);
					}
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach function", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$note") != 0 && pos <= (pos1 + 12))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_function->name, file);
					else if (strcmp(buffer3->array, "$note") == 0)
						fputs(current_function->note, file);
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach enditfunc", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$code") != 0 && pos <= (pos1 + 5))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$code") == 0)
						fputs(current_code->code, file);
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (strcmp("foreach datatype", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$desc") != 0 && pos <= (pos1 + 5))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_datatype->name, file);
					else if (strcmp(buffer3->array, "$desc") == 0)
						fputs(current_datatype->desc, file);
				}
				else if (strcmp("foreach datatypevar", lastloop) == 0)
				{
					while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$messagename") != 0 && strcmp(buffer3->array, "$c_type") != 0 && strcmp(buffer3->array, "$mpi_type") != 0 && strcmp(buffer3->array, "$arraylength") != 0 && strcmp(buffer3->array, "$var_count") != 0 && strcmp(buffer3->array, "$datatypevarname") != 0 && strcmp(buffer3->array, "$default_value") != 0 && strcmp(buffer3->array, "$agent_name") != 0 && pos <= (pos1 + 16))
					{
						add_char(buffer3, c);
						pos++;
						c = filebuffer->array[pos];
					}
					pos--;
					if (strcmp(buffer3->array, "$name") == 0)
						fputs(current_datatypevariable->name, file);
					else if (strcmp(buffer3->array, "$datatypevarname") == 0)
					{
						if(inallvar) fputs(allvar->name, file);
						if(inxagentvar) fputs(current_variable->name, file);
					}
					else if (strcmp(buffer3->array, "$agent_name") == 0)
						fputs(current_xmachine->name, file);
					else if (strcmp(buffer3->array, "$default_value") == 0)
						fputs(current_datatypevariable->defaultvalue, file);
					else if (strcmp(buffer3->array, "$type") == 0)
						fputs(current_datatypevariable->type, file);
					else if (strcmp(buffer3->array, "$messagename") == 0)
						fputs(current_datatypevariable->name, file);
					else if (strcmp(buffer3->array, "$c_type") == 0)
						fputs(current_datatypevariable->c_type, file);
					else if (strcmp(buffer3->array, "$mpi_type") == 0)
						fputs(current_datatypevariable->mpi_type, file);
					else if (strcmp(buffer3->array, "$arraylength") == 0)
					{
						sprintf(data, "%i", current_datatypevariable->arraylength);
						fputs(data, file);
					}
					else if (strcmp(buffer3->array, "$var_count") == 0)
					{
						sprintf(data, "%i", var_count);
						fputs(data, file);
					}
					else
					{
						fputs("$", file);
						pos = pos1;
					}
				}
				else if (write)
					fputc(c, file);

			}
			else if (c == '<')
			{
				tmplcode = 1;
				reset_char_array(buffer);
				add_char(buffer, '<');
				pos1 = pos;
			}
			else if (write)
				fputc(c, file);
		}
		
		pos++;
	}

	if (numtag != 0)
		printf("error: template tag nest not closed, numtag = %d\n", numtag);

	/* Close the files */
	fclose(tmplfile);
	fclose(file);

	/* Clear memory */
	free_char_array(buffer);
	free_char_array(buffer3);
	free_char_array(filebuffer);
}

/** \fn parseAgentHeaderTemplate(char * directory, model_data * modeldata)
 * \brief Produces header files for specific agent types.
 * \param directory The directory to write the files to.
 * \param modeldata The model data.
 */
void parseAgentHeaderTemplate(char * directory, model_data * modeldata)
{
	FILE *file;
	char filename[100];
	char buffer[1000];
	int i;
	xmachine * current_xmachine = * modeldata->p_xmachines;
	variable * current_variable;
	
	while(current_xmachine)
	{
		/* Open the output file */
		strcpy(filename, directory);
		strcat(filename, current_xmachine->name);
		strcat(filename, "_agent_header.h");
		printf("writing file: %s\n", filename);
		file = fopen(filename, "w");
		
		fputs("/**\n", file);
		fputs(" * \\file  ", file);
		fputs(current_xmachine->name, file);
		fputs("_agent_header.h\n", file);
		fputs(" * \\brief Header for agent type memory access.\n", file);
		fputs(" */\n\n", file);
		
		current_variable = current_xmachine->memory->vars;
		while(current_variable)
		{
			strcpy(buffer, current_variable->name);
			for(i = 0; i < strlen(buffer); i++) buffer[i] = (buffer[i] >= 'a' && buffer[i] <= 'z')?('A' + buffer[i] -'a'):buffer[i];
			fputs("/** \\def ", file);
			fputs(buffer, file);
			fputs("\n", file);
			fputs(" * \\brief Direct access to ", file);
			fputs(current_variable->name, file);
			fputs(" of ", file);
			fputs(current_xmachine->name, file);
			fputs(" agent memory variable. */\n", file);
			fputs("#define ", file);
			fputs(buffer, file);
			fputs(" current_xmachine->xmachine_", file);
			fputs(current_xmachine->name, file);
			fputs("->", file);
			fputs(current_variable->name, file);
			fputs("\n", file);
			
			current_variable = current_variable->next;
		}
		
		/* Close the files */
		fclose(file);
		
		current_xmachine = current_xmachine->next;
	}
}
