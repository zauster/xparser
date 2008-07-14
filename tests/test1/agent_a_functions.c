#include "header.h"
#include "agent_a_agent_header.h"

/** \fn agent_a_1()
 * \brief A description of the function
 */
int agent_a_1()
{
	int i;
	int int_list[2];
	float float_list[2];
	double double_list[2];
	char char_list[2];
	
	printf("iteration_loop5 = %d\n", (iteration_loop%5));
	
	int_list[0] = 1; int_list[1] = 2;
	float_list[0] = 3.4; float_list[1] = 5.6;
	double_list[0] = 7.8; double_list[1] = 9.10;
	char_list[0] = 'b'; char_list[1] = 'c';
	
	add_m_sub_1_message(0, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	add_m_sub_1_message(1, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	add_m_sub_1_message(2, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	add_m_sub_1_message(3, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	add_m_sub_1_message(4, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	add_m_sub_1_message(5, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	add_m_sub_1_message(6, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);
	
	printf("my_constant = %d\n", my_constant);
	
	printf("int_list = {%d, %d}\n",
			INT_LIST[0],
			INT_LIST[1]);
	printf("my_dyn_int_array = {");
	for(i = 0; i < INT_DYNAMIC_LIST.size; i++)
	{
		printf("%d", INT_DYNAMIC_LIST.array[i]);
		if(i < (INT_DYNAMIC_LIST.size - 1)) printf(", ");
	}
	printf("}\n");
	
	printf("my_datatype_1_single = {%d, %d}\n",
			MY_DATATYPE_1_SINGLE.id,
			MY_DATATYPE_1_SINGLE.type);
	printf("my_datatype_2_single = {{%d, %d}}\n",
			MY_DATATYPE_2_SINGLE.var_my_datatype_1.id,
			MY_DATATYPE_2_SINGLE.var_my_datatype_1.type);

	INT_LIST[0] = 8;
	INT_LIST[1] = 9;
	
	add_int(&INT_DYNAMIC_LIST, 9);
	
	MY_DATATYPE_1_SINGLE.id = 8;
	MY_DATATYPE_1_SINGLE.type = 9;
	
	MY_DATATYPE_2_SINGLE.var_my_datatype_1.id = 7;
	MY_DATATYPE_2_SINGLE.var_my_datatype_1.type = 8;
	
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
		printf("\t{ [ %d, %d ], [ %d, %d] }\n",
				m_sub_2_message->var_my_datatype_1_list[0].id,
				m_sub_2_message->var_my_datatype_1_list[0].type,
				m_sub_2_message->var_my_datatype_1_list[1].id,
				m_sub_2_message->var_my_datatype_1_list[1].type);
		printf("\t%d { [ %d, %d ], [ %d, %d] }\n", m_sub_2_message->var_my_datatype_2.var_my_datatype_1.id,
				m_sub_2_message->var_my_datatype_2_list[0].var_my_datatype_1.id,
				m_sub_2_message->var_my_datatype_2_list[0].var_my_datatype_1.type,
				m_sub_2_message->var_my_datatype_2_list[1].var_my_datatype_1.id,
				m_sub_2_message->var_my_datatype_2_list[1].var_my_datatype_1.type);
	FINISH_M_SUB_2_MESSAGE_LOOP

   return 0;
}
