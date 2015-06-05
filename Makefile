GCC=g++
CXXFLAGS=`root-config --libs --cflags` -O2 -fPIC -I../  -I./
## to use BareObjects
CXXFLAGS += -L$(PWD)/../NeroProducer/Core/bin -lBare -Wl,-rpath=$(PWD)/../NeroProducer/Core/bin -ggdb
SOFLAGS=-shared

SRCDIR=src
BINDIR=bin
HPPDIR=interface

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%.o , $(SRC)  )
HPPLINKDEF=$(patsubst $(SRCDIR)/%.cpp, ../interface/%.hpp , $(SRC)  )



libChargedHiggs.so: $(OBJ) Dict | $(BINDIR)
	$(GCC) $(CXXFLAGS) $(SOFLAGS) -o $(BINDIR)/$@ $(OBJ) $(BINDIR)/dict.o

$(OBJ) : $(BINDIR)/%.o : $(SRCDIR)/%.cpp interface/%.hpp | $(BINDIR)
	$(GCC) $(CXXFLAGS) -c -o $(BINDIR)/$*.o $<

.PHONY: Dict
Dict: $(BINDIR)/dict.o

$(BINDIR)/dict.o: $(SRC) | $(BINDIR)
	cd $(BINDIR) && rootcint -v4 -f dict.cc -c -I../../ -I../ $(HPPLINKDEF)  ../interface/LinkDef.hpp 
	cd $(BINDIR) && $(GCC) -c -o dict.o $(CXXFLAGS) -I../../ dict.cc

$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean
clean:
	-rm $(OBJ)
	-rm $(BINDIR)/dict*
	-rm $(BINDIR)/libChargedHiggs.so
	-rmdir $(BINDIR)
