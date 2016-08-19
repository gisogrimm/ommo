ARCH = $(shell uname -m)

PREFIX = /usr/local

BINFILES = ommo_bridge test_artnetdmx

INSTBIN = $(patsubst %,$(PREFIX)/bin/%,$(BINFILES))

OBJECTS = libhos_midi_ctl.o ommo_dest.o ommo_midi.o ommo_oscdest.o ommo_artnetdmx.o ommo_lsldest.o

EXTERNALS = alsa jack sndfile libxml++-2.6 liblo

VPATH = ../src

LDLIBS += -ltascar -ldl -llsl
LDFLAGS += -L../tascar/build
CXXFLAGS += -I../tascar/src -Wall
CPPFLAGS += -I../tascar/src -Wall

LDLIBS += `pkg-config --libs $(EXTERNALS)`
CXXFLAGS += `pkg-config --cflags $(EXTERNALS)`

all: libtascar
	mkdir -p build
	$(MAKE) -C build -f ../Makefile $(BINFILES)

include $(wildcard *.mk)

$(PREFIX)/bin/%: %
	cp $< $@

$(BINFILES): $(OBJECTS)

%: %.o
	$(CXX) $(CXXFLAGS) $(LDLIBS) $^ -o $@

%.o: %.cc
	$(CPP) $(CPPFLAGS) -MM -MF $(@:.o=.mk) $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

libtascar: tascar
	$(MAKE) -C tascar lib

install:
	$(MAKE) -C build -f ../Makefile $(INSTBIN)

uninstall:
	rm -f $(INSTBIN)

clean:
	$(MAKE) -C tascar clean
	rm -Rf *~ src/*~ build doc/html

tascar:
	git clone https://github.com/gisogrimm/tascar

.PHONY : doc

doc:
	(cd doc && doxygen doxygen.cfg)
