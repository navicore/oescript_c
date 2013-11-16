SUBDIRS = .

include_HEADERS = 

EXTRA_DIST = 

all:
	cp ../pythonlib/oescriptimpl.py .
	mkdir -p build/lib
	cp ../pythonlib/.libs/liboescript_python.so ./build/lib/_oescriptimpl.so

clean-local:
	rm -f ./_oescriptimpl.so
	rm -f ./oescriptimpl.py
	rm -rf ./build

check:

install-data-hook:
if OE_INSTALL_PYTHON_MODULE
	cp ../pythonlib/.libs/liboescript_python.so ./_oescriptimpl.so
	cp ../pythonlib/oescriptimpl.py .
	python setup.py install
endif

