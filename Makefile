GCC=g++
CXXFLAGS=`root-config --libs --cflags` -O2 -fPIC -I../  -I./
## to use BareObjects
CXXFLAGS += -L$(PWD)/../NeroProducer/Core/bin -lBare -Wl,-rpath=$(PWD)/../NeroProducer/Core/bin -ggdb
SOFLAGS=-shared

SRCDIR=src
BINDIR=bin
HPPDIR=interface
AUXDIR=aux

SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%.o , $(SRC)  )
HPPLINKDEF=$(patsubst $(SRCDIR)/%.cpp, ../interface/%.hpp , $(SRC)  )

.PHONY: all
all:
	$(info, "--- Full compilation --- ")	
	$(info, "-> if you want just to recompile something use 'make fast' ")	
	$(info, "------------------------ ")	
	$(MAKE) clean
	$(MAKE) libChargedHiggs.so

.PHONY: fast
fast:
	$(MAKE) libChargedHiggs.so

# check if CMSSW is defined
ifndef CMSSW_BASE
$(info No CMSSSW !!!!)
$(info I ll sleep 3s to let you acknowledge it)
$(shell sleep 3s)
else
$(info CMSSW found: $(CMSSW_BASE) )
COMBINELIBFILE = $(wildcard $(CMSSW_BASE)/lib/$(SCRAM_ARCH)/libHiggsAnalysisCombinedLimit.so)
COMBINELIB = HiggsAnalysisCombinedLimit
COMBINELIBDIR = $(CMSSW_BASE)/lib/$(SCRAM_ARCH)/
endif

# check if Combine is present and compiled 
ifeq ("$(COMBINELIBFILE)", "") 
$(info No Combine Package found)
else
$(info Using combine: $(COMBINELIB))
CXXFLAGS += -L$(ROOFITSYS)/lib -lRooFit -lRooFitCore -I$(ROOFITSYS)/include
CXXFLAGS += -D HAVE_COMBINE -L$(COMBINELIBDIR) -l$(COMBINELIB) -Wl,-rpath=$(COMBINELIBDIR)
endif

libChargedHiggs.so: $(OBJ) Dict | $(BINDIR)
	$(GCC) $(CXXFLAGS) $(SOFLAGS) -o $(BINDIR)/$@ $(OBJ) $(BINDIR)/dict.o

$(OBJ) : $(BINDIR)/%.o : $(SRCDIR)/%.cpp interface/%.hpp | $(BINDIR)
	$(GCC) $(CXXFLAGS) -c -o $(BINDIR)/$*.o $<

.PHONY: Dict
Dict: $(BINDIR)/dict.o

$(BINDIR)/dict.o: $(SRC) | $(BINDIR)
	cd $(BINDIR) && rootcint -v4 -f dict.cc -c -I../../ -I../  $(HPPLINKDEF)  ../interface/LinkDef.hpp 
	cd $(BINDIR) && $(GCC) -c -o dict.o $(CXXFLAGS) -I../../ dict.cc

$(BINDIR):
	mkdir -p $(BINDIR)
	mkdir -p $(AUXDIR)

.PHONY: clean
clean:
	-rm $(OBJ)
	-rm $(BINDIR)/dict*
	-rm $(BINDIR)/libChargedHiggs.so
	-rmdir $(BINDIR)
