SUBDIRS = corelib modules products

include_HEADERS = config.h

EXTRA_DIST = README README_lua README README_osx README_NetBSD README_FreeBSD README_OpenBSD README_OpenSolaris

swig:
	make -C modules swig

rmmodules:
	rm -rf /usr/lib/perl5/site_perl/5.12.2/i686-linux/oescript*
	rm -rf /usr/lib/perl5/site_perl/5.12.2/i686-linux/auto/oescript
	rm -rf /opt/onextent/server
	rm -rf /usr/lib/python2.6/site-packages/oescript*
	rm -rf /usr/lib/python2.6/site-packages/_oescriptimpl.so
	rm -rf /usr/lib/R/library/oescript*

