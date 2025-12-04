CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex

TARGET = AutoBSP
OBJS = lexerInC.o lexer_helper.o logging.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

lexerInC.o: lexerInC.c lexer_helper.h logging.h

lexer_helper.o: lexer_helper.c lexer_helper.h

logging.o: logging.c logging.h

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) lexerInC.c
