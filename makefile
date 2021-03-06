COMPILER = g++

INCL_DIR =  -I/usr/local/include -I/usr/local/include/gsl/ -I.

HEADERS =
LIBRARY = -L/usr/lib/ -L/usr/local/lib -L. -lgsl -lstdc++ -lgslcblas -lm -mfpmath=sse -msse3 -funroll-loops 
SOURCES =
CONCERTLIBDIR =

MIOBJECTS= MIMain.o ReplicationParser.o TopKHeap.o KDTree.o  MICMath.o MI.o DiGamma.o
OBJS= ReplicationParser.o TopKHeap.o KDTree.o  MICMath.o MI.o DiGamma.o


CFLAGS += -fPIC -I/usr/local/include -I. -O3 
#-I/usr/include

all: Mutual

bb: $(OBJS)
	g++ -O3 -Wall -D_PYMODULE_ -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` MIMain.cc $(OBJS) -o mi.so

clean:
	rm -f *.o Mutual core


Mutual: $(MIOBJECTS) $(SOURCES) $(HEADERS)
	$(COMPILER) -o $@ $(CFLAGS) $(MIOBJECTS)  $(LIBRARY) -lm

%.o:%.cpp
	$(COMPILER) -fPIC $(CFLAGS) $(INCL_DIR) -c $<

%.o:%.cc
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:../common/%.cc
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:../common/%.cpp
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:%.f90
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

