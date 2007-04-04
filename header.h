#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** \def NAME
 * \brief Release name. */
#define NAME "X-machine agent parser"
/** \def VERSIONMAJOR
 * \brief Release number. */
#define VERSIONMAJOR 0
/** \def VERSIONMINOR
 * \brief New features. */
#define VERSIONMINOR 11
/** \def VERSIONMICRO
 * \brief Bug fixes. */
#define VERSIONMICRO 0
/** \def DEBUG
 * \brief Debug output. */
#define DEBUG 0
/** \def LOG
 * \brief Print to log. */
#define LOG 0
/** \def ARRAY_BLOCK_SIZE
 * \brief The block size to allocate to dynamic arrays. */
#define ARRAY_BLOCK_SIZE 5

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

/** \struct variable
 * \brief Holds variable data from xmachine memory and messages.
 *
 * Holds variable data for xmachine memory and messages using a linked list. Some variables being redundant for different purposes.
 */
struct variable
{
	char * type;		/**< Pointer to variable C type. */
	char * name;		/**< Pointer to variable name. */
	char * value;		/**< Pointer to variable value. */
	char mpi_type[50];				/**< Variable MPI type. */
	int arraylength;				/**< Variable array length. Zero for no array, -1 for dynamic array */
	char defaultvalue[5];			/**< Default value for the type. */
	char c_type[5];				/**< Variable C type, e.g 'i' or 'f'. */
	
	struct variable * next;		/**< Pointer to next variable in list. */
};

/** \struct xmachine_memory
 * \brief Holds list of xmachine memories.
 *
 * Holds list of xmachine memories using a linked list.
 */
struct xmachine_memory
{
	struct variable * vars;		/**< Pointer to X-machine variables in memory. */

	struct xmachine_memory * next;	/**< Pointer to next X-machine memory in list. */
};

/** \struct s_attrib
 * \brief Holds names of attribute of a state.
 *
 * Holds names of attribute of a state using a linked list.
 */
struct s_attrib
{
	char * attrib;	/**< Pointer to attribute name. */
	
	struct s_attrib * next;	/**< Pointer to next attribute name. */
};

/** \struct s_trans
 * \brief Holds transistion function and destination of a state transition.
 */
struct s_trans
{
	char *  func;	/**< Pointer to function name. */
	char *  dest;	/**< Pointer to destination state name. */
	
	struct s_trans * next;		/**< Pointer to next transition in list. */
};

/** \struct xmachine_state
 * \brief Holds state name, attributes and transistions.
 */
struct xmachine_state
{
	char * name;		/**< Pointer to state name. */
	struct s_attrib * attributes;	/**< Pointer to state attributes. */
	struct s_trans * transitions;	/**< Pointer to state transitions. */
	
	struct xmachine_state * next;	/**< Pointer to next state in list. */
};

/** \struct f_code
 * \brief Holds code.
 */
struct f_code
{
	char * code;	/**< Pointer to code. */
	
	struct f_code * next;		/**< Pointer to next code in list. */
};

/** \struct xmachine_function
 * \brief Holds transistion name, note and code.
 */
struct xmachine_function
{
	char * name;		/**< Pointer to function name. */
	char * note;		/**< Pointer to function note. */
	struct f_code * code;			/**< Pointer to function code. */
	struct s_trans * depends;		/**< Pointer to function note. */
	
	struct xmachine_function * next;	/**< Pointer to next function in list. */
};

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

/** \struct xmachine_message
 * \brief Holds name and list of variables for a message type.
 *
 * Holds name and list of variables for a message type using a linked list.
 */
struct xmachine_message
{
	char * name;						/**< Pointer to message name. */
	struct variable * vars;			/**< Pointer to message variables. */
	struct env_func * functions;		/**< Pointer to dependency functions. */
	int var_number;						/**< Number of variables in memory. */
	
	struct xmachine_message * next;	/**< Pointer to next message in list.*/
};

/** \struct xmachine
 * \brief Holds xmachines.
 */
struct xmachine
{
	int number;								/**< X-machine number !check. */
	char * name;				/**< Pointer X-machine name. */
	struct xmachine_memory * memory;		/**< Pointer X-machine memory. */
	struct xmachine_state * states;		/**< Pointer X-machine states. */
	struct xmachine_function * functions;	/**< Pointer X-machine functions. */
	char xvar[50];							/**< Variable name for position in x-axis. */
	char yvar[50];							/**< Variable name for position in y-axis. */
	char zvar[50];							/**< Variable name for position in z-axis. */
	int var_number;							/**< Number of variables in memory. */
	
	struct xmachine * next;				/**< Pointer next X-machine in list. */
};

/** \struct layer
 * \brief Holds layers of functions between sync points.
 */
struct layer
{
	struct xmachine_function * functions;	/**< Pointer to list of functions. */
	
	struct layer * next;					/**< Pointer next X-machine in list. */
};

/** \struct model_data
 * \brief Holds model data.
 */
struct model_data
{
	char * name;
	int code_type;
	struct xmachine ** p_xmachines;
	struct xmachine_message ** p_xmessages;
	struct variable ** p_envvars;
	struct variable ** p_envdefines;
	struct env_func ** p_envfuncs;
	struct variable ** p_allvars;
	struct f_code ** p_it_end_code;
	struct layer ** p_layers;
	int number_messages;
	int number_xmachines;
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

/* memory.c */
f_code * addfcode(f_code ** p_code);
void freefcode(f_code ** p_code);
s_trans * addtrans(s_trans ** p_transitions);
void freetrans(s_trans ** p_transitions);
s_attrib * addsattrib(s_attrib ** p_attrib);
void freeattribute(s_attrib ** p_attrib);
variable * addvariable(variable ** p_vars);
void freevariables(variable ** p_vars);
xmachine_memory * addxmemory(xmachine_memory ** p_xmemory);
void freexmemory(xmachine_memory ** p_xmemory);
env_func * addenvfunc(env_func ** p_env_funcs);
void freeenvfunc(env_func ** p_env_funcs);
xmachine_message * addxmessage(xmachine_message ** p_xmessage);
void freexmessages(xmachine_message ** p_xmessage);
xmachine_state * addxstate(xmachine_state ** p_xstates);
void freexstates(xmachine_state ** p_xstates);
xmachine_function * addxfunction(xmachine_function ** p_xfunctions);
void freexfunctions(xmachine_function ** p_xfunctions);
xmachine * addxmachine(xmachine ** p_xmachines);
void freexmachines(xmachine ** p_xmachines);
layer * addlayer(layer ** p_layers);
void freelayers(layer ** p_layers);
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
void add_int(int_array * array, int new_int);
void remove_int(int_array * array, int index);
void print_int_array(int_array * array);
double_array * init_double_array(void);
void free_double_array(double_array * array);
void sort_double_array(double_array * array);
void print_double_array(double_array * array);
void add_double(double_array * array, double new_double);
void remove_double(double_array * array, int index);
/* readmodel.c */
void readModel(char * inputfile, char * directory, model_data * modeldata);
/* dependencygraph.c */
char * copystr(char * string);
void create_dependency_graph(char * filepath, model_data * modeldata);
/* parsetemplate.c */
void parseTemplate(char * filename, char * templatename, model_data * modeldata);
