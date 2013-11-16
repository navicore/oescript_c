check_PROGRAMS = 

EXTRA_DIST = script_runner test_api.py test_swig_apy.py

include_HEADERS = 

TESTS = script_runner

all-local:
	cp ../oescriptimpl.py .
	cp ../../pythonmod/oescript.py .
	ln -f -s ../.libs/liboescript_python.so ./_oescriptimpl.so
	mkdir -p data

clean-local:
	rm -f ./oescriptimpl.py
	rm -f ./oescriptimpl.pyc
	rm -f ./_oescriptimpl.so
	rm -f ./oes*.py
	rm -f ./mysigfile*
	rm -rf ./data
	rm -f ./db_error*
	rm -f ./*.pyc

