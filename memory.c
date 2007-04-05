#include "header.h"

/*****************************************************************
* NAME:    struct datatype functions                             *
* PURPOSE: used to allocate and free memory for datatypes        *
*****************************************************************/

/** \fn f_code * addfcode(f_code ** p_code)
 * \brief Allocate memory for a linked list of datatype code.
 * \param p_code Pointer Pointer to the code list.
 * \return Pointer to the added code.
 */
f_code * addfcode(f_code ** p_code)
{
	f_code * current, * tmp = NULL;
	current = *p_code;
	
	while(current)
	{
		tmp = current;
		current = current->next;
	}
	/* And current is the new element */
	current = (f_code *)malloc(sizeof(f_code));
	/* Make tmp->next point to current if tmp exists */
	if(tmp) tmp->next = current;
	else
	{
		*p_code = current;
	}
	/* Make current->next point to NULL */
	current->code = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freefcode(f_code ** p_code)
 * \brief Free memory from a linked list of code.
 * \param p_code Pointer Pointer to the code list.
 */
void freefcode(f_code ** p_code)
{
	f_code * temp, * head;
	head = *p_code;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->code);*/
		free(head);
		head = temp;
	}
	
	*p_code = NULL;
}

/** \fn s_trans * addtrans(s_trans ** p_transitions)
 * \brief Allocate memory for a linked list of datatype transitions.
 * \param p_transitions Pointer Pointer to the transitions list.
 * \return Pointer to the added transition.
 */
s_trans * addtrans(s_trans ** p_transitions)
{
	s_trans * current, * tmp = NULL;
	current = *p_transitions;
	
	while(current)
	{
		tmp = current;
		current = current->next;
	}
	/* And current is the new element */
	current = (s_trans *)malloc(sizeof(s_trans));
	/* Make tmp->next point to current if tmp exists */
	if(tmp) tmp->next = current;
	else
	{
		*p_transitions = current;
	}
	/* Make current->next point to NULL */
	current->func = NULL;
	current->dest = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freetrans(s_trans ** p_transitions)
 * \brief Free memory for a linked list of datatype transitions.
 * \param p_transitions Pointer Pointer to the transitions list.
 */
void freetrans(s_trans ** p_transitions)
{
	s_trans * temp, * head;
	head = *p_transitions;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->func);*/
		/*freechars(&head->dest);*/
		free(head);
		head = temp;
	}
	
	*p_transitions = NULL;
}

/** \fn s_attrib * addsattrib(s_attrib ** p_attrib)
 * \brief Allocate memory for a linked list of datatype attribute.
 * \param p_attrib Pointer Pointer to the attributes list.
 * \return Pointer to the added attribute.
 */
s_attrib * addsattrib(s_attrib ** p_attrib)
{
	s_attrib * current, * tmp = NULL;
	current = *p_attrib;
	
	while(current)
	{
		tmp = current;
		current = current->next;
	}
	/* And current is the new element */
	current = (s_attrib *)malloc(sizeof(s_attrib));
	/* Make tmp->next point to current if tmp exists */
	if(tmp) tmp->next = current;
	else
	{
		*p_attrib = current;
	}
	/* Make current->next point to NULL */
	current->attrib = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freeattribute(s_attrib ** p_attrib)
 * \brief Free memory for a linked list of datatype attribute.
 * \param p_attrib Pointer Pointer to the attributes list.
 */
void freeattribute(s_attrib ** p_attrib)
{
	s_attrib * temp, * head;
	head = *p_attrib;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->attrib);*/
		free(head);
		head = temp;
	}
	
	*p_attrib = NULL;
}

/** \fn variable * addvariable(variable ** p_vars)
 * \brief Allocate memory for a linked list of datatype variable.
 * \param p_vars Pointer Pointer to the variables list.
 * \return Pointer to the added variable.
 */
variable * addvariable(variable ** p_vars)
{
	variable * current, * tmp = NULL;
	current = *p_vars;
	
	while(current)
	{
		tmp = current;
		current = current->next;
	}
	/* And current is the new element */
	current = (variable *)malloc(sizeof(variable));
	/* Make tmp->next point to current if tmp exists */
	if(tmp) tmp->next = current;
	else
	{
		*p_vars = current;
	}
	/* Make current->next point to NULL */
	current->type = NULL;
	current->name = NULL;
	current->value = NULL;
	current->next = NULL;
	
	/* Return new element */
	return current;
}

/** \fn void freevariables(variable ** p_vars)
 * \brief Free memory for a linked list of datatype variable.
 * \param p_vars Pointer Pointer to the variables list.
 */
void freevariables(variable ** p_vars)
{
	variable * temp, * head;
	head = *p_vars;
	
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->name);*/
		/*freechars(&head->type);*/
		/*freechars(&head->value);*/
		free(head);
		head = temp;
	}
	
	*p_vars = NULL;
}

/** \fn xmachine_memory * addxmemory(xmachine_memory ** p_xmemory)
 * \brief Allocate memory for a linked list of datatype xmachine.
 * \param p_xmemory Pointer Pointer to the xmachines list.
 * \return Pointer to the added xmachine.
 */
xmachine_memory * addxmemory(xmachine_memory ** p_xmemory)
{
	xmachine_memory * current, * temp = NULL;
	current = *p_xmemory;
	
	while(current)
	{
		temp = current;
		current = current->next;
	}
	/* And current is the new element */
	if((current = (xmachine_memory *)malloc(sizeof(xmachine_memory))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make tmp->next point to current if tmp exists */
	if(temp) temp->next = current;
	else
	{
		*p_xmemory = current;
	}
	/* Make current->next point to NULL */
	current->vars = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freexmemory(xmachine_memory ** p_xmemory)
 * \brief Free memory for a linked list of datatype xmachine.
 * \param p_xmemory Pointer Pointer to the xmachines list.
 */
void freexmemory(xmachine_memory ** p_xmemory)
{
	xmachine_memory * temp, * head;
	head = *p_xmemory;
	
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		freevariables(&head->vars);
		free(head);
		head = temp;
	}
	
	*p_xmemory = NULL;
}

/** \fn env_func * addenvfunc(env_func ** p_env_funcs)
 * \brief Allocate memory for a linked list of datatype environment functions.
 * \param p_env_funcs Pointer Pointer to the environment functions list.
 * \return Pointer to the added environment function.
 */
env_func * addenvfunc(env_func ** p_env_funcs)
{
	env_func * current, * temp = NULL;
	current = *p_env_funcs;
	
	while(current)
	{
		temp = current;
		current = current->next;
	}
	/* And current is the new element */
	if((current = (env_func *)malloc(sizeof(env_func))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make tmp->next point to current if tmp exists */
	if(temp) temp->next = current;
	else
	{
		*p_env_funcs = current;
	}
	/* Make current->next point to NULL */
	current->code = NULL;
	current->filepath = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freeenvfunc(env_func ** p_env_funcs)
 * \brief Free memory for a linked list of datatype environment function.
 * \param p_env_funcs Pointer Pointer to the environment functions list.
 */
void freeenvfunc(env_func ** p_env_funcs)
{
	env_func * temp, * head;
	head = *p_env_funcs;
	
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->code);*/
		free(head);
		head = temp;
	}
	
	*p_env_funcs = NULL;
}

/** \fn xmachine_message * addxmessage(xmachine_message ** p_xmessage)
 * \brief Allocate memory for a linked list of datatype message.
 * \param p_xmessage Pointer Pointer to the messages list.
 * \return Pointer to the added message.
 */
xmachine_message * addxmessage(xmachine_message ** p_xmessage)
{
	xmachine_message * current, * temp = NULL;
	current = *p_xmessage;
	
	while(current)
	{
		temp = current;
		current = current->next;
	}
	/* And current is the new element */
	if((current = (xmachine_message *)malloc(sizeof(xmachine_message))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make tmp->next point to current if tmp exists */
	if(temp) temp->next = current;
	else
	{
		*p_xmessage = current;
	}
	/* Make current->next point to NULL */
	current->name = NULL;
	current->vars = NULL;
	current->functions = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freexmessages(xmachine_message ** p_xmessage)
 * \brief Free memory for a linked list of datatype message.
 * \param p_xmessage Pointer Pointer to the messages list.
 */
void freexmessages(xmachine_message ** p_xmessage)
{
	xmachine_message * temp, * head;
	head = *p_xmessage;
	
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->name);*/
		freevariables(&head->vars);
		freeenvfunc(&head->functions);
		free(head);
		head = temp;
	}
	
	*p_xmessage = NULL;
}

/** \fn xmachine_state * addxstate(xmachine_state ** p_xstates)
 * \brief Allocate memory for a linked list of datatype state.
 * \param p_xstates Pointer Pointer to the states list.
 * \return Pointer to the added state.
 */
xmachine_state * addxstate(xmachine_state ** p_xstates)
{
	xmachine_state * current, * temp = NULL;
	current = *p_xstates;
	
	while(current)
	{
		temp = current;
		current = current->next;
	}
	/* And current is the new element */
	if((current = (xmachine_state *)malloc(sizeof(xmachine_state))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make tmp->next point to current if tmp exists */
	if(temp) temp->next = current;
	else
	{
		*p_xstates = current;
	}
	/* Make current->next point to NULL */
	current->name = NULL;
	current->attributes = NULL;
	current->transitions = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freexstates(xmachine_state ** p_xstates)
 * \brief Free memory for a linked list of datatype state.
 * \param p_xstates Pointer Pointer to the states list.
 */
void freexstates(xmachine_state ** p_xstates)
{
	xmachine_state * temp, * head;
	head = *p_xstates;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->name);*/
		freeattribute(&head->attributes);
		freetrans(&head->transitions);
		free(head);
		head = temp;
	}
	
	*p_xstates = NULL;
}

/** \fn xmachine_function * addxfunction(xmachine_function ** p_xfunctions)
 * \brief Allocate memory for a linked list of datatype function.
 * \param p_xfunctions Pointer Pointer to the functions list.
 * \return Pointer to the added function.
 */
xmachine_function * addxfunction(xmachine_function ** p_xfunctions)
{
	xmachine_function * current, * temp = NULL;
	current = *p_xfunctions;
	
	while(current)
	{
		temp = current;
		current = current->next;
	}
	/* And current is the new element */
	if((current = (xmachine_function *)malloc(sizeof(xmachine_function))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make tmp->next point to current if tmp exists */
	if(temp) temp->next = current;
	else
	{
		*p_xfunctions = current;
	}
	/* Make current->next point to NULL */
	current->name = NULL;
	current->note = NULL;
	current->code = NULL;
	current->depends = NULL;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freexfunctions(xmachine_function ** p_xfunctions)
 * \brief Free memory for a linked list of datatype function.
 * \param p_xfunctions Pointer Pointer to the functions list.
 */
void freexfunctions(xmachine_function ** p_xfunctions)
{
	xmachine_function * temp, * head;
	head = *p_xfunctions;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->name);*/
		/*freechars(&head->note);*/
		freefcode(&head->code);
		freetrans(&head->depends);
		free(head);
		head = temp;
	}
	
	*p_xfunctions = NULL;
}

/** \fn xmachine * addxmachine(xmachine ** p_xmachines)
 * \brief Allocate memory for a linked list of datatype xmachine.
 * \param p_xmachines Pointer Pointer to the xmachines list.
 * \return Pointer to the added xmachine.
 */
xmachine * addxmachine(xmachine ** p_xmachines)
{
	int number = 0;
	xmachine * current = *p_xmachines;
	xmachine * temp = NULL;
	
	while(current)
	{
		temp = current;
		current = current->next;
		number++;
	}
	/* And current is the new element */
	if((current = (xmachine *)malloc(sizeof(xmachine))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make tmp->next point to current if tmp exists */
	if(temp) temp->next = current;
	else
	{
		*p_xmachines = current;
	}
	/* Make current->next point to NULL */
	current->number = number;
	current->name = NULL;
	current->memory = NULL;
	current->states = NULL;
	current->functions = NULL;
	current->xvar[0] = 0;
	current->yvar[0] = 0;
	current->zvar[0] = 0;
	current->next = NULL;

	/* Return new element */
	return current;
}

/** \fn void freexmachines(xmachine ** p_xmachines)
 * \brief Free memory for a linked list of datatype xmachine.
 * \param p_xmachines Pointer Pointer to the xmachines list.
 */
void freexmachines(xmachine ** p_xmachines)
{
	xmachine * temp, * head;
	head = *p_xmachines;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		/*freechars(&head->name);*/
		freexmemory(&head->memory);
		freexstates(&head->states);
		freexfunctions(&head->functions);
		free(head);
		head = temp;
	}
	
	*p_xmachines = NULL;
}

/** \fn layer * addlayer(layer ** p_layers)
 * \brief Allocate memory for a linked list of datatype layer.
 * \param p_layers Pointer Pointer to the layers list.
 * \return Pointer to the added layer.
 */
layer * addlayer(layer ** p_layers)
{
	layer * current;
	
	/* And current is the new element */
	if((current = (layer *)malloc(sizeof(layer))) == NULL)
	{
		printf("Error: Cannot allocate memory\n");
		exit(0);
	}
	/* Make current->next point to NULL */
	current->functions = NULL;
	current->next = *p_layers;
	*p_layers = current;
	
	/* Return new element */
	return current;
}

/** \fn void freelayers(layer ** p_layers)
 * \brief Free memory for a linked list of datatype layer.
 * \param p_layers Pointer Pointer to the layers list.
 */
void freelayers(layer ** p_layers)
{
	layer * temp, * head;
	head = *p_layers;
	/* Loop until new elements of cells left */
	while(head)
	{
		temp = head->next;
		/* Free the cell memory */
		freexfunctions(&head->functions);
		free(head);
		head = temp;
	}
	
	*p_layers = NULL;
}

/** \fn int_array * init_int_array()
 * \brief Allocate memory for a dynamic integer array.
 * \return int_array Pointer to the new dynamic integer array.
 */
int_array * init_int_array(void)
{
	int_array * new_array = (int_array *)malloc(sizeof(int_array));
	new_array->size = 0;
	new_array->total_size = ARRAY_BLOCK_SIZE;
	new_array->array = (int *)malloc(ARRAY_BLOCK_SIZE * sizeof(int));
	
	return new_array;
}

/** \fn void free_int_array(int_array * array)
 * \brief Free the memory of a dynamic integer array.
 * \param array Pointer to the dynamic integer array.
 */
void free_int_array(int_array * array)
{
	free(array->array);
	free(array);
}

/** \fn void sort_int_array(int_array * array)
 * \brief Sort the elements of a dynamic integer array with smallest first.
 * \param array Pointer to the dynamic integer array.
 */
void sort_int_array(int_array * array)
{
	int i, j, temp;
	
	/* Using bubble sorts nested loops */
	for(i=0; i<(array->size-1); i++)
	{
		for(j=0; j<(array->size-1)-i; j++) 
		{
			/* Comparing the values between neighbours */
			if(*(array->array+j+1) < *(array->array+j))
			{
				/* Swap neighbours */
				temp = *(array->array+j); 
				*(array->array+j) = *(array->array+j+1);
				*(array->array+j+1) = temp;
			}
		}
	}
}

/** \fn void add_int(int_array * array, int new_int)
 * \brief Add an integer to the dynamic integer array.
 * \param array Pointer to the dynamic integer array.
 * \param new_int The integer to add
 */
void add_int(int_array * array, int new_int)
{
	if(array->size == array->total_size)
	{
		array->total_size = array->total_size + ARRAY_BLOCK_SIZE;
		array->array = (int *)realloc(array->array, (array->total_size * sizeof(int)));
	}
	
	array->array[array->size] = new_int;
	array->size++;
}

/** \fn void remove_int(int_array * array, int index)
 * \brief Remove an integer from a dynamic integer array.
 * \param array Pointer to the dynamic integer array.
 * \param index The index of the integer to remove.
 */
void remove_int(int_array * array, int index)
{
	int i;
	
	if(index <= array->size)
	{
		/* memcopy??*/
		for(i = index; i < array->size; i++)
		{
			array->array[i] = array->array[i+1];
		}
		
		array->size--;
	}
}

/** \fn void print_int_array(int_array * array)
 * \brief Print the elements of a dynamic integer array.
 * \param array Pointer to the dynamic integer array.
 */
void print_int_array(int_array * array)
{
	int i;
	printf("\n");
	for(i=0; i<array->size; i++)
	{
		printf("%d> %d\n", i, array->array[i]);
	}
}

/** \fn double_array * init_double_array()
 * \brief Allocate memory for a dynamic double array.
 * \return double_array Pointer to the new dynamic double array.
 */
double_array * init_double_array()
{
	double_array * new_array = (double_array *)malloc(sizeof(double_array));
	new_array->size = 0;
	new_array->total_size = ARRAY_BLOCK_SIZE;
	new_array->array = (double *)malloc(ARRAY_BLOCK_SIZE * sizeof(double));
	
	return new_array;
}

/** \fn void free_double_array(double_array * array)
 * \brief Free the memory of a dynamic double array.
 * \param array Pointer to the dynamic double array.
 */
void free_double_array(double_array * array)
{
	free(array->array);
	free(array);
}

/** \fn void sort_double_array(double_array * array)
 * \brief Sort the elements of a dynamic double array with smallest first.
 * \param array Pointer to the dynamic double array.
 */
void sort_double_array(double_array * array)
{
	int i, j;
	double temp;
	
	/* Using bubble sorts nested loops */
	for(i=0; i<(array->size-1); i++)
	{
		for(j=0; j<(array->size-1)-i; j++) 
		{
			/* Comparing the values between neighbours */
			if(*(array->array+j+1) < *(array->array+j))
			{
				/* Swap neighbours */
				temp = *(array->array+j); 
				*(array->array+j) = *(array->array+j+1);
				*(array->array+j+1) = temp;
			}
		}
	}
}

/** \fn void add_double(double_array * array, double new_double)
 * \brief Add an double to the dynamic double array.
 * \param array Pointer to the dynamic double array.
 * \param new_double The double to add
 */
void add_double(double_array * array, double new_double)
{
	if(array->size == array->total_size)
	{
		array->total_size = array->total_size + ARRAY_BLOCK_SIZE;
		array->array = (double *)realloc(array->array, (array->total_size * sizeof(double)));
	}
	
	array->array[array->size] = new_double;
	array->size++;
}

/** \fn void remove_double(double_array * array, int index)
 * \brief Remove an double from a dynamic double array.
 * \param array Pointer to the dynamic double array.
 * \param index The index of the double to remove.
 */
void remove_double(double_array * array, int index)
{
	int i;
	
	if(index <= array->size)
	{
		/* memcopy??*/
		for(i = index; i < array->size; i++)
		{
			array->array[i] = array->array[i+1];
		}
		
		array->size--;
	}
}

/** \fn void print_double_array(double_array * array)
 * \brief Print the elements of a dynamic double array.
 * \param array Pointer to the dynamic double array.
 */
void print_double_array(double_array * array)
{
	int i;
	printf("\n");
	for(i=0; i<array->size; i++)
	{
		printf("%d> %f\n", i, array->array[i]);
	}
}

/** \fn char_array * init_char_array()
 * \brief Allocate memory for a dynamic chareger array.
 * \return char_array Pocharer to the new dynamic chareger array.
 */
char_array * init_char_array()
{
	char_array * new_array = (char_array *)malloc(sizeof(char_array));
	new_array->size = 0;
	new_array->total_size = ARRAY_BLOCK_SIZE;
	new_array->array = (char *)malloc(ARRAY_BLOCK_SIZE * sizeof(char));
	new_array->array[0] = '\0';
	
	return new_array;
}

void reset_char_array(char_array * array)
{
	array->size = 0;
	array->array[0] = '\0';
}

/** \fn void free_char_array(char_array * array)
 * \brief Free the memory of a dynamic chareger array.
 * \param array Pocharer to the dynamic chareger array.
 */
void free_char_array(char_array * array)
{
	free(array->array);
	free(array);
}

/** \fn void add_char(char_array * array, char new_char)
 * \brief Add an chareger to the dynamic chareger array.
 * \param array Pocharer to the dynamic chareger array.
 * \param new_char The chareger to add
 */
void add_char(char_array * array, char new_char)
{
	if(array->size + 1 == array->total_size)
	{
		array->total_size = array->total_size + ARRAY_BLOCK_SIZE;
		array->array = (char *)realloc(array->array, (array->total_size * sizeof(char)));
	}
	
	array->array[array->size] = new_char;
	array->array[array->size + 1] = '\0';
	array->size++;
}

/** \fn char * copy_array_to_str(char_array * array)
 * \brief Return pointer to string from a char_array.
 * \param array Pointer to the dynamic integer array.
 * \return char Pointer to the new string.
 */
char * copy_array_to_str(char_array * array)
{
	char * new_string = (char *)malloc( (array->size + 1) * sizeof(char));
	return strcpy(new_string, array->array);
}

/** \fn void remove_char(char_array * array, int index)
 * \brief Remove an chareger from a dynamic chareger array.
 * \param array Pointer to the dynamic integer array.
 * \param index The index of the chareger to remove.
 */
void remove_char(char_array * array, int index)
{
	int i;
	
	if(index <= array->size)
	{
		/* memcopy??*/
		for(i = index; i < array->size + 1; i++)
		{
			array->array[i] = array->array[i+1];
		}
		
		array->size--;
	}
}

/** \fn void print_char_array(char_array * array)
 * \brief Print the elements of a dynamic chareger array.
 * \param array Pointer to the dynamic chareger array.
 */
void print_char_array(char_array * array)
{
	printf("%s\n", array->array);
}
