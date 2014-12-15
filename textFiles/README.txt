Our code is divided so that the controller and agent can be compiled separately.
make
or
make all
Will compile both the controller into a file that can be run with
./controller
and the agent into a file that can be run with
./agent (sudo necessary for executing instructions from the controller)

make controller
will just compile the controller and

make agent
will just compile the agent.

make clean
will remove binaries and extra logging files.

The controller code exists in the files
controller.c: Loads the database file (database.txt) of host information,
Listens for the hosts to connect,
Supplies the command terminal for input,
Contains the main function for the controller

and

commands.c: Constructs the message to send to the agent,
waits for the response from the agent


The agent code exists in the files
agent.c: Attempts to connect to the controller,
Reads credentials.txt and sends preshared secret,
Waits for commands from the controller
Contains the main function for the agent

and

hostCommands.c: Processes a message from the controller,
Executes the corresponding action,
Sends a response to the controller


Each .c file comes with a corresponding .h file with function protoypes and includes.