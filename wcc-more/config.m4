
PHP_ARG_ENABLE([wccm],
  [whether to enable wccm support],
  [AS_HELP_STRING([--enable-wccm],
    [Enable wccm support])],
  [no])

AS_VAR_IF([PHP_WCCM], [no],, [
  
  AC_DEFINE([HAVE_WCCM], [1],
    [Define to 1 if the PHP extension 'wccm' is available.])

  FLAGS="-fPIC"
  CXXFLAGS="$CXXFLAGS -Wall -O2 --std=c++23 -I./include -I../src"
  PHP_REQUIRE_CXX()
  AC_LANG([C++])

  PHP_NEW_EXTENSION([wccm],
    [php_wccm.cpp],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
