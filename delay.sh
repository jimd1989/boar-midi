#!/bin/sh

while read x
do
  sleep $1
  echo $x
done
