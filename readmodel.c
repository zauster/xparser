#include "header.h"


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
	i = strlen(current_variable->type);
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
			current_variable->typenotarray = copy_array_to_str(current_string);
		}
	}
}

void handleVariableName(char_array * current_string, variable * current_variable)
{
	current_variable->name = copy_array_to_str(current_string);
	
	int j, i = 0;
	char buffer[100];
	
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
void readModel(char * inputfile, char * directory, model_data * modeldata)
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
	int foundfunctioninfile;
	/* Char and char buffer for reading file to */
	char c = ' ';
	int numtag = 0;
	char chartag[10][100];
	int tagline[10];
	char chardata[100];
	/*char chardata2[100];*/
	char findfunction[1000];
	int found;
	int linenumber = 1;
	int variable_count;
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
	int header, iteration_end_code, depends, datatype, desc;
	/* Pointer to new structs */
	xmachine_memory * current_memory;
	xmachine_message * current_message;
	xmachine_state * current_state;
	xmachine_function * current_function;
	variable * allvar;
	variable * current_envvar;
	variable * current_envdefine;
	env_func * current_envfunc;
	variable ** p_variable;
	variable * tvariable;
	variable * current_variable;
	model_datatype * current_datatype;
	/*char_list ** p_charlist;*/
	/*char_list * charlist;*/
	/*char_list * current_charlist;*/
	s_attrib ** p_attrib;
	s_attrib * attrib;
	s_attrib * current_attrib;
	s_trans ** p_trans;
	s_trans * trans = NULL;
	s_trans * current_trans;
	f_code ** p_fcode;
	f_code * fcode;
	f_code * current_fcode;
	f_code * end_it_fcode;
	/* new use of char_array */
	char_array * current_string;
	
	/* check var name for array */
	/*char_list * charcheck;*/
	modeldata->number_messages = 0;
	modeldata->number_xmachines = 0;
	modeldata->agents_include_array_variables = 0;
	
	/* Open config file to read-only */
	if((file = fopen(inputfile, "r"))==NULL)
	{
		printf("*** ERROR: Cannot read file: %s\n", inputfile);
		exit(1);
	}
	else { printf("reading xmml: %s\n", inputfile); }
	
	/* Initialise variables */
	/*p_charlist = &charlist;*/
	p_attrib = &attrib;
	p_trans = &trans;
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
	attribute = 0;
	transition = 0;
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
	
	/*printf("%i> ", linenumber);*/
	
	/* Read characters until the end of the file */
	/*while(c != EOF)*/
	/* Read characters until end of xml found */
	while(reading == 1)
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
			if(xmlcomment == 5 && c == '-') xmlcomment = 6;
			if(xmlcomment == 6 && c == '>') { xmlcomment = 0; intag = 0; i = 0; }
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
				if(strcmp(&current_string->array[1], &chartag[numtag][0]) != 0)
				{
					printf("*** ERROR: The tag <%s> on line number %i\n", current_string->array, linenumber);
					printf("*** ERROR: doesn't close the tag <%s> on line number %i\n", &chartag[numtag][0], tagline[numtag]);
					printf("*** ERROR: Exit xparser\n\n");
					exit(1);
				}
			}
			
			if(strcmp(current_string->array, "xmachine_agent_model") == 0) { xagentmodel = 1; }
			if(strcmp(current_string->array, "/xmachine_agent_model") == 0) { xagentmodel = 0; reading = 0; printf("End of xagent model.\n"); }
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
			if(strcmp(current_string->array, "xmachine") == 0)
			{
				xmachine = 1;
				current_xmachine = addxmachine(modeldata->p_xmachines);
			}
			if(strcmp(current_string->array, "/xmachine") == 0)
			{
				xmachine = 0;
				/* Compute x,y,z location variables in memory */
				/* Check for x-axis component as 'posx' or 'px' or 'x' */
				strcpy(current_xmachine->zvar, "0.0");
				variable_count = 0;
				current_memory = current_xmachine->memory;
				while(current_memory)
				{
					current_variable = current_memory->vars;
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
						
						found = 0;
						allvar = * modeldata->p_allvars;
						while(allvar)
						{
							/*copycharlist(&allvar->name, &chardata2[0]);*/
							if(strcmp(current_variable->name, allvar->name) == 0) found = 1;
							
							allvar = allvar->next;
						}
						if(found == 0)
						{
							allvar = addvariable(modeldata->p_allvars);
							allvar->name = current_variable->name;
							allvar->type = current_variable->type;
							allvar->arraylength = current_variable->arraylength;
							allvar->ismodeldatatype = current_variable->ismodeldatatype;
							allvar->datatype = current_variable->datatype;
							allvar->typenotarray = current_variable->typenotarray;
							strcpy(allvar->defaultvalue, current_variable->defaultvalue);
							strcpy(allvar->c_type, current_variable->c_type);
						}
						
						variable_count++;
						
						current_variable = current_variable->next;
					}
					
					current_memory = current_memory->next;
				}
				
				current_xmachine->var_number = variable_count;
				
				modeldata->number_xmachines++;
			}
			if(strcmp(current_string->array, "memory") == 0)
			{
				memory = 1;
				/*current_memory = addxmemory(p_xmemory);*/
				current_memory = addxmemory(&current_xmachine->memory);
				tvariable = NULL;
			}
			if(strcmp(current_string->array, "/memory") == 0)
			{
				memory = 0;
				current_memory->vars = *p_variable;
			}
			if(strcmp(current_string->array, "var") == 0)
			{
				var = 1;
				if(datatype == 1) current_variable = addvariable(p_variable);
				else if(environment == 1) current_envvar = addvariable(modeldata->p_envvars);
				else current_variable = addvariable(p_variable);
			}
			if(strcmp(current_string->array, "/var") == 0) { var = 0; }
			if(strcmp(current_string->array, "type") == 0) { type = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/type") == 0) { type = 0; }
			if(strcmp(current_string->array, "states") == 0) { states = 1; }
			if(strcmp(current_string->array, "/states") == 0) { states = 0; }
			if(strcmp(current_string->array, "state") == 0)
			{
				state = 1;
				/*current_state = addxstate(p_xstates);*/
				current_state = addxstate(&current_xmachine->states);
				attrib = NULL;
				trans = NULL;
			}
			if(strcmp(current_string->array, "/state") == 0)
			{
				state = 0;
				current_state->attributes = *p_attrib;
				current_state->transitions = *p_trans;
			}
			if(strcmp(current_string->array, "attribute") == 0)
			{
				attribute = 1;
				current_attrib = addsattrib(p_attrib);
				/*charlist = NULL;*/
				reset_char_array(current_string);
			}
			if(strcmp(current_string->array, "/attribute") == 0) { attribute = 0; }
			if(strcmp(current_string->array, "trans") == 0)
			{
				transition = 1;
				current_trans = addtrans(p_trans);
				/*charlist = NULL;*/
				reset_char_array(current_string);
			}
			if(strcmp(current_string->array, "/trans") == 0) { transition = 0; }
			if(strcmp(current_string->array, "func") == 0) { func = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/func") == 0) { func = 0; }
			if(strcmp(current_string->array, "dest") == 0) { dest = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/dest") == 0) { dest = 0; }
			if(strcmp(current_string->array, "datatype") == 0)
			{
				datatype = 1;
				current_datatype = adddatatype(modeldata->p_datatypes);
				tvariable = NULL;
				reset_char_array(current_string);
			}
			if(strcmp(current_string->array, "/datatype") == 0)
			{
				datatype = 0;
				current_datatype->vars = *p_variable;
			}
			if(strcmp(current_string->array, "functions") == 0) { functions = 1; }/*charlist = NULL; }*/
			if(strcmp(current_string->array, "/functions") == 0) { functions = 0; }
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
				}
			}
			if(strcmp(current_string->array, "/function") == 0)
			{
				function = 0;
				foundfunctioninfile = 0;
				
				if(foundfunctioncode == 0)
				{
					printf("Function ");
					printf(current_function->name);
					/*printf(" function not found, look in functions file\n");*/
					
					/*charlist = NULL;*/
					reset_char_array(current_string);
					current_fcode = addfcode(p_fcode);
					current_fcode->code = copy_array_to_str(current_string);/*charlist;*/
					current_function->code = *p_fcode;
					
					current_envfunc = * modeldata->p_envfuncs;
					while(current_envfunc)
					{
						if(current_envfunc->header == 2)
						{
							/* parse functions file looking for function and read in code */
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
							/*current_charlist = current_envfunc->filepath;
							while(current_charlist)
							{
								chardata[j] = current_charlist->character;
								j++;
								
								current_charlist = current_charlist->next;
							}
							chardata[j] = 0;*/
							/*printf("01\t%s\n", chardata);*/
							
							strcpy(chardata, directory);
							strcat(chardata, current_envfunc->filepath);
							
							/* Open code file read-only */
							if((filecode = fopen(chardata, "r"))==NULL)
							{
								printf("*** ERROR: Cannot read file: %s\n", chardata);
								printf("*** ERROR: xparser aborted ***\n");
								exit(1);
							}
							
							i = 0;
							j = 0;
							c = ' ';
							reset_char_array(current_string);
							
							/* Read characters until the end of the file */
							while(c != (char)EOF)
							{
								/* Get the next char from the file */
								c = (char)fgetc(filecode);
								
								if(c == '\n')
								{
									if((i > 0) && findfunction[i-1] == ')' &&
										findfunction[0] == 'i' &&
										findfunction[1] == 'n' &&
										findfunction[2] == 't')
									{
										findfunction[i-2] = 0;
										/*printf("ff: %s\n", findfunction+4);*/
										/*copycharlist(&current_function->name, chardata2);*/
										/*printf("chardata: %s\n", chardata);*/
										
										if(strcmp(findfunction+4, current_function->name) == 0)
										{
											printf(": found in %s\n", chardata);
											foundfunctioninfile = 1;
											/* number of open brackets*/
											while(c != '{') c = (char)fgetc(filecode);
											j = 1;
											/*charlist = NULL;*/
											reset_char_array(current_string);
											while(j>0)
											{
												c = (char)fgetc(filecode);
												if(c == '{') j++;
												if(c == '}') j--;
												
												if(j != 0)
												{
													/*current_charlist = addchar(p_charlist);*/
													/*current_charlist->character = c;*/
													add_char(current_string, c);
												}
											}
										}
									}
									
									i = 0;
								}
								else
								{
									findfunction[i] = c;
									i++;
								}
							}
							
							/* Close the file */
							fclose(filecode);
						}
						
						current_envfunc = current_envfunc->next;
					}
					
					current_fcode->code = copy_array_to_str(current_string);/**p_charlist;*/
					
					if(foundfunctioninfile == 0) printf(": not found in function file(s)\n");
					
					/*printf("code: ");*/
					/*printcharlist(&current_fcode->code);*/
					/*printf("\n");*/
				}
				
				if(environment == 0)
				{
					current_function->code = *p_fcode;
				}
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
			}
			if(strcmp(current_string->array, "/message") == 0)
			{
				message = 0;
				current_message->vars = *p_variable;
				
				/* Count number of variables */
				variable_count = 0;
				/* Find 'range' variable */
				found = 0;
				current_variable = current_message->vars;
				while(current_variable)
				{
					variable_count++;
					if(strcmp(current_variable->name, "range") == 0) found = 1;
					
					current_variable = current_variable->next;
				}
				
				if(found == 0)
				{
					printf("*** Error: No 'range' variable in %s message\n", current_message->name);
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
			if(strcmp(current_string->array, "depends") == 0)
			{
				depends = 1;
				/*charlist = NULL;*/
				current_trans = addtrans(&current_function->depends);
			}
			if(strcmp(current_string->array, "/depends") == 0)
			{
				depends = 0;
				/*
				printf("depends: ");
				printcharlist(&current_trans->func);
				printf(" ");
				printcharlist(&current_trans->dest);
				printf("\n");
				*/
			}
			
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
			
			if(environment)
			{
				if(datatype)
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
					}
				}
				else if(var)
				{
					if(type)
					{
						current_envvar->type = copy_array_to_str(current_string);
						
						strcpy(current_envvar->defaultvalue, "");
						strcpy(current_envvar->c_type, "");
						
						if(strcmp(current_envvar->type, "int") == 0 ||
							strcmp(current_envvar->type, "short int") == 0 ||
							strcmp(current_envvar->type, "long int") == 0 ||
							strcmp(current_envvar->type, "unsigned int") == 0 ||
							strcmp(current_envvar->type, "unsigned short int") == 0 ||
							strcmp(current_envvar->type, "unsigned long int") == 0)
						{
							strcpy(current_envvar->defaultvalue, "0");
							strcpy(current_envvar->c_type, "i");
						}
						
						if(strcmp(current_envvar->type, "double") == 0 ||
							strcmp(current_envvar->type, "float") == 0)
						{
							strcpy(current_envvar->defaultvalue, "0.0");
							strcpy(current_envvar->c_type, "f");
						}
						
						if(strcmp(current_envvar->type, "char") == 0 ||
							strcmp(current_envvar->type, "unsigned char") == 0)
						{
							strcpy(current_envvar->defaultvalue, "' '");
							strcpy(current_envvar->c_type, "c");
						}
					}
					if(name) current_envvar->name = copy_array_to_str(current_string);
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
					while(*directory != 0)
					{
						chardata[j] = *directory;
						(directory)++;
						j++;
					}
					/* Make directory pointer point to start of chars again */
					for(k = 0; k < j; k++)
					{
						(directory)--;
					}
					
					chardata[j] = '\0';
					
					current_envfunc->filepath = copy_array_to_str(current_string);
					/*printf("file: %s\n", current_envfunc->filepath);*/
					
					/* Add file name to chardata on end of directory */
					/*current_charlist = *p_charlist;
					while(current_charlist)
					{
						chardata[j] = current_charlist->character;
						j++;
						
						current_charlist = current_charlist->next;
					}
					chardata[j] = 0;*/
					strcat(chardata, current_envfunc->filepath);
					/*printf("01\t%s\n", chardata);*/
					
					/* Open code file read-only */
					if((filecode = fopen(chardata, "r"))==NULL)
					{
						printf("*** ERROR: Cannot read file: %s\n", chardata);
						printf("*** ERROR: xparser aborted ***\n");
						exit(1);
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
				if(attribute) current_attrib->attrib = copy_array_to_str(current_string);
				if(transition)
				{
					if(func) current_trans->func = copy_array_to_str(current_string);
					if(dest) current_trans->dest = copy_array_to_str(current_string);
				}
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
						printf("*** ERROR: Cannot read file: %s\n", chardata);
						printf("*** ERROR: xparser aborted ***\n");
						exit(1);
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
				if(depends)
				{
					if(name) { current_trans->func = copy_array_to_str(current_string); }/*charlist = NULL; }*/
					if(type) { current_trans->dest = copy_array_to_str(current_string); }/*charlist = NULL; }*/
				}
			}
			else if(xmachine)
			{
				if(name)
				{
					current_xmachine->name = copy_array_to_str(current_string);
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
				modeldata->name = copy_array_to_str(current_string);
				printf("Reading xagent model: ");
				printf(modeldata->name);
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
		else if(((iteration_end_code && code) || codefile || name || type || desc || (memory && (var && (type || name)))) ||
					(message && (name || (var && (type || name))))
					|| (state && (name || attribute || (transition && (func || dest))))
						|| (function && (name || note || code || depends || type))
							|| (define && (name || value)))
		{
			/*current_charlist = addchar(p_charlist);*/
			/*current_charlist->character = c;*/
			add_char(current_string, c);
		}
	}
	
	/* Free memory */
	free_char_array(current_string);
	
	/* Close the file */
	fclose(file);
}
