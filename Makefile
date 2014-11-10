all: controller agent

controller: controller.c controller.h commands hostCommands
	gcc controller.c commands.o hostCommands.o -o ctrterminal

commands: commands.c commands.h
	gcc -c commands.c

hostCommands: hostCommands.c hostCommands.h
	gcc -c hostCommands.c

agent: agent.c agent.h
	gcc agent.c -o agent

clean:
	rm -f ctrterminal agent *.o *.exe
