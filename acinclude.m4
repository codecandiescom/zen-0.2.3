# Extra m4 macros.

AC_DEFUN(ZEN_OPTIONAL_CHECK_LIB,
[dnl
dnl Check for a lib like AC_CHECK_LIB, but also add an option to turn it off.
dnl
  AC_ARG_ENABLE($1, 
    [  --$3-$1 $4],
    $3_$1=yes, $3_$1=no)

  if (test "${$3_$1}" = "yes" && test "$3" = "enable") || (test "${$3_$1}" = "no" && test "$3" = "disable")
  then
      AC_CHECK_LIB([$1], [$2], [$5], [$6])
  fi
])

# Special check for libMagick, using its Magick-config program.
dnl ZEN_PATH_MAGICK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for libMagick, and define MAGICK_CFLAGS and MAGICK_LIBS

AC_DEFUN(ZEN_PATH_MAGICK,
[dnl 
dnl Get the cflags and libraries from the ofbis-config script
dnl
AC_ARG_WITH(Magick-prefix,[  --with-Magick-prefix=PFX   Prefix where libMagick is installed (optional)],
            Magick_config_prefix="$withval", Magick_config_prefix="")
AC_ARG_WITH(Magick-exec-prefix,[  --with-Magick-exec-prefix=PFX Exec prefix where libMagick is installed (optional)],
            Magick_config_exec_prefix="$withval", Magick_config_exec_prefix="")

  if test x$Magick_config_exec_prefix != x ; then
     Magick_config_args="$Magick_config_args --exec-prefix=$Magick_config_exec_prefix"
     if test x${MAGICK_CONFIG+set} != xset ; then
        MAGICK_CONFIG=$Magick_config_exec_prefix/bin/Magick-config
     fi
  fi
  if test x$Magick_config_prefix != x ; then
     Magick_config_args="$Magick_config_args --prefix=$Magick_config_prefix"
     if test x${MAGICK_CONFIG+set} != xset ; then
        MAGICK_CONFIG=$Magick_config_prefix/bin/Magick-config
     fi
  fi

  AC_PATH_PROG(MAGICK_CONFIG, Magick-config, no)
  min_Magick_version=ifelse([$1], ,[4.0.0],[$1])
  AC_MSG_CHECKING([for Magick - version >= $min_Magick_version])
  no_Magick=""
  if test "$MAGICK_CONFIG" = "no" ; then
    no_Magick=yes
  else
    MAGICK_CFLAGS=`$MAGICK_CONFIG $Magick_config_args --cppflags`
    MAGICK_LIBS=`$MAGICK_CONFIG $Magick_config_args --libs`
    Magick_config_major_version=`$MAGICK_CONFIG $Magick_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    Magick_config_minor_version=`$MAGICK_CONFIG $Magick_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    Magick_config_micro_version=`$MAGICK_CONFIG $Magick_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LIBS="$LIBS"
    CPPFLAGS="$CPPFLAGS $MAGICK_CFLAGS"
    LIBS="$LIBS $MAGICK_LIBS"
  fi
  if test "x$no_Magick" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$MAGICK_CONFIG" = "no" ; then
       echo "*** The Magick-config script installed by Magick could not be found"
       echo "*** If libMagick was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the MAGICK_CONFIG environment variable to the"
       echo "*** full path to Magick-config."
     else
        :
     fi
     MAGICK_CFLAGS=""
     MAGICK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(MAGICK_CFLAGS)
  AC_SUBST(MAGICK_LIBS)
])


dnl ZEN_CHECK_LIBMAGICK(MINIMUM-VERSION, [ACTION-IF-FOUND, [ACTION-IF-NOT-FOUND]])
dnl Simplifies the check for libMagick.

AC_DEFUN(ZEN_CHECK_LIBMAGICK,
[
AC_ARG_ENABLE(Magick, [  --enable-Magick         Use libMagick for images], , 
	enable_Magick=no)
if test "$1" != "force" && test "$enable_Magick" != "yes"
then
  AC_MSG_CHECKING([if libMagick should be used])
  AC_MSG_RESULT(no)
else
  dnl AC_MSG_CHECKING([for libMagick version >= $2])
  if test x$with_Magick = xyes ; then
  AC_MSG_ERROR([
  *** Directory must be specified for --with-Magick])
  fi
  
  if test x$with_Magick = x ; then 
  dnl Look for separately installed Magick
  
    ZEN_PATH_MAGICK([$2], [ifelse([$3], , :, [$3])],
      [ifelse([$4], ,
       AC_MSG_ERROR([
  *** libMagick $2 or better is required. The latest version of libMagick
  *** is available from http://www.imagemagick.org/.])
       , [$4])],
      Magick)
  
    Magick_cflags=$MAGICK_CFLAGS
    Magick_libs=$MAGICK_LIBS
  else
  dnl Use uninstalled Magick (assume they got the version right)
  
    if test -x $with_Magick/Magick-config ; then 
  
      Magick_release=`$with_Magick/Magick-config --version | sed 's%\\.[[0-9]]*$%%'`
  
  dnl canonicalize relative paths
      case $with_Magick in 
        /*)
          Magick_dir=$with_Magick
          ;;
        *)
          Magick_dir="\$(top_builddir)/$with_Magick"
          ;;
      esac
  
      MAGICK_CFLAGS="-I$Magick_dir"
      MAGICK_LIBS=$Magick_dir/libMagick.la
  
      ifelse([$3], , :, [$3])
    else
      MAGICK_CFLAGS=""
      MAGICK_LIBS=""
      ifelse([$4], ,
        AC_MSG_ERROR([Magick directory ($with_Magick) not present or not configured])
        , [$4])
    fi
  
    AC_SUBST(MAGICK_CFLAGS)
    AC_SUBST(MAGICK_LIBS)
  fi
  
  dnl AC_MSG_RESULT(found)
fi

])
