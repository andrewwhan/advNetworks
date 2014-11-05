all: controller

controller: controller.c controller.h commands.h commands
	gcc controller.c commands.o -o ctrterminal

commands: commands.c commands.h
	gcc -c commands.c  
clean:
	rm -f ctrterminal *.o *.exe