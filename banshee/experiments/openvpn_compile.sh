#!/bin/bash

cd $1
autoreconf -i -v &> ../buildlogs/openvpn_$2.aconfout && ./configure --disable-ssl --disable-lzo CC="/moa/sc1/jkodumal/work/banshee/experiments/gcc_subst.py" &> ../buildlogs/openvpn_$2.confout
make CC="/moa/sc1/jkodumal/work/banshee/experiments/gcc_subst.py -P -save-temps -D__extension__= " &> ../buildlogs/openvpn_$2.makeout
