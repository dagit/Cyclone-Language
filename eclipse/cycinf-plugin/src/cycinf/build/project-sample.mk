# This sample Makefile was written by the Cyclone with Inference Plugin.
# Please adjust the list of sources and make any other customizations you want.

all: # Set the default target in advance

#SOURCES = foo.cyc bar.cyc
include cycinf.mk

#all: baz

baz: $(OBJS)
	$(CYCINF) -o $@ $(OBJS)

clean:
	rm -f baz
