#include "header.h"

/**
 * Added colour arrays
 * added output_stategraph_colour() fn
 * **/
 
 
const char *colour_map[]={"blue", "blueviolet", "brown" ,"burlywood4", "cadetblue", "chartreuse","darkgreen","deeppink",  "cyan4", "firebrick", "gold", "lightpink3", "lightslateblue", "magenta", "red", "yellow"};
const char *colour_map_light[]={"antiquewhite", "aquamarine", "azure3" ,"brown1", "chartreuse1", "orchid1","lightsalmon4","darkgoldenrod1",  "darkkhaki", "darkolivegreen3", "darkslateblue", "forestgreen", "gray70", "indianred", "lightslateblue", "maroon3"};

/** \fn void output_dgraph(char * filename, char * filepath, model_data * modeldata)
 * \brief Create a dependency graph of a model.
 * \param filename Name of file to write to.
 * \param filepath Path to write file to.
 * \param modeldata Data from the model.
 */
void output_dgraph(char * filename, char * filepath, model_data * modeldata)
{
	/* File to write to */
	FILE *file;
	/* Buffer for concatenating strings */
	char buffer[1000];
	xmachine * current_xmachine;
	xmachine_function * current_function;
	adj_function * current_adj_function;

	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, filename);
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
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		current_function = current_xmachine->functions;
		while(current_function)
		{
			fputs("\t", file);
			fputs(current_function->name, file);
			fputs("[label = \"", file);
			fputs(current_function->name, file);
			fputs("\"]\n", file);

			/* For every dependency */
			current_adj_function = current_function->depends;
			while(current_adj_function)
			{
				fputs("\t", file);
				fputs(current_function->name, file);
				fputs(" -> ", file);
				fputs(current_adj_function->function->name, file);
				fputs(" [ label = \"<depends on ", file);
				fputs(current_adj_function->type, file);
				fputs(">\" ];\n", file);

				current_adj_function = current_adj_function->next;
			}

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}
	fputs("}", file);

	/* Close the file */
	fclose(file);
}

/** \fn void printRule(rule_data * current_rule_data, FILE *file)
 * \brief Print to a file the string representation of a rule.
 * \param current_rule_data The rule to print.
 * \param file The file to write to.
 */
void printRule(rule_data * current_rule_data, FILE *file)
{
	/* If current_rule_data is NULL */
	if(current_rule_data != NULL)
	{
		if(current_rule_data->not == 1) fputs("not ( ", file);

		if(current_rule_data->time_rule == 1)
		{
			fputs("Periodicity: ", file);
			fputs(current_rule_data->lhs, file);
			fputs("\\nPhase: ", file);
			fputs(current_rule_data->rhs, file);
		}
		else
		{
			/* Handle values */
			if(current_rule_data->lhs != NULL) fputs(current_rule_data->lhs_print, file);
			else printRule(current_rule_data->lhs_rule, file);

			if(current_rule_data->op != NULL)
			{
				fputs(" ", file);
				fputs(current_rule_data->op_print, file);
				if(strcmp(current_rule_data->op, "&&") == 0 || strcmp(current_rule_data->op, "||") == 0) fputs("\\n", file);
				else fputs(" ", file);
			}

			if(current_rule_data->rhs != NULL) fputs(current_rule_data->rhs_print, file);
			else printRule(current_rule_data->rhs_rule, file);
		}

		if(current_rule_data->not == 1) fputs(" )", file);
	}
}

/** \fn void output_stategraph(char * filename, char * filepath, model_data * modeldata, int flag)
 * \brief Create a state graph of a model.
 * \param filename Name of file to write to.
 * \param filepath Path to write file to.
 * \param modeldata Data from the model.
 * \param flag Flag to specify use of subgraph clusters in dot output.
 */
void output_stategraph(char * filename, char * filepath, model_data * modeldata, int flag)
{
	/* File to write to */
	FILE *file;
	/* Buffer for concatenating strings */
	char buffer[1000];
	int i, j;

	xmachine * current_xmachine;
	xmachine_function * current_function;
	xmachine_state * current_state;
	xmachine_message * current_message;
	xmachine_ioput * current_ioput;
	sync * current_sync;
	function_pointer * current_function_pointer;
	//flame_communication * current_communication;

	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, filename);
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");

	fputs("digraph state_graph {\n", file);
	fputs("\trankdir=TB;\n", file);
	fputs("\tsize=\"8,5;\"\n", file);
	//fputs("\tnode [shape = rect];\n", file);

	/* Invisible level nodes */
	fputs("\t\n\t/* Layers */\n", file);
	for(i = 0; i <= modeldata->layer_total; i++)
	{
		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		//fputs(" [style=invis, shape=point];\n", file);
		fputs(" [shape=plaintext, label=\"layer ", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("\"];\n", file);
		//fputs(";\n", file);

		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("_b", file);
		fputs(" [style=invis, shape=point];\n", file);

		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("_a", file);
		fputs(" [style=invis, shape=point];\n", file);

		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("_b", file);
		fputs(" -> layer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs(" [style=invis];\n", file);

		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs(" -> layer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("_a", file);
		fputs(" [style=invis];\n", file);

		if(i != 0)
		{
			fputs("\tlayer_", file);
			sprintf(buffer, "%d", i-1);
			fputs(buffer, file);
			fputs("_a -> layer_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs("_b [style=invis];\n", file);
		}
	}

	fputs("\t\n\t/* States */\n", file);
	/* For every state */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		current_state = current_xmachine->states;
		while(current_state)
		{
			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_state->name, file);
			fputs(" [label = \"", file);
			fputs(current_state->name, file);
			fputs("\"]\n", file);

			current_state = current_state->next;
		}

		current_xmachine = current_xmachine->next;
	}

	/* For every function */
	fputs("\t\n\t/* Functions */\n", file);
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		current_function = current_xmachine->functions;
		while(current_function)
		{
			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(" [label = \"", file);
			fputs(current_function->name, file);
			fputs("\", shape = rect]\n", file);

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}

	/* For each message type */
	fputs("\t\n\t/* Message types */\n", file);
	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		fputs("\t", file);
		fputs(current_message->name, file);
		fputs("_message", file);
		fputs(" [ label = \"", file);
		fputs(current_message->name, file);
		//fputs("\\n", file);
		//printRule(current_communication->filter_rule, file);
		fputs("\" color=\"#00ff00\" shape = parallelogram];\n", file);

		i = 0;
		current_sync = current_message->syncs;
		while(current_sync)
		{
			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message", file);
			fputs(" -> ", file);
			fputs(current_message->name, file);
			fputs("_message_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ color=\"#00ff00\" ];\n", file);

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ label = \"start ", file);
			sprintf(buffer, "%d", current_sync->lastdepend->function->rank_in);
			fputs(buffer, file);
			fputs("\" color=\"#00ff00\" shape = parallelogram];\n", file);

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" -> ", file);
			fputs(current_message->name, file);
			fputs("_message_end_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ color=\"#00ff00\" ];\n", file);

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_end_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ label = \"end ", file);
			sprintf(buffer, "%d", current_sync->firstdependent->function->rank_in);
			fputs(buffer, file);
			fputs("\" color=\"#00ff00\" shape = parallelogram];\n", file);

			current_function_pointer = current_sync->inputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				fputs("\t", file);
				fputs(current_message->name, file);
				fputs("_message_end_", file);
				sprintf(buffer, "%d", i);
				fputs(buffer, file);
				fputs(" -> ", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\" label = \"", file);
			//	printRule(current_ioput->filter_rule, file);

				current_ioput = current_function->inputs;
				while(current_ioput)
				{
					if(strcmp(current_ioput->messagetype, current_message->name) == 0)
					{
						printRule(current_ioput->filter_rule, file);
					}
					/* Print if random */
					if(current_ioput->random == 1) fputs("\\nrandom", file);
					/* Print if sort */
					if(current_ioput->sort_function != NULL)
					{
						fputs("\\nsort: ", file);
						fputs(current_ioput->sort_function, file);
					}

					current_ioput = current_ioput->next;
				}

				fputs("\" ];\n", file);

				current_function_pointer = current_function_pointer->next;
			}

			i++;

			current_sync = current_sync->next;
		}


		current_message = current_message->next;
	}

	fputs("\t\n\t/* Transitions */\n", file);
	/* For every transition */
	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		if(flag == 0)
		{
			fputs("subgraph cluster_", file);
			fputs(current_xmachine->name, file);
			fputs(" {\n", file);
			fputs("\tlabel = \"Agent ", file);
			fputs(current_xmachine->name, file);
			fputs("\";\n", file);
		}

		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs(" -> ", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			if(current_function->condition_function == NULL)
			{
				fputs(";\n", file);
			}
			else
			{
				fputs(" [ label = \"", file);
				// TODO
				//fputs(current_function->condition, file);
				printRule(current_function->condition_rule, file);
				fputs("\"];\n", file);
			}


			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(" -> ", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(";\n", file);

			current_function = current_function->next;
		}

		if(flag == 0)
		{
			fputs("}\n", file);
		}

		current_xmachine = current_xmachine->next;
	}




	fputs("\t\n\t/* Communications */\n", file);
	/* For every communication */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		current_function = current_xmachine->functions;
		while(current_function)
		{
			current_ioput = current_function->outputs;
			while(current_ioput)
			{
				fputs("\t", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" -> ", file);
				fputs(current_ioput->messagetype, file);
				fputs("_message", file);
				fputs(" [ color=\"#00ff00\" ];\n", file);

				current_ioput = current_ioput->next;
			}

			/*current_ioput = current_function->inputs;
			while(current_ioput)
			{
				fputs("\t", file);
				fputs(current_ioput->messagetype, file);
				fputs("_message", file);
				fputs(" -> ", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\" label = \"", file);
				printRule(current_ioput->filter_rule, file);
				fputs("\" ];\n", file);

				current_ioput = current_ioput->next;
			}*/

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}


	/*current_communication = * modeldata->p_communications;
	while(current_communication)
	{
		fputs("\t", file);
		fputs(current_communication->output_function->agent_name, file);
		fputs("_", file);
		fputs(current_communication->output_function->name, file);
		fputs("_", file);
		fputs(current_communication->output_function->current_state, file);
		fputs("_", file);
		fputs(current_communication->output_function->next_state, file);
		fputs(" -> ", file);
		fputs(current_communication->messagetype, file);
		fputs("_message", file);
		fputs(" [ color=\"#00ff00\" ];\n", file);

		fputs("\t", file);
		fputs(current_communication->messagetype, file);
		fputs("_message", file);
		fputs(" -> ", file);
		fputs(current_communication->input_function->agent_name, file);
		fputs("_", file);
		fputs(current_communication->input_function->name, file);
		fputs("_", file);
		fputs(current_communication->input_function->current_state, file);
		fputs("_", file);
		fputs(current_communication->input_function->next_state, file);
		fputs(" [ color=\"#00ff00\" constraint=false ];\n", file);

		current_communication = current_communication->next;
	}*/

	for(i = 0; i <= modeldata->layer_total; i++)
	{
		fputs("\t{ rank=same; layer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("; ", file);

		current_xmachine = * modeldata->p_xmachines;
		while(current_xmachine)
		{
			// For each function
			current_function = current_xmachine->functions;
			while(current_function)
			{
				if(current_function->rank_in == i)
				{
					fputs(" ", file);
					fputs(current_xmachine->name, file);
					fputs("_", file);
					fputs(current_function->name, file);
					fputs("_", file);
					fputs(current_function->current_state, file);
					fputs("_", file);
					fputs(current_function->next_state, file);
					fputs("; ", file);
				}

				current_function = current_function->next;
			}

			current_xmachine = current_xmachine->next;
		}

		fputs("}\n", file);

		fputs("\t{ rank=same; layer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("_b; ", file);
		current_message = * modeldata->p_xmessages;
		while(current_message)
		{
			j = 0;
			current_sync = current_message->syncs;
			while(current_sync)
			{
				if(i == current_sync->firstdependent->function->rank_in)
				{
					fputs(current_message->name, file);
					fputs("_message_end_", file);
					sprintf(buffer, "%d", j);
					fputs(buffer, file);
					fputs("; ", file);
				}

				j++;

				current_sync = current_sync->next;
			}

			current_message = current_message->next;
		}
		fputs("}\n", file);

		fputs("\t{ rank=same; layer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("_a; ", file);
		current_message = * modeldata->p_xmessages;
		while(current_message)
		{
			j = 0;
			current_sync = current_message->syncs;
			while(current_sync)
			{
				if(i == current_sync->lastdepend->function->rank_in)
				{
					fputs(current_message->name, file);
					fputs("_message_start_", file);
					sprintf(buffer, "%d", j);
					fputs(buffer, file);
					fputs("; ", file);
				}

				j++;

				current_sync = current_sync->next;
			}

			current_message = current_message->next;
		}
		fputs("}\n", file);

	}
	fputs("}", file);

	/* Close the file */
	fclose(file);
}

/** \fn void output_communication_graph(char * filename, char * filepath, model_data * modeldata)
 * \brief Create a graph of communication between agent types.
 * \param filename Name of file to write to.
 * \param filepath Path to write file to.
 * \param modeldata Data from the model.
 */
void output_communication_graph(char * filename, char * filepath, model_data * modeldata)
{
	/* File to write to */
	FILE *file;
	/* Buffer for concatenating strings */
	char buffer[1000];
	int count, flag;
	xmachine * current_xmachine;
	xmachine * current_xmachine2;
	flame_communication * current_communication;
	//xmachine_function * current_function;
	//adj_function * current_adj_function;

	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, filename);
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");

	fputs("/* circo -Tps communication_graph.dot -o communication_graph.ps */\n", file);
	fputs("graph communication_graph {\n", file);
	fputs("\trankdir=BT;\n", file);
	fputs("\tsize=\"8,5;\"\n", file);
	//fputs("\tnode [shape = rect];\n", file);

	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		fputs("\t", file);
		fputs(current_xmachine->name, file);
		fputs("_agent [label = \"", file);
		fputs(current_xmachine->name, file);
		fputs("\"]\n", file);

		current_xmachine = current_xmachine->next;
	}

	/* For every agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		flag = 0;
		/* Count communication with other agent types */
		current_xmachine2 = * modeldata->p_xmachines;
		while(current_xmachine2)
		{
			if(current_xmachine == current_xmachine2) flag = 1;

			if(flag == 1)
			{
				count = 0;
				current_communication = * modeldata->p_communications;
				while(current_communication)
				{
					if( strcmp(current_communication->output_function->agent_name, current_xmachine->name) == 0 &&
						strcmp(current_communication->input_function->agent_name, current_xmachine2->name) == 0	)
					{ count++; }

					if( strcmp(current_communication->output_function->agent_name, current_xmachine2->name) == 0 &&
						strcmp(current_communication->input_function->agent_name, current_xmachine->name) == 0	)
					{ count++; }

					current_communication = current_communication->next;
				}
				if(count > 0)
				{
					fputs(current_xmachine->name, file);
					fputs("_agent -- ", file);
					fputs(current_xmachine2->name, file);
					fputs("_agent [ style=\"setlinewidth(", file);
					sprintf(buffer, "%d", count);
					fputs(buffer, file);
					fputs(")\" weight=", file);
					sprintf(buffer, "%d", count);
					fputs(buffer, file);
					fputs(" color=\"#00ff00\" ];\n", file);
				}
			}
			current_xmachine2 = current_xmachine2->next;
		}

		current_xmachine = current_xmachine->next;
	}

	current_communication = * modeldata->p_communications;
	while(current_communication)
	{
		/*fputs("\t", file);
		fputs(current_communication->output_function->agent_name, file);
		fputs("_agent -> ", file);
		fputs(current_communication->input_function->agent_name, file);
		fputs("_agent [ label = \"", file);
		fputs(current_communication->messagetype, file);
		fputs("\" color=\"#00ff00\" ];\n", file);
		*/

		current_communication = current_communication->next;
	}

	fputs("}", file);

	/* Close the file */
	fclose(file);
}

void output_process_order_graph(char * filename, char * filepath, model_data * modeldata)
{
	/* File to write to */
	FILE *file;
	/* Buffer for concatenating strings */
	char buffer[1000];
	int i;
	xmachine * current_xmachine;
	//xmachine * current_xmachine2;
	//flame_communication * current_communication;
	xmachine_function * current_function;
	xmachine_function * last_function;
	//adj_function * current_adj_function;
	xmachine_message * current_message;
	xmachine_ioput * current_ioput;
	function_pointer * current_function_pointer;
	layer * current_layer;
	sync * current_sync;

	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, filename);
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");

	fputs("digraph communication_graph {\n", file);
	fputs("\trankdir=BT;\n", file);
	fputs("\tsize=\"8,5;\"\n", file);

	i = 0;
	current_layer = * modeldata->p_layers;
	while(current_layer)
	{
		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs(" [ color=\"#ff0000\" label=\"layer ", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("\"];\n", file);

		if(i > 0)
		{
			fputs("\tlayer_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" ->", file);
			fputs(last_function->agent_name, file);
			fputs("_", file);
			fputs(last_function->name, file);
			fputs("_", file);
			fputs(last_function->current_state, file);
			fputs("_", file);
			fputs(last_function->next_state, file);
			fputs(";\n", file);
		}

		current_function_pointer = current_layer->functions;
		while(current_function_pointer)
		{
			current_function = current_function_pointer->function;

			fputs("\t", file);
			fputs(current_function->agent_name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(" [ shape = rect label=\"", file);
			fputs(current_function->agent_name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(" [", file);
			sprintf(buffer, "%d", current_function->score);
			fputs(buffer, file);
			fputs("]\" ];\n", file);

			if(current_function_pointer == current_layer->functions)
			{
				fputs("\t", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" -> layer_", file);
				sprintf(buffer, "%d", i);
				fputs(buffer, file);
				fputs(";\n", file);
			}
			else
			{
				fputs("\t", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" -> ", file);
				fputs(last_function->agent_name, file);
				fputs("_", file);
				fputs(last_function->name, file);
				fputs("_", file);
				fputs(last_function->current_state, file);
				fputs("_", file);
				fputs(last_function->next_state, file);
				fputs(";\n", file);
			}

			last_function = current_function;
			current_function_pointer = current_function_pointer->next;
		}

		i++;
		current_layer = current_layer->next;
	}

	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		fputs("\t", file);
		fputs(current_message->name, file);
		fputs("_message", file);
		fputs(" [ label = \"", file);
		fputs(current_message->name, file);
		//fputs("\\n", file);
		//printRule(current_communication->filter_rule, file);
		fputs("\" color=\"#00ff00\" shape = parallelogram];\n", file);

		/*if(current_message->last != NULL)
		{
			fputs("\t{ rank=same; layer_", file);
			sprintf(buffer, "%d", current_message->last->rank_in + 1);
			fputs(buffer, file);
			fputs("; ", file);
			fputs(current_message->name, file);
			fputs("_message; }\n", file);
		}*/

		i = 0;
		current_sync = current_message->syncs;
		while(current_sync)
		{


			//total += ( first_dependent_index - current_sync->depends->function->index );

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_sync_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ label = \"start", file);
			fputs("\" color=\"#00ff00\" shape = parallelogram];\n", file);

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_sync_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" -> ", file);
			fputs(current_message->name, file);
			fputs("_message [ color=\"#00ff00\" label=\"", file);
			fputs(current_sync->name, file);
			if(current_sync->has_agent_and_message_vars) fputs(" (F)", file);
			fputs("\"];\n", file);

			fputs("\t{ rank=same; ", file);
			fputs(current_sync->lastdepend->function->agent_name, file);
			fputs("_", file);
			fputs(current_sync->lastdepend->function->name, file);
			fputs("_", file);
			fputs(current_sync->lastdepend->function->current_state, file);
			fputs("_", file);
			fputs(current_sync->lastdepend->function->next_state, file);
			fputs("; ", file);
			fputs(current_message->name, file);
			fputs("_message_sync_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs("; }\n", file);

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_sync_end_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ label = \"end", file);
			fputs("\" color=\"#00ff00\" shape = parallelogram];\n", file);

			fputs("\t", file);
			fputs(current_message->name, file);
			fputs("_message_sync_end_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" -> ", file);
			fputs(current_message->name, file);
			fputs("_message_sync_start_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [ color=\"#00ff00\" ];\n", file);

			current_function_pointer = current_sync->inputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				fputs("\t", file);
				fputs(current_message->name, file);
				fputs("_message_sync_end_", file);
				sprintf(buffer, "%d", i);
				fputs(buffer, file);
				fputs(" -> ", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\" constraint=false ];\n", file);

				current_function_pointer = current_function_pointer->next;
			}

			current_function_pointer = current_sync->outputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				fputs("\t", file);
				fputs(current_message->name, file);
				fputs("_message_sync_start_", file);
				sprintf(buffer, "%d", i);
				fputs(buffer, file);
				fputs(" -> ", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\", constraint=false, style=dashed ];\n", file);

				current_function_pointer = current_function_pointer->next;
			}
			current_function_pointer = current_sync->filter_variable_changing_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				fputs("\t", file);
				fputs(current_message->name, file);
				fputs("_message_sync_start_", file);
				sprintf(buffer, "%d", i);
				fputs(buffer, file);
				fputs(" -> ", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\", constraint=false, style=dashed ];\n", file);

				current_function_pointer = current_function_pointer->next;
			}
			current_function_pointer = current_sync->previous_sync_inputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				fputs("\t", file);
				fputs(current_message->name, file);
				fputs("_message_sync_start_", file);
				sprintf(buffer, "%d", i);
				fputs(buffer, file);
				fputs(" -> ", file);
				fputs(current_function->agent_name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\", constraint=false, style=dashed ];\n", file);

				current_function_pointer = current_function_pointer->next;
			}

			fputs("\t{ rank=same; ", file);
			fputs(current_sync->firstdependent->function->agent_name, file);
			fputs("_", file);
			fputs(current_sync->firstdependent->function->name, file);
			fputs("_", file);
			fputs(current_sync->firstdependent->function->current_state, file);
			fputs("_", file);
			fputs(current_sync->firstdependent->function->next_state, file);
			fputs("; ", file);
			fputs(current_message->name, file);
			fputs("_message_sync_end_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs("; }\n", file);

			i++;
			current_sync = current_sync->next;
		}

		current_message = current_message->next;
	}

	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		// For each function
		current_function = current_xmachine->functions;
		while(current_function)
		{
			current_ioput = current_function->outputs;
			while(current_ioput)
			{
				fputs("\t", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" -> ", file);
				fputs(current_ioput->messagetype, file);
				fputs("_message", file);
				fputs(" [ color=\"#00ff00\" constraint=false ];\n", file);

				current_ioput = current_ioput->next;
			}

			/*current_ioput = current_function->inputs;
			while(current_ioput)
			{
				fputs("\t", file);
				fputs(current_ioput->messagetype, file);
				fputs("_message", file);
				fputs(" -> ", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_function->name, file);
				fputs("_", file);
				fputs(current_function->current_state, file);
				fputs("_", file);
				fputs(current_function->next_state, file);
				fputs(" [ color=\"#00ff00\" constraint=false ];\n", file);

				current_ioput = current_ioput->next;
			}*/

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}

	fputs("}", file);

	/* Close the file */
	fclose(file);
}


void output_stategraph_colour(char * filename, char * filepath, model_data * modeldata, int flag)
{
	/* File to write to */
	FILE *file;
	/* Buffer for concatenating strings */
	char buffer[1000];
	int i;
	int counter_colour=0;
	int counter_colour_2=0;
	/*int display_colour=0;*/
	int model_display_colour=0;
	int length_colour_int=0;
	xmachine * current_xmachine;
	xmachine_function * current_function;
	xmachine_state * current_state;
	flame_communication * current_communication;

	model_colour *p_model_colours=NULL;
	agent_colour *p_agent_colours=NULL;
	//agent_colour *agent_colour_tail=NULL;
	
	//agent_colour *temp_agent_colour;
	
	
	//model_colour *temp_model_colour;
	
	/* place in 'data' the file to write to */
	sprintf(buffer, "%s%s", filepath, filename);
	/* print out the location of the source file */
	printf("writing file: %s\n", buffer);
	/* open the file to write to */
	file = fopen(buffer, "w");

	fputs("digraph state_graph {\n", file);
	fputs("\trankdir=TB;\n", file);
	fputs("\tsize=\"8,5;\"\n", file);
	//fputs("\tnode [shape = rect];\n", file);

	/* Invisible level nodes */
	for(i = 0; i <= modeldata->layer_total; i++)
	{
		fputs("\tlayer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		//fputs(" [style=invis, shape=point];\n", file);
		fputs(" [shape=plaintext, label=\"layer ", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("\"];\n", file);
		//fputs(";\n", file);

		if(i != 0)
		{
			fputs("\tlayer_", file);
			sprintf(buffer, "%d", i-1);
			fputs(buffer, file);
			fputs(" -> layer_", file);
			sprintf(buffer, "%d", i);
			fputs(buffer, file);
			fputs(" [style=invis];\n", file);
		}
	}

	fputs("\t\n\t/* States */\n", file);
	/* For every state */
	current_xmachine = * modeldata->p_xmachines;
	
	counter_colour=-1;
	while(current_xmachine)
	{
		counter_colour++;
		if(counter_colour>=15)//more than 16 agents
		{
			counter_colour=0;
		}
		current_state = current_xmachine->states;
		while(current_state)
		{
			//assign colour to start state
		/*	if(current_state==current_xmachine->start_state)
			{//fputs("\", color=green]; \n", file);
				
				fputs("\t", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_state->name, file);
				fputs(" [label = \"", file);
				fputs(current_state->name, file);
				fputs("\", color=",file);
				fputs(colour_map[counter_colour],file);
				fputs("]; \n", file);
				//printf("M:%s - %s - colour %s \n", current_xmachine->name, current_state->name, colour_map[counter_colour]);
				//printf("M3 %s",current_xmachine->number);
				//add the colour map linked list here
				//if((temp_agent_colour=malloc(sizeof(agent_colour)))==NULL)
				//{					
				//	add_agent_colours(p_agent_colours, current_xmachine->name, counter_colour);
				//}
				addagent_colour(&p_agent_colours,current_xmachine->name,counter_colour);
				
			}
			//else print normal state colour
			else
			{ 
				fputs("\t", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_state->name, file);
				fputs(" [label = \"", file);
				fputs(current_state->name, file);
				fputs("\"]\n", file);
			}*/
				fputs("\t", file);
				fputs(current_xmachine->name, file);
				fputs("_", file);
				fputs(current_state->name, file);
				fputs(" [label = \"", file);
				fputs(current_state->name, file);
				fputs("\"]\n", file);
			current_state = current_state->next;
		}
		
		current_xmachine = current_xmachine->next;
	}
	/* For every function */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		counter_colour_2=-1;
		current_function = current_xmachine->functions;
		while(current_function)
		{
			counter_colour_2++;
			if(counter_colour_2>=15) 
				counter_colour_2=0;
			//printf("Length of linked list is %d \n",length_colour(&p_model_colours));
			length_colour_int=length_colour(&p_model_colours);
			addmodel_colour(&p_model_colours,current_function->file,length_colour_int);
			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(" [label = \"", file);
			fputs(current_function->name, file);
			//fputs("\", shape = rect]\n", file);
			//printf("^^^^^ Function %s in file %s\n",current_function->name, current_function->file );
			fputs("\", shape = box,style=filled,color=",file);
			model_display_colour=displaymodel_colour(&p_model_colours,current_function->file);
			fputs(colour_map_light[model_display_colour],file);
			fputs("];\n", file);
			//printout colours somewhere
			
			
			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}
	fputs("\t\n\t/* Transitions */\n", file);
	/* For every transition */
	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		if(flag == 0)
		{
			fputs("subgraph cluster_", file);
			fputs(current_xmachine->name, file);
			fputs(" {\n", file);
			fputs("\tlabel = \"Agent ", file);
			fputs(current_xmachine->name, file);
			fputs("\";\n", file);
		}

		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs(" -> ", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			if(current_function->condition_function == NULL)
			{
				fputs(";\n", file);
			}
			else
			{
				fputs(" [ label = \"", file);
				// TODO
				//fputs(current_function->condition, file);
				printRule(current_function->condition_rule, file);
				fputs("\"];\n", file);
			}


			fputs("\t", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->name, file);
			fputs("_", file);
			fputs(current_function->current_state, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(" -> ", file);
			fputs(current_xmachine->name, file);
			fputs("_", file);
			fputs(current_function->next_state, file);
			fputs(";\n", file);

			current_function = current_function->next;
		}

		if(flag == 0)
		{
			fputs("}\n", file);
		}

		current_xmachine = current_xmachine->next;
	}
	fputs("\t\n\t/* Communications */\n", file);
	/* For every communication */
	current_communication = * modeldata->p_communications;
	while(current_communication)
	{
		fputs("\t", file);
		fputs(current_communication->output_function->agent_name, file);
		fputs("_", file);
		fputs(current_communication->output_function->name, file);
		fputs("_", file);
		fputs(current_communication->output_function->current_state, file);
		fputs("_", file);
		fputs(current_communication->output_function->next_state, file);
		fputs(" -> ", file);
		fputs(current_communication->input_function->agent_name, file);
		fputs("_", file);
		fputs(current_communication->input_function->name, file);
		fputs("_", file);
		fputs(current_communication->input_function->current_state, file);
		fputs("_", file);
		fputs(current_communication->input_function->next_state, file);
		fputs(" [ label = \"", file);
		fputs(current_communication->messagetype, file);
		fputs("\" color=\"#00ff00\" constraint=false];\n", file);
		
		//display_colour=displayagent_colour(&p_agent_colours,current_communication->output_function->agent_name);
		//printf("colousr111111111 %s ", colour_map[display_colour]);
		current_communication = current_communication->next;
		//fputs("\" color=",file);
		//fputs(colour_map[display_colour],file);
		//fputs(" constraint=false];\n", file);
	}

	for(i = 0; i <= modeldata->layer_total; i++)
	{
		fputs("\t{ rank=same; layer_", file);
		sprintf(buffer, "%d", i);
		fputs(buffer, file);
		fputs("; ", file);

		current_xmachine = * modeldata->p_xmachines;
		while(current_xmachine)
		{
			// For each function
			current_function = current_xmachine->functions;
			while(current_function)
			{
				if(current_function->rank_in == i)
				{
					fputs(" ", file);
					fputs(current_xmachine->name, file);
					fputs("_", file);
					fputs(current_function->name, file);
					fputs("_", file);
					fputs(current_function->current_state, file);
					fputs("_", file);
					fputs(current_function->next_state, file);
					fputs("; ", file);
				}

				current_function = current_function->next;
			}

			current_xmachine = current_xmachine->next;
		}

		fputs("}\n", file);
	}
	fputs("}", file);

	freeagent_colours(&p_agent_colours);
	freemodel_colours(&p_model_colours);
	/* Close the file */
	fclose(file);
	//free colour list
	
}

int find_loop(xmachine_function * current, xmachine_function * depends)
{
	adj_function * current_adj_function;
	adj_function * current_adj_function2;
	int flag = 0, rc;

	/*printf("Function: %s - %s (%s-%s)\n", current->name, depends->name, depends->current_state, depends->next_state);*/

	/* Check if current function is dependent on itself */
	current_adj_function = current->alldepends;
	while(current_adj_function)
	{
		//printf("\t%s - %s\n", current->name, current_adj_function->function->name);

		if((xmachine_function*)depends == (xmachine_function*)current)
		{
			flag = 1;
			current_adj_function = NULL;
		}
		else current_adj_function = current_adj_function->next;
	}

	/* If there is a self dependency then generate error */
	if(flag == 1)
	{
		fprintf(stderr, "ERROR: Function %s has loop:\n", depends->name);

		current_adj_function = current->recentdepends;
		while(current_adj_function)
		{
			fprintf(stderr, "       %s\n", current_adj_function->function->name);

			if(depends == current_adj_function->function)
			{
				current_adj_function = NULL;
			}
			else current_adj_function = current_adj_function->next;
		}

		return -1;
	}
	/* If there is no self dependency (yet) then try next layer of dependencies */
	else
	{
		/* Add current dependency to a list so we only check each dependency once */
		add_adj_function_simple(current, depends);

		add_adj_function_recent(current, depends);

		/* Check if function checked already */
		current_adj_function = depends->dependson;
		while(current_adj_function)
		{
			/* If function not check yet */
			flag = 0;
			current_adj_function2 = current->alldepends;
			while(current_adj_function2)
			{
				if(current_adj_function2->function == current_adj_function->function) flag = 1;

				current_adj_function2 = current_adj_function2->next;
			}

			/* If function hasn't been checked then excute recursive algorithm */
			if(flag == 0)
			{
				rc = find_loop(current, current_adj_function->function);
				if(rc == -1) return -1;
			}

			current_adj_function = current_adj_function->next;
		}

		remove_adj_function_recent(current);
	}

	return 0;
}

void find_previous_states(xmachine_function * current_function, xmachine_message * current_message)
{
	adj_function * current_adj_function;

	//printf("\t\tstate = %s (%s)\n", current_function->current_state, current_function->agent_name);

	//addxstate(current_function->current_state, current_function->agent_name, &current_message->states);

	current_adj_function = current_function->dependson;
	while (current_adj_function)
	{
		if(strcmp(current_function->agent_name, current_adj_function->function->agent_name) == 0)
			find_previous_states(current_adj_function->function, current_message);

		current_adj_function = current_adj_function->next;
	}
}

/** \fn void calculate_filter_agent_states(model_data * modeldata)
 * \brief Calculate states that hold agents that could use a filter.
 * \param modeldata Data from the model.
 */
void calculate_filter_agent_states(model_data * modeldata)
{
	/* Calculate the preceding states before an start sync that
	 * could hold agents that use filter for message input    */
/*	for(current_message = * modeldata->p_xmessages; current_message; current_message = current_message->next)
	{
			current_layer = * modeldata->p_layers;
			while(current_layer)
			{
				current_function_pointer = current_layer->functions;
				while(current_function_pointer)
				{
					current_function = current_function_pointer->function;

					printf("current_function->name = %s\n", current_function->name);

					// Find last output of a messagetype
					current_output = current_function->inputs;
					while(current_output)
					{
						if( strcmp(current_message->name, current_output->messagetype) == 0 )
						{
							printf("\tfirst_inputs = %s\n", current_message->name);
							// Find all preceding states of same agent
							find_previous_states(current_function, current_message);

							//for(current_state = current_message->states; current_state; current_state = current_state->next)
							//{
							//	printf("\t\tstate: %s\n", current_state->name);
							//}

						}

						current_output = current_output->next;
					}

					current_function_pointer = current_function_pointer->next;
				}

				current_layer = current_layer->next;
			}
	}*/
}

/** \fn int find_agent_start_states(model_data * modeldata)
 * \brief Finds agent start states.
 * \param modeldata Data from the model.
 * \return Error code, 0 is success
 * 
 * This function searches each agent types functions for start and next states.
 */
int find_agent_start_states(model_data * modeldata)
{
	xmachine * current_xmachine;
	xmachine * current_xmachine2;
	xmachine_function * current_function;
	xmachine_state * current_state;
	int k, m;

	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	current_xmachine2 = NULL;
	while(current_xmachine)
	{
		current_state = current_xmachine->states;
		while(current_state)
		{
			k = 0;
			m = 0;
			current_function = current_xmachine->functions;
			while(current_function)
			{
				if(strcmp(current_function->next_state, current_state->name) == 0)
					k = 1;

				if(strcmp(current_function->current_state, current_state->name) == 0)
					m++;

				current_function = current_function->next;
			}

			if(k == 0)
			{
				/*printf("%s - %s\n", current_xmachine->name, current_state->name);*/
				if(current_xmachine->start_state == NULL)
				{
					current_xmachine->start_state = current_state;
				}
				else
				{
					fprintf(stderr, "ERROR: multiple start states found in '%s' agent\n", current_xmachine->name);
					fprintf(stderr, "\tincludes %s and %s states\n", current_xmachine->start_state->name, current_state->name);
					return -1;
				}
			}

			if(m == 0)
			{
				/*printf("END STATE: %s - %s\n", current_xmachine->name, current_state->name);*/
				addstateholder(current_state, &current_xmachine->end_states);
			}

			if(m > 1)
			{
				/*printf("More than one outgoing edge: %s - %s\n", current_xmachine->name, current_state->name);*/
			}

			current_state = current_state->next;
		}

		/* if no start state then error */
		if(current_xmachine->start_state == NULL)
		{
			/*fprintf(stderr, "ERROR: no start state found in '%s' agent\n", current_xmachine->name);
			return -1;*/
			fprintf(stderr, "WARNING: no start state found in '%s' agent, agent removed from model\n", current_xmachine->name);
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
			current_xmachine2 = current_xmachine;
			current_xmachine = current_xmachine->next;
		}
	}

	return 0;
}

/** \fn void catalogue_agent_communications(model_data * modeldata)
 * \brief Catalogues communication between every agent function.
 * \param modeldata Data from the model.
 * 
 * For every function pair of inputs and outputs a flame communication is added.
 * An adjacent function (outputting function) is added to the inputting function.
 */
void catalogue_agent_communications(model_data * modeldata)
{
	xmachine * current_xmachine;
	xmachine * current_xmachine2;
	xmachine_function * current_function;
	xmachine_function * current_function2;
	xmachine_ioput * current_input;
	xmachine_ioput * current_output;

	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			/* For each input */
			current_input = current_function->inputs;
			while(current_input)
			{
				/* For each agent */
				current_xmachine2 = * modeldata->p_xmachines;
				while(current_xmachine2)
				{
					/* For each function */
					current_function2 = current_xmachine2->functions;
					while(current_function2)
					{
						/* For each output */
						current_output = current_function2->outputs;
						while(current_output)
						{
							/* If the message types are equal */
							if(strcmp(current_input->messagetype, current_output->messagetype) == 0)
							{
								/* Add the communication to the communication list */
								add_flame_communication(current_input->messagetype, current_input->filter_rule, current_function, current_function2, modeldata->p_communications);
								/* Add the outputting function as an adjacent function to the inputting function */
								add_adj_function(current_function2, current_function, current_input->messagetype);
							}

							current_output = current_output->next;
						}

						current_function2 = current_function2->next;
					}

					current_xmachine2 = current_xmachine2->next;
				}

				current_input = current_input->next;
			}

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}
}

/** \fn void catalogue_agent_internal(model_data * modeldata)
 * \brief Catalogues dependencies of functions within agents.
 * \param modeldata Data from the model.
 */
void catalogue_agent_internal(model_data * modeldata)
{
	xmachine * current_xmachine;
	xmachine_function * current_function;
	xmachine_function * current_function2;

	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			/* For each function */
			current_function2 = current_xmachine->functions;
			while(current_function2)
			{
				/* If the first functions current state equals the second functions next state */
				if(strcmp(current_function->current_state, current_function2->next_state) == 0)
				{
					/* Add the first function as a dependency of the second function
					 * (with a type of being 'internal') because the second function
					 * finishes in its next state which is the current state and start
					 * of the first function */
					add_adj_function(current_function2, current_function, "internal");
				}

				current_function2 = current_function2->next;
			}

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}
}

/** \fn int check_dependency_loops(model_data * modeldata)
 * \brief Find loops within agent function dependencies.
 * \param modeldata Data from the model.
 * \return 0
 * 
 * Make sure that there are no cyclic loops of dependencies with agent functions
 */
int check_dependency_loops(model_data * modeldata)
{
	xmachine * current_xmachine;
	xmachine_function * current_function;
	adj_function * current_adj_function;
	int rc;

	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		current_function = current_xmachine->functions;
		while(current_function)
		{
			current_adj_function = current_function->dependson;
			while(current_adj_function)
			{
				rc = find_loop(current_function, current_adj_function->function);
				if(rc == -1) return -1;

				current_adj_function = current_adj_function->next;
			}

			current_function = current_function->next;
		}

		current_xmachine = current_xmachine->next;
	}

	return 0;
}

/* List for each state their incoming transition and outgoing transitions */
void catalogue_states_edges(model_data * modeldata)
{
	xmachine_state * current_state;
	xmachine * current_xmachine;
	xmachine_function * current_function;
	//function_pointer * current_function_pointer;
	
	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each state */
		current_state = current_xmachine->states;
		while(current_state)
		{
			/* For each function */
			current_function = current_xmachine->functions;
			while(current_function)
			{
				if(strcmp(current_function->current_state, current_state->name) == 0)
				{
					addfunction_pointer(&current_state->outgoing_functions, current_function);
				}
				if(strcmp(current_function->next_state, current_state->name) == 0)
				{
					addfunction_pointer(&current_state->incoming_functions, current_function);
				}
				
				current_function = current_function->next;
			}
			
			/* Print results */
			/*printf("State: %s\nincoming:\n", current_state->name);
			for(current_function_pointer = current_state->incoming_functions; current_function_pointer != NULL; current_function_pointer = current_function_pointer->next)
			{
				printf("\t%s\n", current_function_pointer->function->name);
			}
			printf("outgoing:\n");
			for(current_function_pointer = current_state->outgoing_functions; current_function_pointer != NULL; current_function_pointer = current_function_pointer->next)
			{
				printf("\t%s\n", current_function_pointer->function->name);
			}*/
			
			current_state = current_state->next;
		}
		
		current_xmachine = current_xmachine->next;
	}
}

void find_states_with_branches(xmachine_state_holder ** states, model_data * modeldata)
{
	xmachine_state_holder * current_state_holder;
	xmachine_state * current_state;
	xmachine * current_xmachine;
	xmachine_function * current_function;
	xmachine_function * current_function2;
	//function_pointer * current_function_pointer;
	int found;
	
	/* For each agent */
	current_xmachine = * modeldata->p_xmachines;
	while(current_xmachine)
	{
		/* For each function */
		current_function = current_xmachine->functions;
		while(current_function)
		{
			current_function2 = current_xmachine->functions;
			while(current_function2)
			{
				if(strcmp(current_function->name, current_function2->name) != 0 &&
						strcmp(current_function->current_state, current_function2->current_state) == 0)
				{
					current_state = current_xmachine->states;
					while(current_state)
					{
						if(strcmp(current_state->name, current_function->current_state) == 0)
						{
							found = 0;
							current_state_holder = *states;
							while(current_state_holder)
							{
								if(current_state == current_state_holder->state)
								{
									found = 1;
								}
								
								current_state_holder = current_state_holder->next;
							}
							if(found == 0)
							{
								addstateholder(current_state, states);
							}
						}
							
						current_state = current_state->next;
					}
					current_function2 = NULL;
				}
				else current_function2 = current_function2->next;
			}
			
			current_function = current_function->next;
		}
		
		current_xmachine = current_xmachine->next;
	}
	
	/*current_state_holder = *states;
	while(current_state_holder)
	{
		printf("**** %s - %s\n",
				current_state_holder->state->agent_name,
				current_state_holder->state->name);
		current_function_pointer = current_state_holder->state->branching_functions;
		while(current_function_pointer)
		{
			printf("\t%s\n", current_function_pointer->function->name);
			
			current_function_pointer = current_function_pointer->next;
		}
		
		current_state_holder = current_state_holder->next;
	}*/
}

/** \fn void calculate_dependency_graph(model_data * modeldata)
 * \brief Calculate function layers of dependencies.
 * \param modeldata Data from the model.
 * 
 * Create layers of dependencies where the top layer of functions have no
 * dependencies and each successive layer depends only on functions in the
 * layer(s) above.
 */
void calculate_dependency_graph(model_data * modeldata)
{
	xmachine_state_holder * states = NULL;
	xmachine * current_xmachine;
	xmachine_function * current_function;
	adj_function * current_adj_function;
	xmachine_state_holder * current_state_holder;
	xmachine_state_holder * current_state_holder2;
	function_pointer * current_function_pointer;
	layer * current_layer;
	int k, m, newlayer;

	/* Calculate states with out going branches */
	find_states_with_branches(&states, modeldata);
	
	/* Calculate layers of dgraph */
	/* This is achieved by finding functions with no dependencies */
	/* giving them a layer no, taking those functions away and doing the operation again */

	current_layer = addlayer(modeldata->p_layers);
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
					current_adj_function = current_function->dependson;
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

						addfunction_pointer(&current_layer->functions, current_function);
						
						/* Find function in state branch functions and flag as found */
						current_state_holder = states;
						while(current_state_holder)
						{
							current_function_pointer = current_state_holder->state->incoming_functions;
							while(current_function_pointer)
							{
								if(current_function == current_function_pointer->function)
								{
									//printf("** found %s\n", current_function_pointer->function->name);
									current_function_pointer->found = 1;
								}
								
								current_function_pointer = current_function_pointer->next;
							}
							
							current_state_holder = current_state_holder->next;
						}
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
			/* If a state with branch functions has no incoming functions
			 * then add to the top layer and not the following one */
			current_state_holder = states;
			while(current_state_holder)
			{
				if(current_state_holder->state->incoming_functions == NULL)
				{
					//printf("state found\n");
					/* remove from states and add to layer states */
					addstateholder(current_state_holder->state, &current_layer->branching_states);
					
					if(states == current_state_holder)
					{
						states = current_state_holder->next;
						free(current_state_holder);
						current_state_holder = states;
					}
					else
					{
						current_state_holder2->next = current_state_holder->next;
						free(current_state_holder);
						current_state_holder = current_state_holder2->next;
					}
				}
				else
				{
					current_state_holder2 = current_state_holder;
					current_state_holder = current_state_holder->next;
				}
			}
			
			current_layer = addlayer(modeldata->p_layers);
			
			/* If all states have branch functions have been found... */
			current_state_holder = states;
			while(current_state_holder)
			{
				k = 0;
				current_function_pointer = current_state_holder->state->incoming_functions;
				while(current_function_pointer)
				{
					if(current_function_pointer->found == 0) k = 1;
					
					current_function_pointer = current_function_pointer->next;
				}
				
				if(k == 0)
				{
					//printf("state found\n");
					/* remove from states and add to layer states */
					addstateholder(current_state_holder->state, &current_layer->branching_states);
					
					if(states == current_state_holder)
					{
						states = current_state_holder->next;
						free(current_state_holder);
						current_state_holder = states;
					}
					else
					{
						current_state_holder2->next = current_state_holder->next;
						free(current_state_holder);
						current_state_holder = current_state_holder2->next;
					}
				}
				else
				{
					current_state_holder2 = current_state_holder;
					current_state_holder = current_state_holder->next;
				}
			}
		}
	}
	/* Make the layer value equal to the number of layers */
	newlayer--;

	modeldata->layer_total = newlayer;
	
	freestateholder(&states);
}

int handle_rule_value_for_agent_variable(char * value, variable * var, xmachine * current_xmachine, int flag)
{
	variable * current_variable, * current_variable2;
	int found_flag = 0;

	//printf("\tvalue = %s\n", value);

	/* If value is an agent variable */
	if(strncmp(value, "a->", 3) == 0)
	{
		/* Only check for agent vars */
		if(flag == 0) return 1;
		/* Only check for non-constant agent vars */
		if(flag == 2)
		{
			if(var->constant == 0) return 1;
			else return 0;
		}
		
		/* 22/04/09 Simon (bug from Shawn)
		 * Check not adding duplicate var */
		for(current_variable2 = current_xmachine->variables; current_variable2 != NULL;
			current_variable2 = current_variable2->next)
		{
			if( strcmp(var->name, current_variable2->name) == 0 ) found_flag = 1;
		}
		
		if(found_flag == 0)
		{
			current_variable = addvariable(&current_xmachine->variables);
			current_variable->name = copystr(var->name);
			current_variable->type = copystr(var->type);
			current_variable->constant = var->constant;
			current_variable->agent = var->agent;
			strcpy(current_variable->c_type, var->c_type);
		}
	}
	
	return 0;
}

int handle_rule_for_agent_variable(rule_data * current_rule_data, xmachine * current_xmachine, int flag)
{
	int rc;
	
	/* Handle values */
	if(current_rule_data->lhs == NULL) rc = handle_rule_for_agent_variable(current_rule_data->lhs_rule, current_xmachine, flag);
	else rc = handle_rule_value_for_agent_variable(current_rule_data->lhs, current_rule_data->lhs_variable, current_xmachine, flag);
	if((flag == 0 || flag == 2) && rc == 1) return 1;
	
	if(current_rule_data->rhs == NULL) rc = handle_rule_for_agent_variable(current_rule_data->rhs_rule, current_xmachine, flag);
	else rc = handle_rule_value_for_agent_variable(current_rule_data->rhs, current_rule_data->rhs_variable, current_xmachine, flag);
	if((flag == 0 || flag == 2) && rc == 1) return 1;
	
	return 0;
}

int handle_rule_for_message_variable(rule_data * current_rule_data)
{
	if(current_rule_data->lhs == NULL) { if(handle_rule_for_message_variable(current_rule_data->lhs_rule) == 1) return 1; }
	else if(strncmp(current_rule_data->lhs, "m->", 3) == 0) return 1;
	
	if(current_rule_data->rhs == NULL) { if(handle_rule_for_message_variable(current_rule_data->rhs_rule) == 1) return 1; }
	else if(strncmp(current_rule_data->rhs, "m->", 3) == 0) return 1; 
	
	return 0;
}

void calculate_communication_syncs(model_data * modeldata)
{
	xmachine * current_xmachine;
	variable * current_variable;
	variable * current_variable2;
	xmachine_function * current_function;
	xmachine_function * current_function2;
	//xmachine_function * last_output_function;
	function_pointer * current_function_pointer;
	function_pointer * current_function_pointer2;
	xmachine_message * current_message;
	adj_function * current_adj_function;
	xmachine_ioput * current_input;
	xmachine_ioput * current_output;
	layer * current_layer;
	sync * current_sync = NULL;
	sync * previous_sync = NULL;
	int current_start_layer;
	function_pointer * temp_function_pointer = NULL;
	char buffer[1000];
	char buffer2[1000];
	int flag;

	/* For each message type find:
	 * 1) outputting functions
	 * 2) inputting functions */
	current_start_layer = -1;
	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		//printf("**** message type: %s\n", current_message->name);
		
		temp_function_pointer = NULL;

		current_layer = * modeldata->p_layers;
		while(current_layer)
		{
			current_function_pointer = current_layer->functions;

			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				/* Find last output of a messagetype */
				current_output = current_function->outputs;
				while(current_output)
				{
						if( strcmp(current_message->name, current_output->messagetype) == 0 )
						{
							current_start_layer = current_layer->number;
							//last_output_function = current_function;

							/* Create list of outputting functions of this message type */
							addfunction_pointer(&temp_function_pointer, current_function);
						}

					current_output = current_output->next;
				}

				current_function_pointer = current_function_pointer->next;
			}

			current_layer = current_layer->next;
		}

		/* If there are no outputting functions then no syncs are needed */
		if(temp_function_pointer != NULL)
		{
			//printf("******* sync needed ... ");
			
			current_layer = * modeldata->p_layers;
			while(current_layer)
			{
				current_function_pointer = current_layer->functions;

				while(current_function_pointer)
				{
					current_function = current_function_pointer->function;

					current_input = current_function->inputs;
					while(current_input)
					{
						if( strcmp(current_message->name, current_input->messagetype) == 0 )
						{
							/* If current function layer equals current start layer
							 * then add function to current sync */
							//if(current_start_layer == current_layer->number)
							
							/* If current_message has no sync then add sync */
							if(current_message->syncs != NULL)
							{
								addfunction_pointer(&current_sync->inputting_functions, current_function);
							}
							/* else add a new sync */
							else
							{
								//printf("added\n");
								
								/* Save current sync as previous sync */
								previous_sync = current_sync;
								/* Add new sync */
								current_sync = addsync(&current_message->syncs);
								/* If new sync is not the first sync then
								 * make previous sync point to previous sync */
								if(current_sync != current_message->syncs)
								{
									current_sync->previous_sync_inputting_functions = previous_sync->inputting_functions;
								}
								/* Copy message pointer */
								current_sync->message = current_message;
								/* Sync name = message_name _ layer_number */
								strcpy(buffer, current_message->name);
								strcat(buffer, "_");
								sprintf(buffer2, "%d", current_layer->number);
								strcat(buffer, buffer2);
								current_sync->name = copystr(buffer);
								/* Add current function to inputting functions */
								addfunction_pointer(&current_sync->inputting_functions, current_function);
								/* Next function pointers are place holders */
								addfunction_pointer(&current_sync->firstdependent, NULL); //current_function);
								addfunction_pointer(&current_sync->lastdepend, NULL); //last_output_function);
								/* Update current start layer */
								current_start_layer = current_layer->number;

								/* Copy list of outputting functions to this sync */
								current_function_pointer2 = temp_function_pointer;
								while(current_function_pointer2)
								{
									addfunction_pointer(&current_sync->outputting_functions, current_function_pointer2->function);
									current_function_pointer2 = current_function_pointer2->next;
								}
							}
							
							//printf("****     function: %s\n", current_function->name);
						}

						current_input = current_input->next;
					}

					current_function_pointer = current_function_pointer->next;
				}

				current_layer = current_layer->next;
			}

			/* Check if any inputs do not use a filter */
			flag = 0;
			current_sync = current_message->syncs;
			if(current_sync != NULL)
			{
				current_function_pointer = current_sync->inputting_functions;
				while(current_function_pointer)
				{
					current_function = current_function_pointer->function;
	
					/* If function has a filter for the message type */
					current_input = current_function->inputs;
					while(current_input)
					{
						if( current_sync->message == current_input->message )
						{
							/* If input message has a filter */
							if(current_input->filter_rule == NULL)
							{
								flag = 1;
							}
						}
						
						current_input = current_input->next;
					}
	
					current_function_pointer = current_function_pointer->next;
				}
			}
			
			/* For each sync with associated filter add filter
			 * variable changing functions and
			 * the agent including agent filter variables */
			if(flag == 0)
			{
				current_sync = current_message->syncs;
				while(current_sync)
				{
					/* For each inputting function find the previous function (if any)
					 * with the highest layer number */
					current_function_pointer = current_sync->inputting_functions;
					while(current_function_pointer)
					{
						current_function = current_function_pointer->function;
	
						/* If function has a filter for the message type */
						current_input = current_function->inputs;
						while(current_input)
						{
							if( current_sync->message == current_input->message )
							{
								/* If input message has a filter */
								if(current_input->filter_rule != NULL)
								{
									/* If agent variables exist */
									//if(handle_rule_for_agent_variable(current_input->filter_rule, current_xmachine, 0))
									/* If filter has the form a.var == m.var/# */
									if(strcmp(current_input->filter_rule->op, "==") == 0)
									{
										flag = 0;
										if(current_input->filter_rule->lhs_variable != NULL)
										{
											if(current_input->filter_rule->lhs_variable->agent != NULL &&
												current_input->filter_rule->lhs_variable->constant == 1) flag = 1;
										}
										if(current_input->filter_rule->rhs_variable != NULL)
										{
											if(current_input->filter_rule->rhs_variable->agent != NULL &&
												current_input->filter_rule->rhs_variable->constant == 1) flag = 1;
										}
										if(flag == 1)		
										{
											/* Add agent and agent variables to the sync */
											current_xmachine = addxmachine(&current_sync->agents, current_function->agent_name);
											current_sync->filter_agent_count++;
											/* Add agent filter variables to sync agent */
											handle_rule_for_agent_variable(current_input->filter_rule, current_xmachine, 1);
											/* If filter has message vars update sync flag */
											if(handle_rule_for_message_variable(current_input->filter_rule)) current_sync->has_agent_and_message_vars = 1;
			
											/* Add filter functions to sync agent */
											current_function2 = addxfunction(&current_xmachine->functions);
											current_function2->name = copystr(current_input->filter_function);
											current_function2->agent_name = copystr(current_function->agent_name);
											(void) add_rule_data(&current_function2->filter_rule);
											copy_rule_data(current_function2->filter_rule, current_input->filter_rule);
											current_function2->has_message_var = current_input->filter_rule->has_message_var;
											current_function2->has_agent_var = current_input->filter_rule->has_agent_var;
									
											/* Add possible states that hold the agents for the filter inputting function */
											addxstate(current_function->current_state, current_function->agent_name, &current_xmachine->states);
			
											/* Find functions that can change the filter variables */
											/* If filter variables are constant no functions can change them */
											if(handle_rule_for_agent_variable(current_input->filter_rule, current_xmachine, 2))
											{
												current_adj_function = current_function_pointer->function->dependson;
												while(current_adj_function)
												{
													if(strcmp(current_function_pointer->function->agent_name, current_adj_function->function->agent_name) == 0)
													{
														/* Add functions that can change the filter variables */
														addfunction_pointer(&current_sync->filter_variable_changing_functions, current_adj_function->function);
				
														/*if(current_sync->lastdepend->function->rank_in < current_adj_function->function->rank_in)
														{
															//current_sync->start_layer = current_adj_function->function->rank_in;
															current_sync->lastdepend->function = current_adj_function->function;
														}*/
													}
				
													current_adj_function = current_adj_function->next;
												}
											}
											else
											{
												current_input->non_constant_vars = 0;
												/*printf("*** constant vars %s\n", current_input->filter_function);*/
											}
										}
									}
								}
							}
	
							current_input = current_input->next;
						}
	
						current_function_pointer = current_function_pointer->next;
					}
	
					/* For each agent filter var add to sync vars */
					current_xmachine = current_sync->agents;
					while(current_xmachine)
					{
						current_variable = current_xmachine->variables;
						while(current_variable)
						{
							flag = 0;
							current_variable2 = current_sync->vars;
							while(current_variable2)
							{
								if(strcmp(current_variable->type, current_variable2->type) == 0) flag = 1;
	
								current_variable2 = current_variable2->next;
							}
							if(flag == 0)
							{
								current_variable2 = addvariable(&current_sync->vars);
								current_variable2->name = copystr(current_variable->name);
								current_variable2->type = copystr(current_variable->type);
								current_variable2->constant = current_variable->constant;
								current_variable2->agent = current_variable->agent;
								
								/*printf("agent sync var to sync vars: %s-%s -> %s-%s\n",
																	current_variable->agent->name, current_variable->name,
																	current_variable2->agent->name, current_variable2->name);*/
							}
	
							current_variable = current_variable->next;
						}
	
						current_xmachine = current_xmachine->next;
					}
					
					current_sync = current_sync->next;
				}
			}
		}

		/* free temp_function_pointer */
		freefunction_pointers(&temp_function_pointer);
		temp_function_pointer = NULL;

		current_message = current_message->next;
	}
}

int calculate_sync_lengths(model_data * modeldata)
{
	/*
	 * This is not calculated totally correctly.
	 * Sync lengths should be between start and end syncs.
	 * But some start syncs depend upon functions before inputting
	 * functions that are not linked to the sync.
	 */

	xmachine_message * current_message;
	sync * current_sync;
	int total = 0;

	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		current_sync = current_message->syncs;
		while(current_sync)
		{
			total += ( current_sync->firstdependent->function->index - current_sync->lastdepend->function->index );

			current_sync = current_sync->next;
		}

		current_message = current_message->next;
	}

	return total;
}

void order_functions_in_process_layers(model_data * modeldata)
{
	/* Place outputting functions first and start their sync afterwards if any.
	 * Outputting functions with shortest syncs first.
	 * Place inputting functions last with complete syncs just before if any.
	 * Inputting functions with shortest syncs last. */

	/* Add check for messages that don't have a first or last */

	layer * current_layer;
	xmachine_function * current_function;
	xmachine_function * current_function2;
	function_pointer * current_function_pointer;
	function_pointer * current_function_pointer2;
	function_pointer * temp_function_pointer;
	function_pointer * temp_function_pointer2;
	function_pointer * next_function_pointer;
	function_pointer * previous_function_pointer;
	//xmachine_ioput * current_input;
	//xmachine_ioput * current_output;
	xmachine_message * current_message;
	sync * current_sync;
	int input_score;
	int output_score;

	current_layer = * modeldata->p_layers;
	while(current_layer)
	{
		temp_function_pointer = current_layer->functions;
		current_layer->functions = NULL;
		current_function_pointer = temp_function_pointer;
		while(current_function_pointer)
		{
			input_score = 0;
			output_score = 0;

			current_function = current_function_pointer->function;

			/* Search syncs for depends and dependent functions */
			current_message = * modeldata->p_xmessages;
			while(current_message)
			{
				current_sync = current_message->syncs;
				while(current_sync)
				{
					/*if(current_sync->outputting_functions->function == current_function)
					{
						output_score += ( current_sync->firstdependent->function->index - current_function->index);
					}*/

					current_function_pointer2 = current_sync->outputting_functions;
					while(current_function_pointer2)
					{
						current_function2 = current_function_pointer2->function;

						if(current_function2 == current_function)
						{
							output_score += ( current_sync->firstdependent->function->index - current_function->index );
						}

						current_function_pointer2 = current_function_pointer2->next;
					}

					current_function_pointer2 = current_sync->inputting_functions;
					while(current_function_pointer2)
					{
						current_function2 = current_function_pointer2->function;

						if(current_function2 == current_function)
						{
							input_score += ( current_function->index - current_sync->lastdepend->function->index);
						}

						current_function_pointer2 = current_function_pointer2->next;
					}

					//total += ( current_sync->depends->function->index - current_sync->outputting_functions->function->index );

					current_sync = current_sync->next;
				}

				current_message = current_message->next;
			}

			/*current_input = current_function->inputs;
			while(current_input)
			{
				//printf("\tinput = %s\n", current_input->messagetype);

				current_message = * modeldata->p_xmessages;
				while(current_message)
				{
					if( strcmp(current_input->messagetype, current_message->name) == 0 )
					{
						//printf("\t\tlast out on execution layer %d\n", current_message->last);
						input_score += ( current_function->index - current_message->depends->function->index);
					}

					current_message = current_message->next;
				}

				current_input = current_input->next;
			}

			current_output = current_function->outputs;
			while(current_output)
			{
				//printf("\toutput = %s\n", current_output->messagetype);

				current_message = * modeldata->p_xmessages;
				while(current_message)
				{
					if( strcmp(current_output->messagetype, current_message->name) == 0 )
					{
						//printf("\t\tfirst input on execution layer %d\n", current_message->first);
						output_score += ( current_message->dependents->function->index - current_function->index);
					}

					current_message = current_message->next;
				}

				current_output = current_output->next;
			}*/

			//printf("input_score = %d;\noutput_score = %d\n", input_score, output_score);
			if(input_score > output_score) current_function->score = input_score;
			else if (input_score < output_score) current_function->score = -output_score;
			else current_function->score = 0;

			//printf("current_function->name = %s - score = %d\n", current_function->name, current_function->score);

			next_function_pointer = current_function_pointer->next;

			if(current_layer->functions == NULL)
			{
				current_layer->functions = current_function_pointer;
				current_function_pointer->next = NULL;
			}
			else
			{
				current_function_pointer2 = current_layer->functions;
				previous_function_pointer = NULL;
				while(current_function_pointer2)
				{
					temp_function_pointer2 = current_function_pointer2;

					/* Order functions with least distance outputting functions first
					 * going to
					 * least distance inputting functions last */
					if(		(current_function->score > 0 &&
							current_function_pointer2->function->score > 0 &&
							current_function->score > current_function_pointer2->function->score)
							||
							(current_function->score < 0 &&
							current_function_pointer2->function->score < 0 &&
							current_function->score > current_function_pointer2->function->score)
							||
							(((current_function->score <= 0 &&
								current_function_pointer2->function->score >= 0)
								||
								(current_function->score >= 0 &&
								current_function_pointer2->function->score <= 0)) &&
							current_function->score < current_function_pointer2->function->score)
						)
					{
						current_function_pointer2 = NULL;
					}
					else
					{
						previous_function_pointer = current_function_pointer2;
						current_function_pointer2 = current_function_pointer2->next;
					}
				}

				if(previous_function_pointer == NULL)
				{
					current_function_pointer->next = current_layer->functions;
					current_layer->functions = current_function_pointer;
				}
				else
				{
					current_function_pointer->next = previous_function_pointer->next;
					previous_function_pointer->next = current_function_pointer;
				}

			}

			current_function_pointer = next_function_pointer;
		}

		current_layer = current_layer->next;
	}
}

void assign_function_order_index_and_sync_dependency_functions(model_data * modeldata)
{
	layer * current_layer;
	xmachine_function * current_function;
	xmachine_function * first_input_function;
	xmachine_function * last_dependent_function;
	function_pointer * current_function_pointer;
	xmachine_message * current_message;
	sync * current_sync;
	int i = 0;

	current_layer = * modeldata->p_layers;
	while(current_layer)
	{
		current_function_pointer = current_layer->functions;
		while(current_function_pointer)
		{
			current_function = current_function_pointer->function;

			current_function->index = i;
			i++;

			current_function_pointer = current_function_pointer->next;
		}

		current_layer = current_layer->next;
	}


	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		current_sync = current_message->syncs;
		while(current_sync)
		{
			/* Calculate first input function in index for syncs */
			first_input_function = NULL;
			current_function_pointer = current_sync->inputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				if(first_input_function == NULL)
				{
					first_input_function = current_function;
				}
				else if(current_function->index < first_input_function->index)
				{
					first_input_function = current_function;
				}

				current_function_pointer = current_function_pointer->next;
			}
			current_sync->firstdependent->function = first_input_function;

			/* Calculate last sync start dependent function */
			last_dependent_function = NULL;
			current_function_pointer = current_sync->outputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				if(last_dependent_function == NULL)
				{
					last_dependent_function = current_function;
				}
				else if(current_function->index > last_dependent_function->index)
				{
					last_dependent_function = current_function;
				}

				current_function_pointer = current_function_pointer->next;
			}
			current_function_pointer = current_sync->filter_variable_changing_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				if(last_dependent_function == NULL)
				{
					last_dependent_function = current_function;
				}
				else if(current_function->index > last_dependent_function->index)
				{
					last_dependent_function = current_function;
				}

				current_function_pointer = current_function_pointer->next;
			}
			/* If the sync has a previous sync */
			current_function_pointer = current_sync->previous_sync_inputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				if(last_dependent_function == NULL)
				{
					last_dependent_function = current_function;
				}
				else if(current_function->index > last_dependent_function->index)
				{
					last_dependent_function = current_function;
				}

				current_function_pointer = current_function_pointer->next;
			}

			current_sync->lastdepend->function = last_dependent_function;

			current_sync = current_sync->next;
		}

		current_message = current_message->next;
	}
}

void calculate_message_start_end_syncs(model_data * modeldata)
{
	/*	xmachine_message * current_message;
	xmachine_function * current_function;
	xmachine_function * current_function2;
	xmachine_ioput * current_input;
	xmachine_ioput * current_output;
	xmachine_ioput * current_ioput;
	layer * current_layer;
	function_pointer * current_function_pointer;

	// Calculate points to start sync and end sync communication
	// and points where states have more than one leading edge
	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		// Points to the last function that outs a message type
		current_function2 = NULL;

		current_layer = * modeldata->p_layers;
		while(current_layer)
		{
			current_function_pointer = current_layer->functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				// Find first input of a messagetype
				current_input = current_function->inputs;
				while(current_input)
				{
					if(current_message->first == NULL)
					{
						if( strcmp(current_message->name, current_input->messagetype) == 0 )
						{
							current_ioput = addioput(&current_function->first_inputs);
							current_ioput->messagetype = copystr(current_message->name);
						}
					}

					current_input = current_input->next;
				}

				// Find last output of a messagetype
				current_output = current_function->outputs;
				while(current_output)
				{
						if( strcmp(current_message->name, current_output->messagetype) == 0 )
						{
							current_function2 = current_function;
						}

					current_output = current_output->next;
				}

				current_function_pointer = current_function_pointer->next;
			}

			current_layer = current_layer->next;
		}

		if(current_function2 != NULL)
		{
			current_ioput = addioput(&current_function2->last_outputs);
			current_ioput->messagetype = copystr(current_message->name);
			current_ioput->message = current_message;
		}

		current_message = current_message->next;
	}*/
}

void apply_sync_data_to_functions(model_data * modeldata)
{
	xmachine_message * current_message;
	sync * current_sync;
	function_pointer * current_function_pointer;
	xmachine_function * current_function;
	//xmachine_ioput * current_ioput;

	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		current_sync = current_message->syncs;
		while(current_sync)
		{
			/* For the depends */
			current_function_pointer = current_sync->lastdepend;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				//printf("**** %s last_outputs %s\n", current_function->name, current_message->name);

				/*current_ioput = addioput(&current_function->last_outputs);
				current_ioput->messagetype = copystr(current_message->name);
				current_ioput->message = current_message;*/
				/* Need to add filters */
				add_sync_pointer(&current_function->start_syncs, current_sync);
				/*printf("Function: %s start_sync: %s\n", current_function->name, current_sync->name);*/

				current_function_pointer = current_function_pointer->next;
			}

			/* For the first input */
			current_function_pointer = current_sync->firstdependent;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;

				//printf("**** %s first_inputs %s\n", current_function->name, current_message->name);

				/*current_ioput = addioput(&current_function->first_inputs);
				current_ioput->messagetype = copystr(current_message->name);
				current_ioput->message = current_message;*/
				add_sync_pointer(&current_function->complete_syncs, current_sync);
				/*printf("Function: %s complete_sync: %s\n", current_function->name, current_sync->name);*/

				current_function_pointer = current_function_pointer->next;
			}

			current_sync = current_sync->next;
		}

		current_message = current_message->next;
	}
}

void calculate_partition_data(model_data * modeldata)
{
	xmachine_message * current_message;
	xmachine * current_xmachine;
	xmachine_function * current_function;
	xmachine_ioput * current_ioput;

	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		/* For each agent */
		current_xmachine = * modeldata->p_xmachines;
		while(current_xmachine)
		{
			/* For each function */
			current_function = current_xmachine->functions;
			while(current_function)
			{
				current_ioput = current_function->outputs;
				while(current_ioput)
				{
					if( strcmp(current_message->name, current_ioput->messagetype) == 0 )
					{
						printf("Agent: %s outputs message: %s\n", current_xmachine->name, current_message->name);
					}

					current_ioput = current_ioput->next;
				}
				
				current_ioput = current_function->inputs;
				while(current_ioput)
				{
					if( strcmp(current_message->name, current_ioput->messagetype) == 0 )
					{
						printf("Agent: %s inputs message: %s\n", current_xmachine->name, current_message->name);
					}

					current_ioput = current_ioput->next;
				}
				
				current_function = current_function->next;
			}

			current_xmachine = current_xmachine->next;
		}
		
		current_message = current_message->next;
	}
}

void print_sync_data(model_data * modeldata)
{
	xmachine_message * current_message;
	sync * current_sync;
	xmachine * current_xmachine;
	xmachine_function * current_function;
	/*xmachine_ioput * current_ioput;
	function_pointer * current_function_pointer;
	xmachine_function * current_function;*/

	printf("\n");
	
	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		printf("%s\n", current_message->name);
		
		current_sync = current_message->syncs;
		while(current_sync)
		{
			printf("\t%s\n", current_sync->name);
			if(current_sync->filter_rule != NULL)
			printf("\trule: %s %s %s\n",
					current_sync->filter_rule->lhs_print,
					current_sync->filter_rule->op_print,
					current_sync->filter_rule->rhs_print);
			current_xmachine = current_sync->agents;
			while(current_xmachine)
			{
				printf("\t\t%s\n", current_xmachine->name);
				current_function = current_xmachine->functions;
				while(current_function)
				{
					printf("\t\t\tf: %s\n", current_function->name);
					
					printf("\t\t\trule: %s %s %s\n",
						current_function->filter_rule->lhs,
						current_function->filter_rule->op,
						current_function->filter_rule->rhs);
					
					current_function = current_function->next;
				}
				
				current_xmachine = current_xmachine->next;
			}
			
			/*if(current_sync->has_agent_and_message_vars)
			{
				printf("\t%s\n", current_sync->name);
				
			}*/
			//printf("\thas_agent_and_message_vars == %d\n", current_sync->has_agent_and_message_vars);
			
			/*current_function_pointer = current_sync->inputting_functions;
			while(current_function_pointer)
			{
				current_function = current_function_pointer->function;
				printf("\t\t%s", current_function->name);
				current_ioput = current_function->inputs;
				while(current_ioput)
				{
					if( strcmp(current_ioput->messagetype, current_message->name) == 0 &&
						current_ioput->filter_rule != NULL)
					{
						printf("\tfilter");
					}
					
					current_ioput = current_ioput->next;
				}
				printf("\n");
				
				current_function_pointer = current_function_pointer->next;
			}*/

			current_sync = current_sync->next;
		}
		
		current_message = current_message->next;
	}
	
	printf("\n");
}

/** \fn void find_constant_filter_vars(model_data * modeldata)
 * \brief Find constant variables of agents used in filters and add non-duplicates to global list.
 * \param modeldata Data from the model.
 */
void find_constant_filter_vars(model_data * modeldata)
{
	xmachine_message * current_message;
	sync * current_sync;
	variable * current_variable;
	variable * current_variable2;
	xmachine * current_xmachine;
	int flag;
		
	current_message = * modeldata->p_xmessages;
	while(current_message)
	{
		current_sync = current_message->syncs;
		while(current_sync)
		{
			current_xmachine = current_sync->agents;
			while(current_xmachine)
			{
				current_variable = current_xmachine->variables;
				while(current_variable)
				{
					if(current_variable->constant == 1)
					{
						/* check for duplicates */
						flag = 0;
						current_variable2 = *modeldata->p_constant_filter_vars;
						while(current_variable2)
						{
							if(current_variable->agent == current_variable2->agent &&
								strcmp(current_variable->name, current_variable2->name) == 0) flag = 1;
							
							current_variable2 = current_variable2->next;
						}
						if(flag == 0)
						{
							current_variable2 = addvariable(modeldata->p_constant_filter_vars);
							current_variable2->name = copystr(current_variable->name);
							current_variable2->type = copystr(current_variable->type);
							current_variable2->agent = current_variable->agent;
						}
					}
					
					current_variable = current_variable->next;
				}
				
				current_xmachine = current_xmachine->next;
			}
			
			current_sync = current_sync->next;
		}
					
		current_message = current_message->next;
	}
}

/** \fn void create_dependency_graph(char * filepath, model_data * modeldata)
 * \brief Calculate agent functions dependency graph and produce a dot graph description output.
 * \param filepath Pointer to the file path and name.
 * \param modeldata Data from the model.
 */
int create_dependency_graph(char * filepath, model_data * modeldata)
{
	int rc;

	/* Find start state of agents, find error if more than one? */
	rc = find_agent_start_states(modeldata);
	if(rc != 0) return -1;

	printf("Creating dependency graph\n");
	/* Gather information */
	catalogue_agent_communications(modeldata);
	/* Look for internal dependencies */
	catalogue_agent_internal(modeldata);

	/* check loops in dependencies */
	rc = check_dependency_loops(modeldata);
	if(rc != 0) return -1;
	printf("Finished dependency loop check\n");

	catalogue_states_edges(modeldata);
	calculate_dependency_graph(modeldata);

	calculate_communication_syncs(modeldata);

	assign_function_order_index_and_sync_dependency_functions(modeldata);
	printf("Total communication sync lengths: %d\n", calculate_sync_lengths(modeldata));
	printf("Order functions in process layers\n");
	//output_process_order_graph("process_order_graph_pre.dot", filepath, modeldata);
	order_functions_in_process_layers(modeldata);
	assign_function_order_index_and_sync_dependency_functions(modeldata);
	printf("Total communication sync lengths: %d\n", calculate_sync_lengths(modeldata));

	/*if(modeldata->depends_style == 0)*/
	output_stategraph("stategraph.dot", filepath, modeldata, 1);
	output_stategraph_colour("stategraph_colour.dot", filepath, modeldata, 1);
	/*if(modeldata->depends_style == 1) output_dgraph("dgraph.dot", filepath, modeldata);*/
	//output_communication_graph("communication_graph.dot", filepath, modeldata);
	output_process_order_graph("process_order_graph.dot", filepath, modeldata);

	/*calculate_message_start_end_syncs(modeldata);
	 * replaced with:
	 */
	apply_sync_data_to_functions(modeldata);

	calculate_filter_agent_states(modeldata);
	
	/* Calculate data for possible partitioning scheme */
	/*calculate_partition_data(modeldata);*/
	/*print_sync_data(modeldata);*/

	find_constant_filter_vars(modeldata);
	
	return 0;
}

/** \defgroup dependencygraph Dependency Graph
 *
 * The dependency graph
 * 
 * \dot
 * digraph finite_state_machine
 * {
 *	rankdir=LR;
 *	size="8,5"
 *	node [shape = doublecircle]; LR_0 LR_3 LR_4 LR_8;
 *	node [shape = circle];
 *	LR_0 -> LR_2 [ label = "SS(B)" ];
 *	LR_0 -> LR_1 [ label = "SS(S)" ];
 *	LR_1 -> LR_3 [ label = "S($end)" ];
 *	LR_2 -> LR_6 [ label = "SS(b)" ];
 *	LR_2 -> LR_5 [ label = "SS(a)" ];
 *	LR_2 -> LR_4 [ label = "S(A)" ];
 *	LR_5 -> LR_7 [ label = "S(b)" ];
 *	LR_5 -> LR_5 [ label = "S(a)" ];
 *	LR_6 -> LR_6 [ label = "S(b)" ];
 *	LR_6 -> LR_5 [ label = "S(a)" ];
 *	LR_7 -> LR_8 [ label = "S(b)" ];
 *	LR_7 -> LR_5 [ label = "S(a)" ];
 *	LR_8 -> LR_6 [ label = "S(b)" ];
 *	LR_8 -> LR_5 [ label = "S(a)" ];
 * }
 * \enddot
 * 
 */
