
PHP_ARG_ENABLE([wccd],
  [whether to enable wccd support],
  [AS_HELP_STRING([--enable-wccd],
    [Enable wccd support])],
  [no])

AS_VAR_IF([PHP_WCCD], [no],, [
  
  AC_DEFINE([HAVE_WCCD], [1],
    [Define to 1 if the PHP extension 'wccd' is available.])

  FLAGS="-fPIC"
  CXXFLAGS="$CXXFLAGS -Wall -O2 --std=c++23 -I./include -I../zpp_php"
  PHP_REQUIRE_CXX()
  AC_LANG([C++])

  PHP_NEW_EXTENSION([wccd],
    [php_wccd.cpp],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
