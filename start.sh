#!/bin/bash
# Stop php-fpm services using modules to be replaced
# Disable PHP extension modules that may be not working silently. 
#
. ./pvid.sh
IFILE="/etc/php/8.5/fpm/conf.d/29-wcc.ini"
OFILE="/etc/php/8.5/fpm/conf.d/29-wcc.off"
if [ -f "$OFILE" ]; then
	echo "Turn on extensions"
	sudo mv "$OFILE" "$IFILE"
fi
sudo systemctl restart $FPM

