#!/usr/bin/env bash

OLD="template"
NEW="$1"
CURR=$PWD
DPATH="${CURR}/template_test/*"
BPATH="${CURR}/$1_test"
TFILE="/tmp/out.tmp.$$"

[ ! -d $BPATH ] && mkdir -p $BPATH || :
for f in $DPATH
do
  if [ -f $f -a -r $f ]; then
    /bin/cp -f "$f" "$BPATH/"
  else
   echo "Error: Cannot read $f"
   exit
  fi
done

for f in ${CURR}/${1}_test/*
do
  if [ -f $f -a -r $f ]; then
    echo "sed-ing $f"
   sed "s/$OLD/$NEW/g" "$f" > "$f.tmp" && mv "$f.tmp" "$f"
  else
   echo "Error: Cannot read $f"
   exit
  fi
done

mv "${BPATH}/template_test.hpp" "${BPATH}/$1_test.hpp"
mv "${BPATH}/template_test.cpp" "${BPATH}/$1_test.cpp"

echo "generated test suite named $1 at $1_test/"
