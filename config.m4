dnl config.m4 for extension wcc

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

PHP_ARG_ENABLE([wcc],
  [whether to enable wcc support],
  [ --enable-wcc], [Enable wcc support.])

dnl Be sure to activate this by ./configure --enable-wcc

if test "$PHP_WCC" != "no"; then

  AC_DEFINE(COMPILE_DL_WCC, 1, [ Have wcc support ])
  
  FLAGS="-fPIC"
  CXXFLAGS="$CXXFLAGS -Wall -O2 --std=c++23 -I./include -I/usr/include/libxml2"
  PHP_REQUIRE_CXX()
  AC_LANG([C++])
  
  PHP_NEW_EXTENSION(wcc, wcc.cpp , $ext_shared, , $FLAGS)
  
fi
