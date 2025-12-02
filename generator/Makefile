CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex

TARGET = AutoBSP
OBJS = lexerInC.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

lexerInC.o: lexerInC.c

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

clean:
	rm -f $(TARGET) $(OBJS) lexerInC.c
