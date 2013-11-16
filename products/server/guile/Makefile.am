
SUBDIRS = . 

include_HEADERS =

guilescriptdir = ${bindir}/guile
#guilescript_DATA = oeserver.guile

install-data-hook:
#	chmod +x ${guile_script_dir}/oeserver.guile

all-local:
	mkdir -p data
	mkdir -p bin/guile
	mkdir -p scripts

clean-local:
	rm -rf data
	rm -rf bin
	rm -rf scripts

