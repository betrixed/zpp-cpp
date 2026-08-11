#!/bin/bash
# Stop php-fpm services using modules to be replaced
# Disable PHP extension modules that may be not working silently. 
IFILE="/etc/php/8.5/fpm/conf.d/29-wcc.ini"
OFILE="/etc/php/8.5/fpm/conf.d/29-wcc.off"
if [ -f "$IFILE" ]; then
	echo "Turn off extensions"
	sudo mv "$IFILE" "$OFILE"
fi
. ./pvid.sh
sudo systemctl stop $FPM 

