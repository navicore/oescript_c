
SUBDIRS = . 

include_HEADERS =

phpscriptdir = ${bindir}/php
#phpscript_DATA = oeserver.php

install-data-hook:
#	chmod +x ${php_script_dir}/oeserver.php

all-local:
	mkdir -p data
	mkdir -p bin/php
	mkdir -p scripts

clean-local:
	rm -rf data
	rm -rf bin
	rm -rf scripts

