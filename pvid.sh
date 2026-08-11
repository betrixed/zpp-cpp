#!/bin/bash
# Find out if PHP binaries are likely to have a version number X.X or are plain
export XDEBUG_MODE=off 
. /etc/os-release
if [[ $ID == "debian" ]]; then
   pvid=`php -r "echo(PHP_MAJOR_VERSION . '.' . PHP_MINOR_VERSION);"`
elif [[ $ID == "arch" || $ID == "endeavouros" || $ID == "cachyos" ]]; then
   pvid=""
#  pvid=`php -r "echo(PHP_MAJOR_VERSION . PHP_MINOR_VERSION);"`
elif [[ $ID == "archarm" ]]; then
   pvid=""
else
   pvid="Unknown case for $ID"	
fi
export FPM="php${pvid}-fpm"
echo "FPM service is $FPM"

# sudo systemctl restart php${pvid}-fpm
