DESIGN_NAME=fpm
ABSTRACTION_LEVEL=RTL

EXECUTABLE=bin/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).x

SYSTEMC=$$SYSTEMC_HOME

INCDIR = -I./include/ -I. -I$(SYSTEMC)/include/
LIBDIR = $(SYSTEMC)/lib-linux64

CXX=g++
CFLAGS= -c
LD=g++

all: $(EXECUTABLE)

$(EXECUTABLE): obj/main_$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).o obj/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL)_testbench.o obj/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).o
	@echo Building $@
	$(CXX) -g $(INCDIR) -o $@ $^ $(LIBDIR)/libsystemc.a -lpthread

obj/main_$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).o: src/main_$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).cc
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $< 

obj/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL)_testbench.o: src/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL)_testbench.cc include/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL)_testbench.hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $< 

obj/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).o: src/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).cc include/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $<


clean:
	@echo Cleaning
	@rm -f obj/*.o *~
	@rm -f $(EXECUTABLE)

.PHONY: all clean
