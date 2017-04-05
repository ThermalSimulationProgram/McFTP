#################   DEFINITIONS    #################

#Main compiler/linker variables
CXX=g++
CFLAGS=-Wall -I./src/
CARG=-lrt
LFLAGS= -lm 
LARG=-ldl -lpthread -lrt -lX11 #-lSM -lICE -lXext -lpng -lz -Wl-rpath #-lGLU -lGL
M_FLAGS=-Wall -pthread -c -g -lX11 #-DVIEWER -lX11 #MJPEG Flags

#Architecture dependent variable
ARCH := $(shell getconf LONG_BIT)

#Directories
BINDIR=bin
LIBDIR=lib
OBJDIR=obj
SRCDIR=src

#Libraries
XML=xml/pugixml.cpp
MATHGL= -rdynamic /usr/local/lib/libmgl.so.7.0.0

#SOURCE FILES
SF3PSRC=mainSF3P.cpp
SIMFIGSRC=mainSimulationFigure.cpp
CALCULATESRC=mainCalculate.cpp
SHOWSRC=mainShow.cpp
PUBLISHSRC=mainPublish.cpp
SIMULATESRC=mainSimulate.cpp

#################    MAIN TARGETS   #################

all: directories libraries SF3P executable tools

SF3P: core criteria dispatchers pthread queues results schedulers servers tasks util exe

#################     DIRECTORIES   #################

directories:
	mkdir -p $(BINDIR) $(OBJDIR) $(OBJDIR)/SF3P $(OBJDIR)/lib

#################        SF3P        #################

### SF3P CORE
CORE:= $(patsubst $(SRCDIR)/core/%.cpp, %, $(wildcard $(SRCDIR)/core/*.cpp))
core: $(CORE)
$(CORE):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/core/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P CRITERIA
CRITERIA:= $(patsubst $(SRCDIR)/criteria/%.cpp, %, $(wildcard $(SRCDIR)/criteria/*.cpp))
criteria: $(CRITERIA)
$(CRITERIA):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/criteria/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)


### SF3P DISPATCHERS
DISPATCHERS:= $(patsubst $(SRCDIR)/dispatchers/%.cpp, %, $(wildcard $(SRCDIR)/dispatchers/*.cpp))
dispatchers: $(DISPATCHERS)
$(DISPATCHERS):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/dispatchers/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P PTHREAD
PTHREAD:= $(patsubst $(SRCDIR)/pthread/%.cpp, %, $(wildcard $(SRCDIR)/pthread/*.cpp))
pthread: $(PTHREAD)
$(PTHREAD):
	$(CXX) $(CFLAGS) -pthread -c $(SRCDIR)/pthread/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P QUEUES
QUEUES:= $(patsubst $(SRCDIR)/queues/%.cpp, %, $(wildcard $(SRCDIR)/queues/*.cpp))
queues: $(QUEUES)
$(QUEUES):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/queues/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P RESULTS
RESULTS:= $(patsubst $(SRCDIR)/results/%.cpp, %, $(wildcard $(SRCDIR)/results/*.cpp))
results: $(RESULTS)
$(RESULTS):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/results/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P SCHEDULERS
SCHEDULERS:= $(patsubst $(SRCDIR)/schedulers/%.cpp, %, $(wildcard $(SRCDIR)/schedulers/*.cpp))
schedulers: $(SCHEDULERS)
$(SCHEDULERS):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/schedulers/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P SERVERS
SERVERS:= $(patsubst $(SRCDIR)/servers/%.cpp, %, $(wildcard $(SRCDIR)/servers/*.cpp))
servers: $(SERVERS)
$(SERVERS):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/servers/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P TASKS
TASKS:= $(patsubst $(SRCDIR)/tasks/%.cpp, %, $(wildcard $(SRCDIR)/tasks/*.cpp))
tasks: $(TASKS)
$(TASKS):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/tasks/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

### SF3P UTIL
UTIL:= $(patsubst $(SRCDIR)/util/%.cpp, %, $(wildcard $(SRCDIR)/util/*.cpp))
util: $(UTIL)
$(UTIL):
	$(CXX) $(CFLAGS) -c $(SRCDIR)/util/$@.cpp -o $(OBJDIR)/SF3P/$@.o $(CARG)

#################  SF3P EXECUTABLE   #################
exe: executable
SF3PLIBOBJS=$(OBJDIR)/lib/mjpeg.o $(OBJDIR)/lib/processframe.o $(OBJDIR)/lib/pugixml.o
executable:  
	$(CXX) $(CFLAGS) -c $(SRCDIR)/$(SF3PSRC) -o $(OBJDIR)/SF3P.o $(CARG)    #compile mainSF3P.cpp
	$(CXX) $(LFLAGS) $(OBJDIR)/SF3P/*.o $(SF3PLIBOBJS) $(OBJDIR)/SF3P.o -o $(BINDIR)/SF3P $(LARG)  #link all object files

################# SIMFIG EXECUTABLE #################
SIMGFIGOBJ:=$(OBJDIR)/SF3P/Trace.o $(OBJDIR)/SF3P/Operators.o $(OBJDIR)/SF3P/TimeUtil.o $(OBJDIR)/SF3P/Enumerations.o $(OBJDIR)/lib/SimulationFigure.o
simfig:
	$(CXX) $(CFLAGS) -c $(LIBDIR)/mathgl/SimulationFigure.cpp -o $(OBJDIR)/lib/SimulationFigure.o $(CARG)
	$(CXX) $(CFLAGS) -c $(SRCDIR)/$(SIMFIGSRC) -o $(OBJDIR)/simfig.o $(CARG)    #compile mainSimulationFigure.cpp
	$(CXX) $(LFLAGS) $(SIMGFIGOBJ) $(OBJDIR)/simfig.o $(MATHGL) -o $(BINDIR)/simfig -lrt

#################   CALCULATE TOOL    #################
calculate:
	$(CXX) $(CFLAGS) -c $(SRCDIR)/$(CALCULATESRC) -o $(OBJDIR)/mainCalculate.o
	$(CXX) $(LFLAGS) $(OBJDIR)/mainCalculate.o -o $(BINDIR)/calculate 

#################    SHOW   TOOL    #################
show:
	$(CXX) $(CFLAGS) -c $(SRCDIR)/$(SHOWSRC) -o $(OBJDIR)/mainShow.o
	$(CXX) $(LFLAGS) $(OBJDIR)/mainShow.o -o $(BINDIR)/show 

#################  PUBLISH  TOOL    #################
publish:
	$(CXX) $(CFLAGS) -c $(SRCDIR)/$(PUBLISHSRC) -o $(OBJDIR)/mainPublish.o
	$(CXX) $(LFLAGS) $(OBJDIR)/mainPublish.o -o $(BINDIR)/publish 

#################   SIMULATE TOOL   #################
simulate:
	$(CXX) $(CFLAGS) -c $(SRCDIR)/$(SIMULATESRC) -o $(OBJDIR)/mainSimulate.o
	$(CXX) $(LFLAGS) $(OBJDIR)/mainSimulate.o -o $(BINDIR)/simulate

#################     LIBRARIES     #################
lib: libraries
libraries: XML MJPEG

### PUGI XML LIB
XML:
	$(CXX) -Wall -c $(LIBDIR)/$(XML) -o $(OBJDIR)/lib/pugixml.o 

### MJPEG LIB
MJPEG_SRC=mjpeg processframe
MJPEG: $(MJPEG_SRC)
$(MJPEG_SRC):
	$(CXX) $(M_FLAGS) $(LIBDIR)/mjpeg/$@.c -o $(OBJDIR)/lib/$@.o

#################  MISCELLANEOUS   #################

tools: simulate calculate show publish simfig 

run:
	sudo $(OBJDIR)/$(EXEC)

clean:
	rm -fr $(BINDIR)/*
	rm -fr $(OBJDIR)/*.o
	rm -fr $(OBJDIR)/SF3P/*.o
	rm -fr $(OBJDIR)/lib/*.o
