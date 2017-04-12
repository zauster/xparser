Tests for XParser
Author: Sander van der Hoog
Date: 6 March 2017

These tests are meant for checking the behavior of XParser for message input/output defined in the model.xml file.
In some cases XParser segfaults, while it should simply raise an error and exit gracefully. In other cases, it raises an error while it should instead raise a warning. And in some cases it just removes a message.

General considerations for how to deal with messages in the model.xml files:

1. If messages are not well-defined in model.xml but are used as in/output to functions: this should raise an ERROR to stderr.

2. If messages are well-defined in model.xml but are not used as input (output) to any function, while there does exist another function that uses the message as an output (input): this should raise a WARNING to stderr.

3. If messages are well-defined in model.xml but are not used as input or output to any functions: these message definitions should not be removed as is now the default.

============================================
Model 0: Message is defined, but not used at all, message is removed from model.

Agent_a has a function function_1 that has no message input/output.
A message 'info' is defined.

Result: 
Output to stdout.txt:
!!! WARNING: info message is not input or output by any agent function, message removed from model

Expected:
Do not remove the message. No output to stderr.txt about this.
============================================
Model 1a: Message defined, used as output, but not as input.

Agent_a has a function function_1 that outputs a message: info that is well-defined in the model.xml.
But there is no agent that has info as an input message.

Result:
Segmentation fault of xparser.

Expected:
Output to stderr, raise a warning message:
WARNING: Message <message_name> is defined as output message to function <function_name> but is not defined as input to any function.

Exit gracefully.
============================================
Model 1b: Message is defined, and used as input, but not as output.

Agent_a has a function function_1 that inputs a message: info that is well-defined in the model.xml.
But there is no agent that has info as an output message.

Result: 
Output to stdout.txt:
!!! WARNING: info message is not output by any agent function

Expected:
Correct: Produces a warning.
============================================
Model 2: Message is not defined, but used as output.

Agent_a has a function function_1 that outputs a message: info_2 that is not defined in the model.xml.

Result: This model parses without errors. No error is raised about the message being used in a function not being defined.

Expected:
Output to stderr: Error message from xparser: 
ERROR: Message <message_name> is not defined in the model in file <file_name>, but is defined as output to function <function_name> of agent <agent_name>.
============================================
Model 3: Message is not defined, but used as and input. 

Agent_a has a function function_1 that outputs a message: info_3 that is not defined in the model.xml.
Agent_b has a function function_2 that inputs a message: info_4 that is not defined in the model.xml.

Result: 
About info_4 an error is raised. This is correct:
ERROR: input message type 'info_4' in function 'function_2' in agent 'Agent_b' in 'filename' does not exist.

About info_3 no error is raised. This is incorrect.

Expected:
Also raise an error about info_3 that also does not exist, but is defined as an output message (same as in test Model 2).
XParser should not stop on the first error it encounters, but continue to parse the model if possible.
This behavior would be similar to how gcc continues compiling C code, and raises warnings and errors further downstream.
============================================
Model 4: Combination of model 1a and 1b

a. A message is defined, and used as output, but not as input. No warning is raised.
b. A message is defined, and used as input, but not as output. No warning is raised.

Agent_a has a function function_1 that outputs a message: info_5 that is well-defined in the model.xml.
But there is no other function that has this message as input.

Agent_b has a function function_2 that inputs a message: info_4 that is well-defined in the model.xml.
But there is no other function that has this message as output.

Result: 
Segmentation fault of xparser.

Expected:
Raise warnings about input messages that are well-defined and used as input, but not used as output for any function.
Raise warnings about output messages that are well-defined and used as output, but not used as input for any function.
Exit gracefully.
============================================