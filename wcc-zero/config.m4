PHP_ARG_ENABLE([wccz],
  [whether to enable wccz support],
  [AS_HELP_STRING([--enable-wccz],
    [Enable wccz support])],
  [no])

AS_VAR_IF([PHP_WCCZ], [no],, [
  AC_DEFINE([HAVE_WCCZ], [1],
    [Define to 1 if the PHP extension 'wccz' is available.])
    FLAGS="-fPIC"
DNL -Map,output.map -Wno-undef -fexceptions -frtti 

    CXXFLAGS="$CXXFLAGS -Wl, -Wall -O2 --std=c++23 -I./include -I../zpp_php"
    PHP_REQUIRE_CXX()
    AX_CXX_COMPILE_STDCXX_17
    AX_CXX_COMPILE_STDCXX_20
    AX_CXX_COMPILE_STDCXX_23
    AC_LANG([C++])

  PHP_NEW_EXTENSION([wccz],
    [php_wccz.cpp],
    [$ext_shared],,
    [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
])
