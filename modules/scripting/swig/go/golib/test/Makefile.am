check_PROGRAMS = 

EXTRA_DIST = script_runner

include_HEADERS = 

TESTS = script_runner

all-local:
	ln -f -s ../.libs/liboescript_go.so ./oescriptimpl_go.so
	mkdir -p data

clean-local:
	rm -f ./oescriptimpl_go.so
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*

