ARCH = $(shell uname -m)

PREFIX = /usr/local

BINFILES = ommo_bridge test_artnetdmx

INSTBIN = $(patsubst %,$(PREFIX)/bin/%,$(BINFILES))

OBJECTS = libhos_midi_ctl.o ommo_dest.o ommo_midi.o ommo_oscdest.o ommo_artnetdmx.o ommo_lsldest.o

EXTERNALS = alsa jack sndfile libxml++-2.6 liblo fftw3f

VPATH = ../src

INCLUDES=-I/usr/include/tascar

LDLIBS += -ltascar -ldl -llsl64
#LDFLAGS += -L../tascar/libtascar/build
CXXFLAGS += $(INCLUDES) -Wall -std=c++11
CPPFLAGS += $(INCLUDES) -Wall

LDLIBS += `pkg-config --libs $(EXTERNALS)`
CXXFLAGS += `pkg-config --cflags $(EXTERNALS)`

all:
	mkdir -p build
	$(MAKE) -C build -f ../Makefile $(BINFILES)
	$(MAKE) doc

include $(wildcard *.mk)

$(PREFIX)/bin/%: %
	cp $< $@

$(BINFILES): $(OBJECTS)

%: %.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

%.o: %.cc
	$(CPP) $(CPPFLAGS) -MM -MF $(@:.o=.mk) $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

install:
	$(MAKE) -C build -f ../Makefile $(INSTBIN)

uninstall:
	rm -f $(INSTBIN)

clean:
	rm -Rf *~ src/*~ build doc/html

.PHONY : doc

doc:
	(cd doc && doxygen doxygen.cfg)
