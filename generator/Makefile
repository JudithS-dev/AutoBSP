CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex
YACC = bison

TARGET = AutoBSP
OBJS = parserInC.o lexerInC.o lexerHelper.o lexerErrorHelper.o astBuild.o astPrint.o astCheck.o astEnums2Str.o logging.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

logging.o: logging.c logging.h

astBuild.o: astBuild.c astBuild.h logging.h astEnums.h moduleEnums.h astCheck.h

astPrint.o: astPrint.c astPrint.h logging.h astEnums.h moduleEnums.h astEnums2Str.h

astCheck.o: astCheck.c astCheck.h logging.h astEnums.h moduleEnums.h

astEnums2Str.o: astEnums2Str.c astEnums2Str.h astEnums.h moduleEnums.h logging.h

lexerInC.o: lexerInC.c lexerHelper.h logging.h moduleEnums.h lexerErrorHelper.h  

lexerHelper.o: lexerHelper.c lexerHelper.h logging.h

lexerErrorHelper.o: lexerErrorHelper.c lexerErrorHelper.h logging.h

parserInC.o: parserInC.c parserInC.h logging.h moduleEnums.h astBuild.h astEnums.h astPrint.h

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

parserInC.c parserInC.h: parserDefinition.y
	$(YACC) -Wcounterexamples -d -o parserInC.c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) parserInC.c parserInC.h lexerInC.c
