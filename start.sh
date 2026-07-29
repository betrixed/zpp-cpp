#!/bin/bash
# Stop php-fpm services using modules to be replaced
# Disable PHP extension modules that may be not working silently. 
IFILE="/etc/php/conf.d/wcc.ini"
OFILE="/etc/php/conf.d/wcc.off"
if [ -f "$OFILE" ]; then
	echo "Turn on extensions"
	sudo mv "$OFILE" "$IFILE"
fi
sudo systemctl restart php-fpm

