#include "header.h"
#include "agent_a_agent_header.h"

/** \fn agent_a_1()
 * \brief A description of the function
 */
int agent_a_1()
{
	int int_list[2];
	float float_list[2];
	double double_list[2];
	char char_list[2];

	int_list[0] = 1; int_list[1] = 2;
	float_list[0] = 3.4; float_list[1] = 5.6;
	double_list[0] = 7.8; double_list[1] = 9.10;
	char_list[0] = 'b'; char_list[1] = 'c';

	add_m_sub_1_message(INT_SINGLE, 0.0, 0.0, 'a', int_list, float_list, double_list, char_list);

	return 0;
}

/** \fn agent_a_2()
 * \brief A description of the function
 */
int agent_a_2()
{
	int message_count = 0;
	
	/* code here */
	START_M_SUB_1_MESSAGE_LOOP
		//printf("%d> %d - m_sub_1 int_single: %d\n", node_number, INT_SINGLE, m_sub_1_message->int_single);
		message_count++;
	FINISH_M_SUB_1_MESSAGE_LOOP

	add_m_sub_2_message(message_count);
	
   return 0;
}

/** \fn agent_a_3()
 * \brief A description of the function
 */
int agent_a_3()
{
	add_m_sub_3_message(0);
	
   return 0;
}

/** \fn agent_a_4()
 * \brief A description of the function
 */
int agent_a_4()
{
	/* code here */
	START_M_SUB_3_MESSAGE_LOOP
		//printf("%d> %d - m_sub_1 int_single: %d\n", node_number, INT_SINGLE, m_sub_1_message->int_single);
	FINISH_M_SUB_3_MESSAGE_LOOP

	add_m_sub_4_message(0);
	
   return 0;
}

/** \fn agent_a_5()
 * \brief A description of the function
 */
int agent_a_5()
{
	/* code here */
	START_M_SUB_4_MESSAGE_LOOP
		//printf("%d> %d - m_sub_1 int_single: %d\n", node_number, INT_SINGLE, m_sub_1_message->int_single);
	FINISH_M_SUB_4_MESSAGE_LOOP
	
   return 0;
}
