#include "header.h"

void writeRule(rule_data * current_rule_data, FILE *file)
{
	char data[100];
	
	if(current_rule_data->time_rule == 1)
	{
		if(current_rule_data->not == 1) fputs("!", file);
		fputs("(", file);
		fputs("iteration_loop", file);
		fputs(current_rule_data->op, file);
		fputs(" == ", file);
		fputs(current_rule_data->rhs, file);
		fputs(")", file);
	}
	else
	{
		if(current_rule_data->not == 1) fputs("!", file);
		fputs("(", file);
		/* If rule operator is 'IN' i.e. integer value within an integer list */
		if(strcmp(current_rule_data->op, "IN") == 0)
		{
			fputs("FLAME_integer_in_array(", file);
			fputs(current_rule_data->lhs, file);
			fputs(", ", file);
			fputs(current_rule_data->rhs, file);
			if(current_rule_data->rhs_variable->arraylength == -1)
			{
				fputs(".array", file);
			}
			fputs(", ", file);
			/* If array is dynamic */
			if(current_rule_data->rhs_variable->arraylength == -1)
			{
				fputs(current_rule_data->rhs, file);
				fputs(".size", file);
			}
			/* If array is static */
			if(current_rule_data->rhs_variable->arraylength > 0)
			{
				sprintf(data, "%i", current_rule_data->rhs_variable->arraylength);
				fputs(data, file);
			}
			fputs(")", file);
		}
		else
		{
			if(current_rule_data->lhs == NULL) writeRule(current_rule_data->lhs_rule, file);
			else fputs(current_rule_data->lhs, file);
			fputs(" ", file);
			fputs(current_rule_data->op, file);
			fputs(" ", file);
			if(current_rule_data->rhs == NULL) writeRule(current_rule_data->rhs_rule, file);
			else fputs(current_rule_data->rhs, file);
		}
		fputs(")", file);
	}
}

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
	/*int lastwrite = 1;
	int loopwrite[100];
	int loopwritepos = 0;*/
	int writetag[100];
	int log = 0;
	int var_count;
	int message_count;
	int xagent_count;
	int found;
	char_array * buffer = init_char_array();
	char_array * buffer3 = init_char_array();
	char_array * filebuffer = init_char_array();
	char * previous_name;
	int inenvvar = 0;
	int inallvar = 0;
	int inxagentvar = 0;
	int indatatypevar = 0;
	int inmessagevar = 0;

	/* pointers to model datatypes */
	xmachine * current_xmachine;
	xmachine_message * current_message = NULL;
	xmachine_function * current_function;
	xmachine_state * current_state;
	xmachine_ioput * current_ioput;
	variable * current_variable;
	f_code * current_code;
	function_pointer * current_function_pointer;
	layer * current_layer = NULL;
	env_func * current_envfunc;
	variable * allvar;
	variable * current_envvar;
	variable * current_datatypevariable = NULL;
	time_data * current_time_unit = NULL;
	model_datatype * current_datatype = NULL;
	xmachine_state_holder * current_end_state = NULL;
	sync * current_sync = NULL;
	sync_pointer * current_sync_pointer;

	/* Initialise variables */
	lastloop[0] = '\0';
	writetag[0] = 1;

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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if has_arrays?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_datatype != NULL)
					{
						if (current_datatype->has_arrays == 0) write = 0;
					}
					writetag[numtag] = write;
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
					else if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (current_variable->arraylength != -1)
							write = 0;
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (current_variable->arraylength < 1)
							write = 0;
					if (indatatypevar)
					{
						if (current_datatypevariable->arraylength < 1)
							write = 0;
					}
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (current_variable->arraylength > 0)
							write = 0;
					if (indatatypevar)
					{
						if (current_datatypevariable->arraylength > 0)
							write = 0;
					}
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (current_variable->arraylength != 0)
							write = 0;
					if (indatatypevar)
					{
						if (current_datatypevariable->arraylength != 0)
							write = 0;
					}
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (current_variable->arraylength == 0)
							write = 0;
					if (indatatypevar)
					{
						if (current_datatypevariable->arraylength == 0)
							write = 0;
					}
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
					{
						if (current_variable->ismodeldatatype == 0) write = 0;
					}
					if (indatatypevar)
					{
						if (current_datatypevariable->ismodeldatatype == 0) write = 0;
					}
					writetag[numtag] = write;
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
					if (inxagentvar || inmessagevar)
						if(current_variable != NULL) if (current_variable->ismodeldatatype == 1)
							write = 0;
					if (indatatypevar)
						if (current_datatypevariable->ismodeldatatype == 1)
							write = 0;
					writetag[numtag] = write;
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
						if (inenvvar)
							if (strcmp(current_envvar->type, "char") != 0 && strcmp(current_envvar->type, "char_array") != 0)
								write = 0;
						if (inallvar)
							if (strcmp(allvar->type, "char") != 0 && strcmp(allvar->type, "char_array") != 0)
								write = 0;
						if ((inxagentvar || inmessagevar) && current_variable != NULL)
							if (strcmp(current_variable->type, "char") != 0 && strcmp(current_variable->type, "char_array") != 0)
								write = 0;
					}
					writetag[numtag] = write;
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
						if (inenvvar)
							if (strcmp(current_envvar->type, "char") == 0 || strcmp(current_envvar->type, "char_array") == 0)
								write = 0;
						if (inallvar)
							if (strcmp(allvar->type, "char") == 0 || strcmp(allvar->type, "char_array") == 0)
								write = 0;
						if ((inxagentvar || inmessagevar) && current_variable != NULL)
							if (strcmp(current_variable->type, "char") == 0 || strcmp(current_variable->type, "char_array") == 0)
								write = 0;
					}
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (strcmp(current_variable->type, "char_array") != 0)
							write = 0;
					if (indatatypevar)
						if (strcmp(current_datatypevariable->type, "char_array") != 0 &&
								strcmp(current_datatypevariable->type, "char") != 0)
							write = 0;
					writetag[numtag] = write;
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
					if ((inxagentvar || inmessagevar) && current_variable != NULL)
						if (strcmp(current_variable->type, "char_array") == 0)
							write = 0;
					if (indatatypevar)
						if (strcmp(current_datatypevariable->type, "char_array") == 0 ||
								strcmp(current_datatypevariable->type, "char") == 0)
							write = 0;
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					{
						if(current_variable != NULL)
						{
							if (current_variable->next == NULL) write = 0;
						}
						else write = 0;
					}
					if (strcmp(lastloop, "foreach message") == 0)
						if (current_message->next == NULL)
							write = 0;
					if (strcmp(lastloop, "foreach messagevar") == 0)
					{
						if(current_variable != NULL)
						{
							if (current_variable->next == NULL)
							write = 0;
						}
						else write = 0;
					}
					if (strcmp(lastloop, "foreach function") == 0)
						if (current_function->next == NULL && current_xmachine->next == NULL)
							write = 0;
					if(strcmp(lastloop, "foreach datatypevar") == 0)
					{
						if(current_datatypevariable != NULL)
						{
							if (current_datatypevariable->next == NULL) write = 0;
						}
						else write = 0;
					}
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					current_variable = current_xmachine->variables;
					while (current_variable)
					{
						if (strcmp(current_variable->name, allvar->name) == 0)
							found = 1;

						current_variable = current_variable->next;
					}
					if (found == 0)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if use_xvar?>") == 0)
				{
					if (log)
						printf("start :%d\tif use_xvar\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(current_xmachine->xvar, "0.0") == 0)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if no_xvar?>") == 0)
				{
					if (log)
						printf("start :%d\tif no_xvar\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(current_xmachine->xvar, "0.0") != 0)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if use_yvar?>") == 0)
				{
					if (log)
						printf("start :%d\tif use_yvar\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(current_xmachine->yvar, "0.0") == 0)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if no_yvar?>") == 0)
				{
					if (log)
						printf("start :%d\tif no_yvar\n", numtag);
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if (strcmp(current_xmachine->yvar, "0.0") != 0)
						write = 0;
					writetag[numtag] = write;
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
					writetag[numtag] = write;
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
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if single_vars?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_datatype != NULL)
					{
						if (current_datatype->has_single_vars == 0) write = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if has_dynamic_arrays?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_datatype != NULL)
					{
						if (current_datatype->has_dynamic_arrays == 0) write = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if no_dynamic_arrays?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_datatype != NULL)
					{
						if (current_datatype->has_dynamic_arrays == 1) write = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if condition?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_function != NULL)
					{
						if (current_function->condition_function == NULL) write = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if filter?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;

					if(current_ioput != NULL)
					{
						if (current_ioput->filter_rule == NULL) write = 0;
					}
					else write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if no_filter?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;

					if(current_ioput != NULL)
					{
						if (current_ioput->filter_function != NULL) write = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if sync_filter?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					
					if(current_sync != NULL)
					{
						if(current_sync->has_agent_and_message_vars == 0) write = 0;
					}
					else write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if function_input?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_function != NULL)
					{
						if(current_function->inputs == NULL)
						{
							write = 0;
						}
					}
					else write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if has_agent_var?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_ioput != NULL)
					{
						if(current_ioput->filter_rule != NULL)
						{
							if(current_ioput->filter_rule->has_agent_var == 0) write = 0;
						}
					}
					else if(current_function != NULL)
					{
						if(current_function->filter_rule != NULL)
						{
							if(current_function->filter_rule->has_agent_var == 0) write = 0;
						}
						else if(current_function->has_agent_var == 0) write = 0;
					}
					else if(current_xmachine != NULL)
					{
						if(current_xmachine->variables == NULL) write = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?if has_message_var?>") == 0)
				{
					strcpy(&chartag[numtag][0], "if");
					if (write == 1)
						lastiftag = numtag;
					numtag++;
					if(current_ioput != NULL)
					{
						if(current_ioput->filter_rule != NULL)
						{
							if (current_ioput->filter_rule->has_message_var == 0) write = 0;
						}
					}
					else if(current_function != NULL)
					{
						 if(current_function->has_agent_var == 0) write = 0;
					}
					writetag[numtag] = write;
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
					/* if corresponds with first if*/
					if (log)
						printf("\tnumtag:%d\tlastiftag:%d\twrite:%d\tpos:%d\n", numtag, lastiftag, write, pos);
					if (numtag == lastiftag)
					{
						/*write = 1;
						lastwrite = 1;*/
						lastiftag = -1;
					}
					write = writetag[numtag];
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
						{
							exitforeach = 1;
							inenvvar = 0;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
						}
					}
					else if (strcmp("foreach xagent", &chartag[numtag][0]) == 0)
					{
						if (current_xmachine != NULL)
						{
							previous_name = current_xmachine->name;
							current_xmachine = current_xmachine->next;
						}
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
					else if (strcmp("foreach state", &chartag[numtag][0]) == 0)
					{
						if (current_state != NULL)
						{
							current_state = current_state->next;
						}
						if (current_state == NULL)
						{
							exitforeach = 1;
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
						if (current_message != NULL)
						{
							previous_name = current_message->name;
							current_message = current_message->next;
						}
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
						{
							current_variable = current_variable->next;
							if(current_variable != NULL) current_datatype = current_variable->datatype;
						}
						if (current_variable == NULL)
						{
							exitforeach = 1;
							inmessagevar = 0;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach sync", &chartag[numtag][0]) == 0)
					{
						if (current_sync != NULL)
						{
							current_sync = current_sync->next;
						}
						if (current_sync == NULL)
						{
							exitforeach = 1;
							inmessagevar = 0;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach syncvar", &chartag[numtag][0]) == 0)
					{
						if (current_variable != NULL)
						{
							current_variable = current_variable->next;
						}
						if (current_variable == NULL)
						{
							exitforeach = 1;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach paramvar", &chartag[numtag][0]) == 0)
					{
						if (current_variable != NULL)
						{
							current_variable = current_variable->next;
						}
						if (current_variable == NULL)
						{
							if(current_xmachine != NULL)
								current_xmachine = current_xmachine->next;
							if(current_xmachine != NULL)
								current_variable = current_xmachine->variables;
						}
						if (current_variable == NULL)
						{
							exitforeach = 1;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach layer", &chartag[numtag][0]) == 0)
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
							current_function_pointer = current_layer->functions;
							current_function = current_function_pointer->function;
						}
					}
					else if (strcmp("foreach function_input", &chartag[numtag][0]) == 0)
					{
						if (current_ioput != NULL)
						{
							current_ioput = current_ioput->next;

							if(current_sync != NULL)
							{
								while(current_ioput != NULL &&
										strcmp(current_ioput->messagetype, current_sync->message->name) != 0)
								{
									current_ioput = current_ioput->next;
								}
							}
						}
						if (current_ioput == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach function_output", &chartag[numtag][0]) == 0)
					{
						if (current_ioput != NULL)
							current_ioput = current_ioput->next;
						if (current_ioput == NULL)
							exitforeach = 1;
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach complete_sync", &chartag[numtag][0]) == 0)
					{
						if (current_sync_pointer != NULL)
							current_sync_pointer = current_sync_pointer->next;
						if (current_sync_pointer == NULL)
						{
							current_sync = NULL;
							exitforeach = 1;
						}
						else
						{
							current_sync = current_sync_pointer->current_sync;
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach start_sync", &chartag[numtag][0]) == 0)
					{
						if (current_sync_pointer != NULL)
							current_sync_pointer = current_sync_pointer->next;
						if (current_sync_pointer == NULL)
						{
							current_sync = NULL;
							exitforeach = 1;
						}
						else
						{
							current_sync = current_sync_pointer->current_sync;
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach function", &chartag[numtag][0]) == 0)
					{
						if (current_layer != NULL)// || current_sync != NULL)
						{
							//if(current_layer != NULL) printf("current_layer: %d\n", current_layer->number);

							if (current_function_pointer != NULL)
							{
								//printf("foreach function: %s next: ", current_function_pointer->function->name);

								current_function_pointer = current_function_pointer->next;

								//if (current_function_pointer == NULL) printf("NULL\n");
								//else printf("%s\n", current_function_pointer->function->name);
							}
							if (current_function_pointer == NULL)
							{
								//printf("foreach function: NULL\n");
								current_function = NULL;
								exitforeach = 1;
							}
							else
							{
							 	current_function = current_function_pointer->function;
								pos = looppos[numtag];
								numtag++;
								var_count++;
							}
						}
						else
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
					else if (strcmp("foreach timeunit", &chartag[numtag][0]) == 0)
					{
						if (current_time_unit != NULL)
							current_time_unit = current_time_unit->next;
						if (current_time_unit == NULL)
						{
							exitforeach = 1;
						}
						else
						{
							pos = looppos[numtag];
							numtag++;
							var_count++;
						}
					}
					else if (strcmp("foreach endstate", &chartag[numtag][0]) == 0)
					{
						if (current_end_state != NULL)
							current_end_state = current_end_state->next;
						if (current_end_state == NULL)
						{
							exitforeach = 1;
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

						/*if(loopwritepos > 0)
						{
							loopwritepos--;
							write = loopwrite[loopwritepos];
						}
						else write = lastwrite;*/
						write = writetag[numtag];

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
					/*lastwrite = write;*/

					current_envfunc = * modeldata->p_envfuncs;
					if (current_envfunc == NULL)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach allvar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach allvar\tpos: %d\n", numtag, looppos[numtag]);
					strcpy(&chartag[numtag][0], "foreach allvar");
					strcpy(lastloop, "foreach allvar");
					looppos[numtag] = pos;
					numtag++;
					/*lastwrite = write;*/
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
					/*lastwrite = write;*/
					inenvvar = 1;
					current_envvar = * modeldata->p_envvars;
					if (current_envvar == NULL)
					{
						write = 0;
						inenvvar = 0;
					}
					writetag[numtag] = write;
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
					/*lastwrite = write;*/
					previous_name = NULL;

					if(current_sync != NULL)
					{
						current_xmachine = current_sync->agents;
					}
					else current_xmachine = * modeldata->p_xmachines;

					if (current_xmachine == NULL)
						write = 0;
					writetag[numtag] = write;
					strcpy(lastloop, "foreach xagent");
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
					/*lastwrite = write;*/
					inxagentvar = 1;

					if(current_xmachine != NULL)
						current_variable = current_xmachine->variables;
					else current_variable = NULL;

					if(current_variable != NULL)
					{
						current_datatype = current_variable->datatype;
					}
					if (current_variable == NULL)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach state?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach state\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach state");
					strcpy(lastloop, "foreach state");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/

					if(current_xmachine == NULL) current_state = NULL;
					else current_state = current_xmachine->states;

					if (current_state == NULL) write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach function_input?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach function_input\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach function_input");

					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/

					if(current_function != NULL) current_ioput = current_function->inputs;
					else current_ioput = NULL;

					if(current_sync != NULL)
					{
						while(current_ioput != NULL && strcmp(current_ioput->messagetype, current_sync->message->name) != 0)
						{
							current_ioput = current_ioput->next;
						}

						//printf("filter messagetype = %s\tsync messagetype = %s\n", current_ioput->messagetype, current_sync->message->name);
					}

					strcpy(lastloop, "foreach function_input");

					if (current_ioput == NULL)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach function_output?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach function_output\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach function_output");
					strcpy(lastloop, "foreach function_output");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					current_ioput = current_function->outputs;
					if (current_ioput == NULL)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach complete_sync?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach complete_sync\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach complete_sync");
					strcpy(lastloop, "foreach complete_sync");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					current_sync_pointer = current_function->complete_syncs;
					if (current_sync_pointer == NULL) write = 0;
					else current_sync = current_sync_pointer->current_sync;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach start_sync?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach start_sync\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach start_sync");
					strcpy(lastloop, "foreach start_sync");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					if(current_function != NULL) current_sync_pointer = current_function->start_syncs;
					else current_sync_pointer = NULL;
					if (current_sync_pointer == NULL)
						write = 0;
					else current_sync = current_sync_pointer->current_sync;
					writetag[numtag] = write;
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
					/*lastwrite = write;*/
					previous_name = NULL;

					current_message = * modeldata->p_xmessages;
					if (current_message == NULL)
						write = 0;
					writetag[numtag] = write;
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
					/*lastwrite = write;*/
					inmessagevar = 1;
					if(current_message != NULL) current_variable = current_message->vars;
					else current_variable = NULL;
					if(current_variable != NULL)
					{
						current_datatype = current_variable->datatype;
					}
					if (current_variable == NULL)
						write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach sync?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach sync\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach sync");
					strcpy(lastloop, "foreach sync");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/

					if(current_message != NULL) current_sync = current_message->syncs;
					else current_sync = NULL;
					if(current_sync == NULL) write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach syncvar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach syncvar\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach syncvar");
					strcpy(lastloop, "foreach syncvar");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/

					if(current_sync != NULL) current_variable = current_sync->vars;
					else current_variable = NULL;
					if(current_variable == NULL) write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach paramvar?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach paramvar\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach paramvar");
					strcpy(lastloop, "foreach paramvar");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					/*if(current_message != NULL)
					{
						if(current_message->agents != NULL)
						{
							current_xmachine = current_message->agents;
							current_variable = current_xmachine->variables;
						}
					}
					else*/ current_variable = NULL;
					if (current_variable == NULL) write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach layer?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach layer\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach layer");
					strcpy(lastloop, "foreach layer");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/

					current_layer = * modeldata->p_layers;
					if (current_layer == NULL)
						write = 0;
					else
					{
						current_function_pointer = current_layer->functions;
						current_function = current_function_pointer->function;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach function?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach function\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach function");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/

					if (strcmp(lastloop, "foreach sync") == 0)
					{
						if(current_sync != NULL)
						{
							current_function_pointer = current_sync->inputting_functions;
							if (current_function_pointer == NULL) write = 0;
							else current_function = current_function_pointer->function;
						}
						else
						{
							current_function_pointer = NULL;
							current_function = NULL;
							write = 0;
						}
					}
					else if (strcmp(lastloop, "foreach layer") == 0)
					{
						//printf("layer: %d -> %p\n", current_layer->number, (void *)current_layer->functions);

						current_function_pointer = current_layer->functions;
						if (current_function_pointer == NULL) write = 0;
						else current_function = current_function_pointer->function;
					}
					else if (strcmp(lastloop, "foreach xagent") == 0)
					{
						if(current_xmachine != NULL)
						{
							current_function = current_xmachine->functions;
							/*current_ioput = current_function->filter_rule;*/
						}
						else current_function = NULL;
						if (current_function == NULL) write = 0;
					}

					strcpy(lastloop, "foreach function");
					
					writetag[numtag] = write;
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
					/*lastwrite = write;*/

					current_code = * modeldata->p_it_end_code;
					if (current_code == NULL)
						write = 0;
					writetag[numtag] = write;
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
					/*loopwrite[loopwritepos] = write;
					loopwritepos++;*/

					current_datatype = * modeldata->p_datatypes;
					if (current_datatype == NULL) write = 0;
					writetag[numtag] = write;
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
					/*loopwrite[loopwritepos] = write;
					loopwritepos++;*/

					indatatypevar = 1;
					current_datatypevariable = NULL;
					if(current_datatype != NULL) current_datatypevariable = current_datatype->vars;
					if (current_datatypevariable == NULL)
					{
						write = 0;
						indatatypevar = 0;
					}
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach timeunit?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach timeunit\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach timeunit");
					strcpy(lastloop, "foreach timeunit");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					/*loopwrite[loopwritepos] = write;
					loopwritepos++;*/

					current_time_unit = * modeldata->p_time_units;
					if (current_time_unit == NULL) write = 0;
					writetag[numtag] = write;
				}
				else if (strcmp(buffer->array, "<?foreach endstate?>") == 0)
				{
					if (log)
						printf("start :%d\tforeach endstate\tpos: %d\n", numtag, pos);
					strcpy(&chartag[numtag][0], "foreach endstate");
					strcpy(lastloop, "foreach endstate");
					looppos[numtag] = pos;
					numtag++;
					var_count = 0;
					/*lastwrite = write;*/
					/*loopwrite[loopwritepos] = write;
					loopwritepos++;*/

					current_end_state = current_xmachine->end_states;
					if (current_end_state == NULL) write = 0;
					writetag[numtag] = write;
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

				while (strcmp(buffer3->array, "$number_messagesplusone") != 0 &&
					strcmp(buffer3->array, "$number_xagentsplusone") != 0 &&
					strcmp(buffer3->array, "$model_name") != 0 &&
					strcmp(buffer3->array, "$xagent_total") != 0 &&
					pos <= (pos1 + 24) && pos <= filebuffer->size)
				{
					add_char(buffer3, c);
					pos++;
					c = filebuffer->array[pos];
				}
				pos--;
				if (strcmp(buffer3->array, "$model_name") == 0) fputs(modeldata->name, file);
				else if (strcmp(buffer3->array, "$number_messagesplusone") == 0)
				{
					sprintf(data, "%i", modeldata->number_messages + 1);
					fputs(data, file);
				}
				else if (strcmp(buffer3->array, "$number_xagentsplusone") == 0)
				{
					sprintf(data, "%i", modeldata->number_xmachines + 1);
					fputs(data, file);
				}
				else if (strcmp(buffer3->array, "$xagent_total") == 0)
				{
					sprintf(data, "%i", modeldata->number_xmachines);
					fputs(data, file);
				}
				else
				{
					reset_char_array(buffer3);
					pos = pos1;
					c = '$';

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
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$uc_name") != 0 && strcmp(buffer3->array, "$default_value") != 0 && strcmp(buffer3->array, "$c_type") != 0 && pos <= (pos1 + 14))
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
						else if (strcmp(buffer3->array, "$uc_name") == 0)
						{
							strcpy(data, current_envvar->name);
							for(i = 0; i < (int)strlen(data); i++) data[i] = (data[i] >= 'a' && data[i] <= 'z')?('A' + data[i] -'a'):data[i];
							fputs(data, file);
						}
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
						while (strcmp(buffer3->array, "$name") != 0 &&
								strcmp(buffer3->array, "$rangevar") != 0 &&
								strcmp(buffer3->array, "$idvar") != 0 &&
								strcmp(buffer3->array, "$xvar") != 0 &&
								strcmp(buffer3->array, "$yvar") != 0 &&
								strcmp(buffer3->array, "$zvar") != 0 &&
								strcmp(buffer3->array, "$var_number") != 0 &&
								strcmp(buffer3->array, "$allvar_name") != 0 &&
								strcmp(buffer3->array, "$xagentcountplusone") != 0 &&
								strcmp(buffer3->array, "$xagent_count") != 0 &&
								strcmp(buffer3->array, "$previous_name") != 0 &&
								strcmp(buffer3->array, "$start_state") != 0 &&
								strcmp(buffer3->array, "$message_name") != 0 &&
								strcmp(buffer3->array, "$layer") != 0 &&
								pos <= (pos1 + 22))
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
						else if (strcmp(buffer3->array, "$start_state") == 0)
							fputs(current_xmachine->start_state->name, file);
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
						else if (strcmp(buffer3->array, "$message_name") == 0)
							fputs(current_message->name, file);
						else if (strcmp(buffer3->array, "$layer") == 0)
						{
							if(current_sync->inputting_functions == NULL) fputs("unknown", file);
							else
							{
								sprintf(data, "%i", current_sync->inputting_functions->function->rank_in);
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
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$agent_name") != 0 && strcmp(buffer3->array, "$c_type") != 0 && strcmp(buffer3->array, "$mpi_type") != 0 && strcmp(buffer3->array, "$arraylength") != 0 && strcmp(buffer3->array, "$var_count") != 0 && strcmp(buffer3->array, "$notarraytype") != 0 && strcmp(buffer3->array, "$default_value") != 0 && pos <= (pos1 + 15))
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
						else if (strcmp(buffer3->array, "$notarraytype") == 0)
							fputs(current_variable->typenotarray, file);
						else if (strcmp(buffer3->array, "$default_value") == 0)
							fputs(current_variable->defaultvalue, file);
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach state", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 &&
								strcmp(buffer3->array, "$agent_name") != 0 &&
								strcmp(buffer3->array, "$agent_start") != 0 &&
								strcmp(buffer3->array, "$xagent_count") != 0 &&
								pos <= (pos1 + 15))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$name") == 0)
							fputs(current_state->name, file);
						else if (strcmp(buffer3->array, "$agent_name") == 0)
							fputs(current_state->agent_name, file);
						else if (strcmp(buffer3->array, "$agent_start") == 0)
							fputs(current_xmachine->start_state->name, file);
						else if (strcmp(buffer3->array, "$xagent_count") == 0)
						{
							sprintf(data, "%i", xagent_count);
							fputs(data, file);
						}
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach endstate", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$agent_name") != 0 && strcmp(buffer3->array, "$agent_start") != 0 && pos <= (pos1 + 15))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$name") == 0)
							fputs(current_end_state->state->name, file);
						else if (strcmp(buffer3->array, "$agent_name") == 0)
							fputs(current_xmachine->name, file);
						else if (strcmp(buffer3->array, "$agent_start") == 0)
							fputs(current_xmachine->start_state->name, file);
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach function_input", lastloop) == 0 ||
							strcmp("foreach function_output", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$agent_name") != 0 &&
								strcmp(buffer3->array, "$filter") != 0 && strcmp(buffer3->array, "$rule") != 0 && pos <= (pos1 + 15))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$name") == 0)
							fputs(current_ioput->messagetype, file);
						else if (strcmp(buffer3->array, "$agent_name") == 0)
							fputs(current_function->agent_name, file);
						else if (strcmp(buffer3->array, "$filter") == 0)
							fputs(current_ioput->filter_function, file);
						else if (strcmp(buffer3->array, "$rule") == 0)
							writeRule(current_ioput->filter_rule, file);
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach start_sync", lastloop) == 0 ||
							strcmp("foreach complete_sync", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$message_name") != 0 &&
								strcmp(buffer3->array, "$filter_name") != 0 &&
								pos <= (pos1 + 15))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$message_name") == 0)
							fputs(current_sync->message->name, file);
						else if (strcmp(buffer3->array, "$filter_name") == 0)
							fputs(current_sync->name, file);
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
							for(i = 0; i < (int)strlen(data); i++) data[i] = (data[i] >= 'a' && data[i] <= 'z')?('A' + data[i] -'a'):data[i];
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
					else if (strcmp("foreach sync", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 &&
								strcmp(buffer3->array, "$messagename") != 0 &&
								strcmp(buffer3->array, "$layer") != 0 &&
								strcmp(buffer3->array, "$filteragentcount") != 0 &&
								pos <= (pos1 + 20))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$name") == 0)
							fputs(current_sync->name, file);
						else if (strcmp(buffer3->array, "$messagename") == 0)
							fputs(current_sync->message->name, file);
						else if (strcmp(buffer3->array, "$layer") == 0)
						{
							if(current_sync->inputting_functions == NULL) fputs("unknown", file);
							else
							{
								sprintf(data, "%i", current_sync->inputting_functions->function->rank_in);
								fputs(data, file);
							}
						}
						else if (strcmp(buffer3->array, "$filteragentcount") == 0)
						{
							sprintf(data, "%i", current_sync->filter_agent_count);
							fputs(data, file);
						}
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach syncvar", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$type") != 0 &&
								pos <= (pos1 + 14))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$type") == 0)
							fputs(current_variable->type, file);
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach paramvar", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0
								&& strcmp(buffer3->array, "$agent_name") != 0 && strcmp(buffer3->array, "$c_type") != 0 && pos <= (pos1 + 14))
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
							fputs(current_variable->agent->name, file);
						else if (strcmp(buffer3->array, "$c_type") == 0)
							fputs(current_variable->c_type, file);
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if (strcmp("foreach function", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$note") != 0 && strcmp(buffer3->array, "$agent_name") != 0 &&
						strcmp(buffer3->array, "$current_state") != 0 && strcmp(buffer3->array, "$next_state") != 0 && strcmp(buffer3->array, "$condition") != 0 &&
						strcmp(buffer3->array, "$message_name") != 0 &&
						strcmp(buffer3->array, "$layer") != 0 &&
						strcmp(buffer3->array, "$filter_rule") != 0 &&
						strcmp(buffer3->array, "$rule") != 0 && pos <= (pos1 + 15))
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
						else if (strcmp(buffer3->array, "$agent_name") == 0)
							fputs(current_function->agent_name, file);
						else if (strcmp(buffer3->array, "$current_state") == 0)
							fputs(current_function->current_state, file);
						else if (strcmp(buffer3->array, "$next_state") == 0)
							fputs(current_function->next_state, file);
						else if (strcmp(buffer3->array, "$condition") == 0)
						{
							if(current_function->condition_function == NULL) fputs("1", file);
							else fputs(current_function->condition_function, file);
						}
						else if (strcmp(buffer3->array, "$rule") == 0)
						{
							writeRule(current_function->condition_rule, file);
						}
						else if (strcmp(buffer3->array, "$message_name") == 0)
							fputs(current_message->name, file);
						else if (strcmp(buffer3->array, "$layer") == 0)
						{
							if(current_sync->inputting_functions == NULL) fputs("unknown", file);
							else
							{
								sprintf(data, "%i", current_sync->inputting_functions->function->rank_in);
								fputs(data, file);
							}
						}
						else if (strcmp(buffer3->array, "$filter_rule") == 0)
						{
							writeRule(current_function->filter_rule, file);
						}
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
					else if (strcmp("foreach timeunit", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$unit_name") != 0 && strcmp(buffer3->array, "$period") != 0 && pos <= (pos1 + 11))
						{
							add_char(buffer3, c);
							pos++;
							c = filebuffer->array[pos];
						}
						pos--;
						if (strcmp(buffer3->array, "$name") == 0)
							fputs(current_time_unit->name, file);
						else if (strcmp(buffer3->array, "$unit_name") == 0)
						{
							if(current_time_unit->unit == NULL) fputs("ITERATION", file);
							else fputs(current_time_unit->unit->name, file);
						}
						else if (strcmp(buffer3->array, "$period") == 0)
						{
							if(current_time_unit->unit != NULL)
							{
								sprintf(data, "%i", current_time_unit->period * current_time_unit->unit->period);
								fputs(data, file);
							}
							else fputs("ITERATION", file);
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
							if(current_datatype->desc != NULL) fputs(current_datatype->desc, file);
					}
					else if (strcmp("foreach datatypevar", lastloop) == 0)
					{
						while (strcmp(buffer3->array, "$name") != 0 && strcmp(buffer3->array, "$type") != 0 && strcmp(buffer3->array, "$messagename") != 0 && strcmp(buffer3->array, "$c_type") != 0 && strcmp(buffer3->array, "$mpi_type") != 0 && strcmp(buffer3->array, "$arraylength") != 0 && strcmp(buffer3->array, "$var_count") != 0 && strcmp(buffer3->array, "$datatypevarname") != 0 && strcmp(buffer3->array, "$default_value") != 0 && strcmp(buffer3->array, "$agent_name") != 0 && strcmp(buffer3->array, "$notarraytype") != 0 && pos <= (pos1 + 16))
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
						else if (strcmp(buffer3->array, "$notarraytype") == 0)
							fputs(current_datatypevariable->typenotarray, file);
						else
						{
							fputs("$", file);
							pos = pos1;
						}
					}
					else if(write) fputc(c, file);
				}
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
	char filename[1000];
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

		current_variable = current_xmachine->variables;
		while(current_variable)
		{
			strcpy(buffer, current_variable->name);
			for(i = 0; i < (int)strlen(buffer); i++) buffer[i] = (buffer[i] >= 'a' && buffer[i] <= 'z')?('A' + buffer[i] -'a'):buffer[i];
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
			fputs(" (current_xmachine_", file);
			fputs(current_xmachine->name, file);
			fputs("->", file);
			fputs(current_variable->name, file);
			fputs(")\n", file);

			current_variable = current_variable->next;
		}

		/* Close the files */
		fclose(file);

		current_xmachine = current_xmachine->next;
	}
}

void parseUnittest(char * directory, model_data * modeldata)
{
	FILE *file;
	char filename[100];
	/*xmachine * current_xmachine = * modeldata->p_xmachines;
	xmachine_function * current_function;
	xmachine_ioput * current_ioput;*/

	/* Open the output file */
	strcpy(filename, directory);
	strcat(filename, "unittest.c");
	printf("writing file: %s\n", filename);
	file = fopen(filename, "w");

	fputs("/**\n", file);
	fputs(" * \\file unittest.c\n", file);
	fputs(" * \\brief Unit test program.\n", file);
	fputs(" */\n\n", file);
	fputs("#include \"header.h\"\n", file);
	fputs("#include <CUnit/Basic.h>\"\n\n", file);
	fputs("int main(int argc, char * argv[])\n", file);
	fputs("{\n", file);
	fputs("\tprintf(\"unittest\\n\");\n", file);
	fputs("\t/* Exit successfully by returning zero to Operating System */\n", file);
	fputs("\treturn 0;\n", file);
	fputs("}\n", file);

	/* Close the files */
	fclose(file);
}

void parser0dtd(char * directory, model_data * modeldata)
{
	FILE *file;
	char filename[100];
	/*xmachine * current_xmachine = * modeldata->p_xmachines;*/
	variable * current_envvar;
	variable * allvar;
	
	/* Open the output file */
	strcpy(filename, directory);
	strcat(filename, "0.dtd");
	printf("writing file: %s\n", filename);
	file = fopen(filename, "w");
	
	fputs("<!ELEMENT states (itno?,imports?,outputs?,environment?,xagents?)>\n", file);
	fputs("<!ELEMENT imports (import*)>\n", file);
	fputs("<!ELEMENT outputs (output*)>\n", file);
	fputs("<!ELEMENT environment (", file);
	for(current_envvar = * modeldata->p_envvars; current_envvar != NULL; current_envvar = current_envvar->next)
	{
		fputs(current_envvar->name, file);
		if(current_envvar->next != NULL) fputs(",", file);
	}
	fputs(")>\n", file);
	fputs("<!ELEMENT xagents (xagent*)>\n", file);
	fputs("<!ELEMENT import (type,format,location)>\n", file);
	fputs("<!ELEMENT output (type,name?,format,location,time)>\n", file);
	/* Unfortunately xagent tags can hold all the different agent type variables */
	fputs("<!ELEMENT xagent ANY>\n", file);
	fputs("<!ELEMENT time (period, phase, duration?)>\n", file);
	fputs("<!ELEMENT itno (#PCDATA)>\n", file);
	fputs("<!ELEMENT location (#PCDATA)>\n", file);
	fputs("<!ELEMENT format (#PCDATA)>\n", file);
	fputs("<!ELEMENT type (#PCDATA)>\n", file);
	fputs("<!ELEMENT name (#PCDATA)>\n", file);
	fputs("<!ELEMENT period (#PCDATA)>\n", file);
	fputs("<!ELEMENT phase (#PCDATA)>\n", file);
	fputs("<!ELEMENT duration (#PCDATA)>\n", file);
	/* For each agent variable */
	for(allvar = * modeldata->p_allvars; allvar != NULL; allvar = allvar->next)
	{
		if(strcmp(allvar->name, "itno") != 0 ||
			strcmp(allvar->name, "location") != 0 ||
			strcmp(allvar->name, "format") != 0 ||
			strcmp(allvar->name, "type") != 0 ||
			strcmp(allvar->name, "name") != 0 ||
			strcmp(allvar->name, "period") != 0 ||
			strcmp(allvar->name, "phase") != 0 ||
			strcmp(allvar->name, "duration") != 0)
		{
			fputs("<!ELEMENT ", file);
			fputs(allvar->name, file);
			fputs(" (#PCDATA)>\n", file);
		}
	}
	/* Close the file */
	fclose(file);
}
