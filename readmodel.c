#include "header.h"

/** \fn checkRule(rule_data * current_rule_data)
 * \brief Checks the rule to see if it contains agent variables.
 * \param current_rule_data The rule to check.
 */
int checkRuleAgentVar(rule_data * current_rule_data)
{
	int flag = 0;

	if(current_rule_data->time_rule == 1)
	{
		if(strncmp(current_rule_data->rhs, "a.", 2) == 0) flag = 1;
	}
	else
	{
		if(current_rule_data->lhs == NULL)
		{
			if(checkRuleAgentVar(current_rule_data->lhs_rule) == 1) flag = 1;
		}
		else
		{
			if(strncmp(current_rule_data->lhs, "a.", 2) == 0) flag = 1;
		}

		if(current_rule_data->rhs == NULL)
		{
			if(checkRuleAgentVar(current_rule_data->rhs_rule) == 1) flag = 1;
		}
		else
		{
			if(strncmp(current_rule_data->rhs, "a.", 2) == 0) flag = 1;
		}
	}

	return flag;
}

int checkRuleMessageVar(rule_data * current_rule_data)
{
	int flag = 0;

		if(current_rule_data->lhs == NULL)
		{
			if(checkRuleMessageVar(current_rule_data->lhs_rule) == 1) flag = 1;
		}
		else
		{
			if(strncmp(current_rule_data->lhs, "m.", 2) == 0) flag = 1;
		}

		if(current_rule_data->rhs == NULL)
		{
			if(checkRuleAgentVar(current_rule_data->rhs_rule) == 1) flag = 1;
		}
		else
		{
			if(strncmp(current_rule_data->rhs, "m.", 2) == 0) flag = 1;
		}

	return flag;
}


void handleVariableType(char_array * current_string, variable * current_variable, model_data * modeldata)
{
	model_datatype * current_datatype;
	char buffer[100];
	int i;

	current_variable->type = copy_array_to_str(current_string);

	current_variable->arraylength = 0;
	current_variable->ismodeldatatype = 0;

	strcpy(current_variable->defaultvalue, "");
	strcpy(current_variable->c_type, "");

	if(strcmp(current_variable->type, "int") == 0 ||
		strcmp(current_variable->type, "short int") == 0 ||
		strcmp(current_variable->type, "long int") == 0 ||
		strcmp(current_variable->type, "unsigned int") == 0 ||
		strcmp(current_variable->type, "unsigned short int") == 0 ||
		strcmp(current_variable->type, "unsigned long int") == 0 ||
		strcmp(current_variable->type, "int_array") == 0)
	{
		strcpy(current_variable->defaultvalue, "0");
		strcpy(current_variable->c_type, "i");
	}

	if(strcmp(current_variable->type, "double") == 0 ||
		strcmp(current_variable->type, "float") == 0 ||
		strcmp(current_variable->type, "double_array") == 0 ||
		strcmp(current_variable->type, "float_array") == 0)
	{
		strcpy(current_variable->defaultvalue, "0.0");
		strcpy(current_variable->c_type, "f");
	}

	if(strcmp(current_variable->type, "char") == 0 ||
		strcmp(current_variable->type, "unsigned char") == 0 ||
		strcmp(current_variable->type, "char_array") == 0)
	{
		strcpy(current_variable->defaultvalue, "' '");
		strcpy(current_variable->c_type, "c");
	}

	/*copycharlist(&current_variable->type, &chardata[0]);*/
	/* These are C to MPI datatype mappings */
	if(strcmp(current_variable->type, "int") == 0) strcpy(current_variable->mpi_type, "MPI_INT");
	if(strcmp(current_variable->type, "double") == 0) strcpy(current_variable->mpi_type, "MPI_DOUBLE");
	if(strcmp(current_variable->type, "float") == 0) strcpy(current_variable->mpi_type, "MPI_FLOAT");
	if(strcmp(current_variable->type, "char") == 0) strcpy(current_variable->mpi_type, "MPI_CHAR");
	if(strcmp(current_variable->type, "short int") == 0) strcpy(current_variable->mpi_type, "MPI_SHORT");
	if(strcmp(current_variable->type, "long int") == 0) strcpy(current_variable->mpi_type, "MPI_LONG");
	if(strcmp(current_variable->type, "long double") == 0) strcpy(current_variable->mpi_type, "MPI_LONG_DOUBLE");
	if(strcmp(current_variable->type, "unsigned int") == 0) strcpy(current_variable->mpi_type, "MPI_UNSIGNED");
	if(strcmp(current_variable->type, "unsigned short int") == 0) strcpy(current_variable->mpi_type, "MPI_UNSIGNED_SHORT");
	if(strcmp(current_variable->type, "unsigned long int") == 0) strcpy(current_variable->mpi_type, "MPI_UNSIGNED_LONG");
	if(strcmp(current_variable->type, "unsigned char") == 0) strcpy(current_variable->mpi_type, "MPI_UNSIGNED_CHAR");
	if(strcmp(current_variable->type, "int_array") == 0)
	{
		current_variable->arraylength = -1;
		strcpy(current_variable->mpi_type, "MPI_INT");
	}
	if(strcmp(current_variable->type, "float_array") == 0)
	{
		current_variable->arraylength = -1;
		strcpy(current_variable->mpi_type, "MPI_FLOAT");
	}
	if(strcmp(current_variable->type, "double_array") == 0)
	{
		current_variable->arraylength = -1;
		strcpy(current_variable->mpi_type, "MPI_DOUBLE");
	}
	if(strcmp(current_variable->type, "char_array") == 0)
	{
		current_variable->arraylength = -1;
		strcpy(current_variable->mpi_type, "MPI_CHAR");
	}
	/* Handle model defined data types */
	current_datatype = * modeldata->p_datatypes;
	while(current_datatype)
	{
		if(strcmp(current_variable->type, current_datatype->name) == 0)
		{
			current_variable->ismodeldatatype = 1;
			current_variable->datatype = current_datatype;
		}

		strcpy(buffer, current_datatype->name);
		strcat(buffer, "_array");

		if(strcmp(current_variable->type, buffer) == 0)
		{
			current_variable->ismodeldatatype = 1;
			current_variable->datatype = current_datatype;
			current_variable->arraylength = -1;
		}

		current_datatype = current_datatype->next;
	}

	current_variable->typenotarray = copy_array_to_str(current_string);
	/* Handle model data type arrays */
	i = (int)strlen(current_variable->type);
	if(i > 6)
	{
		if(current_variable->type[i-1] == 'y' &&
			current_variable->type[i-2] == 'a' &&
			current_variable->type[i-3] == 'r' &&
			current_variable->type[i-4] == 'r' &&
			current_variable->type[i-5] == 'a' &&
			current_variable->type[i-6] == '_')
		{
			remove_char(current_string, i-1);
			remove_char(current_string, i-2);
			remove_char(current_string, i-3);
			remove_char(current_string, i-4);
			remove_char(current_string, i-5);
			remove_char(current_string, i-6);
			free(current_variable->typenotarray);
			current_variable->typenotarray = copy_array_to_str(current_string);
		}
	}
}

void handleVariableName(char_array * current_string, variable * current_variable)
{
	int j, i = 0;
	char buffer[100];
	
	current_variable->name = copy_array_to_str(current_string);

	/* Handle static arrays */
	while(current_variable->name[i] != '\0')
	{
		if(current_variable->name[i] == '[')
		{
			current_variable->name[i] = '\0';
			j = 0;
			i++;
			while(current_variable->name[i] != ']')
			{
				buffer[j] = current_variable->name[i];
				j++;
				i++;
			}

			buffer[j] = '\0';

			/* If no number*/
			if(j == 0) current_variable->arraylength = -1;
			else
			{
				current_variable->arraylength = atoi(buffer);
			}
		}

		i++;
	}
}

/** \fn void readModel(char * inputfile, char * directory, model_data * modeldata)
 * \brief Read xmml model description and populate information into data structures.
 * \param inputfile Pointer to the xmml file path and name.
 * \param directory Pointer to the xmml file path and name.
 * \param modeldata Data from the model.
 */
void readModel(input_file * inputfile, char * directory, model_data * modeldata)
{
	xmachine * current_xmachine;
	/* Pointer to file */
	FILE *file;
	/* Pointer to possible code file */
	FILE *filecode;
	/* Comment variable */
	int xmlcomment = 0;
	/* Variables for handling files for function code */
	int j, k;
	/* Variable to see if function defined, if not look in functions file */
	int foundfunctioncode;
	/* Char and char buffer for reading file to */
	char c = ' ';
	int numtag = 0;
	char chartag[100][100];
	int tagline[100];
	char chardata[1000];
	int dynamic_array_found;
	int linenumber = 1;
	int variable_count;
	int lastd;
	int lhs_last = 0;
	/* Variable to keep reading file */
	int reading;
	/* Variable for buffer position */
	int i;
	/* Variable to handle xml codes */
	int xmlcode;
	/* Variable for if in a tag */
	int intag;
	/* Variables for checking tags */
	int xagentmodel, date, author, name, xmachine, memory, var, type, notes;
	int states, state, attribute, transition, func, dest, functions, function;
	int note, messages, message, code, cdata, environment, define, value, codefile;
	int header, iteration_end_code, depends, datatype, desc, cur_state, next_state;
	int input, output, messagetype, timetag, unit, period, lhs, op, rhs, condition;
	int model, filter, phase, enabled, not, time;
	int not_value;
	/* Pointer to new structs */
	xmachine_message * current_message;
	xmachine_state * current_state;
	xmachine_function * current_function;
	adj_function * current_adj_function;
	variable * current_envdefine;
	env_func * current_envfunc;
	variable ** p_variable;
	variable * tvariable;
	variable * current_variable;
	model_datatype * current_datatype;
	/*char_list ** p_charlist;*/
	/*char_list * charlist;*/
	/*char_list * current_charlist;*/
	f_code ** p_fcode;
	f_code * fcode;
	f_code * current_fcode;
	f_code * end_it_fcode;
	xmachine_ioput * current_ioput;
	input_file * current_input_file;
	rule_data * current_rule_data;
	rule_data * last_rule_data;
	/* new use of char_array */
	char_array * current_string;
	char * temp_char = NULL;
	char * temp_char2 = NULL;
	/* variables in time_data */
	char * time_name = NULL;
	char * unit_name = NULL;
	int period_int;

	/* check var name for array */
	/*char_list * charcheck;*/
	modeldata->number_messages = 0;
	modeldata->number_xmachines = 0;
	modeldata->agents_include_array_variables = 0;

	/* Open config file to read-only */
	if((file = fopen(inputfile->fullfilepath, "r"))==NULL)
	{
		fprintf(stderr, "ERROR: Cannot read file: %s\n", inputfile->fullfilepath);
		exit(1);
	}
	else { printf("reading xmml: %s\n", inputfile->fullfilepath); }

	/* Initialise variables */
	/*p_charlist = &charlist;*/
	p_fcode = &fcode;
	p_variable = &tvariable;
	current_string = init_char_array();

	i = 0;
	xmlcode = 0;
	reading = 1;
	intag = 0;
	xagentmodel = 0;
	date = 0;
	author = 0;
	notes = 0;
	name = 0;
	xmachine = 0;
	memory = 0;
	var = 0;
	type = 0;
	states = 0;
	state = 0;
	func = 0;
	dest = 0;
	functions = 0;
	function = 0;
	note = 0;
	messages = 0;
	message = 0;
	code = 0;
	cdata = 0;
	environment = 0;
	define = 0;
	value = 0;
	codefile = 0;
	header = 0;
	iteration_end_code = 0;
	depends = 0;
	datatype = 0;
	desc = 0;
	cur_state = 0;
	next_state = 0;
	input = 0;
	output = 0;
	messagetype = 0;
	timetag = 0;
	unit = 0;
	period = 0;
	lhs = 0;
	rhs = 0;
	op = 0;
	condition = 0;
	model = 0;
	filter = 0;
	period = 0;
	phase = 0;
	enabled = 0;
	not = 0;
	time = 0;

	/*printf("%i> ", linenumber);*/

	/* Read characters until the end of the file */
	/*while(c != EOF)*/
	/* Read characters until end of xml found */
	while(reading == 1 && c != (char)EOF)
	{
		/* Get the next char from the file */
		c = (char)fgetc(file);

		/* Print char */
		/*printf("%c\n", c);*/
		if(c == '\n' || c == '\r') linenumber++;
		/*printf("%i> ", linenumber);*/

		/*printf("xmlcomment: %d\t%c\n", xmlcomment, c);*/

		/* If in cdata then don't parse as xml */
		if(cdata)
		{
			/*printf("in cdata\n");*/

			/* Handle CDATA tags */
			if(xmlcode != 0)
			{
				if((xmlcode == 1) && (c == ']')) xmlcode = 2;
				else if((xmlcode == 11) && (c == '!')) xmlcode = 2;
				else if((xmlcode == 11) && (c == 'f')) xmlcode = 22;
				else if(xmlcode == 2 && (c == '>' || c == '['))
				{
					if(c == '>') cdata = 0;
					else xmlcode = 3;
				}
				else if(xmlcode == 3 && c == 'C') xmlcode = 4;
				else if(xmlcode == 4 && c == 'D') xmlcode = 5;
				else if(xmlcode == 5 && c == 'A') xmlcode = 6;
				else if(xmlcode == 6 && c == 'T') xmlcode = 7;
				else if(xmlcode == 7 && c == 'A') xmlcode = 8;
				else if(xmlcode == 8 && c == '[') xmlcode = 9;
				else if(xmlcode == 22 && c == 'i') xmlcode = 23;
				else if(xmlcode == 23 && c == 'l') xmlcode = 24;
				else if(xmlcode == 24 && c == 'e') xmlcode = 25;
				else if(xmlcode == 25 && c == '>')
				{
					cdata = 0;
					codefile = 1;

					strcpy(&chartag[numtag][0], "file");
					numtag++;

					reset_char_array(current_string);
				}
				else
				{
					if(xmlcode == 1)
					{
						/*current_charlist = addchar(p_charlist);*/
						/*current_charlist->character = ']';*/
						add_char(current_string, ']');
					}
					else if(xmlcode == 11)
					{
						/*current_charlist = addchar(p_charlist);*/
						/*current_charlist->character = '<';*/
						add_char(current_string, '<');
					}
					xmlcode = 0;
				}
			}
			else if(c == ']') xmlcode = 1;
			else if(c == '<') xmlcode = 11;
			if(xmlcode == 0)
			{
				/*current_charlist = addchar(p_charlist);*/
				/*current_charlist->character = c;*/
				add_char(current_string, c);
			}
			if(xmlcode == 9) xmlcode = 0;

			/* Print xmlcode */
			/*printf("xmlcode: %d", xmlcode);*/
		}
		/* If in xml comment then don't parse as xml */
		else if(xmlcomment > 3)
		{
			if(xmlcomment == 4 && c == '-') xmlcomment = 5;
			else if(xmlcomment == 5 && c == '-') xmlcomment = 6;
			else if(xmlcomment == 6 && c == '>') { xmlcomment = 0; intag = 0; i = 0; }
			else xmlcomment = 4;
		}
		/* If the end of a tag */
		else if(c == '>')
		{
			/*printf("in c == >\n");*/

			/* Place 0 at end of buffer to make chars a string */
			/*buffer[i] = 0;*/

			/*printf("buffer = %s\n", buffer);*/
			/*printf("string = ");*/
			/*print_char_array(current_string);*/

			/* Handle truncated empty content tag */
			/* If last character of string in tag is not a slash */
			if(current_string->array[current_string->size-1] != '/')
			{
				/* Handle XML nested tag errors */
				/* If start tag or xml start tag */
				if(current_string->array[0] != '/')
				{
					/* Add to list of tags */
					strcpy(&chartag[numtag][0], current_string->array);
					tagline[numtag] = linenumber;
					/* Advance list to next free place */
					numtag++;
				}
				/* If end tag */
				else
				{
					/* Look at last tag */
					numtag--;
					/* If different then exit */
					if(strcmp(&current_string->array[1], &chartag[numtag][0]) != 0 && strcmp(current_string->array, "/xmodel") != 0)
					{
						fprintf(stderr, "ERROR: The tag <%s> on line number %i\n", current_string->array, linenumber);
						fprintf(stderr, "ERROR: doesn't close the tag <%s> on line number %i\n", &chartag[numtag][0], tagline[numtag]);
						exit(1);
					}
				}
			}

			if(strcmp(current_string->array, "xmachine_agent_model") == 0 || strcmp(current_string->array, "xmodel") == 0) { xagentmodel = 1; }
			if(strcmp(current_string->array, "/xmachine_agent_model") == 0 || strcmp(current_string->array, "/xmodel") == 0) { xagentmodel = 0; reading = 0; printf("End of xagent model.\n"); }
			if(strcmp(current_string->array, "date") == 0) { date = 1; }
			if(strcmp(current_string->array, "/date") == 0) { date = 0; }
			if(strcmp(current_string->array, "author") == 0) { author = 1; }
			if(strcmp(current_string->array, "/author") == 0) { author = 0; }
			if(strcmp(current_string->array, "notes") == 0) { notes = 1; cdata = 1;}
			if(strcmp(current_string->array, "/notes") == 0) { notes = 0; }
			if(strcmp(current_string->array, "name") == 0) { name = 1; }/*reset_char_array(current_string); }*/
			if(strcmp(current_string->array, "/name") == 0) { name = 0; }
			if(strcmp(current_string->array, "environment") == 0) { environment = 1; }
			if(strcmp(current_string->array, "/environment") == 0) { environment = 0; }
			if(strcmp(current_string->array, "define") == 0)
			{
				define = 1;
				current_envdefine = addvariable(modeldata->p_envdefines);
			}
			if(strcmp(current_string->array, "/define") == 0) { define = 0; }
			if(strcmp(current_string->array, "value") == 0) { value = 1; }/* charlist = NULL; }*/
			if(strcmp(current_string->array, "/value") == 0) { value = 0; }
			if(strcmp(current_string->array, "xmachine") == 0) { xmachine = 1; }
			if(strcmp(current_string->array, "/xmachine") == 0) { xmachine = 0; }
			if(strcmp(current_string->array, "xagent") == 0) { xmachine = 1; }
			if(strcmp(current_string->array, "/xagent") == 0) { xmachine = 0; }
			if(strcmp(current_string->array, "memory") == 0)
			{
				memory = 1;

				tvariable = NULL;
			}
			if(strcmp(current_string->array, "/memory") == 0)
			{
				memory = 0;
				//current_memory->vars = *p_variable;
			}
			if(strcmp(current_string->array, "var") == 0 || strcmp(current_string->array, "variable") == 0)
			{
				var = 1;
				if(datatype == 1) current_variable = addvariable(p_variable);
				else if(environment == 1) current_variable = addvariable(modeldata->p_envvars);
				else if(memory == 1)
				{
					current_variable = addvariable(&current_xmachine->variables);
					current_variable->agent = current_xmachine;
				}
				else current_variable = addvariable(p_variable);

				current_variable->file = copystr(inputfile->fullfilepath);
			}
			if(strcmp(current_string->array, "/var") == 0 || strcmp(current_string->array, "/variable") == 0) { var = 0; }
			if(strcmp(current_string->array, "type") == 0) { type = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/type") == 0) { type = 0; }
			if(strcmp(current_string->array, "states") == 0) { states = 1; }
			if(strcmp(current_string->array, "/states") == 0) { states = 0; }
			/*if(strcmp(current_string->array, "state") == 0)
			{
				state = 1;
				current_state = addxstate(&current_xmachine->states);
				attrib = NULL;
				trans = NULL;
			}
			if(strcmp(current_string->array, "/state") == 0)
			{
				state = 0;
				current_state->attributes = *p_attrib;
				current_state->transitions = *p_trans;
			}*/
			if(strcmp(current_string->array, "func") == 0) { func = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/func") == 0) { func = 0; }
			if(strcmp(current_string->array, "dest") == 0 || strcmp(current_string->array, "description") == 0) { dest = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/dest") == 0 || strcmp(current_string->array, "description") == 0) { dest = 0; }
			if(strcmp(current_string->array, "datatype") == 0 || strcmp(current_string->array, "dataType") == 0)
			{
				datatype = 1;
				current_datatype = adddatatype(modeldata->p_datatypes);
				tvariable = NULL;
				reset_char_array(current_string);
			}
			if(strcmp(current_string->array, "/datatype") == 0 || strcmp(current_string->array, "/dataType") == 0)
			{
				datatype = 0;
				current_datatype->vars = *p_variable;

				current_datatype->has_single_vars = 0;
				current_datatype->has_dynamic_arrays = 0;
				current_datatype->has_arrays = 0;
				/* Check if datatype has single variables and dynamic arrays */
				current_variable = current_datatype->vars;
				while(current_variable)
				{
					if((strcmp(current_variable->type, "int") == 0 ||
						strcmp(current_variable->type, "float") == 0 ||
						strcmp(current_variable->type, "double") == 0 ||
						strcmp(current_variable->type, "char") == 0) &&
						current_variable->arraylength == 0) current_datatype->has_single_vars = 1;

					if(current_variable->arraylength != 0) current_datatype->has_arrays = 1;
					if(current_variable->arraylength == -1) current_datatype->has_dynamic_arrays = 1;
					if(current_variable->ismodeldatatype == 1 && current_variable->datatype->has_dynamic_arrays == 1) current_datatype->has_dynamic_arrays = 1;

					current_variable = current_variable->next;
				}
			}
			if(strcmp(current_string->array, "functions") == 0) { functions = 1; }
			if(strcmp(current_string->array, "/functions") == 0) { functions = 0; }
			if(strcmp(current_string->array, "functionFiles") == 0) { functions = 1; }
			if(strcmp(current_string->array, "/functionFiles") == 0) { functions = 0; }
			if(strcmp(current_string->array, "current_state") == 0 || strcmp(current_string->array, "currentState") == 0) { cur_state = 1; }
			if(strcmp(current_string->array, "/current_state") == 0 || strcmp(current_string->array, "/currentState") == 0) { cur_state = 0; }
			if(strcmp(current_string->array, "next_state") == 0 || strcmp(current_string->array, "nextState") == 0) { next_state = 1; }
			if(strcmp(current_string->array, "/next_state") == 0 || strcmp(current_string->array, "/nextState") == 0) { next_state = 0; }
			if(strcmp(current_string->array, "currentState") == 0) { cur_state = 1; }
			if(strcmp(current_string->array, "/currentState") == 0) { cur_state = 0; }
			if(strcmp(current_string->array, "nextState") == 0) { next_state = 1; }
			if(strcmp(current_string->array, "/nextState") == 0) { next_state = 0; }
			if(strcmp(current_string->array, "input") == 0)
			{
				input = 1;

				current_ioput = addioput(&current_function->inputs);
				current_ioput->function = current_function;
			}
			if(strcmp(current_string->array, "/input") == 0) { input = 0; }
			if(strcmp(current_string->array, "output") == 0)
			{
				output = 1;

				current_ioput = addioput(&current_function->outputs);
				current_ioput->function = current_function;
			}
			if(strcmp(current_string->array, "/output") == 0) { output = 0; }
			if(strcmp(current_string->array, "messagetype") == 0 || strcmp(current_string->array, "messageName") == 0) { messagetype = 1; }
			if(strcmp(current_string->array, "/messagetype") == 0 || strcmp(current_string->array, "/messageName") == 0) { messagetype = 0; }
			if(strcmp(current_string->array, "function") == 0)
			{
				function = 1;
				/* Flag to see if function defined in tags, if not look in functions file */
				foundfunctioncode = 0;

				if(environment == 1)
				{
					current_envfunc = addenvfunc(modeldata->p_envfuncs);
				}
				else
				{
					/*current_function = addxfunction(p_xfunctions);*/
					current_function = addxfunction(&current_xmachine->functions);
					fcode = NULL;
					/*current_trans = addtrans(&current_function->depends);*/
					current_function->file = copystr(inputfile->fullfilepath);
				}
			}
			if(strcmp(current_string->array, "/function") == 0)
			{
				function = 0;

				current_function->agent_name = copystr(current_xmachine->name);

				if(current_function->current_state != NULL) addxstate(current_function->current_state, current_function->agent_name, &current_xmachine->states);
				if(current_function->next_state != NULL) addxstate(current_function->next_state, current_function->agent_name, &current_xmachine->states);
			}
			if(strcmp(current_string->array, "header") == 0) { header = 1; current_envfunc = addenvfunc(modeldata->p_envfuncs); }
			if(strcmp(current_string->array, "/header") == 0) { header = 0; }
			if(strcmp(current_string->array, "note") == 0) { note = 1; }
			if(strcmp(current_string->array, "/note") == 0) { note = 0; }
			if(strcmp(current_string->array, "desc") == 0) { desc = 1; }
			if(strcmp(current_string->array, "/desc") == 0) { desc = 0; }
			if(strcmp(current_string->array, "messages") == 0) { messages = 1; }
			if(strcmp(current_string->array, "/messages") == 0) { messages = 0; }
			if(strcmp(current_string->array, "message") == 0)
			{
				message = 1;
				current_message = addxmessage(modeldata->p_xmessages);
				tvariable = NULL;
				modeldata->number_messages++;
				current_message->file = copystr(inputfile->fullfilepath);
			}
			if(strcmp(current_string->array, "/message") == 0)
			{
				message = 0;
				current_message->vars = *p_variable;

				/* Count number of variables */
				variable_count = 0;
				/* Find unallowed dynamic arrays */
				dynamic_array_found = 0;
				current_variable = current_message->vars;
				while(current_variable)
				{
					variable_count++;
					if(current_variable->arraylength == -1 || (current_variable->ismodeldatatype == 1 && current_variable->datatype->has_dynamic_arrays == 1))
					{
						fprintf(stderr, "Error: %s - dyamic array found in message\n", current_variable->name);
						dynamic_array_found = 1;
					}

					current_variable->message = current_message;

					current_variable = current_variable->next;
				}

				if(dynamic_array_found == 1)
				{
					fprintf(stderr, "Error: Dynamic array found in %s message\n", current_message->name);
					exit(1);
				}

				current_message->var_number = variable_count;
			}
			if(strcmp(current_string->array, "code") == 0)
			{
				/*printf("in code == 0\n");*/

				code = 1;
				cdata = 1;
				/*charlist = NULL;*/
				reset_char_array(current_string);
				if(environment == 0 && iteration_end_code == 0)
				{
					current_fcode = addfcode(p_fcode);
				}
			}
			if(strcmp(current_string->array, "/code") == 0) { code = 0; }
			if(strcmp(current_string->array, "file") == 0) { codefile = 1; } /* actually handled in cdata */
			if(strcmp(current_string->array, "/file") == 0) { codefile = 0; }
			if(strcmp(current_string->array, "iteration_end_code") == 0) { iteration_end_code = 1; }
			if(strcmp(current_string->array, "/iteration_end_code") == 0) { iteration_end_code = 0; }
			if(strcmp(current_string->array, "timeunit") == 0 || strcmp(current_string->array, "timeUnit") == 0)
			{
				timetag = 1;
			}
			if(strcmp(current_string->array, "/timeunit") == 0 || strcmp(current_string->array, "/timeUnit") == 0)
			{
				timetag = 0;
				add_time_unit(time_name, unit_name, period_int, modeldata->p_time_units);
			}
			if(strcmp(current_string->array, "unit") == 0) { unit = 1; }
			if(strcmp(current_string->array, "/unit") == 0) { unit = 0; }
			if(strcmp(current_string->array, "time") == 0)
			{
				time = 1;
				if(lhs || rhs)
				{
					last_rule_data = current_rule_data;
					if(lhs_last) { current_rule_data = add_rule_data(&current_rule_data->lhs_rule); }
					else { current_rule_data = add_rule_data(&current_rule_data->rhs_rule); }
				}
				else //if(condition)
				{
					current_rule_data = add_rule_data(&current_function->condition_rule);
				}

				current_rule_data->time_rule = 1;
				if(not == 1) current_rule_data->not = 1;
			}
			if(strcmp(current_string->array, "/time") == 0)
			{
				time = 0;
				current_rule_data = last_rule_data;
			}
			if(strcmp(current_string->array, "period") == 0) { period = 1; }
			if(strcmp(current_string->array, "/period") == 0) { period = 0; }
			if(strcmp(current_string->array, "phase") == 0) { phase = 1; }
			if(strcmp(current_string->array, "/phase") == 0) { phase = 0; }
			if(strcmp(current_string->array, "not") == 0) { not = 1; }
			if(strcmp(current_string->array, "/not") == 0) { not = 0; }
			if(strcmp(current_string->array, "lhs") == 0)
			{
				lhs = 1;

				if(current_rule_data == NULL)
				{
					if(condition) { current_rule_data = add_rule_data(&current_function->condition_rule); }
					if(filter) { current_rule_data = add_rule_data(&current_ioput->filter_rule); }
					last_rule_data = NULL;
				}
				else
				{
					last_rule_data = current_rule_data;
					if(lhs_last) { current_rule_data = add_rule_data(&current_rule_data->lhs_rule); }
					else { current_rule_data = add_rule_data(&current_rule_data->rhs_rule); }
				}

				if(not == 1) current_rule_data->not = 1;

				lhs_last = 1;
			}
			if(strcmp(current_string->array, "/lhs") == 0) { lhs = 0; }
			if(strcmp(current_string->array, "rhs") == 0)
			{
				rhs = 1;
				lhs_last = 0;
			}
			if(strcmp(current_string->array, "/rhs") == 0)
			{
				rhs = 0;
				current_rule_data = last_rule_data;
			}
			if(strcmp(current_string->array, "op") == 0) { op = 1; }
			if(strcmp(current_string->array, "/op") == 0) { op = 0; }
			if(strcmp(current_string->array, "condition") == 0)
			{
				condition = 1;
				current_rule_data = NULL;
				not_value = 0;
			}
			if(strcmp(current_string->array, "/condition") == 0)
			{
				condition = 0;
				if(not_value == 1) { current_rule_data->not = 1; printf("*********** condition is not\n"); }
			}
			if(strcmp(current_string->array, "model") == 0)
			{
				model = 1;
				current_input_file = add_input_file(modeldata->p_files);
			}
			if(strcmp(current_string->array, "/model") == 0)
			{
				model = 0;

				printf("Input model file: %s ", current_input_file->fullfilepath);
				if(current_input_file->enabled == 1) printf("enabled\n");
				else printf("disabled\n");
			}
			if(strcmp(current_string->array, "depends") == 0)
			{
				depends = 1;
				/*charlist = NULL;*/
				current_adj_function = add_depends_adj_function(current_function);

				modeldata->depends_style = 1;
			}
			if(strcmp(current_string->array, "/depends") == 0)
			{
				depends = 0;

				//add_adj_function(current_function2, current_function, "internal");

				//printf("depends: %s %s %s\n", current_function->name, current_adj_function->name, current_adj_function->type);


				/*printf("depends: ");
				printcharlist(&current_trans->func);
				printf(" ");
				printcharlist(&current_trans->dest);
				printf("\n");*/

			}
			if(strcmp(current_string->array, "filter") == 0)
			{
				filter = 1;
				current_rule_data = NULL;
			}
			if(strcmp(current_string->array, "/filter") == 0)
			{
				filter = 0;
				/* Check if filter rule contains agent/message variables */
				current_ioput->filter_rule->has_agent_var = checkRuleAgentVar(current_ioput->filter_rule);
				current_ioput->filter_rule->has_message_var = checkRuleMessageVar(current_ioput->filter_rule);
			}
			if(strcmp(current_string->array, "value") == 0) { value = 1; }
			if(strcmp(current_string->array, "/value") == 0) { value = 0; }
			if(strcmp(current_string->array, "enabled") == 0) { enabled = 1; }
			if(strcmp(current_string->array, "/enabled") == 0) { enabled = 0; }

			/* End of tag and reset buffer */
			intag = 0;
			/*i = 0;*/
			reset_char_array(current_string);
			/* Reset xml code char list */
			xmlcode = 0;
		}
		/* If start of tag */
		else if(c == '<')
		{
			/* Place /0 at end of buffer to end numbers or string*/
			/*buffer[i] = 0;*/
			/* Flag in tag */
			intag = 1;
			xmlcomment = 1;

			if(model)
			{
				if(enabled)
				{
					temp_char = copy_array_to_str(current_string);
					if(strcmp(temp_char, "true") == 0) current_input_file->enabled = 1;
					if(strcmp(temp_char, "false") == 0) current_input_file->enabled = 0;
					free(temp_char);
				}
				if(codefile)
				{
					temp_char = copy_array_to_str(current_string);
					temp_char2 = (char *)malloc( (strlen(temp_char) + strlen(directory) + 1) * sizeof(char));
					strcpy(temp_char2, directory);
					strcat(temp_char2, temp_char);
					current_input_file->file = copy_array_to_str(current_string);
					current_input_file->fullfilepath = copystr(temp_char2);
					current_input_file->fulldirectory = copystr(temp_char2);
					current_input_file->localdirectory = copystr(temp_char);
					temp_char2[0] = 0;
					free(temp_char2);
					free(temp_char);

					/* calculate directory of file */
					/* Calculate directory where xparser and template files are */
					i = 0;
					lastd = 0;
					while(current_input_file->fulldirectory[i] != '\0')
					{
						/* For windows directories */
						if(current_input_file->fulldirectory[i] == '\\') lastd=i;
						/* For unix directories */
						if(current_input_file->fulldirectory[i] == '/') lastd=i;
						i++;
					}
					/* If a directory is in the path */
					if(lastd != 0)
					{
						current_input_file->fulldirectory[lastd+1] = '\0';
					}
					else current_input_file->fulldirectory[0] = '\0';

					i = 0;
					lastd = 0;
					while(current_input_file->localdirectory[i] != '\0')
					{
						/* For windows directories */
						if(current_input_file->localdirectory[i] == '\\') lastd=i;
						/* For unix directories */
						if(current_input_file->localdirectory[i] == '/') lastd=i;
						i++;
					}
					/* If a directory is in the path */
					if(lastd != 0)
					{
						current_input_file->localdirectory[lastd+1] = '\0';
					}
					else current_input_file->localdirectory[0] = '\0';
				}
			}
			if(environment)
			{
				if(timetag)
				{
					if(name)
					{
						free(time_name);
						time_name = copy_array_to_str(current_string);
					}
					if(unit)
					{
						free(unit_name);
						unit_name = copy_array_to_str(current_string);
					}
					if(period)
					{
						period_int = atoi(current_string->array);
					}
				}
				else if(datatype)
				{
					if(desc) current_datatype->desc = copy_array_to_str(current_string);

					if(var)
					{
						if(type) { handleVariableType(current_string, current_variable, modeldata); }
						if(name) { handleVariableName(current_string, current_variable); }
						/*if(type) current_variable->type = copy_array_to_str(current_string);*/
						/*if(name) current_variable->name = copy_array_to_str(current_string);*/
					}
					else if(name)
					{
						current_datatype->name = copy_array_to_str(current_string);
						printf("Reading data type named: %s\n", current_datatype->name);
					}
				}
				else if(var)
				{
					if(type)
					{
						current_variable->type = copy_array_to_str(current_string);

						strcpy(current_variable->defaultvalue, "");
						strcpy(current_variable->c_type, "");

						if(strcmp(current_variable->type, "int") == 0 ||
							strcmp(current_variable->type, "short int") == 0 ||
							strcmp(current_variable->type, "long int") == 0 ||
							strcmp(current_variable->type, "unsigned int") == 0 ||
							strcmp(current_variable->type, "unsigned short int") == 0 ||
							strcmp(current_variable->type, "unsigned long int") == 0)
						{
							strcpy(current_variable->defaultvalue, "0");
							strcpy(current_variable->c_type, "i");
						}

						if(strcmp(current_variable->type, "double") == 0 ||
							strcmp(current_variable->type, "float") == 0)
						{
							strcpy(current_variable->defaultvalue, "0.0");
							strcpy(current_variable->c_type, "f");
						}

						if(strcmp(current_variable->type, "char") == 0 ||
							strcmp(current_variable->type, "unsigned char") == 0)
						{
							strcpy(current_variable->defaultvalue, "' '");
							strcpy(current_variable->c_type, "c");
						}
					}
					if(name) current_variable->name = copy_array_to_str(current_string);
				}
				else if(define)
				{
					if(name) current_envdefine->name = copy_array_to_str(current_string);
					if(value)
					{
						strcpy(current_envdefine->value, "");
						current_envdefine->value = copy_array_to_str(current_string);
					}
					strcpy(current_envdefine->type, "");
				}

				if(codefile)
				{
					if(functions)
					{
						current_envfunc = addenvfunc(modeldata->p_envfuncs);
						current_envfunc->header = 2;
					}


					/* Place directory at start of chardata */
					j = 0;
					while(*inputfile->localdirectory != 0)
					{
						chardata[j] = *inputfile->localdirectory;
						(inputfile->localdirectory)++;
						j++;
					}
					/* Make directory pointer point to start of chars again */
					for(k = 0; k < j; k++)
					{
						(inputfile->localdirectory)--;
					}

					chardata[j] = '\0';
					strcat(chardata, current_string->array);

					current_envfunc->filepath = copystr(chardata);
					//printf("inputfile->directory: %s\n", inputfile->localdirectory);
					//printf("current_envfunc->filepath: %s\n", current_envfunc->filepath);


					/* Place directory at start of chardata */
					j = 0;
					while(*inputfile->fulldirectory != 0)
					{
						chardata[j] = *inputfile->fulldirectory;
						(inputfile->fulldirectory)++;
						j++;
					}
					/* Make directory pointer point to start of chars again */
					for(k = 0; k < j; k++)
					{
						(inputfile->fulldirectory)--;
					}

					chardata[j] = '\0';



					/* Add file name to chardata on end of directory */
					/*current_charlist = *p_charlist;
					while(current_charlist)
					{
						chardata[j] = current_charlist->character;
						j++;

						current_charlist = current_charlist->next;
					}
					chardata[j] = 0;*/

					strcat(chardata, current_string->array);
					/*printf("01\t%s\n", chardata);*/

					/* Open code file read-only */
					if((filecode = fopen(chardata, "r"))==NULL)
					{
						fprintf(stderr, "ERROR - cannot read file: %s\n", chardata);
						printf("*** xparser aborted ***\n");
						exit(0);
					}
					else printf("reading file: %s\n", chardata);

					/*charlist = NULL;*/
					reset_char_array(current_string);

					if(functions)
					{
						printf("%s is a functions file\n", chardata);
					}
					else
					{
						/* Read characters until the end of the file */
						while(c != (char)EOF)
						{
							/* Get the next char from the file */
							c = (char)fgetc(filecode);

							if(c != (char)EOF)
							{
								/*current_charlist = addchar(p_charlist);*/
								/*current_charlist->character = c;*/
								add_char(current_string, c);
							}
						}

						/*if(environment == 1) current_envfunc->code = *p_charlist;*/
						/*else current_fcode->code = *p_charlist;*/
					}

					/* Close the file */
					fclose(filecode);
				}

				if(code && function)
				{
					current_envfunc->code = copy_array_to_str(current_string);
					current_envfunc->header = 0;
				}
				if(header) current_envfunc->header = 1;
			}
			else if(var)
			{
				if(type) { handleVariableType(current_string, current_variable, modeldata); }
				if(name) { handleVariableName(current_string, current_variable); }
			}
			else if(message && name)
			{
				current_message->name = copy_array_to_str(current_string);
				printf("Reading message named: ");
				printf(current_message->name);
				printf("\n");
			}
			else if(state)
			{
				if(name) current_state->name = copy_array_to_str(current_string);
			}
			else if(function)
			{
				if(codefile)
				{
					/* Place directory at start of chardata */
					/*j = 0;
					while(*directory != 0)
					{
						chardata[j] = *directory;
						(directory)++;
						j++;
					}*/
					/* Make directory pointer point to start of chars again */
					/*for(k = 0; k < j; k++)
					{
						(directory)--;
					}*/
					/* Add file name to chardata on end of directory */
					/*current_charlist = copy_array_to_str(current_string);
					while(current_charlist)
					{
						chardata[j] = current_charlist->character;
						j++;

						current_charlist = current_charlist->next;
					}
					chardata[j] = 0;*/
					/*printf("01\t%s\n", chardata);*/

					strcpy(chardata, directory);
					strcat(chardata, copy_array_to_str(current_string));

					/* Open code file read-only */
					if((filecode = fopen(chardata, "r"))==NULL)
					{
						fprintf(stderr, "ERROR - cannot read file: %s\n", chardata);
						printf("*** xparser aborted ***\n");
						exit(0);
					}
					else printf("reading file: %s\n", chardata);

					/*charlist = NULL;*/
					reset_char_array(current_string);

					/* Read characters until the end of the file */
					while(c != (char)EOF)
					{
						/* Get the next char from the file */
						c = (char)fgetc(filecode);

						if(c != (char)EOF)
						{
							/*current_charlist = addchar(p_charlist);*/
							/*current_charlist->character = c;*/
							add_char(current_string, c);
						}
					}

					/*if(environment == 1) current_envfunc->code = *p_charlist;*/
					/*else current_fcode->code = *p_charlist;*/

					/* Close the file */
					fclose(filecode);
				}

				if(name && depends == 0) current_function->name = copy_array_to_str(current_string);
				if(note) current_function->note = copy_array_to_str(current_string);
				if(code) { current_fcode->code = copy_array_to_str(current_string); foundfunctioncode = 1; }
				if(cur_state) current_function->current_state = copy_array_to_str(current_string);
				if(next_state) current_function->next_state = copy_array_to_str(current_string);
				if(input)
				{
					if(messagetype) current_ioput->messagetype = copy_array_to_str(current_string);
					if(filter)
					{
						if(lhs && value) current_rule_data->lhs = copy_array_to_str(current_string);
						else if(op) current_rule_data->op = copy_array_to_str(current_string);
						else if(rhs && value) current_rule_data->rhs = copy_array_to_str(current_string);
						//else current_ioput->filter_function = copy_array_to_str(current_string);
					}
				}
				if(output)
				{
					if(messagetype) current_ioput->messagetype = copy_array_to_str(current_string);
				}
				if(condition)
				{
					if(lhs && value) current_rule_data->lhs = copy_array_to_str(current_string);
					else if(op) current_rule_data->op = copy_array_to_str(current_string);
					else if(rhs && value) current_rule_data->rhs = copy_array_to_str(current_string);
					//else current_function->condition_function = copy_array_to_str(current_string);
					if(period) current_rule_data->lhs = copy_array_to_str(current_string);
					if(phase) current_rule_data->rhs = copy_array_to_str(current_string);
				}
				if(depends)
				{
					if(name) { current_adj_function->name = copy_array_to_str(current_string); }/*charlist = NULL; }*/
					if(type) { current_adj_function->type = copy_array_to_str(current_string); }/*charlist = NULL; }*/
				}
			}
			else if(xmachine && !memory && !function)
			{
				if(name)
				{
					current_xmachine = addxmachine(modeldata->p_xmachines, current_string->array);
					printf("Reading xagent named : ");
					printf(current_xmachine->name);
					printf("\n");
				}
			}
			else if(iteration_end_code)
			{
				if(code)
				{
					end_it_fcode = addfcode(modeldata->p_it_end_code);
					end_it_fcode->code = copy_array_to_str(current_string);
				}
			}
			else if(name)
			{
				if(modeldata->name == NULL) modeldata->name = copy_array_to_str(current_string);
				printf("Reading xagent model: ");
				printf(current_string->array);
				printf("\n");
			}

			/* Reset buffer */
			/*i = 0;*/
			reset_char_array(current_string);
		}
		/* If in tag put read char into buffer */
		else if(intag)
		{
			if(xmlcomment > 0)
			{
				if(xmlcomment == 1)
				{
					if(c == '!') { xmlcomment = 2; }
					else { xmlcomment = 0; add_char(current_string, c); }
				}
				else if(xmlcomment == 2)
				{
					if(c == '-') { xmlcomment = 3; }
					else { xmlcomment = 0; add_char(current_string, '!'); add_char(current_string, c); }
				}
				else if(xmlcomment == 3)
				{
					if(c == '-') { xmlcomment = 4; }
					else { xmlcomment = 0; add_char(current_string, '!'); add_char(current_string, '-'); add_char(current_string, c);}
				}
			}
			else
			{
				/*buffer[i] = c;*/
				/*i++;*/
				add_char(current_string, c);
			}
		}
		/* If in data read char into buffer */
		else if(model || enabled || codefile || ((iteration_end_code && code) || codefile || name || type || desc || (memory && (var && (type || name)))) ||
					(message && (name || (var && (type || name))))
					|| (state && (name || attribute || (transition && (func || dest))))
						|| (function && (not || name || note || code || depends || type || cur_state || next_state || input || output || messagetype || value || period || phase))
							|| (define && (name || value)) || (timetag && (name || unit || period)) || condition )
		{
			/*current_charlist = addchar(p_charlist);*/
			/*current_charlist->character = c;*/
			add_char(current_string, c);
		}
	}

	/* Free memory */
	free(time_name);
	free(unit_name);
	free_char_array(current_string);

	/* Close the file */
	fclose(file);
}

void handleRuleValue(char ** p_value, variable ** p_variable, xmachine_function * current_function, xmachine * current_xmachine, char * messagetype, model_data * modeldata)
{
	variable * current_variable;
	xmachine_message * current_message;
	//xmachine * current_xmachine2;
	char buffer[1000];
	int found, i, j;

	/* If starts with 'a.' change to 'a->' and check rest is a valid agent memory variable */
	if(strncmp(*p_value, "a.", 2) == 0)
	{
		/* check valid agent memory valiable */
		found = 0;
		strcpy(buffer, *p_value+2);
		current_variable = current_xmachine->variables;
		while(current_variable)
		{
			if(strcmp(buffer, current_variable->name) == 0)
			{
				found = 1;
				*p_variable = current_variable;
			}

			current_variable = current_variable->next;
		}
		if(found == 0)
		{
			fprintf(stderr, "ERROR: value '%s' in filter rule not in agent memory\n", buffer);
			fprintf(stderr, "       in function '%s' %s->%s in agent '%s'\n", current_function->name, current_function->current_state, current_function->next_state, current_xmachine->name);
			fprintf(stderr, "       in file: '%s'\n", current_function->file);
			exit(0);
		}

		/* Save agent variable to message type agent_var filter list */
/*		current_message = *modeldata->p_xmessages;
		if(messagetype == NULL) current_message = NULL;
		while(current_message)
		{
			if(strcmp(messagetype, current_message->name) == 0)
			{
				// Check agent variable not already in list
				found = 0;


				for(current_xmachine2 = current_message->agents; current_xmachine2; current_xmachine2 = current_xmachine2->next)
				{
					if(strcmp(current_xmachine2->name, current_xmachine->name) == 0)
					{
						for(current_variable = current_xmachine2->variables; current_variable; current_variable = current_variable->next)
						{
							if(strcmp(current_variable->name, buffer) == 0) found = 1;
						}
					}
				}



				if(found == 0)
				{
					current_xmachine2 = addxmachine(&current_message->agents, current_xmachine->name);
					// TODO

					current_variable = addvariable(&current_xmachine2->variables);
					current_variable->name = copystr(buffer);
					current_variable->type = copystr((*p_variable)->type);
					current_variable->agent = current_xmachine;
					strcpy(current_variable->c_type, (*p_variable)->c_type);
				}
			}

			current_message = current_message->next;
		}*/

		/* Update value */
		strcpy(buffer, "a->");
		strcat(buffer, *p_value+2);
		free(*p_value);
		*p_value = copystr(buffer);
	}
	/* If starts with 'm.' change to 'm->' and check rest is a valid message variable */
	else if(strncmp(*p_value, "m.", 2) == 0)
	{
		found = 0;
		strcpy(buffer, *p_value+2);
		current_message = *modeldata->p_xmessages;
		while(current_message)
		{
			if(strcmp(messagetype, current_message->name) == 0)
			{
				current_variable = current_message->vars;
				while(current_variable)
				{
					if(strcmp(buffer, current_variable->name) == 0)
					{
						found = 1;
						*p_variable = current_variable;
					}

					current_variable = current_variable->next;
				}
			}

			current_message = current_message->next;
		}
		if(found == 0)
		{
			fprintf(stderr, "ERROR: value '%s' in filter rule not in message variables\n", buffer);
			fprintf(stderr, "       in function '%s' %s->%s in agent '%s'\n", current_function->name, current_function->current_state, current_function->next_state, current_xmachine->name);
			fprintf(stderr, "       in file: '%s'\n", current_function->file);
			exit(0);
		}

		strcpy(buffer, "m->");
		strcat(buffer, *p_value+2);
		free(*p_value);
		*p_value = copystr(buffer);
	}
	/* If algorithm (i.e. radius check) then parser a. and m. to a-> and m-> */
	else
	{
		//printf("%s\n", *p_value);
		j = 0;
		for(i = 0; i < (int)strlen(*p_value); i++)
		{
			//printf("[%d] %c\n", i, (*p_value)[i]);
			if(i > 0)
			{
				if(((*p_value)[i-1] == 'a' || (*p_value)[i-1] == 'm') && (*p_value)[i] == '.')
				{
					buffer[j] = '-';
					buffer[++j] = '>';
				}
				else buffer[j] = (*p_value)[i];
			}
			else buffer[j] = (*p_value)[i];

			j++;
		}
		buffer[j++] = 0;
		//printf("buffer = [%s]\n", buffer);
		free(*p_value);
		*p_value = copystr(buffer);
	}

	/* Check if a number */

	/* Check if a time */

	/* What else can the value (be allowed) be? environment_constant? */

}

void handleRule(rule_data * current_rule_data, xmachine_function * current_function, xmachine * current_xmachine, char * messagetype, model_data * modeldata)
{
	time_data * current_time_unit;
	char buffer[100];

	/* If current_rule_data is NULL */
	if(current_rule_data != NULL)
	{
		if(current_rule_data->time_rule == 1)
		{
			/* Check period with time units */
			current_time_unit = * modeldata->p_time_units;
			while(current_time_unit)
			{
				if(strcmp(current_rule_data->lhs, current_time_unit->name) == 0)
				{
					sprintf(buffer, "%%%d", current_time_unit->iterations);
					current_rule_data->op = copystr(buffer);
				}

				current_time_unit = current_time_unit->next;
			}
			if(strcmp(current_rule_data->lhs, "iteration") == 0)
			{
				current_rule_data->op = copystr("");
			}
			if(current_rule_data->op == NULL)
			{
				fprintf(stderr, "ERROR: condition period '%s' is not a time unit\n", current_rule_data->lhs);
				exit(0);
			}

			handleRuleValue(&current_rule_data->rhs, &current_rule_data->rhs_variable, current_function, current_xmachine, messagetype, modeldata);
		}
		else
		{
			/* Handle values */
			if(current_rule_data->lhs == NULL) handleRule(current_rule_data->lhs_rule, current_function, current_xmachine, messagetype, modeldata);
			else handleRuleValue(&current_rule_data->lhs, &current_rule_data->lhs_variable, current_function, current_xmachine, messagetype, modeldata);

			if(current_rule_data->rhs == NULL) handleRule(current_rule_data->rhs_rule, current_function, current_xmachine, messagetype, modeldata);
			else handleRuleValue(&current_rule_data->rhs, &current_rule_data->rhs_variable, current_function, current_xmachine, messagetype, modeldata);

			/* Handle op */
			if(strcmp(current_rule_data->op, "EQ") == 0) { strcpy(current_rule_data->op, "=="); }
			else if(strcmp(current_rule_data->op, "NEQ") == 0) { strcpy(current_rule_data->op, "!="); }
			else if(strcmp(current_rule_data->op, "LEQ") == 0) { strcpy(current_rule_data->op, "<="); }
			else if(strcmp(current_rule_data->op, "GEQ") == 0) { strcpy(current_rule_data->op, ">="); }
			else if(strcmp(current_rule_data->op, "LT") == 0) { strcpy(current_rule_data->op, "<"); }
			else if(strcmp(current_rule_data->op, "GT") == 0) { strcpy(current_rule_data->op, ">"); }
			else if(strcmp(current_rule_data->op, "AND") == 0) { strcpy(current_rule_data->op, "&&"); }
			else if(strcmp(current_rule_data->op, "OR") == 0) { strcpy(current_rule_data->op, "||"); }
			else
			{
				fprintf(stderr, "ERROR: '%s' operator not one of EQ/NEQ/LEQ/GEQ/LT/GT/AND/OR\n", current_rule_data->op);
				fprintf(stderr, "       in function '%s' %s->%s in agent '%s'\n", current_function->name, current_function->current_state, current_function->next_state, current_xmachine->name);
				fprintf(stderr, "       in file: '%s'\n", current_function->file);
				exit(0);
			}
		}
	}
}

int checkmodel(model_data * modeldata)
{
	xmachine * current_xmachine;
	xmachine * current_xmachine2;
	variable * current_variable;
	variable * current_variable2;
	model_datatype * current_datatype;
	xmachine_function * current_function;
	xmachine_function * current_function2;
	adj_function * current_adj_function;
	xmachine_ioput * current_ioput;
	rule_data * current_rule_data;
	xmachine_message * current_message;
	xmachine_message * current_message2;
	variable * allvar;
	variable * envvar;
	char buffer[1000];
	int variable_count;
	int found;
	int state_number;
	int newlayer = 0;
	int totallayers = 0;
	int m = 0;
	int k;

	/* Check model for:
	 * agent/message variables are only known data types
	 * agent/message variables only appear once, show warning for duplicates and show from which files
	 * try and parse functions files for implementations of functions defined in model xml
	 * */

	/* Check for no agents */
	if(*modeldata->p_xmachines == NULL)
	{
		fprintf(stderr, "ERROR: no agents defined\n");
		return -1;
	}

	/* Check duplicate message names */
	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		current_message2 = * modeldata->p_xmessages;
		{
			if(current_message != current_message2)
			{
				if(strcmp(current_message->name, current_message2->name) == 0)
				{
					fprintf(stderr, "ERROR: multiple definitions of message '%s'\n", current_message->name);
					fprintf(stderr, "       in file: '%s'\n", current_message->file);
					fprintf(stderr, "       in file: '%s'\n", current_message2->file);
					return -1;
				}
			}

			current_message2 = current_message2->next;
		}

		current_message = current_message->next;
	}

	/* Check agent memory variables for being a model data type and update variable attributes */
	current_xmachine = *modeldata->p_xmachines;
	current_xmachine2 = NULL;
	while(current_xmachine)
	{
		current_variable = current_xmachine->variables;

		/* Error if no variables */
		if(current_variable == NULL)
		{
			/*fprintf(stderr, "ERROR: agent '%s' has no memory variables\n", current_xmachine->name);
			return -1;*/
			fprintf(stderr, "WARNING: no memory variables found in '%s' agent, agent removed from model\n", current_xmachine->name);
			/* Remove agent from the agent list */
			if(current_xmachine2 == NULL) * modeldata->p_xmachines = current_xmachine->next;
			else current_xmachine2->next = current_xmachine->next;

			free(current_xmachine->name);
			freevariables(&current_xmachine->variables);
			freexstates(&current_xmachine->states);
			freexfunctions(&current_xmachine->functions);
			freestateholder(&current_xmachine->end_states);
			free(current_xmachine);

			if(current_xmachine2 == NULL) current_xmachine = * modeldata->p_xmachines;
			else current_xmachine = current_xmachine2->next;
		}
		else
		{
			/* Error if variables type is not a C type or a data type or an array of these */
			while(current_variable)
			{
				found = 0;
				if(strcmp(current_variable->type, "int") == 0 ||
					strcmp(current_variable->type, "short int") == 0 ||
					strcmp(current_variable->type, "long int") == 0 ||
					strcmp(current_variable->type, "unsigned int") == 0 ||
					strcmp(current_variable->type, "unsigned short int") == 0 ||
					strcmp(current_variable->type, "unsigned long int") == 0 ||
					strcmp(current_variable->type, "int_array") == 0 ||
					strcmp(current_variable->type, "double") == 0 ||
					strcmp(current_variable->type, "float") == 0 ||
					strcmp(current_variable->type, "double_array") == 0 ||
					strcmp(current_variable->type, "float_array") == 0 ||
					strcmp(current_variable->type, "char") == 0 ||
					strcmp(current_variable->type, "unsigned char") == 0 ||
					strcmp(current_variable->type, "char_array") == 0)
					{
						found = 1;
					}

					/* Handle model defined data types */
					current_datatype = * modeldata->p_datatypes;
					while(current_datatype)
					{
						if(strcmp(current_variable->type, current_datatype->name) == 0)
						{
							found = 1;
						}

						strcpy(buffer, current_datatype->name);
						strcat(buffer, "_array");

						if(strcmp(current_variable->type, buffer) == 0)
						{
							found = 1;
						}

						current_datatype = current_datatype->next;
					}

					if(found == 0)
					{
						fprintf(stderr, "ERROR: type variable '%s' in variable '%s' in agent '%s' doesn't exist\n", current_variable->type, current_variable->name, current_xmachine->name);
						fprintf(stderr, "       in file: '%s'\n", current_variable->file);
						return -1;
					}

				current_variable = current_variable->next;
			}

			/* Error if a variable name is defined twice in same agent */
			current_variable = current_xmachine->variables;
			while(current_variable)
			{
				current_variable2 = current_xmachine->variables;
				while(current_variable2)
				{
					if(strcmp(current_variable->name, current_variable2->name) == 0 && current_variable != current_variable2)
					{
						fprintf(stderr, "ERROR: multiple uses of variable '%s' in agent '%s'\n", current_variable->name, current_xmachine->name);
						fprintf(stderr, "       in file: '%s'\n", current_variable->file);
						fprintf(stderr, "       in file: '%s'\n", current_variable2->file);
						return -1;
					}

					current_variable2 = current_variable2->next;
				}

				current_variable = current_variable->next;
			}

			current_variable = current_xmachine->variables;
			while(current_variable)
			{
				/* Handle model defined data types */
				current_datatype = * modeldata->p_datatypes;
				while(current_datatype)
				{
					if(strcmp(current_variable->type, current_datatype->name) == 0)
					{
						current_variable->ismodeldatatype = 1;
						current_variable->datatype = current_datatype;
					}

					strcpy(buffer, current_datatype->name);
					strcat(buffer, "_array");

					if(strcmp(current_variable->type, buffer) == 0)
					{
						current_variable->ismodeldatatype = 1;
						current_variable->datatype = current_datatype;
						current_variable->arraylength = -1;
					}

					current_datatype = current_datatype->next;
				}

				current_variable = current_variable->next;
			}

			/* Compute x,y,z location variables in memory */
			/* Check for x-axis component as 'posx' or 'px' or 'x' */
			strcpy(current_xmachine->xvar, "0.0");
			strcpy(current_xmachine->yvar, "0.0");
			strcpy(current_xmachine->zvar, "0.0");
			variable_count = 0;

			current_variable = current_xmachine->variables;
			while(current_variable)
			{
				if(current_variable->arraylength != 0) modeldata->agents_include_array_variables = 1;

				/*copycharlist(&current_variable->name, &chardata[0]);*/
				if(strcmp(current_variable->name, "x") == 0)    strcpy(current_xmachine->xvar, "x");
				if(strcmp(current_variable->name, "px") == 0)   strcpy(current_xmachine->xvar, "px");
				if(strcmp(current_variable->name, "posx") == 0) strcpy(current_xmachine->xvar, "posx");
				if(strcmp(current_variable->name, "y") == 0)    strcpy(current_xmachine->yvar, "y");
				if(strcmp(current_variable->name, "py") == 0)   strcpy(current_xmachine->yvar, "py");
				if(strcmp(current_variable->name, "posy") == 0) strcpy(current_xmachine->yvar, "posy");
				if(strcmp(current_variable->name, "z") == 0)    strcpy(current_xmachine->zvar, "z");
				if(strcmp(current_variable->name, "pz") == 0)   strcpy(current_xmachine->zvar, "pz");
				if(strcmp(current_variable->name, "posz") == 0) strcpy(current_xmachine->zvar, "posz");
				if(strcmp(current_variable->name, "range") == 0) strcpy(current_xmachine->rangevar, "range");
				if(strcmp(current_variable->name, "radius") == 0) strcpy(current_xmachine->rangevar, "radius");
				if(strcmp(current_variable->name, "id") == 0) strcpy(current_xmachine->idvar, "id");
				if(strcmp(current_variable->name, "agent_id") == 0) strcpy(current_xmachine->idvar, "agent_id");

				/* Agent variable names cannot be the same as environment variable names
				 * as the preprocessor define macro will be broken */
				envvar = * modeldata->p_envvars;
				while(envvar)
				{
					if(strcmp(current_variable->name, envvar->name) == 0)
					{
						fprintf(stderr, "ERROR: variable '%s' in agent '%s' has same name as an environment variable\n", current_variable->name, current_xmachine->name);
						return -1;
					}

					envvar = envvar->next;
				}
				
				allvar = addvariable(modeldata->p_allvars);
				allvar->name = copystr(current_variable->name);
				allvar->type = copystr(current_variable->type);
				allvar->arraylength = current_variable->arraylength;
				allvar->ismodeldatatype = current_variable->ismodeldatatype;
				allvar->datatype = current_variable->datatype;
				allvar->typenotarray = copystr(current_variable->typenotarray);
				strcpy(allvar->defaultvalue, current_variable->defaultvalue);
				strcpy(allvar->c_type, current_variable->c_type);

				variable_count++;

				current_variable = current_variable->next;
			}

			current_xmachine->var_number = variable_count;

			modeldata->number_xmachines++;

			current_xmachine = current_xmachine->next;
		}
	}
	
	/* If functions have old style depends tags create states */
	if(modeldata->depends_style == 1)
	{
		/* Go through all adj functions in depends
		 * and link the function pointer to the actual function */
		current_xmachine = *modeldata->p_xmachines;
		while(current_xmachine)
		{
			current_function = current_xmachine->functions;
			while(current_function)
			{
				current_adj_function = current_function->depends;
				while(current_adj_function)
				{
					current_xmachine2 = *modeldata->p_xmachines;
					while(current_xmachine2)
					{
						current_function2 = current_xmachine2->functions;
						while(current_function2)
						{
							if(strcmp(current_adj_function->name, current_function2->name) == 0)
							{
								//printf("** %s -> %s == %s\n", current_function->name, current_adj_function->name, current_function2->name);
								current_adj_function->function = current_function2;
							}

							current_function2 = current_function2->next;
						}

						current_xmachine2 = current_xmachine2->next;
					}

					current_adj_function = current_adj_function->next;
				}

				current_function = current_function->next;
			}

			current_xmachine = current_xmachine->next;
		}

		/* Find order of functions in each agent */
		newlayer = 0;
		m = 0;
		while(m == 0)
		{
			/* For each agent */
			current_xmachine = * modeldata->p_xmachines;
			while(current_xmachine)
			{
				/* For each function */
				current_function = current_xmachine->functions;
				while(current_function)
				{
					/* If rank_in is unknown */
					if(current_function->rank_in == -1)
					{
						k = 0;
						/* Search dependencies on the current function */
						current_adj_function = current_function->depends;
						while(current_adj_function)
						{
							/* Check if the function has a dependency on it (and not from itself) */
							/* Check to see if the dependency is a function and not be assigned a layer in last layers */
							if(current_adj_function->function->rank_in == -1 || current_adj_function->function->rank_in == newlayer)
							{
								/* Change flag to say there is a dependency */
								k = 1;
							}

							current_adj_function = current_adj_function->next;
						}
						if(k == 0)
						{
							current_function->rank_in = newlayer;

							printf("%s - %s - %d\n", current_xmachine->name, current_function->name, newlayer);

							//addfunction_pointer(&current_layer->functions, current_function);
						}
					}

					current_function = current_function->next;
				}

				current_xmachine = current_xmachine->next;
			}
			/* Increment layer */
			newlayer++;
			/* If all the functions have layers then stop */
			/* Set flag to all functions have a layer */
			k = 0;
			current_xmachine = * modeldata->p_xmachines;
			while(current_xmachine)
			{
				/* For each function */
				current_function = current_xmachine->functions;
				while(current_function)
				{
					/* If a function does not have a layer yet */
					if(current_function->rank_in == -1) k = 1;

					current_function = current_function->next;
				}

				current_xmachine = current_xmachine->next;
			}
			/* If flag is still zero then stop */
			if(k == 0) m = 1;
			else
			{
				//current_layer = addlayer(modeldata->p_layers);
			}
		}

		totallayers = newlayer;

		current_xmachine = *modeldata->p_xmachines;
		while(current_xmachine)
		{
			state_number = 0;

			//qwerty
			newlayer = 0;
			m = 0;
			while(newlayer <= totallayers)
			{
				/* For each function */
				current_function = current_xmachine->functions;
				while(current_function)
				{
					if(current_function->rank_in == newlayer)
					{
						if(state_number == 0) sprintf(buffer, "%s", "start");
						else sprintf(buffer, "%i", state_number);
						current_function->current_state = copystr(buffer);
						state_number++;
						sprintf(buffer, "%i", state_number);
						current_function->next_state = copystr(buffer);
						addxstate(current_function->current_state, current_function->agent_name, &current_xmachine->states);
						/* Save last function */
						current_function2 = current_function;
					}

					current_function = current_function->next;
				}

				newlayer++;
			}

			free(current_function2->next_state);
			sprintf(buffer, "%s", "end");
			current_function2->next_state = copystr(buffer);
			addxstate(current_function2->next_state, current_function2->agent_name, &current_xmachine->states);

			current_function = current_xmachine->functions;
			while(current_function)
			{
				current_function->rank_in = -1;

				current_adj_function = current_function->depends;
				while(current_adj_function)
				{
					// Find pointers to functions for function names
					current_xmachine2 = *modeldata->p_xmachines;
					while(current_xmachine2)
					{
						current_function2 = current_xmachine2->functions;
						while(current_function2)
						{
							if(strcmp(current_adj_function->name, current_function2->name) == 0)
							{
								current_adj_function->function = current_function2;
							}

							current_function2 = current_function2->next;
						}

						current_xmachine2 = current_xmachine2->next;
					}

					if(current_adj_function->function == NULL)
					{
						fprintf(stderr, "ERROR: depends function '%s' does not exist\n",
						current_adj_function->name);
						return -1;
					}

					if(strcmp(current_adj_function->type, "internal") != 0)
					{
						/* If communication dependency */
						/* Add input to current function */
						/* check if already exists */
						found = 0;
						current_ioput = current_function->inputs;
						while(current_ioput)
						{
							if(strcmp(current_adj_function->type, current_ioput->messagetype) == 0) found = 1;

							current_ioput = current_ioput->next;
						}
						if(found == 0)
						{
							current_ioput = addioput(&current_function->inputs);
							current_ioput->function = current_function;
							current_ioput->messagetype = copystr(current_adj_function->type);
						}
						/* Add output to depends on function */
						found = 0;
						current_ioput = current_adj_function->function->outputs;
						while(current_ioput)
						{
							if(strcmp(current_adj_function->type, current_ioput->messagetype) == 0) found = 1;

							current_ioput = current_ioput->next;
						}
						if(found == 0)
						{
							current_ioput = addioput(&current_adj_function->function->outputs);
							current_ioput->messagetype = copystr(current_adj_function->type);
							current_ioput->function = current_function;
						}
					}

					current_adj_function = current_adj_function->next;
				}

				current_function = current_function->next;
			}

			current_xmachine = current_xmachine->next;
		}
	}

	/* Check condition/filter rules */
	current_xmachine = *modeldata->p_xmachines;
	while(current_xmachine)
	{
		// TODO
		/* Message filters */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			/* If function has a condition... */
			if(current_function->condition_rule != NULL)
			{
				strcpy(buffer, "FLAME_condition_");
				strcat(buffer, current_xmachine->name);
				strcat(buffer, "_");
				strcat(buffer, current_function->name);
				strcat(buffer, "_");
				strcat(buffer, current_function->current_state);
				strcat(buffer, "_");
				strcat(buffer, current_function->next_state);
				current_function->condition_function = copystr(buffer);

				handleRule(current_function->condition_rule, current_function, current_xmachine, NULL, modeldata);
			}

			current_ioput = current_function->inputs;
			while(current_ioput)
			{
				/* Check if message type exists */
				found = 0;
				current_message = * modeldata->p_xmessages;
				while(current_message)
				{
					if(strcmp(current_ioput->messagetype, current_message->name) == 0)
					{
						found = 1;
						current_ioput->message = current_message;
					}

					current_message = current_message->next;
				}
				if(found == 0)
				{
					fprintf(stderr, "ERROR: input message type '%s' in function '%s' in agent '%s' in file '%s' doesn't exist\n",
								current_ioput->messagetype, current_function->name, current_xmachine->name, current_function->file);
					return -1;
				}



				/* If input message has a filter */
				if(current_ioput->filter_rule != NULL)
				{
					current_rule_data = current_ioput->filter_rule;
					strcpy(buffer, "FLAME_filter_");
					strcat(buffer, current_xmachine->name);
					strcat(buffer, "_");
					strcat(buffer, current_function->name);
					strcat(buffer, "_");
					strcat(buffer, current_function->current_state);
					strcat(buffer, "_");
					strcat(buffer, current_function->next_state);
					strcat(buffer, "_");
					strcat(buffer, current_ioput->messagetype);
					current_ioput->filter_function = copystr(buffer);

					handleRule(current_ioput->filter_rule, current_function, current_xmachine, current_ioput->messagetype, modeldata);
				}

				current_ioput = current_ioput->next;
			}

			current_ioput = current_function->outputs;
			while(current_ioput)
			{
				/* Check if message type exists */
				found = 0;
				current_message = * modeldata->p_xmessages;
				while(current_message)
				{
					if(strcmp(current_ioput->messagetype, current_message->name) == 0)
					{
						found = 1;
					}

					current_message = current_message->next;
				}
				if(found == 0)
				{
					fprintf(stderr, "ERROR: output message type '%s' in function '%s' in agent '%s' in file '%s' doesn't exist\n",
								current_ioput->messagetype, current_function->name, current_xmachine->name, current_function->file);
					return -1;
				}

				current_ioput = current_ioput->next;
			}


			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}

	return 0;
}
