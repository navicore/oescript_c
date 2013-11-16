check_PROGRAMS = 

EXTRA_DIST = script_runner test_api.lua

include_HEADERS = 

TESTS = script_runner

all-local:
	ln -f -s ../.libs/liboescript_lua.so ./oescriptimpl.so
	cp ../../luamod/src/oes*.lua .
	mkdir -p data
	mkdir -p aspace
	mkdir -p somespace
	mkdir -p someotherspace

clean-local:
	rm -f ./oescriptimpl.so
	rm -f ./oes*.lua
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*
	rm -rf ./aspace
	rm -rf ./somespace
	rm -rf ./someotherspace

