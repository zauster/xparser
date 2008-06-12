#include "header.h"
#include "agent_b_agent_header.h"

/** \fn agent_b_1()
 * \brief A description of the function
 */
int agent_b_1()
{
	/* code here */
	START_M_SUB_1_MESSAGE_LOOP
		printf("%d - m_sub_1 from %d\n", ID, m_sub_1_message->id);
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
	/* Assign values to variable to send in message */
	var_my_datatype_1.id = ID;
	var_my_datatype_1.type = TYPE;
	/*  Send message with variable */
	add_m_sub_2_message(var_my_datatype_1);
	
	return 0;
}