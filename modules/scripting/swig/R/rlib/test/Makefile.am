check_PROGRAMS = 

EXTRA_DIST = script_runner

include_HEADERS = 

TESTS = script_runner

all-local:
	cp ../oescriptimpl.R .
	ln -f -s ../.libs/liboescript_R.so ./oescriptimpl.so
	mkdir -p data

clean-local:
	rm -f ./oescriptimpl.R
	rm -f ./oescriptimpl.so
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*

