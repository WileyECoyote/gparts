AC_INIT([gparts],[0.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])dnl

AC_PROG_CC

AC_PROG_CC_C_O
AM_PROG_LIBTOOL

##  Checking for programs

AC_PATH_PROG([DOXYGEN], 'doxygen')
if test $DOXYGEN; then :; else
    AC_MSG_WARN([Unable to locate doxygen.  Documentation will not be created.])
fi

AC_PATH_PROG([PKG_CONFIG],'pkg-config')
if test $PKG_CONFIG; then :; else
    AC_MSG_ERROR([Unable to locate pkg-config.  Make sure pkg-config is installed and in the path.])
fi

##  gEDA
##
##  Not all versions of libgeda's libgeda.pc contained the varaibles gedadatarc
##  and gedarcdir. For versions that did not define these variables, leave the
##  macro undefined and let the program try and find the right location.
##
GEDA_DATA_DIR=`$PKG_CONFIG libgeda --variable=gedadatadir`
GEDA_RC_DIR=`$PKG_CONFIG libgeda --variable=gedarcdir`
if test "X$GEDA_DATA_DIR" = "X"; then
    AC_MSG_WARN([libgeda data directory (gedadatadir) not available from pkg-config.])
else
    AC_DEFINE_UNQUOTED([GEDA_DATA_DIR], ["$GEDA_DATA_DIR"], [Path to gEDA data directory])
fi
if test "X$GEDA_RC_DIR" = "X"; then
    AC_MSG_WARN([libgeda config directory (gedarcdir) not available from pkg-config.])
else
    AC_DEFINE_UNQUOTED([GEDA_RC_DIR], ["$GEDA_RC_DIR"], [Path to gEDA configuration directory])
fi

##  GIO
##
GIO_VERSION=`$PKG_CONFIG gio-2.0 --modversion`
GIO_CFLAGS=`$PKG_CONFIG gio-2.0 --cflags`
GIO_LIBS=`$PKG_CONFIG gio-2.0 --libs`
AC_SUBST(GIO_CFLAGS)
AC_SUBST(GIO_LIBS)


##  GLIB
##
##  GTK's CFLAGS and LIBS includes GLib's, so including them is redundant.
##
GLIB_VERSION=`$PKG_CONFIG glib-2.0 --modversion`
GLIB_CFLAGS=`$PKG_CONFIG glib-2.0 --cflags`
GLIB_LIBS=`$PKG_CONFIG glib-2.0 --libs`
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

##  GModule
##
##  GModule is needed to dynamically load database interfaces.
##
GMODULE_VERSION=`$PKG_CONFIG gmodule-2.0 --modversion`
GMODULE_CFLAGS=`$PKG_CONFIG gmodule-2.0 --cflags`
GMODULE_LIBS=`$PKG_CONFIG gmodule-2.0 --libs`
AC_SUBST(GMODULE_CFLAGS)
AC_SUBST(GMODULE_LIBS)

##  GTK 3.0
##
#GTK_VERSION=`$PKG_CONFIG gtk+-3.0 --modversion`
#GTK_CFLAGS=`$PKG_CONFIG gtk+-3.0 --cflags`
#GTK_LIBS=`$PKG_CONFIG gtk+-3.0 --libs`

##  GTK 2.0
##
GTK_VERSION=`$PKG_CONFIG gtk+-2.0 --modversion`
GTK_CFLAGS=`$PKG_CONFIG gtk+-2.0 --cflags`
GTK_LIBS=`$PKG_CONFIG gtk+-2.0 --libs`

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

## XML
##
LIBXML_VERSION=`$PKG_CONFIG libxml-2.0 --modversion`
LIBXML_CFLAGS=`$PKG_CONFIG libxml-2.0 --cflags`
LIBXML_LIBS=`$PKG_CONFIG libxml-2.0 --libs`
AC_SUBST(LIBXML_CFLAGS)
AC_SUBST(LIBXML_LIBS)


##  Guile
##
AX_CHECK_GUILE([1.8.0])

AC_PATH_PROG([GUILE_SNARF], [guile-snarf], [notfound])
if test "X$GUILE_SNARF" = "Xnotfound" ; then
	AC_MSG_ERROR([Unable to locate guile-snarf.  Make sure guile is installed and guile-snarf is in the path])
fi

##  MySQL
##
##  Building the interface library for MySQL is optional.
##
AC_ARG_WITH(
    [mysql],
    [AS_HELP_STRING([--with-mysql], [Support the MySQL database])],
    [],
    [with_mysql=check]
    )

if test "X$with_mysql" = "Xno"; then :; else
    AC_PATH_PROG([MYSQL_CONFIG],'mysql_config')
    if test $MYSQL_CONFIG; then
        MYSQL_VERSION=`$MYSQL_CONFIG --version`
        MYSQL_CFLAGS=`$MYSQL_CONFIG --cflags`
        MYSQL_LIBS=`$MYSQL_CONFIG --libs`
        MYSQL_LIB='libgparts-mysql.la'
    else
        if test "X$with_mysql" = "Xcheck"; then :; else
            AC_MSG_FAILURE([Unable to locate mysql_config. Make sure MySQL development libraries are installed and mysql_config is in the path.])
        fi
    fi
fi

AC_SUBST(MYSQL_CFLAGS)
AC_SUBST(MYSQL_LIBS)
AC_SUBST(MYSQL_LIB)

##  PostgreSQL
##
##  Building the interface library for PostgreSQL is optional.
##
AC_ARG_WITH(
    [postgresql],
    [AS_HELP_STRING([--with-postgresql], [support the PostgreSQL database])],
    [],
    [with_postgresql=check]
    )

if test "X$with_postgresql" = "Xno"; then :; else
    AC_PATH_PROG([POSTGRESQL_CONFIG],'pg_config')
    if test $POSTGRESQL_CONFIG; then
        POSTGRESQL_VERSION=`$POSTGRESQL_CONFIG --version`
        POSTGRESQL_CFLAGS=`$POSTGRESQL_CONFIG --cflags`
        POSTGRESQL_LIBS=`$POSTGRESQL_CONFIG --libs`
        POSTGRESQL_LIB='libgparts-postgresql.la'
    else
        if test "X$with_postgresql" = "Xcheck"; then :; else
            AC_MSG_FAILURE([Unable to locate pg_config. Make sure PostgreSQL development libraries are installed and pg_config is in the path.])
        fi
    fi
fi

AC_SUBST(POSTGRESQL_CFLAGS)
AC_SUBST(POSTGRESQL_LIBS)
AC_SUBST(POSTGRESQL_LIB)

##  SQLite
##
##  Building the interface library for SQLite is optional.
##
AC_ARG_WITH(
    [sqlite],
    [AS_HELP_STRING([--with-sqlite], [Support the SQLite database])],
    [],
    [with_sqlite=check]
    )

if test "X$with_sqlite" = "Xno"; then :; else
    SQLITE_VERSION=`$PKG_CONFIG sqlite3 --version`
    SQLITE_CFLAGS=`$PKG_CONFIG sqlite3 --cflags`
    SQLITE_LIBS=`$PKG_CONFIG sqlite3 --libs`
    SQLITE_LIB='libgparts-sqlite.la'
fi

AC_SUBST(SQLITE_CFLAGS)
AC_SUBST(SQLITE_LIBS)
AC_SUBST(SQLITE_LIB)


AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    xml/forms/Makefile
    ])
AC_OUTPUT

AC_MSG_RESULT([
****************** SUMMARY FOR gparts CONFIGURATION ******************

    GIO
        Installed Version        $GIO_VERSION

    GLib
        Installed Version        $GLIB_VERSION

    GModule
        Installed Version        $GMODULE_VERSION

    GTK
        Installed Version        $GTK_VERSION

    LIBXML
        Installed Version        $LIBXML_VERSION

    Guile
        Installed Version        $GUILE_VERSION

    MySQL
        Installed Version        $MYSQL_VERSION

    PostgreSQL
        Installed Version        $POSTGRESQL_VERSION

    SQLite
        Installed Version        $SQLITE_VERSION

])

