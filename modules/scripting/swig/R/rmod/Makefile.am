SUBDIRS = .

include_HEADERS = 

EXTRA_DIST = 

all:
	mkdir -p ./oescript/R
	mkdir -p ./oescript/libs/i386
	cp ./DESCRIPTION ./oescript/
	cp ../rlib/oescriptimpl.R ./oescript/R/
	cp ./oescript.R ./oescript/R/
	cp ../rlib/.libs/liboescript_R.so ./oescript/libs/i386/oescriptimpl.so

clean-local:
	rm -f ./oescript/R/*
	rm -rf ./oescript/libs/*
	rm -f ./oescript/DESCRIPTION

check:

Rscriptdir = ${datarootdir}/R/bin
Rscript_DATA = oescript.R

install-data-hook:

# install hook is probably an R CMD -build or R INSTALL cmd

