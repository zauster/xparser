#include "header.h"
#include "agent_b_agent_header.h"

/** \fn agent_b_1()
 * \brief A description of the function
 */
int agent_b_1()
{
	/* code here */
	START_M_SUB_1_MESSAGE_LOOP
		printf("%d - m_sub_1 from %d\n", ID, m_sub_1_message->int_single);
		printf("\t%f\n", m_sub_1_message->float_single);
		printf("\t%f\n", m_sub_1_message->double_single);
		printf("\t%c\n", m_sub_1_message->char_single);
		printf("\t[ %d, %d ]\n", m_sub_1_message->int_list[0], m_sub_1_message->int_list[1]);
		printf("\t[ %f, %f ]\n", m_sub_1_message->float_list[0], m_sub_1_message->float_list[1]);
		printf("\t[ %f, %f ]\n", m_sub_1_message->double_list[0], m_sub_1_message->double_list[1]);
		printf("\t[ %c, %c ]\n", m_sub_1_message->char_list[0], m_sub_1_message->char_list[1]);
	FINISH_M_SUB_1_MESSAGE_LOOP

	return 0;
}

/** \fn agent_b_2()
 * \brief A description of the function
 */
int agent_b_2()
{
	/* Declare variable to send in message */
	my_datatype_1 var_my_datatype_1;
	my_datatype_2 var_my_datatype_2;
	my_datatype_1 var_my_datatype_1_list[2];
	my_datatype_2 var_my_datatype_2_list[2];
	/* Assign values to variable to send in message */
	var_my_datatype_1.id = ID;
	var_my_datatype_1.type = TYPE;
	var_my_datatype_1_list[0].id = 0;
	var_my_datatype_1_list[0].type = 1;
	var_my_datatype_1_list[1].id = 2;
	var_my_datatype_1_list[1].type = 3;
	var_my_datatype_2.var_my_datatype_1.id = 9;
	var_my_datatype_2.var_my_datatype_1.type = 8;
	var_my_datatype_2_list[0].var_my_datatype_1.id = 7;
	var_my_datatype_2_list[0].var_my_datatype_1.type = 6;
	var_my_datatype_2_list[1].var_my_datatype_1.id = 5;
	var_my_datatype_2_list[1].var_my_datatype_1.type = 4;
	/*  Send message with variable */
	add_m_sub_2_message(var_my_datatype_1, var_my_datatype_2, var_my_datatype_1_list, var_my_datatype_2_list);
	
	return 0;
}
