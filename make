rm -f ctrterminal agent *.o *.exe
gcc agent.c -o agent
gcc -c hostCommands.c
gcc -c commands.c
gcc controller.c commands.o hostCommands.o -o ctrterminal
