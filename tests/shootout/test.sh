#!/bin/sh
# usage: test.sh n prog [inputfile]
#   where n is the number of times to run each file
#         prog1 is the program to run (including its args)
#         inputfile is the optional input file to redirect to stdin

#TIME="/usr/bin/time --format=\"%e\""
TMP=/tmp/test$$

# Args

if [ $# -lt 2 ]; then
cat<<EOF
usage: $0 n prog [inputfile]
  where n is the number of times to run each file
        prog1 is the program to run (including its args)
        inputfile is the optional input file to redirect to stdin
EOF
exit 1

else
  N=$1
  CMD=$2
  if [ $# -ge 3 ]; then
    INPUTFILE=$3
  fi
fi

# Run it

NAME=`echo $CMD | awk '{ print $1; }'`
NAME=${NAME%.exe}
echo -n "${NAME#./} "

i=0
rm -f $TMP
while [ "$i" != "$N" ]; do
  if [ -n "$INPUTFILE" ]; then
    $TIME $CMD 2>> $TMP 1> /dev/null <$INPUTFILE
  else
    $TIME $CMD 2>> $TMP 1> /dev/null
  fi
  i=`expr $i + 1`
done
cat $TMP | awk '{ printf("%s ",$1); }'
echo
rm -f $TMP

