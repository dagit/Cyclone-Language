#!/bin/sh
# usage: test.sh n prog arg [inputfile]
#   where n is the number of times to run each file
#         prog1 is the program to run
#         arg is the argument to run it with
#         inputfile is the optional input file to redirect to stdin

#TIME="/usr/bin/time --format=\"%e\""
TMP=/tmp/test$$
TMP2=/tmp/testinput$$

JAVAOPTS=-server

# Args

if [ $# -lt 4 ]; then
cat<<EOF
usage: $0 n dir prog arg [inputfile]
  where n is the number of times to run each file
        dir is the directory to run in
        prog is the class to run
        arg is the argument to run it with
        inputfile is the optional input file to redirect to stdin
EOF
exit 1

else
  N=$1
  DIR=$2
  CMD=$3
  ARG=$4
  if [ $# -ge 5 ]; then
    if [ "${CMD%regexmatch}" = "${CMD}" ]; then
      INPUTFILE=$5
      ./catn $ARG $INPUTFILE > $TMP2
      INPUTFILE=$TMP2
    else
      cat $5 > $TMP2
      INPUTFILE=$TMP2
    fi
  fi
fi

# Run it

echo -n "java/${CMD} "

i=0
rm -f $TMP
cd $DIR
while [ "$i" != "$N" ]; do
  if [ -n "$INPUTFILE" ]; then
    CLASSPATH=$CLASSPATH:../jakarta-oro-2.0.8.jar $TIME java $JAVAOPTS $CMD $ARG 2>> $TMP 1> /dev/null < $INPUTFILE
  else
    CLASSPATH=$CLASSPATH:../jakarta-oro-2.0.8.jar $TIME java $JAVAOPTS $CMD $ARG 2>> $TMP 1> /dev/null
  fi
  i=`expr $i + 1`
done
cat $TMP | awk '{ printf("%s ",$1); }'
echo
cd -
rm -f $TMP
rm -f $TMP2
