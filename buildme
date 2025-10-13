#!/bin/bash
set -e
sudo make clean
clear

FILES=./stub/*.stub.php
for f in $FILES
do
target="${f/.stub.php/_arginfo.h}"
make $target
done

sudo make install
