# Makefile.am for src/backend/dbi/test

include $(top_srcdir)/test-templates/Makefile.decl

AM_CFLAGS = \
  -I${top_srcdir}/lib/libc \
  -I${top_srcdir}/src \
  -I${top_srcdir}/src/core-utils \
  -I${top_srcdir}/src/gnc-module \
  -I${top_srcdir}/src/test-core \
  -I${top_srcdir}/src/engine \
  -I${top_srcdir}/src/engine/test-core \
  -I${top_srcdir}/src/libqof/qof \
  -I${top_srcdir}/src/backend/sql \
  -DTEST_MYSQL_URL=\"${TEST_MYSQL_URL}\" \
  -DTEST_PGSQL_URL=\"${TEST_PGSQL_URL}\" \
  ${GLIB_CFLAGS} \
  ${GUILE_INCS} \
  ${GCONF_CFLAGS}

LDADD = ${top_builddir}/src/test-core/libtest-core.la \
        ${top_builddir}/src/gnc-module/libgnc-module.la \
        ${top_builddir}/src/engine/libgncmod-engine.la \
        ${top_builddir}/src/engine/test-core/libgncmod-test-engine.la \
        ${top_builddir}/src/core-utils/libgnc-core-utils.la \
        ${top_builddir}/src/libqof/qof/libgnc-qof.la \
        ../libgncmod-backend-dbi.la \
        ${top_builddir}/src/backend/sql/libgnc-backend-sql.la \
        ${top_builddir}/lib/libc/libc-missing.la

GNC_TEST_DEPS = \
  --gnc-module-dir ${top_builddir}/src/engine \
  --guile-load-dir ${top_builddir}/src/engine \
  --library-dir    ${top_builddir}/src/libqof/qof \
  --library-dir    ${top_builddir}/src/core-utils \
  --library-dir    ${top_builddir}/src/gnc-module \
  --library-dir    ${top_builddir}/src/engine

TESTS_ENVIRONMENT = \
  GNC_ACCOUNT_PATH=${top_srcdir}/accounts/C \
  SRCDIR=${srcdir} \
  ${gnc_dbd_dir_override} \
  $(shell ${top_srcdir}/src/gnc-test-env --no-exports ${GNC_TEST_DEPS})

EXTRA_DIST += \
    test-dbi-stuff.h \
    test-dbi-business-stuff.h

TEST_PROGS += \
    test-backend-dbi

noinst_PROGRAMS = ${TEST_PROGS} ${CHECK_PROGS}


if CUSTOM_GNC_DBD_DIR
gnc_dbd_dir_override = GNC_DBD_DIR="@GNC_DBD_DIR@"
endif

test_backend_dbi_SOURCES = \
    test-backend-dbi.c \
    utest-backend-dbi-basic.c \
    utest-backend-dbi-business.c \
    test-dbi-stuff.c \
    test-dbi-business-stuff.c

INCLUDES = -DG_LOG_DOMAIN=\"gnc.backend.dbi\"

# If you let make run parallel builds it complains that there is no rule to make
# libgncmod-test-dbi.la.  This is wrong, but I can't figure out how to fix it
# so I'll turn off parallel builds
.NOTPARALLEL:
