#!/bin/bash
# Stop php-fpm services using modules to be replaced
# Disable PHP extension modules that may be not working silently. 
IFILE="/etc/php/conf.d/wcc.ini"
OFILE="/etc/php/conf.d/wcc.off"
if [ -f "$IFILE" ]; then
	echo "Turn off extensions"
	sudo mv "$IFILE" "$OFILE"
fi
. ./pvid.sh
sudo systemctl stop $FPM 

