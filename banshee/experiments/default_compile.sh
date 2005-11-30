#!/bin/bash
cd $1
aclocal
autoconf
automake
./configure CC="/moa/sc1/jkodumal/work/banshee/experiments/gcc_subst.py" &>/dev/null
make clean all CC="/moa/sc1/jkodumal/work/banshee/experiments/gcc_subst.py -P -save-temps" &>/dev/null
rm -rf config
rm -rf experiments
rm -rf tests
rm -rf examples
