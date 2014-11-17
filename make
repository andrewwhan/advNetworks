#rm -f ctrterminal agent *.o *.exe
g++ -c hostCommands.c
g++ agent.c hostCommands.o -o agent
g++ -c commands.c
g++ controller.c commands.o -o ctrterminal
