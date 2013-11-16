check_PROGRAMS = 

EXTRA_DIST = script_runner test_swig_api.php

include_HEADERS = 

TESTS = script_runner

all-local:
	cp ../oescriptimpl.php .
	ln -f -s ../.libs/liboescript_php@OE_SHSUF@ ./oescriptimpl@OE_SHSUF@
	mkdir -p data

clean-local:
	rm -f ./oescriptimpl.php
	rm -f ./oescriptimpl@OE_SHSUF@
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*

