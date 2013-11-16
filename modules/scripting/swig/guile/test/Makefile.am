check_PROGRAMS = 

EXTRA_DIST = script_runner test_swig_api.rb

include_HEADERS = 

TESTS = script_runner

all-local:
	ln -f -s ../.libs/liboescript_guile@OE_SHSUF@ ./oescriptimpl.so
	mkdir -p data

clean-local:
	rm -f ./oescriptimpl.so
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*

