#!/bin/bash
# USE_ZEND_ALLOC=0 
valgrind --track-origins=yes --leak-check=full --log-file="dump.txt" php $1 
#ZEND_DONT_UNLOAD_MODULES=1 valgrind -s --log-file="dump.txt" php tests/toml.php
