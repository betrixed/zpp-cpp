#!/bin/bash
#USE_ZEND_ALLOC=0
#ZEND_DONT_UNLOAD_MODULES=1
clear
USE_ZEND_ALLOC=0 ZEND_DONT_UNLOAD_MODULES=1 valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all --log-file="dump.txt" php $1 
#ZEND_DONT_UNLOAD_MODULES=1 valgrind -s --log-file="dump.txt" php tests/bug.php
