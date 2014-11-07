all: controller

controller: controller.c controller.h commands.h hostCommands.h commands
	gcc controller.c commands.o hostCommands.o -o ctrterminal

commands: commands.c commands.h hostCommands.h hostCommands
	gcc -c commands.c

hostCommands: hostCommands.c hostCommands.h
	gcc -c hostCommands.c

clean:
	rm -f ctrterminal *.o *.exe
