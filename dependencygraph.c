#include "header.h"

/** \fn char * copystr(char * string)
 * \brief Copy a string into new memory.
 * \param string The string to copy.
 * \return The copy of the string.
 */
char * copystr(char * string)
{
	char * new_string = (char *)malloc( (strlen(string) + 1) * sizeof(char));
	return strcpy(new_string, string);
}

/** \fn void create_dependency_graph(char * filepath, model_data * modeldata)
 * \brief Calculate agent functions dependency graph and produce a dot graph description output.
 * \param filepath Pointer to the file path and name.
 * \param modeldata Data from the model.
 */
void create_dependency_graph(char * filepath, model_data * modeldata)
{
	/* pointers to model datatypes */
	xmachine * current_xmachine;
	xmachine_function * current_function;
	xmachine_message * current_message;
	f_code * current_code;
	s_trans * current_trans;
	layer * current_layer;
	char_list ** p_charlist;
	char_list * charlist;
	
	/* Holds all the function names */
	char functions[100][100]; 
	/* Holds the xagent associated with the function */
	char funcxagent[100][100];
	/* Buffer to calculate function name */
	char funcname[100];
	/* Holds the dgraph 'layer' or order of the functions in functions[] */
	int funclayer[100];
	/* Buffer to hold source code of a function */
	char funcchar[100000];
	/* Holds the no of lines in functions in functions[] */
	int nolinesinf[100];
	/* Buffer to hold message name */
	char messagen[100];
	/* Buffer to hold string used to read messages */
	char messager[100];
	/* Buffer to hold string used when writing a message */
	char messagew[100];
	/* Next 2 arrays hold function name, message name, and action: */
	/* 1 for sending, 2 for reading, 3 for internal */
	char actionfname[100][100];
	char actionmname[100][100];
	int  action[100];
	/* Next 3 arrays hold function name and the function it depends on and why */
	char dependsfname[100][100];
	char dependson[100][100];
	char dependsaction[100][100];
	/* Buffer for concatenating strings */
	char buffer[1000];
	
	int i = 0;
	/* Positioning of rectangles in svg file */
	int x, y, layerheight, layerwidth, boxheight, boxwidth, tallestbox;
	int j, k, m, n;
	int nofunctions;
	int nolines;
	int nodepends;
	int layer = 0;
	/* File to write dgraph.dot to */
	FILE *file;
	
	/* Initialise variables */
	p_charlist = &charlist;
	
	printf("Creating dependency graph\n");
	
	/* Gather information */
	
	/* nofunctions = -1 so that when first incremented equals zero */
	nofunctions = -1;
	nodepends = 0;
	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			/*copycharlist(&current_xmachine->name, buffer);
			strcpy(funcname, buffer);
			strcat(funcname, "_");
			copycharlist(&current_function->name, buffer);
			strcat(funcname, buffer);*/
			
			//copycharlist(&current_function->name, funcname);
			strcpy(funcname, current_function->name);
			
			nofunctions++;
			strcpy(functions[nofunctions], funcname);
			//copycharlist(&current_xmachine->name, funcxagent[nofunctions]);
			strcpy(funcxagent[nofunctions], current_xmachine->name);
			funclayer[nofunctions] = -1;
			
			/* For each (should only be one) source code of function */
			current_code = current_function->code;
			while(current_code)
			{
				/* Copy the source code (in charlist data structure) into a char array */
				//copycharlist(&current_code->code, funcchar);
				strcpy(funcchar, current_code->code);
				
				/* Count the number of lines */
				nolines = 0;
				j = 0;
				while( funcchar[j] != '\0' )
				{
					if(funcchar[j] == '\n') nolines++;
					j++;
				}
				/* Save to list */
				nolinesinf[nofunctions] = nolines;
				/* For every defined message in the model */
				if(0) /* Remove automatic messaging dependencies */
				{
				current_message = * modeldata->p_xmessages;
				while(current_message)
				{
					/* Copy message name to messagen */
					//copycharlist(&current_message->name, messagen);
					strcpy(messagen, current_message->name);
					/* Create strings used when sending and receiving messages */
					/* messager for reading */
					strcpy(messager, "get_first_");
					strcat(messager, messagen);
					/* messagew for writing */
					strcpy(messagew, "add_");
					strcat(messagew, messagen);
					
					/* If function code reads a message add to action list */
					if(strstr(funcchar, messager) != NULL)
					{
						strcpy(actionfname[i], funcname);
						strcpy(actionmname[i], messagen);
						action[i] = 1;
						i++;
					}
					/* If function code writes a message add to action list */
					if(strstr(funcchar, messagew) != NULL)
					{
						strcpy(actionfname[i], funcname);
						strcpy(actionmname[i], messagen);
						action[i] = 2;
						i++;
					}
					
					current_message = current_message->next;
				}
				}
				
				current_code = current_code->next;
			}
			
			/* If function depends on another function add to action list */
			current_trans = current_function->depends;
			while(current_trans)
			{
				/*strcpy(actionfname[i], funcname);
				//copycharlist(&current_trans->func, buffer);
				strcpy(buffer, current_trans->func);*/
				/* actionmname here holds the internal dependency function name */
				/*strcpy(actionmname[i], buffer);
				action[i] = 3;
				i++;*/
				
				strcpy(dependsfname[nodepends], funcname);
				strcpy(dependson[nodepends], current_trans->func);
				strcpy(dependsaction[nodepends], current_trans->dest);
				nodepends++;
				
				current_trans = current_trans->next;
			}
			
			current_function = current_function->next;
		}
		
		current_xmachine = current_xmachine->next;
	}
	
	/* Calculate dependencies */
	
	i = 0;
	/* For every action */
	while(action[i] > 0)
	{
		/*printf("%d - %s - %d - %s\n", i, actionfname[i], action[i], actionmname[i]);*/
		
		/* If function reads message */
		if(action[i] == 1)
		{
			j = 0;
			/* Find functions that write the same message */
			while(action[j] > 0)
			{
				/* Add to depends list functions that write the same message */
				if(strcmp(actionmname[i], actionmname[j]) == 0 && action[j] == 2)
				{
					strcpy(dependsfname[nodepends], actionfname[i]);
					strcpy(dependson[nodepends], actionfname[j]);
					strcpy(buffer, actionmname[i]);
					strcat(buffer, "_message");
					strcpy(dependsaction[nodepends], buffer);
					nodepends++;
				}
				
				j++;
			}
		}
		/* If an internal dependency add to depends list */
		/*else if(action[i] == 3)
		{
			strcpy(dependsfname[nodepends], actionfname[i]);
			strcpy(dependson[nodepends], actionmname[i]);
			strcpy(dependsaction[nodepends], "internal");
			nodepends++;
		}*/
		
		i++;
	}
	
	/* Write dgraph.dot */
	
	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, "dgraph.dot");
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");
	
	fputs("digraph dependency_graph {\n", file);
	fputs("\trankdir=BT;\n", file);
	fputs("\tsize=\"8,5\"\n", file);
	fputs("\tnode [shape = rect];\n", file);
	
	fputs("\t\n\t/* Functions */\n", file);
	/* For every function */
	for(m=0; m<=nofunctions; m++)
	{
		fputs("\t", file);
		sprintf(buffer, "%s", functions[m]);
		fputs(buffer, file);
		fputs("[height = ", file);
		sprintf(buffer, "%f", ((double)nolinesinf[m]/50.0));
		fputs(buffer, file);
		fputs(", label = \"", file);
		sprintf(buffer, "%s", functions[m]);
		fputs(buffer, file);
		fputs("\\n(", file);
		sprintf(buffer, "%d", nolinesinf[m]);
		fputs(buffer, file);
		fputs(" lines)\"]\n", file);
	}
	fputs("\t\n\t/* Dependencies */\n", file);
	/* For every dependency */
	for(m=0; m<nodepends; m++)
	{
		/*printf("%d - %s - %s - %s\n", m, dependsfname[m], dependson[m], dependsaction[m]);*/
		
		fputs("\t", file);
		sprintf(buffer, "%s", dependsfname[m]);
		fputs(buffer, file);
		fputs(" -> ", file);
		sprintf(buffer, "%s", dependson[m]);
		fputs(buffer, file);
		fputs(" [ label = \"<depends on ", file);
		sprintf(buffer, "%s", dependsaction[m]);
		fputs(buffer, file);
		fputs(">\" ];\n", file);
	}
	fputs("}", file);
	
	/* Close the file */
	fclose(file);
	
	/* Calculate layers of dgraph */
	/* This is achieved by finding functions with no dependencies */
	/* giving them a layer no, taking those functions away and do the operation again */
	
	/* For a set amount of times for each layer (cannot be more layers than functions?) */
	/* WARNING: there is no check for depencency loops that can cause an infinite loop */
	m = 0;
	current_layer = addlayer(modeldata->p_layers);
	while(m == 0)
	{
		/* For every function */
		for(i=nofunctions; i>=0; i--)
		{
			/* If the layer is unknown, checking for the default value of -1 */
			if(funclayer[i] == -1)
			{
				/* Set flag to no dependencies */
				k = 0; 
				/* Search dependencies on the current function */
				for(j=0; j<nodepends; j++)
				{
					/* Check if the function has a dependency on it (and not from itself) */
					if(strcmp(functions[i], dependson[j]) == 0 && strcmp(functions[i], dependsfname[j]) != 0)
					{
						for(n=0; n<=nofunctions; n++)
						{
							/* Check to see if the dependency is a function and not be assigned a layer in last layers */
							if(strcmp(dependsfname[j], functions[n]) == 0 && (funclayer[n] == -1 || funclayer[n] == layer))
							{
								/* Change flag to say there is a dependency */
								k = 1;
							}
						}
					}
				}
				/* If there is no dependency then add function to current layer */
				if(k == 0)
				{
					funclayer[i] = layer;
					
					current_function = addxfunction(&current_layer->functions);
					//freechars(p_charlist);
					//charlist = NULL;
					//makecharlist(functions[i], p_charlist);
					current_function->name = copystr(functions[i]);
					//charlist = NULL;
					//makecharlist(funcxagent[i], p_charlist);
					current_function->note = copystr(funcxagent[i]);
				}
			}
		}
		/* Increment layer */
		layer++;
		/* If all the functions have layers then stop */
		/* Set flag to all functions have a layer */
		k = 0;
		for(n=0; n<=nofunctions; n++)
		{
			/* If a function does not have a layer yet */
			if(funclayer[n] == -1) k = 1;
		}
		/* If flag is still zero then stop */
		if(k == 0) m = 1;
		else
		{
			current_layer = addlayer(modeldata->p_layers);
		}
	}
	/* Make the layer value equal to the number of layers */
	layer--;
	
	/* Write dgraph.svg */
	
	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, "dgraph.svg");
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");
	
	fputs("<?xml version=\"1.0\" standalone=\"no\"?>\n", file);
	fputs("<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n", file);
	fputs("<svg width=\"100%\" height=\"100%\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">\n", file);
	fputs("\n", file);
	
	layerheight = 20;
	for(m=layer; m>=0; m--)
	{
		/* Number of functions already on layer */
		i = 0;
		/* Variable to hold the tallest box in the current layer */
		tallestbox = 0;
		/* Variable to hold where to draw next box in a layer */
		layerwidth = 20;
		/* For each function check if they are on this layer */
		for(n=0; n<=nofunctions; n++)
		{
			if(funclayer[n] == m)
			{
				/* Make box height the no of lines in the function code */
				boxheight = nolinesinf[n];
				/* Give a minimum box height */
				if(boxheight < 40) boxheight = 40;
				/* Save the height of the tallest box */
				if(boxheight > tallestbox) tallestbox = boxheight;
				/* Calculate the x-axis position of the box depending on the no of functions in this layer */
				x = layerwidth;//20 + i*(boxwidth+10);
				/* Specify the y-axis position of the box */
				y = layerheight;
				/* Calculate boxwidth from function name */
				boxwidth = 10 + (strlen(functions[n])*7);
				if(boxwidth < 100) boxwidth = 100;
				/* Draw Rectangle */
				fputs("<rect x=\"", file);
				sprintf(buffer, "%d", x);
				fputs(buffer, file);
				fputs("\" y=\"", file);
				sprintf(buffer, "%d", y);
				fputs(buffer, file);
				fputs("\" width=\"", file);
				sprintf(buffer, "%d", boxwidth);
				fputs(buffer, file);
				fputs("\" height=\"", file);
				sprintf(buffer, "%d", boxheight);
				fputs(buffer, file);
				fputs("\" stroke=\"black\" stroke-width=\"2\" fill=\"", file);
				if(nolinesinf[n] == 0) fputs("grey", file);
				else  fputs("white", file);
				fputs("\"/>\n", file);
				/* Print function name */
				fputs("<text x=\"", file);
				sprintf(buffer, "%d", x + 5);
				fputs(buffer, file);
				fputs("\" y=\"", file);
				sprintf(buffer, "%d", y + 15);
				fputs(buffer, file);
				fputs("\" style=\"font-family: Lucida Console, monospace; font-size: 12px\">", file);
				sprintf(buffer, "%s", functions[n]);
				fputs(buffer, file);
				fputs("</text>\n", file);
				/* Print function line length */
				fputs("<text x=\"", file);
				sprintf(buffer, "%d", x + 5);
				fputs(buffer, file);
				fputs("\" y=\"", file);
				sprintf(buffer, "%d", y + 32);
				fputs(buffer, file);
				fputs("\" style=\"font-family: Lucida Console, monospace; font-size: 12px\">(", file);
				sprintf(buffer, "%d", nolinesinf[n]);
				fputs(buffer, file);
				fputs(" lines)</text>\n", file);
				
				layerwidth = layerwidth + boxwidth + 10;
				
				i++;
			}
		}
		
		layerheight = layerheight + tallestbox + 10;
		
		/* Draw sync point line */
		if(m != 0)
		{
			fputs("<line x1=\"0\" y1=\"", file);
			sprintf(buffer, "%d", layerheight - 5);
			fputs(buffer, file);
			fputs("\" x2=\"120\" y2=\"", file);
			sprintf(buffer, "%d", layerheight - 5);
			fputs(buffer, file);
			fputs("\" style=\"stroke:rgb(256,0,0);stroke-width:2\"/>\n", file);
		}
	}
	
	fputs("\n", file);
	fputs("</svg>", file);
	
	/* Close the file */
	fclose(file);
}
