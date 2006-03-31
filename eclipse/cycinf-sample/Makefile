# This sample Makefile was written by the Cyclone with Inference Plugin.
# Please adjust the list of sources and make any other customizations you want.

all: # Set the default target in advance

SOURCES = sample.cyc
include cycinf.mk

all: sample

sample: $(OBJS)
	$(CYCINF) -o $@ $(OBJS)

clean:
	rm -f sample
