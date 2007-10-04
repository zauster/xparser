#include "header.h"

/** \struct node
 * \brief Holds dependency graph node info.
 *
 * Holds dependency graph node info in a linked-list.
 */
struct node
{
	char * agent_name;				/**< Agent name that holds this function. */
	char * name;					/**< Node name as function name. */
	int rank;						/**< Node rank in directed dependency graph. */
	int linesofcode;				/**< Lines of code in the function. */
	int x;							/**< SVG diagram x-axis position. */
	int y;							/**< SVG diagram y-axis position. */
	struct adj_node * dependson;	/**< Node list of dependencies. */
	struct adj_node * dependants;	/**< Node list of dependants. */
	
	struct node * next;			/**< Pointer to next node in list. */
};

/** \struct adj_node
 * \brief Holds dependency graph node dependency info.
 *
 * Holds dependency graph node dependency info in a linked-list.
 */
struct adj_node
{
	struct node * node;		/**< Pointer to node this adj_node refers to. */
	char * type;				/**< The dependency type. */
	
	struct adj_node * next;	/**< Pointer to the next adj_node in the list. */
};

/** \typedef struct node node
 * \brief Typedef for node struct.
 */
typedef struct node node;
/** \typedef struct adj_node adj_node
 * \brief Typedef for adj_node struct.
 */
typedef struct adj_node adj_node;

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

/** \fn void add_node(char * agent_name, char * name, node ** graph)
 * \brief Add a node to the node list.
 * \param agent_name The agent name.
 * \param name The function name.
 * \param graph The node list to add to.
 */
void add_node(char * agent_name, char * name, node ** graph)
{
	node * current, * tmp = * graph;
	
	/* And current is the new element */
	current = (node *)malloc(sizeof(node));
	/* Make tmp->next point to current if tmp exists */
	current->next = tmp;
	* graph = current;
	
	/* Make current->next point to NULL */
	current->agent_name = copystr(agent_name);
	current->name = copystr(name);
	current->dependson = NULL;
	current->dependants = NULL;
	current->rank = -1;
	current->linesofcode = 0;
}

/** \fn void add_dependency(char * function_name, char * function_depends, char * dependency, node ** graph, xmachine_message ** p_xmessages)
 * \brief Add a dependency to the node list.
 * \param function_name The node name to add the dependency.
 * \param function_depends The node name of the dependency node.
 * \param dependency The dependency type.
 * \param graph The node list to add to.
 * \param p_xmessages Pointer to list of model messages.
 */
void add_dependency(char * function_name, char * function_depends, char * dependency, node ** graph, xmachine_message ** p_xmessages)
{
	node * current2 = * graph, * current  = * graph;
	adj_node * current_adj_node, * temp;
	xmachine_message * current_message;
	int found_message = 0;
	
	/* Check dependency type
	 * Must be either 'internal' or a message type */
	if(strcmp(dependency, "internal") != 0)
	{
		current_message = * p_xmessages;
		while(current_message)
		{
			if(strcmp(dependency, current_message->name) == 0) found_message = 1;
			
			current_message = current_message->next;
		}
		
		if(found_message == 0)
		{
			printf("Error: The function %s dependency %s is not 'internal' or not a valid message type\n", function_name, dependency);
			exit(0);
		}
	}
	
	while(current)
	{
		if(strcmp(current->name, function_name) == 0)
		{
			current_adj_node = (adj_node *)malloc(sizeof(adj_node));
			temp = current->dependson;
			current_adj_node->next = temp;
			current_adj_node->type = copystr(dependency);
			current->dependson = current_adj_node;
			current2 = * graph;
			current_adj_node->node = NULL;
			while(current2)
			{
				if(strcmp(current2->name, function_depends) == 0)
				{
					current_adj_node->node = current2;
				}
				
				current2 = current2->next;
			}
			if(current_adj_node->node == NULL)
			{
				printf("Error: The function %s depends on the nonexistent function %s\n", function_name, function_depends);
				exit(0);
			}
		}
		
		if(strcmp(current->name, function_depends) == 0)
		{
			current_adj_node = (adj_node *)malloc(sizeof(adj_node));
			temp = current->dependants;
			current_adj_node->next = temp;
			current_adj_node->type = copystr(dependency);
			current->dependants = current_adj_node;
			current2 = * graph;
			while(current2)
			{
				if(strcmp(current2->name, function_name) == 0)
				{
					current_adj_node->node = current2;
				}
				
				current2 = current2->next;
			}
		}
		
		current = current->next;
	}
}

/** \fn void print_graph(node ** graph)
 * \brief Print the node list.
 * \param graph The node list to print.
 */
void print_graph(node ** graph)
{
	node * current = * graph;
	adj_node * current_adj_node;
	
	while(current)
	{
		printf("node: %s - ", current->name);
		current_adj_node = current->dependson;
		while(current_adj_node)
		{
			printf("%s(%s), ", current_adj_node->node->name, current_adj_node->type);
			
			current_adj_node = current_adj_node->next;
		}
		printf("\n  dependants: ");
		current_adj_node = current->dependants;
		while(current_adj_node)
		{
			printf("%s(%s), ", current_adj_node->node->name, current_adj_node->type);
			
			current_adj_node = current_adj_node->next;
		}
		printf("\n");
		
		current = current->next;
	}
}

/** \fn void free_graph(node ** graph)
 * \brief Free the node list.
 * \param graph The node list to free.
 */
void free_graph(node ** graph)
{
	node * temp, * head;
	adj_node * adj_temp, * adj_head;
	head = * graph;
	
	while(head)
	{
		temp = head->next;
		adj_head = head->dependson;
		while(adj_head)
		{
			adj_temp = adj_head->next;
			free(adj_head);
			adj_head = adj_temp;
		}
		adj_head = head->dependants;
		while(adj_head)
		{
			adj_temp = adj_head->next;
			free(adj_head);
			adj_head = adj_temp;
		}
		
		free(head);
		head = temp;
	}
	
	* graph = NULL;
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
	f_code * current_code;
	s_trans * current_trans;
	communication_layer * current_com_layer;
	layer * current_layer;
	
	/* Buffer for concatenating strings */
	char buffer[1000];
	
	/* Positioning of rectangles in svg file */
	int x, y, layerheight, layerwidth, boxheight, boxwidth, tallestbox;
	int i, j, k, m;
	int nolines;
	int layer = 0;
	int communicationlayer;
	/* File to write to */
	FILE *file;
	
	/* pointers to graph nodes */
	node ** graph;
	node * temp_node;
	node * current_node;
	adj_node * current_adj_node;
	graph = &temp_node;
	temp_node = NULL;
	
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			add_node(current_xmachine->name, current_function->name, graph);
			
			current_function = current_function->next;
		}
		
		current_xmachine = current_xmachine->next;
	}
	
	printf("Creating dependency graph\n");
	
	/* Gather information */
	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			/* For each (should only be one) source code of function */
			current_code = current_function->code;
			while(current_code)
			{
				/* Count the number of lines */
				nolines = 0;
				j = 0;
				while( current_code->code[j] != '\0' )
				{
					if(current_code->code[j] == '\n') nolines++;
					j++;
				}
				
				current_node = * graph;
				while(current_node)
				{
					if(strcmp(current_function->name, current_node->name) == 0) current_node->linesofcode = nolines;
					
					current_node = current_node->next;
				}
				
				current_code = current_code->next;
			}
			
			/* If function depends on another function add to action list */
			current_trans = current_function->depends;
			while(current_trans)
			{
				add_dependency(current_function->name, current_trans->func, current_trans->dest, graph, modeldata->p_xmessages);
				
				current_trans = current_trans->next;
			}
			
			current_function = current_function->next;
		}
		
		current_xmachine = current_xmachine->next;
	}
	
	/*print_graph(graph);*/
	
	/* Write dgraph.dot */
	
	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, "dgraph.dot");
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");
	
	fputs("digraph dependency_graph {\n", file);
	fputs("\trankdir=BT;\n", file);
	fputs("\tsize=\"8,5;\"\n", file); /* cg 9/8/07 - bug - ; missing */
	fputs("\tnode [shape = rect];\n", file);
	
	fputs("\t\n\t/* Functions */\n", file);
	/* For every function */
	current_node = * graph;
	while(current_node)
	{
		fputs("\t", file);
		fputs(current_node->name, file);
		fputs("[height = ", file);
		sprintf(buffer, "%f", ((double)current_node->linesofcode/50.0));
		fputs(buffer, file);
		fputs(", label = \"", file);
		fputs(current_node->name, file);
		fputs("\\n(", file);
		sprintf(buffer, "%d", current_node->linesofcode);
		fputs(buffer, file);
		fputs(" lines)\"]\n", file);
		
		current_node = current_node->next;
	}
	fputs("\t\n\t/* Dependencies */\n", file);
	/* For every dependency */
	current_node = * graph;
	while(current_node)
	{
		current_adj_node = current_node->dependson;
		while(current_adj_node)
		{
			fputs("\t", file);
			fputs(current_node->name, file);
			fputs(" -> ", file);
			fputs(current_adj_node->node->name, file);
			fputs(" [ label = \"<depends on ", file);
			fputs(current_adj_node->type, file);
			fputs(">\" ];\n", file);
			
			current_adj_node = current_adj_node->next;
		}
		
		current_node = current_node->next;
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
	communicationlayer = 0;
	current_com_layer = addcommunication_layer(modeldata->p_com_layers);
	current_layer = addlayer(current_com_layer);
	while(m == 0)
	{
		/* For every function */
		current_node = * graph;
		while(current_node)
		{
			/* If the layer is unknown, checking for the default value of -1 */
			if(current_node->rank == -1)
			{
				/* Set flag to no dependencies */
				k = 0; 
				/* Search dependencies on the current function */
				current_adj_node = current_node->dependants;
				while(current_adj_node)
				{
					/* Check if the function has a dependency on it (and not from itself) */
					/* Check to see if the dependency is a function and not be assigned a layer in last layers */
					if(current_adj_node->node->rank == -1 || current_adj_node->node->rank == layer)
					{
						/* Change flag to say there is a dependency */
						k = 1;
					}
					
					current_adj_node = current_adj_node->next;
				}
				/* If there is no dependency then add function to current layer */
				if(k == 0)
				{
					current_node->rank = layer;
					current_function = addxfunction(&current_layer->functions);
					current_function->name = copystr(current_node->name);
					current_function->note = copystr(current_node->agent_name);
					//printf("%s on this layer\n", current_node->name);
					/* If dependency is a message is flag for a communication layer */
					
					/* Search dependencies on the current function */
					current_adj_node = current_node->dependson;
					while(current_adj_node)
					{
						/* If communication dependency, i.e. not internal */
						if(strcmp(current_adj_node->type,"internal") != 0) communicationlayer = 1;
						
						current_adj_node = current_adj_node->next;
					}
				}
			}
			
			current_node = current_node->next;
		}
		/* Increment layer */
		layer++;
		/* If all the functions have layers then stop */
		/* Set flag to all functions have a layer */
		k = 0;
		current_node = * graph;
		while(current_node)
		{
			/* If a function does not have a layer yet */
			if(current_node->rank == -1) k = 1;
			
			current_node = current_node->next;
		}
		/* If flag is still zero then stop */
		if(k == 0) m = 1;
		else
		{
			if(communicationlayer == 1) current_com_layer = addcommunication_layer(modeldata->p_com_layers);
			communicationlayer = 0;
			current_layer = addlayer(current_com_layer);
			//printf("--- new layer ---\n");
		}
	}
	/* Make the layer value equal to the number of layers */
	layer--;
	
	/* Test modeldata layers data */
	/*printf("** modeldata->p_com_layers\n");
	current_com_layer = * modeldata->p_com_layers;
	while(current_com_layer)
	{
		printf("*** com layer ***\n");
		
		current_layer = current_com_layer->layers;
		while(current_layer)
		{
			current_function = current_layer->functions;
			while(current_function)
			{
				printf("**** %s\n", current_function->name);
				
				current_function = current_function->next;
			}
			
			printf("-----------\n");
			
			current_layer = current_layer->next;
		}
		
		current_com_layer = current_com_layer->next;
	}*/
	
	/* Write dgraph.svg */
	
	layerheight = 20;
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
	
	//layerheight = 20;
	//for(m=0; m<=layer; m++)
	//{
		
		
		/*current_node = * graph;
		while(current_node)
		{
			if(current_node->rank == m + 1)
			{
				current_adj_node = current_node->dependson;
				while(current_adj_node)
				{
					if(strcmp(current_adj_node->type,"message") == 0) communicationlayer = 1;
					
					current_adj_node = current_adj_node->next;
				}
			}
		}*/
		
		current_com_layer = * modeldata->p_com_layers;
		while(current_com_layer)
		{
			current_layer = current_com_layer->layers;
			while(current_layer)
			{
				/* Number of functions already on layer */
				i = 0;
				/* Variable to hold the tallest box in the current layer */
				tallestbox = 0;
				/* Variable to hold where to draw next box in a layer */
				layerwidth = 20;
				/* For each function check if they are on this layer */
				
				current_function = current_layer->functions;
				while(current_function)
				{
					/* Make box height the no of lines in the function code */
					boxheight = 1;//current_node->linesofcode;
					/* Give a minimum box height */
					if(boxheight < 40) boxheight = 40;
					/* Save the height of the tallest box */
					if(boxheight > tallestbox) tallestbox = boxheight;
					/* Calculate the x-axis position of the box depending on the no of functions in this layer */
					x = layerwidth;/*20 + i*(boxwidth+10);*/
					/* Specify the y-axis position of the box */
					y = layerheight;
					/* Calculate boxwidth from function name */
					boxwidth = 17 + (strlen(current_function->name)*7);
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
					if(boxheight == 0) fputs("grey", file);
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
					fputs(current_function->name, file);
					fputs("</text>\n", file);
					/* Print function line length */
					fputs("<text x=\"", file);
					sprintf(buffer, "%d", x + 5);
					fputs(buffer, file);
					fputs("\" y=\"", file);
					sprintf(buffer, "%d", y + 32);
					fputs(buffer, file);
					fputs("\" style=\"font-family: Lucida Console, monospace; font-size: 12px\">(", file);
					sprintf(buffer, "%d", boxheight);
					fputs(buffer, file);
					fputs(" lines)</text>\n", file);
					
					layerwidth = layerwidth + boxwidth + 10;
					i++;
					
					current_function = current_function->next;
				}
				
				layerheight = layerheight + tallestbox + 10;
				
				current_layer = current_layer->next;
			}
			
			fputs("<line x1=\"0\" y1=\"", file);
			sprintf(buffer, "%d", layerheight - 5);
			fputs(buffer, file);
			fputs("\" x2=\"120\" y2=\"", file);
			sprintf(buffer, "%d", layerheight - 5);
			fputs(buffer, file);
			fputs("\" style=\"stroke:rgb(256,0,0);stroke-width:2\"/>\n", file);
			
			current_com_layer = current_com_layer->next;
		}
	
	fputs("\n", file);
	fputs("</svg>", file);
	
	/* Close the file */
	fclose(file);
	
	free_graph(graph);
}
