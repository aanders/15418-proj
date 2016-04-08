# Makefile for 15-418 project
# Team VeggieTales

APPNAME=sortedCollection

SRCDIR=src
OBJDIR=obj

#
# Add source files to compile here
#
SRCS = harness/main.cpp harness/runner.cpp arrays/vector_v1.cpp trees/simpleTree.cpp sortedCollection/sortedCollection.cpp


OBJS = $(SRCS:.cpp=.o)
LIBS =

CXX=g++
CXXFLAGS=-Wall -Wextra -O2 --std=c++11
INCLUDES=-I$(SRCDIR)/
LDFLAGS=-lpthread


all: $(APPNAME)

$(APPNAME): $(addprefix $(OBJDIR)/,$(OBJS))
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)

veryclean: clean
	rm -rf $(APPNAME)

.PHONY: all clean veryclean
