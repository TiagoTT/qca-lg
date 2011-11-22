# 
# Makefile
# 

# Compilation definitions
CC=gcc
CFLAGS=-Wall -ansi -pedantic -g -DDEBUG=0$(DEBUG)
# Do "export DEBUG=1" to activate debug code.
LIBS=-ly -lfl -lm

# Parser configuration
LEX_FILE_LSI=lexico_lsi.l
YACC_FILE_LSI=gramatica_lsi.y
LEX_CFILE_LSI=lex.lsi.c
YACC_CFILE_LSI=lsi.tab.c
YACC_HFILE_LSI=lsi.tab.h
LEX_OFILE_LSI=lex.lsi.o
YACC_OFILE_LSI=lsi.tab.o
YACC_OUTPUT_LSI=lsi.output
YACC_PREFIX_LSI=lsi

LEX_FILE_MVSIS_GATE=lexico_mvsis_gate.l
YACC_FILE_MVSIS_GATE=gramatica_mvsis_gate.y
LEX_CFILE_MVSIS_GATE=lex.mvsis_gate.c
YACC_CFILE_MVSIS_GATE=mvsis_gate.tab.c
YACC_HFILE_MVSIS_GATE=mvsis_gate.tab.h
LEX_OFILE_MVSIS_GATE=lex.mvsis_gate.o
YACC_OFILE_MVSIS_GATE=mvsis_gate.tab.o
YACC_OUTPUT_MVSIS_GATE=mvsis_gate.output
YACC_PREFIX_MVSIS_GATE=mvsis_gate

# Parser definitions
LEX=lex --noyywrap
YACC=yacc -d
LEX_CFILES=$(LEX_CFILE_LSI) $(LEX_CFILE_MVSIS_GATE)
LEX_OFILES=$(LEX_OFILE_LSI) $(LEX_OFILE_MVSIS_GATE)
YACC_CFILES=$(YACC_CFILE_LSI) $(YACC_CFILE_MVSIS_GATE)
YACC_OFILES=$(YACC_OFILE_LSI) $(YACC_OFILE_MVSIS_GATE)
YACC_HFILES=$(YACC_HFILE_LSI) $(YACC_HFILE_MVSIS_GATE)
YACC_OUTPUTS=$(YACC_OUTPUT_LSI) $(YACC_OUTPUT_MVSIS_GATE)

# General code definitions
OBJECTS=celulas.o circuit.o fifo.o hash.o layout.o main.o mesh.o stack.o wire.o yacc_util.o $(YACC_OFILES) $(LEX_OFILES)
HEADERS=celulas.h circuit.h errors.h fifo.h hash.h layout.h mesh.h stack.h wire.h yacc_util.h
PARSER=parser
TARGET=target
TEST_DIR=testes

#
# Default target
#
default: $(TARGET)
	mv $< $(TEST_DIR)

#
# Build the main program.
#
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

#
# LSI parser files.
#
$(LEX_OFILE_LSI): $(LEX_CFILE_LSI) $(YACC_HFILE_LSI) $(HEADERS)
	$(CC) $(CFLAGS) -c $<

$(YACC_OFILE_LSI): $(YACC_CFILE_LSI) $(YACC_HFILE_LSI) $(HEADERS)
	$(CC) $(CFLAGS) -c $<

$(LEX_CFILE_LSI): $(LEX_FILE_LSI) $(YACC_HFILE_LSI)
	$(LEX) -P $(YACC_PREFIX_LSI)_ -o $(LEX_CFILE_LSI) $<

$(YACC_CFILE_LSI) $(YACC_HFILE_LSI): $(YACC_FILE_LSI)
	$(YACC) -b $(YACC_PREFIX_LSI) -p $(YACC_PREFIX_LSI)_ $<

#
# MVSIS GATE parser files.
#
$(LEX_OFILE_MVSIS_GATE): $(LEX_CFILE_MVSIS_GATE) $(YACC_HFILE_MVSIS_GATE) $(HEADERS)
	$(CC) $(CFLAGS) -c $<

$(YACC_OFILE_MVSIS_GATE): $(YACC_CFILE_MVSIS_GATE) $(YACC_HFILE_MVSIS_GATE) $(HEADERS)
	$(CC) $(CFLAGS) -c $<

$(LEX_CFILE_MVSIS_GATE): $(LEX_FILE_MVSIS_GATE) $(YACC_HFILE_MVSIS_GATE)
	$(LEX) -P $(YACC_PREFIX_MVSIS_GATE)_ -o $(LEX_CFILE_MVSIS_GATE) $<

$(YACC_CFILE_MVSIS_GATE) $(YACC_HFILE_MVSIS_GATE): $(YACC_FILE_MVSIS_GATE)
	$(YACC) -b $(YACC_PREFIX_MVSIS_GATE) -p $(YACC_PREFIX_MVSIS_GATE)_ $<

#
# Clean everything.
#
clean: clean_lex clean_yacc clean_parser
	rm -f $(OBJECTS) $(TARGET) $(PARSER) *.o 

clean_lex:
	rm -f $(LEX_CFILES)

clean_yacc:
	rm -f $(YACC_CFILES) $(YACC_HFILES) $(YACC_OUTPUTS)

clean_parser:
	rm -f $(PARSER_MODULES)

#
# EOF
#
