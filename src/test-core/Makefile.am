noinst_LTLIBRARIES = libtest-core.la

SWIG_FILES = \
	unittest-support.i

libtest_core_la_SOURCES = \
	test-stuff.c \
	unittest-support.c

libtest_core_la_LIBADD = \
  ${top_builddir}/src/libqof/qof/libgnc-qof.la \
  ${GLIB_LIBS}

noinst_HEADERS = \
	test-stuff.h \
	unittest-support.h

libtest_core_la_CPPFLAGS = \
  -I${top_srcdir}/src \
  -I${top_srcdir}/src/libqof/qof \
  ${GLIB_CFLAGS}

libtest_core_la_LDFLAGS = \
  -rpath ${exec-prefix}/lib

if BUILDING_FROM_SVN
swig-unittest-support-guile.c: unittest-support.i $(top_srcdir)/src/base-typemaps.i
	$(SWIG) -guile $(SWIG_ARGS) -Linkage module \
	-I${top_srcdir}/src \
	${AM_CPPFLAGS} -o $@ $<

swig-unittest-support-python.c: unittest-support.i $(top_srcdir)/src/base-typemaps.i
	$(SWIG) -python  -Wall -Werror $(SWIG_ARGS) \
	-I${top_srcdir}/src \
	${AM_CPPFLAGS} -o $@ $<

unittest-support.py: swig-unittest-support-python.c ${SWIG_FILES}
endif
noinst_LTLIBRARIES += libtest-core-guile.la
libtest_core_guile_la_SOURCES = swig-unittest-support-guile.c
libtest_core_guile_la_LIBADD = \
  ${GUILE_LIBS} \
  ${GLIB_LIBS} \
  libtest-core.la

libtest_core_guile_la_CFLAGS = \
	${libtest_core_la_CPPFLAGS} \
	${GUILE_CFLAGS}

libtest_core_guile_la_LDFLAGS = \
  -rpath ${exec-prefix}/lib

if WITH_PYTHON

noinst_LTLIBRARIES += _unittest_support.la

_unittest_support_la_SOURCES = \
  swig-unittest-support-python.c
_unittest_support_la_CFLAGS = \
  ${libtest_core_la_CPPFLAGS} \
  ${PYTHON_CPPFLAGS}
_unittest_support_la_LDFLAGS = \
  ${PYTHON_LDFLAGS} \
  -module \
  -rpath ${exec-prefix}/lib

_unittest_support_la_LIBADD = \
  ${PYTHON_LIBS} \
  ${PYTHON_EXTRA_LIBS} \
  libtest-core.la

endif
SCM_FILES = unittest-support.scm

gncmoddir = ${GNC_SHAREDIR}/guile-modules/gnucash
gncmod_DATA = $(SCM_FILES)

.scm-links:
	$(RM) -rf gnucash
	mkdir -p  gnucash
if GNUCASH_SEPARATE_BUILDDIR
	for X in ${SCM_FILES} ; do \
	  $(LN_S) -f ${srcdir}/$$X . ; \
	done
endif
	( cd gnucash; for A in $(SCM_FILES) ; do $(LN_S) -f ../$$A . ; done )
if ! OS_WIN32
# Windows knows no "ln -s" but uses "cp": must copy every time (see bug #566567).
	touch .scm-links
endif

clean-local:
	$(RM) -rf gnucash

noinst_DATA = .scm-links
CLEANFILES = .scm-links

EXTRA_DIST = \
  $(SCM_FILES) \
  swig-unittest-support-python.c \
  swig-unittest-support-guile.c \
  unittest-support.i

MAINTAINERCLEANFILES = \
	swig-unittest-support-guile.c \
	swig-unittest-support-python.c