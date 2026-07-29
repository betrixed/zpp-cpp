
PHP_ARG_ENABLE([wcch],d
  [whether to enable wcch support],
  [AS_HELP_STRING([--enable-wcch],
    [Enable wcch support])],
  [no])

AS_VAR_IF([PHP_WCCH], [no],, [
  PHP_ARG_ENABLE([wcch],
  [whether to enable wcch support],
  [AS_HELP_STRING([--enable-wcch],
    [Enable wcch support])],
  [no])

  AC_DEFINE([HAVE_WCCH], [1],
    [Define to 1 if the PHP extension 'wcch' is available.])

  FLAGS="-fPIC"
  CXXFLAGS="$CXXFLAGS -Wall -O2 --std=c++23 -I./include -I../wcc"
  PHP_REQUIRE_CXX()
  AC_LANG([C++])

  PHP_NEW_EXTENSION([wcch],
    [php_wcch.cpp],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
