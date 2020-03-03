COMPILER = g++-9

INCL_DIR =  -I/usr/local/include -I/usr/local/include/gsl/ -I.

HEADERS =
LIBRARY = -L/usr/local/lib/ -L/usr/lib/ -L. -lgsl -lstdc++ -lgslcblas -lm -lgfortran -mfpmath=sse -msse3 -funroll-loops 
SOURCES =
CONCERTLIBDIR =

MIOBJECTS= MIMain.o ReplicationParser.o TopKHeap.o KDTree.o  MICMath.o MI.o DiGamma.o


CFLAGS += -I/usr/local/include -I. 
#-I/usr/include

all: Mutual

clean:
	rm -f *.o Mutual core


Mutual: $(MIOBJECTS) $(SOURCES) $(HEADERS)
	$(COMPILER) -o $@ $(CFLAGS) $(MIOBJECTS)  $(LIBRARY) -lm

%.o:%.cpp
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:%.cc
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:../common/%.cc
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:../common/%.cpp
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

%.o:%.f90
	$(COMPILER) $(CFLAGS) $(INCL_DIR) -c $<

