PHP_ARG_ENABLE([cppzpp],
  [whether to enable cppzpp support],
  [AS_HELP_STRING([--enable-cppzpp],
    [Enable cppzpp support])],
  [no])

AS_VAR_IF([PHP_CPPZPP], [no],, [
  AC_DEFINE([HAVE_CPPZPP], [1],
    [Define to 1 if the PHP extension 'cppzpp' is available.])
    FLAGS="-fPIC"
DNL -Map,output.map -Wno-undef -fexceptions -frtti 

    CXXFLAGS="$CXXFLAGS -Wl, -Wall -O2 --std=c++23 -I./include -I../src"
    PHP_REQUIRE_CXX()
    AX_CXX_COMPILE_STDCXX_17
    AX_CXX_COMPILE_STDCXX_20
    AX_CXX_COMPILE_STDCXX_23
    AC_LANG([C++])

  PHP_NEW_EXTENSION([cppzpp],
    [php_cppzpp.cpp],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
