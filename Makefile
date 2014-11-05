all: controller

controller: controller.c controller.h
	gcc controller.c controller.h -o ctrterminal

clean:
	rm -f ctrterminal *.exe