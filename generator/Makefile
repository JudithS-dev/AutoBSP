CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex
YACC = bison

TARGET = AutoBSP
OBJS = parserInC.o lexerInC.o lexerHelper.o lexerErrorHelper.o ast.o astPrint.o astChecking.o logging.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

logging.o: logging.c logging.h

ast.o: ast.c ast.h logging.h astEnums.h moduleEnums.h astChecking.h

astPrint.o: astPrint.c astPrint.h logging.h astEnums.h moduleEnums.h

astChecking.o: astChecking.c astChecking.h logging.h astEnums.h moduleEnums.h

lexerInC.o: lexerInC.c lexerHelper.h logging.h moduleEnums.h lexerErrorHelper.h  

lexerHelper.o: lexerHelper.c lexerHelper.h logging.h

lexerErrorHelper.o: lexerErrorHelper.c lexerErrorHelper.h logging.h

parserInC.o: parserInC.c parserInC.h logging.h moduleEnums.h ast.h astEnums.h astPrint.h

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

parserInC.c parserInC.h: parserDefinition.y
	$(YACC) -Wcounterexamples -d -o parserInC.c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) parserInC.c parserInC.h lexerInC.c
