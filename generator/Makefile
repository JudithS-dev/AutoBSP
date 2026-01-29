CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror #-ggdb 
LDFLAGS = -lm
LEX = flex
YACC = bison

TARGET = AutoBSP
OBJS = parserInC.o lexerInC.o lexerHelper.o lexerErrorHelper.o astBuild.o astPrint.o astCheck.o astCheckSTM32F446RE.o astCheckESP32.o astGenerate.o astGenerateSTM32F446RE.o astGenerateESP32.o astEnums2Str.o astHelper.o logging.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

logging.o: logging.c logging.h

astBuild.o: astBuild.c astBuild.h logging.h astEnums.h moduleEnums.h astCheck.h astEnums2Str.h

astPrint.o: astPrint.c astPrint.h logging.h astEnums.h moduleEnums.h astEnums2Str.h

astCheck.o: astCheck.c astCheck.h logging.h astEnums.h moduleEnums.h astCheckSTM32F446RE.h  astCheckESP32.h astEnums2Str.h

astCheckSTM32F446RE.o: astCheckSTM32F446RE.c astCheckSTM32F446RE.h STM32F44RE.h logging.h astEnums.h moduleEnums.h astEnums2Str.h

astCheckESP32.o: astCheckESP32.c astCheckESP32.h ESP32.h logging.h astEnums.h moduleEnums.h astEnums2Str.h

astGenerate.o: astGenerate.c astGenerate.h astGenerateSTM32F446RE.h astGenerateESP32.h astHelper.h logging.h astEnums.h moduleEnums.h

astGenerateSTM32F446RE.o: astGenerateSTM32F446RE.c astGenerateSTM32F446RE.h astHelper.h logging.h astEnums.h moduleEnums.h

astGenerateESP32.o: astGenerateESP32.c astGenerateESP32.h astHelper.h logging.h astEnums.h moduleEnums.h

astEnums2Str.o: astEnums2Str.c astEnums2Str.h astEnums.h moduleEnums.h logging.h

astHelper.o: astHelper.c astHelper.h logging.h astEnums.h moduleEnums.h

lexerInC.o: lexerInC.c logging.h parserInC.h moduleEnums.h  lexerHelper.h lexerErrorHelper.h

lexerHelper.o: lexerHelper.c lexerHelper.h logging.h

lexerErrorHelper.o: lexerErrorHelper.c lexerErrorHelper.h logging.h

parserInC.o: parserInC.c parserInC.h logging.h astEnums.h moduleEnums.h astEnums2Str.h astBuild.h astCheck.h astPrint.h astGenerate.h

lexerInC.c: lexerDefinition.l
	$(LEX) -o $@ $<	

parserInC.c parserInC.h: parserDefinition.y
	$(YACC) -Wcounterexamples -d -o parserInC.c $<

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) parserInC.c parserInC.h lexerInC.c
