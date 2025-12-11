CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex
YACC = bison

TARGET = AutoBSP
OBJS = parserInC.o lexerInC.o lexerHelper.o lexerErrorHelper.o logging.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

logging.o: logging.c logging.h

lexerInC.o: lexerInC.c lexerHelper.h lexerErrorHelper.h moduleEnums.h logging.h

lexerHelper.o: lexerHelper.c lexerHelper.h logging.h

lexerErrorHelper.o: lexerErrorHelper.c lexerErrorHelper.h logging.h

parserInC.o: parserInC.c parserInC.h moduleEnums.h logging.h

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

parserInC.c parserInC.h: parserDefinition.y
	$(YACC) -Wcounterexamples -d -o parserInC.c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) parserInC.c parserInC.h lexerInC.c
