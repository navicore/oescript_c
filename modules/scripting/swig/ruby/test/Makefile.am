check_PROGRAMS = 

EXTRA_DIST = script_runner test_swig_api.rb

include_HEADERS = 

TESTS = script_runner

all-local:
	ln -f -s ../.libs/liboescript_ruby@OE_SHSUF@ ./oescriptimpl@OE_SHSUF@
	cp ../mod/lib/oes*.rb .
	mkdir -p data

clean-local:
	rm -f ./oescriptimpl@OE_SHSUF@
	rm -f ./oes*.rb
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*

