ARCH = $(shell uname -m)

PREFIX = /usr/local

BINFILES = ommo_bridge test_artnetdmx

INSTBIN = $(patsubst %,$(PREFIX)/bin/%,$(BINFILES))

OBJECTS = libhos_midi_ctl.o artnet.o

EXTERNALS = alsa libxml++-2.6 liblo

VPATH = ../src

LDLIBS += -ltascar
LDFLAGS += -L../tascar/build
CXXFLAGS += -I../tascar/src
CPPFLAGS += -I../tascar/src

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
