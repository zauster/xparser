#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************
 * 
 * Change FLAME_XPARSER_DIR here if required
 * 
 *****************************************************************/
 /** \def FLAME_XPARSER_DIR
* \brief FLAME_XPARSER_DIR - the location of the xparser templates
*/
#define FLAME_XPARSER_DIR "."

/** \def NAME
 * \brief Release name. */
#define NAME "FLAME X-machine agent parser"
/** \def VERSIONMAJOR
 * \brief Release number. */
#define VERSIONMAJOR 0
/** \def VERSIONMINOR
 * \brief New features. */
#define VERSIONMINOR 17
/** \def VERSIONMICRO
 * \brief Bug fixes. */
#define VERSIONMICRO 1
/** \def DEBUG
 * \brief Debug output. */
#define DEBUG 0
/** \def LOG
 * \brief Print to log. */
#define LOG 0
/** \def ARRAY_BLOCK_SIZE
 * \brief The block size to allocate to dynamic arrays. */
#define ARRAY_BLOCK_SIZE 10
/** \def ARRAY_GROWTH_RATE
 * \brief Growth ratio to scale size of dynamic arrays. 
 * Use golden ratio. This makes for a Fibonacci sequence, so the next allocation size 
 * is the sum of the current size and the previous size thus improving the chance of 
 * memory block reuse. It also allows for moderate rate of growth.
 */
#define ARRAY_GROWTH_RATE 1.618034
/** \def LIBMBOARD_MINVER_STR
 * \brief Minimum version of libmboard needed to compile/run generated code */
#define LIBMBOARD_MINVER_STR "0.3.0"

/*****************************************************************
* NAME:    struct datatype definitions                           *
* PURPOSE: define data types used in the program                 *
*****************************************************************/

/** \struct char_list
 * \brief Holds characters for a string.
 *
 * Holds characters for a string by using a linked list of characters.
 */
struct char_list
{
	char character;				/**< Character being held. */

	struct char_list * next;		/**< Pointer to next character in list. */
};

/** \struct int_array
 * \brief Dynamic array to hold integers.
 *
 * Holds a pointer to an integer array and values for size and memory size.
 */
struct int_array
{
	int size;
	int total_size;

	int * array;
};

/** \struct double_array
 * \brief Dynamic array to hold doubles.
 *
 * Holds a pointer to a double array and values for size and memory size.
 */
struct double_array
{
	int size;
	int total_size;

	double * array;
};

/** \struct char_array
 * \brief Dynamic array to hold chars.
 *
 * Holds a pointer to a char array and values for size and memory size.
 */
struct char_array
{
	int size;
	int total_size;

	char * array;
};

/** \struct input_file
 * \brief Model file data.
 *
 * Holds file name, directory, and enabled status of model files.
 */
struct input_file
{
	char * file;
	char * fullfilepath;
	char * fulldirectory;
	char * localdirectory;
	int enabled;

	struct input_file * next;
};

typedef struct input_file input_file;

/** \struct variable
 * \brief Holds variable data from xmachine memory and messages.
 *
 * Holds variable data for xmachine memory and messages using a linked list. Some variables being redundant for different purposes.
 */
struct variable
{
	char * type;		/**< Pointer to variable C type. */
	struct model_datatype * datatype;	/**< Pointer to variable model datatype. */
	char * name;		/**< Pointer to variable name. */
	char * value;		/**< Pointer to variable value. */
	char * typenotarray;		/**< Pointer to type name used if array. */
	char mpi_type[50];				/**< Variable MPI type. */
	int arraylength;				/**< Variable array length. Zero for no array, -1 for dynamic array */
	char defaultvalue[5];			/**< Default value for the type. */
	char c_type[5];				/**< Variable C type, e.g 'i' or 'f'. */
	int ismodeldatatype;			/**< Flag for model defined data type. */
	char * file;			/**< File from where element read. */
	struct xmachine_message * message;	/**< Pointer to message containing this variable, NULL if not. */
	struct xmachine * agent;			/**< Pointer to agent containing this variable, NULL if not. */
	int constant;			/**< Flag for constant value. */
	char * description;		/**< Description of the variable. */

	struct variable * next;		/**< Pointer to next variable in list. */
};

/** \struct xmachine_state
 * \brief Holds state name, attributes and transitions.
 */
struct xmachine_state
{
	char * name;		/**< Pointer to state name. */
	char * agent_name;	/**< Pointer to agent name that holds this state. */
	struct function_pointer * incoming_functions; /**< List of incoming functions. */
	struct function_pointer * outgoing_functions; /**< List of outgoing functions. */

	struct xmachine_state * next;	/**< Pointer to next state in list. */
};

/** \struct xmachine_state_holder
 * \brief Holds pointer to an agent state.
 */
struct xmachine_state_holder
{
	struct xmachine_state * state;

	struct xmachine_state_holder * next;	/**< Pointer to next state in list. */
};

typedef struct xmachine_state_holder xmachine_state_holder;

/*struct state_pointer
{
	struct xmachine_state * state;

	struct state_pointer * next;
};

typedef struct state_pointer state_pointer;*/

/** \struct f_code
 * \brief Holds code.
 */
struct f_code
{
	char * code;	/**< Pointer to code. */

	struct f_code * next;		/**< Pointer to next code in list. */
};


/** \struct rule_data
 * \brief Holds rule data.
 */
struct rule_data
{
	char * lhs;
	char * op;
	char * rhs;
	char * lhs_print;
	char * op_print;
	char * rhs_print;
	struct variable * lhs_variable; /* Pointer to the lhs variable */
	struct variable * rhs_variable; /* Pointer to the rhs variable */
	struct rule_data * lhs_rule; /* If rule==NULL then use lhs,op,rhs data */
	struct rule_data * rhs_rule; /* If rule==NULL then use lhs,op,rhs data */
	int time_rule;
	int box;  /* 0 = no, 2 = 2d, 3 = 3d */
	int not;
	int has_agent_var;
	int has_message_var;
	struct rule_data * parent_rule;

	struct rule_data * next;
};

typedef struct rule_data rule_data;

/** \struct xmachine_ioput
 * \brief Holds input and output data for agent functions.
 */
struct xmachine_ioput
{
	char * messagetype;
	char * filter_function;		/**< The filter rule function name. */
	struct rule_data * filter_rule;
	struct xmachine_function * function;
	struct xmachine_message * message;
	int random;					/**< Flag for randomisation, 0 for false, 1 for true. */
	char * sort_function;		/**< The sort function name if defined. */
	char * sort_key;			/**< The sort message variable. */
	char * sort_order;			/**< The sort order, ascend/descend. */
	int non_constant_vars;		/**< Flag if filter includes non-constant vars. */
	char * box_apothem;         /**< The box apothem value/variable. */

	struct xmachine_ioput * next;
};

typedef struct xmachine_ioput xmachine_ioput;

/** \struct xmachine_function
 * \brief Holds function data.
 */
struct xmachine_function
{
	char * name;		/**< Pointer to function name. */
	char * note;		/**< Pointer to function note. */
	struct f_code * code;			/**< Pointer to function code. */

	char * agent_name;

	char * file;		/**< File from where element read from. */

	double x;
	double y;
	int rank_in;		/**< Execution layer. */
	int index;			/**< Process order index. */
	int score;			/**< Order score. */

	struct rule_data * condition_rule;	/**< The condition rule function name. */
	char * condition_function;

	struct rule_data * filter_rule; /**< Only for use of message sync  */
	int has_message_var; /**< Only for use of message sync  */
	int has_agent_var; /**< Only for use of message sync  */
	
	char * current_state;
	char * next_state;
	struct xmachine_ioput * inputs;
	struct xmachine_ioput * outputs;

	/* Holds first inputs and last outputs of specific message types */
	/*struct xmachine_ioput * first_inputs;*/
	/*struct xmachine_ioput * last_outputs;*/
	struct sync_pointer * start_syncs;
	struct sync_pointer * complete_syncs;

	struct adj_function * dependson;	/**< Node list of dependencies. */
	struct adj_function * dependants;	/**< Node list of dependants. */

	struct adj_function * alldepends;
	struct adj_function * recentdepends;

	/* To hold depends tag info */
	struct adj_function * depends;		/**< Pointer to function note. */
	
	char * description;		/**< Description of the function. */

	struct xmachine_function * next;	/**< Pointer to next function in list. */
};

/** \struct adj_function
 * \brief Pointer to an agent function.
 */
struct adj_function
{
	struct xmachine_function * function;
	char * type;

	char * name;

	struct adj_function * next;
};

typedef struct adj_function adj_function;

/** \struct env_func
 * \brief Holds environment functions.
 */
struct env_func
{
	int header;	/**< Pointer to flag if header file (1) or functions file (2). */
	char * code;	/**< Pointer to environment function code. */
	char * filepath;	/**< Pointer to environment function filepath. */

	struct env_func * next;	/**< Pointer to next environment function in list. */
};

/** \struct sync
 * \brief Holds sync data for a message type.
 */
struct sync
{
	struct xmachine_message * message;

	char * name;		/**< The sync name and therefore the filter rule function name. */
	struct rule_data * filter_rule;
	struct variable * vars;			/**< Pointer to agent filter variable types. */

	struct xmachine * inputting_agents;	/**< Agents that have inputting filter functions, states holding agents that could use the filter */
	int filter_agent_count;
	int has_agent_and_message_vars;

	struct function_pointer * lastdepend;	/**< The last function start sync depends on. */
	struct function_pointer * firstdependent;	/**< The first function with input in this sync. */

	/* Sync start depends on last... */
	struct function_pointer * outputting_functions;	/**< Functions that output the message type. */
	struct function_pointer * filter_variable_changing_functions;	/**< Functions that change the variables of filters required by the sync. */
	struct function_pointer * previous_sync_inputting_functions;	/**< Inputting functions of previous syncs that need to end first. */
	/* Sync complete just before first... */
	struct function_pointer * inputting_functions;	/**< Functions that input the message type. */

	struct xmachine_ioput * filters;		/**< Pointer to functions with filters for this sync. */

	struct sync * next;
};

typedef struct sync sync;

/** \struct sync_pointer
 * \brief Holds pointer to a sync.
 */
struct sync_pointer
{
	struct sync * current_sync;	/**< Pointer to sync. */

	struct sync_pointer * next;		/**< Pointer next sync in list. */
};

typedef struct sync_pointer sync_pointer;

/** \struct xmachine_message
 * \brief Holds name and list of variables for a message type.
 *
 * Holds name and list of variables for a message type using a linked list.
 */
struct xmachine_message
{
	char * name;						/**< Pointer to message name. */
	struct variable * vars;				/**< Pointer to message variables. */
	struct sync * syncs;				/**< List of syncs for this message board. */
	int var_number;						/**< Number of variables in memory. */
	char * file;
	struct xmachine * outputting_agents;	/**< Agents that output this message type */
	struct xmachine * inputting_agents;		/**< Agents that input this message type */
	char * description;						/**< Description of the message. */
	/* Catalogue use of radius/apothem to build trees for function input */
    int has_box2d;
    int has_box3d;
    int make_x_function;
    int make_y_function;
    int make_z_function;

	struct xmachine_message * next;	/**< Pointer to next message in list.*/
};

/** \struct xmachine
 * \brief Holds xmachines.
 */
struct xmachine
{
	int number;								/**< X-machine number !check. */
	char * name;							/**< Pointer X-machine name. */
	struct variable * variables;		/**< Pointer X-machine memory. */
	struct xmachine_state * states;			/**< Pointer X-machine states. */
	struct xmachine_function * functions;	/**< Pointer X-machine functions. */

	struct xmachine_state * start_state;
	struct xmachine_state_holder * end_states;

	char rangevar[50];						/**< Variable name for range */
	char idvar[50]; 						/**< Variable name for agent id. */
	char xvar[50];							/**< Variable name for position in x-axis. */
	char yvar[50];							/**< Variable name for position in y-axis. */
	char zvar[50];							/**< Variable name for position in z-axis. */
	int var_number;							/**< Number of variables in memory. */
	int gsl_lib;							/** bool to indicate the GSL_RND_SEED is present - thus gsl_lib being used */

	struct xmachine * next;				/**< Pointer next X-machine in list. */
};

/** \struct layer
 * \brief Holds layers of functions.
 */
struct layer
{
	int number;
	struct function_pointer * functions;	/**< Pointer to list of functions. */
	struct sync * start_syncs;				/**< List of start syncs. */
	struct sync * complete_syncs;				/**< List of complete syncs. */
	struct xmachine_state_holder * branching_states; /**< List of branching states whose agents have entered and can be checked. */
	struct xmachine_message * finished_messages;	/**< List of messages that are not used after this layer. */

	struct layer * next;					/**< Pointer next X-machine in list. */
};

/** \struct function_pointer
 * \brief Holds pointer to a function.
 */
struct function_pointer
{
	struct xmachine_function * function;	/**< Pointer to list of functions. */
	int found;						/**< Flag to help with state branching functions. */

	struct function_pointer * next;		/**< Pointer next X-machine in list. */
};

/** \struct model_datatype
 * \brief Holds model datatypes.
 */
struct model_datatype
{
	char * name;					/**< Name of the datatype. */
	char * desc;					/**< Description of the datatype. */
	struct variable * vars;		/**< Pointer to variables of the datatype. */
	int has_single_vars;			/**< Flag if the datatype holds single variables. */
	int has_dynamic_arrays;			/**< Flag if the datatype holds dynamic arrays. */
	int has_arrays;					/**< Flag if the datatype holds arrays. */
	int has_arrays_or_adts;			/**< Flag if the datatype holds arrays or adts. */
	
	struct model_datatype * next;		/**< Pointer next model datatype. */
};

/** \struct flame_communication
 * \brief Holds agent communication data.
 */
struct flame_communication
{
	char * messagetype;
	struct rule_data * filter_rule;
	struct xmachine_function * output_function;
	struct xmachine_function * input_function;

	struct flame_communication * next;
};

typedef struct flame_communication flame_communication;

/** \struct time_data
 * \brief Holds time data.
 */
struct time_data
{
	char * name;
	int period;
	struct time_data * unit;
	int iterations;

	struct time_data * next;
};

typedef struct time_data time_data;


/** \struct agent_colour for link list
 * \brief Holds agent colour.
 */
struct agent_colour
{
	char * name;							/**< Pointer X-machine name. */
	int colour;								/**< X-machine colour in map */
	
	struct agent_colour * next;
};

typedef struct agent_colour agent_colour;



/** \struct agent_colour for link list
 * \brief Holds agent colour.
 */
struct model_colour
{
	char * name;							/**< Pointer X-machine name. */
	int colour;								/**< X-machine colour in map */
	
	struct model_colour * next;
};

typedef struct model_colour model_colour;

/** \struct sub_model_data
 * \brief Holds sub model data.
 */
struct sub_model_data
{
	char * name;
	char * location;
	struct xmachine ** p_xmachines;
	struct xmachine_message ** p_xmessages;
};

/** \typedef struct sub_model_data sub_model_data
 * \brief Typedef for sub_model_data struct.
 */
typedef struct sub_model_data sub_model_data;

/** \struct model_data
 * \brief Holds model data.
 */
struct model_data
{
	char * name;
	int code_type;
	struct model_datatype ** p_datatypes;
	struct xmachine ** p_xmachines;
	struct xmachine_message ** p_xmessages;
	struct variable ** p_envvars;
	struct variable ** p_envdefines;
	struct variable ** p_constant_filter_vars;
	struct env_func ** p_envfuncs;
	struct variable ** p_allvars;
	struct f_code ** p_it_end_code;
	struct layer ** p_layers;
	struct flame_communication ** p_communications;
	int number_messages;
	int number_xmachines;
	int agents_include_array_variables;
	int layer_total;
	struct time_data ** p_time_units;
	int depends_style;
	input_file ** p_files;
	int debug_mode;
	int gsl_lib;
};

/* explicit define datatypes so dont need to use struct anymore */
/** \typedef struct model_data model_data
 * \brief Typedef for model_data struct.
 */
typedef struct model_data model_data;
/** \typedef struct xmachine xmachine
 * \brief Typedef for xmachine struct.
 */
typedef struct xmachine xmachine;
/** \typedef struct xmachine_memory xmachine_memory
 * \brief Typedef for xmachine_memory struct.
 */
typedef struct xmachine_memory xmachine_memory;
/** \typedef struct function_dependency function_dependency
 * \brief Typedef for function_dependency struct.
 */
typedef struct function_dependency function_dependency;
/** \typedef struct xmachine_message xmachine_message
 * \brief Typedef for xmachine_message struct.
 */
typedef struct xmachine_message xmachine_message;
/** \typedef struct xmachine_state xmachine_state
 * \brief Typedef for xmachine_state struct.
 */
typedef struct xmachine_state xmachine_state;
/** \typedef struct xmachine_function xmachine_function
 * \brief Typedef for xmachine_function struct.
 */
typedef struct xmachine_function xmachine_function;
/** \typedef struct variable variable
 * \brief Typedef for variable struct.
 */
typedef struct variable variable;
/** \typedef struct char_list char_list
 * \brief Typedef for char_list struct.
 */
typedef struct char_list char_list;
/** \typedef struct int_array int_array
 * \brief Typedef for int_array struct.
 */
typedef struct int_array int_array;
/** \typedef struct double_array double_array
 * \brief Typedef for double_array struct.
 */
typedef struct double_array double_array;
/** \typedef struct char_array char_array
 * \brief Typedef for char_array struct.
 */
typedef struct char_array char_array;
/** \typedef struct s_attrib s_attrib
 * \brief Typedef for s_attrib struct.
 */
typedef struct s_attrib s_attrib;
/** \typedef struct s_trans s_trans
 * \brief Typedef for s_trans struct.
 */
typedef struct s_trans s_trans;
/** \typedef struct f_code f_code
 * \brief Typedef for f_code struct.
 */
typedef struct f_code f_code;
/** \typedef struct env_func env_func
 * \brief Typedef for env_func struct.
 */
typedef struct env_func env_func;
/** \typedef struct layer layer
 * \brief Typedef for layer struct.
 */
typedef struct layer layer;
/** \typedef struct function_pointer function_pointer
 * \brief Typedef for function_pointer struct.
 */
typedef struct function_pointer function_pointer;
/** \typedef struct model_datatype model_datatype
 * \brief Typedef for model_datatype struct.
 */
typedef struct model_datatype model_datatype;

/* memory.c */
sync * addsync(sync ** p_sync);
void freesync(sync ** p_sync);
void add_sync_pointer(sync_pointer ** p_sync_pointers, sync * current_sync);
void free_sync_pointers(sync_pointer ** p_sync_pointers);
void free_modeldata(model_data * modeldata);
input_file * add_input_file(input_file ** p_files);
void free_input_files(input_file ** p_files);
void addstateholder(xmachine_state * state, xmachine_state_holder ** p_list);
void freestateholder(xmachine_state_holder ** p_list);
rule_data * add_rule_data(rule_data ** p_data);
void free_rule_data(rule_data ** p_data);
void copy_rule_data(rule_data * to, rule_data * from);
void add_time_unit(char * name, char * unit_name, int period, time_data ** p_time_units);
void free_time_units(time_data ** p_time_units);
xmachine_ioput * addioput(xmachine_ioput ** p_ioput);
void free_ioput(xmachine_ioput ** p_ioput);
f_code * addfcode(f_code ** p_code);
void freefcode(f_code ** p_code);
variable * addvariable(variable ** p_vars);
void freevariables(variable ** p_vars);
xmachine_memory * addxmemory(xmachine_memory ** p_xmemory);
void freexmemory(xmachine_memory ** p_xmemory);
env_func * addenvfunc(env_func ** p_env_funcs);
void freeenvfunc(env_func ** p_env_funcs);
xmachine_message * addxmessage(xmachine_message ** p_xmessage);
void freexmessage(xmachine_message ** p_xmessage, xmachine_message * message);
void freexmessages(xmachine_message ** p_xmessage);
void addxstate(char * name, char * agent_name, xmachine_state ** p_xstates);
void freexstates(xmachine_state ** p_xstates);
xmachine_function * addxfunction(xmachine_function ** p_xfunctions);
void freexfunctions(xmachine_function ** p_xfunctions);
xmachine * addxmachine(xmachine ** p_xmachines, char * name);
void freexmachines(xmachine ** p_xmachines);
layer * addlayer(layer ** p_layer);
void freelayers(layer ** p_layers);
void addfunction_pointer(function_pointer ** p_function_pointers, xmachine_function * function);
void freefunction_pointers(function_pointer ** p_function_pointers);
model_datatype * adddatatype(model_datatype ** p_datatypes);
void freedatatypes(model_datatype ** p_datatypes);
void add_flame_communication(char * messagetype, rule_data * filter_rule, xmachine_function * function1, xmachine_function * function2, flame_communication ** communications);
void free_flame_communications(flame_communication ** communications);
adj_function * add_depends_adj_function(xmachine_function * current_function);
void add_adj_function_simple(xmachine_function * function1, xmachine_function * function2);
void remove_adj_function_simple(xmachine_function * function1);
void add_adj_function_recent(xmachine_function * function1, xmachine_function * function2);
void remove_adj_function_recent(xmachine_function * function1);
void add_adj_function(xmachine_function * function1, xmachine_function * function2, char * type);
void free_adj_function(adj_function *adj_functions);

/*functions for output_colour*/
void addagent_colour(agent_colour **p_agent_colours, char * name, int colour_value);
void freeagent_colours(agent_colour **p_agent_colours);
int displayagent_colour(agent_colour **p_agent_colours, char * name);
void addmodel_colour(model_colour **p_model_colours, char * name, int colour_value);
void freemodel_colours(model_colour **p_model_colours);
int displaymodel_colour(model_colour **p_model_colours, char * name);
int length_colour(model_colour **p_model_colours);
/* charlist.c */
void ws(int tabcount, char * string);
void printcharlist(char_list ** p_charlist);
void writecharlist(char_list ** p_charlist, FILE * file);
void copycharlist(char_list ** p_charlist, char * chardata);
void makecharlist(char * chardata, char_list ** p_charlist);
/* dynamicarray.c */
char_array * init_char_array(void);
int_array * init_int_array(void);
void reset_char_array(char_array * array);
void free_char_array(char_array * array);
void free_int_array(int_array * array);
void add_char(char_array * array, char new_char);
char * copy_array_to_str(char_array * array);
void remove_char(char_array * array, int index);
void print_char_array(char_array * array);
void sort_int_array(int_array * array);
int quicksort_int(int * array, int elements);
void add_int(int_array * array, int new_int);
void remove_int(int_array * array, int index);
void print_int_array(int_array * array);
double_array * init_double_array(void);
void free_double_array(double_array * array);
void sort_double_array(double_array * array);
int quicksort_double(double * array, int elements);
void print_double_array(double_array * array);
void add_double(double_array * array, double new_double);
void remove_double(double_array * array, int index);
/* readmodel.c */
void readModel(input_file * inputfile, char * directory, model_data * modeldata);
int checkmodel(model_data * modeldata);
/* dependencygraph.c */
char * copystr(char * string);
int create_dependency_graph(char * filepath, model_data * modeldata);
/* parsetemplate.c */
void parseTemplate(char * filename, char * templatename, model_data * modeldata);
void parseAgentHeaderTemplate(char * directory, model_data * modeldata);
void parseUnittest(char * directory, model_data * modeldata);
void parser0xsd(char * directory, model_data * modeldata);
void parser0dtd(char * directory, model_data * modeldata);
