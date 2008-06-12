#include "header.h"
#include "agent_a_agent_header.h"

/** \fn agent_a_1()
 * \brief A description of the function
 */
int agent_a_1()
{
	int i;
	
	printf("iteration_loop5 = %d\n", (iteration_loop%5));
	
	add_m_sub_1_message(0);
	add_m_sub_1_message(1);
	add_m_sub_1_message(2);
	add_m_sub_1_message(3);
	add_m_sub_1_message(4);
	add_m_sub_1_message(5);
	add_m_sub_1_message(6);
	
	printf("my_constant = %d\n", my_constant);
	
	printf("my_int_array = {%d, %d}\n",
			MY_INT_ARRAY[0],
			MY_INT_ARRAY[1]);
	printf("my_dyn_int_array = {");
	for(i = 0; i < MY_DYN_INT_ARRAY.size; i++)
	{
		printf("%d", MY_DYN_INT_ARRAY.array[i]);
		if(i < (MY_DYN_INT_ARRAY.size - 1)) printf(", ");
	}
	printf("}\n");
	
	printf("var_my_datatype_1 = {%d, %d}\n",
			VAR_MY_DATATYPE_1.id,
			VAR_MY_DATATYPE_1.type);
	printf("var_my_datatype_2 = {{%d, %d}}\n",
			VAR_MY_DATATYPE_2.var_my_datatype_1.id,
			VAR_MY_DATATYPE_2.var_my_datatype_1.type);

	MY_INT_ARRAY[0] = 8;
	MY_INT_ARRAY[1] = 9;
	
	add_int(&MY_DYN_INT_ARRAY, 9);
	
	VAR_MY_DATATYPE_1.id = 8;
	VAR_MY_DATATYPE_1.type = 9;
	
	VAR_MY_DATATYPE_2.var_my_datatype_1.id = 7;
	VAR_MY_DATATYPE_2.var_my_datatype_1.type = 8;
	
	return 0;
}

/** \fn agent_a_2()
 * \brief A description of the function
 */
int agent_a_2()
{
	/* code here */
	START_M_SUB_2_MESSAGE_LOOP
		printf("%d - m_sub_2 from %d\n", ID, m_sub_2_message->var_my_datatype_1.id);
	FINISH_M_SUB_2_MESSAGE_LOOP

   return 0;
}
