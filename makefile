SOURCES = iosched.c iosched_objects.c queue.c
OBJECTS = iosched.o iosched_objects.o queue.o
HEADERS = iosched_objects.h queue.h operation.h
EXEBIN  = iosched

all: $(EXEBIN)

$(EXEBIN) : $(OBJECTS) $(HEADERS)
	gcc -o $(EXEBIN) $(OBJECTS)

$(OBJECTS) : $(SOURCES) $(HEADERS)
	gcc -c $(SOURCES)

clean :
	rm -f $(EXEBIN) $(OBJECTS)