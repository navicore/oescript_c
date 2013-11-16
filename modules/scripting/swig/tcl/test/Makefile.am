check_PROGRAMS = 

EXTRA_DIST = script_runner test_api.tcl test_swig_api.tcl

include_HEADERS = 

TESTS = script_runner

#	ln -f -s ../.libs/liboescript_tcl@OE_SHSUF@ ./_oescriptimpl.so
all-local:
	cp ../mod/lib/oes*.tcl .
	mkdir -p data

#ln -f -s ../.libs/liboescript_tcl.so ./_oescriptimpl.so

clean-local:
	rm -f ./oes*.tcl
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*

