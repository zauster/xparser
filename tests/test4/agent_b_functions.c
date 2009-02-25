#include "header.h"
#include "agent_b_agent_header.h"

/** \fn agent_b_1()
 * \brief A description of the function
 */
int agent_b_1()
{
	return 0;
}

/** \fn agent_b_2()
 * \brief A description of the function
 */
int agent_b_2()
{
	int message_count = 0;
	
	START_M_SUB_2_MESSAGE_LOOP
		//printf("%d> %d - m_sub_1 int_single: %d\n", node_number, INT_SINGLE, m_sub_1_message->int_single);
		message_count += m_sub_2_message->int_single;
	FINISH_M_SUB_2_MESSAGE_LOOP
	
	/*printf("env_int = %d\tmessage_count = %d\n", ENV_INT, message_count);*/
	if(ENV_INT == message_count + 1) printf("SUCCESS\n");
	else printf("FAILURE\n");
	
	return 0;
}
