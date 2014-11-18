all: controller agent

controller: controller.c controller.h commands
	gcc controller.c commands.o -o controller

commands: commands.c commands.h
	gcc -c commands.c

agent: agent.c agent.h hostCommands
	gcc agent.c hostCommands.o -o agent

hostCommands: hostCommands.c hostCommands.h
	gcc -c hostCommands.c


clean:
	rm -f ctrterminal agent *.o *.exe
