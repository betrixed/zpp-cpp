
PHP_ARG_ENABLE([wccr],
  [whether to enable wccr support],
  [AS_HELP_STRING([--enable-wccr],
    [Enable wccr support])],
  [no])

AS_VAR_IF([PHP_WCCR], [no],, [
  
  AC_DEFINE([HAVE_WCCR], [1],
    [Define to 1 if the PHP extension 'wccr' is available.])
  FLAGS="-fPIC"
  CXXFLAGS="$CXXFLAGS -Wall -O2 --std=c++23 -I./include -I../zpp_php"
  PHP_REQUIRE_CXX()
  AC_LANG([C++])

  PHP_NEW_EXTENSION([wccr],
    [php_wccr.cpp],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
