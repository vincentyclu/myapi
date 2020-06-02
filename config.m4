dnl config.m4 for extension myapi

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([myapi],
dnl   [for myapi support],
dnl   [AS_HELP_STRING([--with-myapi],
dnl     [Include myapi support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([myapi],
  [whether to enable myapi support],
  [AS_HELP_STRING([--enable-myapi],
    [Enable myapi support])],
  [no])

if test "$PHP_MYAPI" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, MYAPI_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-myapi -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/myapi.h"  # you most likely want to change this
  dnl if test -r $PHP_MYAPI/$SEARCH_FOR; then # path given as parameter
  dnl   MYAPI_DIR=$PHP_MYAPI
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for myapi files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MYAPI_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MYAPI_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the myapi distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-myapi -> add include path
  dnl PHP_ADD_INCLUDE($MYAPI_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-myapi -> check for lib and symbol presence
  dnl LIBNAME=MYAPI # you may want to change this
  dnl LIBSYMBOL=MYAPI # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_MYAPI_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your myapi library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MYAPI_DIR/$PHP_LIBDIR, MYAPI_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MYAPI_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your myapi library.])
  dnl ],[
  dnl   -L$MYAPI_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(MYAPI_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_MYAPI, 1, [ Have myapi support ])

  PHP_REQUIRE_CXX()
  CXXFLAGS="$CXXFLAGS --std=c++17"

  PHP_SUBST(VEHICLE_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, VEHICLE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(myapi, myapi.cpp api.cpp controller.cpp model.cpp myapi_config.cpp MyApiTool.cpp, $ext_shared)
fi

if test -z "$PHP_DEBUG" ; then
    AC_ARG_ENABLE(debug, [--enable-debug compile with debugging system], [PHP_DEBUG=$enableval],[PHP_DEBUG=no] )
fi
