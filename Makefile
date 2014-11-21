all: controller agent

controller: controller.c controller.h commands hostCommands
	gcc controller.c commands.o hostCommands.o sendAndExecute.o -o controller

commands: commands.c commands.h
	gcc -c commands.c

agent: agent.c agent.h hostCommands
	gcc agent.c hostCommands.o sendAndExecute.o -o agent

hostCommands: hostCommands.c hostCommands.h sendAndExecute
	gcc -c hostCommands.c

sendAndExecute: sendAndExecute.c sendAndExecute.h
	gcc -c sendAndExecute.c
clean:
	rm -f controller agent hostSTDERR.txt hostSTDOUT.txt *.o *.exe
