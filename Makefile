# Makefile for 15-418 project
# Team VeggieTales

APPNAME=sortedCollection

SRCDIR=src
OBJDIR=obj

#
# Add source files to compile here
#
SRCS = harness/main.cpp harness/runner.cpp 

#
# Add template file dependencies here
#
TEMPLATES = arrays/vector_v1.tpp trees/simpleTree.tpp \
	    sortedCollection/queue.tpp sortedCollection/sortedCollection.tpp


OBJS = $(SRCS:.cpp=.o)
LIBS =

CXX=g++
CXXFLAGS=-Wall -Wextra -O2 --std=c++11 -g
INCLUDES=-I$(SRCDIR)/
LDFLAGS=-lpthread


all: $(APPNAME)

$(APPNAME): $(addprefix $(OBJDIR)/,$(OBJS))
	$(CXX) -o $@ $^ $(LDFLAGS)

# It's not likely that a cpp file depends on ALL the templates
# but there's no easy way to find out which one
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(addprefix $(SRCDIR)/,$(TEMPLATES))
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -rf $(OBJDIR)

veryclean: clean
	rm -rf $(APPNAME)

.PHONY: all clean veryclean