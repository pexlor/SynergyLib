# Compiler
CC = g++

# Compiler flags
CFLAGS  = -g -Wall

# Include paths
INCLUDES = -I./common -I./synergy

# Source files
COMMOM_SOURCES = $(wildcard common/*.cc)
SYNERGY_SOURCES = $(wildcard synergy/*.cc)
TESTCASE_SOURCES = $(wildcard testcase/*.cc)

# Object files
COMMOM_OBJECTS = $(COMMOM_SOURCES:.cc=.o)
SYNERGY_OBJECTS = $(SYNERGY_SOURCES:.cc=.o)

# Executables
TESTCASE_EXECUTABLES = $(TESTCASE_SOURCES:.cc=)

.PHONY: all clean

all: $(TESTCASE_EXECUTABLES)

$(TESTCASE_EXECUTABLES): $(COMMOM_OBJECTS) $(SYNERGY_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) $(COMMOM_OBJECTS) $(SYNERGY_OBJECTS) $(@:.=).cc -o $@ -lpthread

%.o: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(COMMOM_OBJECTS) $(SYNERGY_OBJECTS) $(TESTCASE_EXECUTABLES)