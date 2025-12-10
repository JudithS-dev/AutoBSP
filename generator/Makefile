CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex
YACC = bison

TARGET = AutoBSP
OBJS = parserInC.o lexerInC.o lexer_helper.o logging.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

lexerInC.o: lexerInC.c lexer_helper.h logging.h

lexer_helper.o: lexer_helper.c lexer_helper.h

logging.o: logging.c logging.h

parserInC.o: parserInC.c parserInC.h logging.h

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

parserInC.c parserInC.h: parserDefinition.y
	$(YACC) -Wcounterexamples -d -o parserInC.c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) parserInC.c parserInC.h lexerInC.c
