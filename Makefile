all: controller agent

controller: controller.c controller.h commands
	gcc controller.c commands.o -o controller

commands: commands.c commands.h
	gcc -c commands.c

agent: agent.c agent.h hostCommands hostPacket
	gcc agent.c hostCommands.o sendAndExecute.o hostPacket.o -o agent

hostCommands: hostCommands.c hostCommands.h sendAndExecute
	gcc -c hostCommands.c
	
hostPacket: hostPacket.c hostPacket.h
	gcc -c hostPacket.c

sendAndExecute: sendAndExecute.c sendAndExecute.h
	gcc -c sendAndExecute.c

clean:
	rm -f controller agent hostSTDERR.txt hostSTDOUT.txt *.o *.exe
