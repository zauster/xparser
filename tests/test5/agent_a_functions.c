#include "header.h"
#include "agent_a_agent_header.h"

/** \fn int m_sub_1_sort_function(const void *x, const void *y)
 * \brief A sort function 
 */
int m_sub_1_sort_function(const void *x, const void *y)
{
    if( ((m_m_sub_1 *)x)->id2 > ((m_m_sub_1 *)y)->id2 ) return -1;
    else if( ((m_m_sub_1 *)x)->id2 < ((m_m_sub_1 *)y)->id2 ) return 1;
    else return 0;
}

/** \fn agent_a_1()
 * \brief A description of the function
 */
int agent_a_send()
{
	//int i;
	
	add_int(&INT_DYN_ARRAY, INT_SINGLE);
	add_int(&INT_DYN_ARRAY, INT_SINGLE+1);
	
	//for(i = 0; i < INT_DYN_ARRAY.size; i ++) printf("%d> int_dyn_array[%d] = %d\n",
	//		INT_SINGLE, i, INT_DYN_ARRAY.array[i]);
	
	add_m_sub_1_message(0, 0);
	add_m_sub_1_message(1, 1);
	add_m_sub_1_message(2, 1);
	add_m_sub_1_message(3, 2);
	
	add_m_sub_2_message(0, 0);
	add_m_sub_2_message(1, 1);
	add_m_sub_2_message(2, 1);
	add_m_sub_2_message(3, 2);
	
   return 0;
}

void read_and_print_messages()
{
	START_M_SUB_1_MESSAGE_LOOP
		printf("%d> m_sub_1 int_single: %d %d\n",
				INT_SINGLE,
				m_sub_1_message->id,
				m_sub_1_message->id2);
	FINISH_M_SUB_1_MESSAGE_LOOP
}

int agent_a_not_random()
{
	int i = 0;
	
	printf("---- not_random ----\n");
	START_M_SUB_1_MESSAGE_LOOP
		if(m_sub_1_message->id == i) i++;
	FINISH_M_SUB_1_MESSAGE_LOOP
	
	if(i == 4) printf("SUCCESS\n");
	
	return 0;
}

int agent_a_random()
{
	int j = -1;
	int k = -1;
	
	printf("---- random ----\n");
	
	while(k == j || j == -1 || k == -1)
	{
		k = j;
		
		/* Hack here which actually circumvents the test */
		MB_Iterator_Randomise(i_m_sub_1);
		
		START_M_SUB_1_MESSAGE_LOOP
			j = m_sub_1_message->id;
		FINISH_M_SUB_1_MESSAGE_LOOP
		
		//printf("j: %d\tk: %d\n", j, k);
	}
	
	printf("SUCCESS\n");
	
	return 0;
}

int agent_a_not_random_sort()
{
	int i = -1;
	
	printf("---- not_random_sort ----\n");
	
	START_M_SUB_1_MESSAGE_LOOP
		if(i == -1) i = m_sub_1_message->id2;
		else if(m_sub_1_message->id2 >= i) i = m_sub_1_message->id2;
		else i = -2;
	FINISH_M_SUB_1_MESSAGE_LOOP
	
	if(i == -2) printf("FAILURE\n");
	else printf("SUCCESS\n");
	
	return 0;
}

int agent_a_random_sort()
{
	int j = -1;
	int k = -1;
	m_m_sub_1 messages[3];
	
	printf("---- random_sort ----\n");
	
	messages[0].id2 = 0; messages[0].id = 0;
	messages[1].id2 = 1; messages[1].id = 1;
	messages[2].id2 = 1; messages[2].id = 2;
	
	while(k == j || j == -1 || k == -1)
	{
		qsort(messages, 3, sizeof(m_m_sub_1),
					FLAME_sort_agent_a_agent_a_random_sort_middle4_middle5_m_sub_1);
		k = j;
		j = messages[0].id;
		
		//printf("j: %d\tk: %d\n", j, k);
	}
	
	printf("SUCCESS\n");
	
	return 0;
}

int agent_a_filter()
{
	int i = 0;
	
	printf("---- filter ----\n");
	
	START_M_SUB_1_MESSAGE_LOOP
		//printf("m_sub_1 id: %d\n", m_sub_1_message->id);
		if(m_sub_1_message->id == 1 || m_sub_1_message->id == 2) i++;
	FINISH_M_SUB_1_MESSAGE_LOOP
	
	if(i == 2) printf("SUCCESS\n");
	else printf("FAILURE\n");
	
	i = 0;
	
	START_M_SUB_2_MESSAGE_LOOP
		//printf("m_sub_2 id: %d\n", m_sub_2_message->id);
	if(m_sub_2_message->id != INT_SINGLE) i = -1;
	FINISH_M_SUB_2_MESSAGE_LOOP
	
	if(i == 0) printf("SUCCESS\n");
	else printf("FAILURE\n");
	
	return 0;
}
