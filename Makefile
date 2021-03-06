SRCDIR   := src
BUILDDIR := build
INCLUDE  := include
BINDIR   := bin
LDFLAGS  := -lrt -pthread
CXXFLAGS := -pedantic-errors -Wall -Wextra -std=c++11 -I include

all: dirs $(BINDIR)/evaluador

VPATH := src include build

$(BINDIR)/evaluador: evaluador.o controller.o initializer.o registry.o reporter.o stop.o helper.o default.o list.o update.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/evaluador.o: evaluador.cpp controller.h initializer.h registry.h reporter.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/controller.o: controller.cpp controller.h helper.h default.h list.h update.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/initializer.o: initializer.cpp initializer.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/registry.o: registry.cpp registry.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/reporter.o: reporter.cpp reporter.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/list.o: list.cpp list.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/update.o: update.cpp update.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/stop.o: stop.cpp stop.h helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/helper.o: helper.cpp helper.h default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(BUILDDIR)/default.o: default.cpp default.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	rm -f $(SRCDIR)/*.cpp~ $(INCLUDE)/*.h~ Makefile~
	rm -f -R $(BUILDDIR)
	rm -f -R $(BINDIR)

.PHONY: dirs
dirs:
	@test -d build || mkdir build
	@test -d bin || mkdir bin
	@test -d examples || mkdir examples
