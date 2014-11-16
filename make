rm -f ctrterminal agent *.o *.exe
gcc -c hostCommands.c
gcc agent.c hostCommands.o -o agent
gcc -c commands.c
gcc controller.c commands.o -o ctrterminal
