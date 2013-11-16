SUBDIRS = @OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD@

include_HEADERS =

EXTRA_DIST = 

swigdir = ${bindir}
swig_DATA = 

swig:
	make -C lua swig
	make -C perl swig
	make -C python swig
	make -C ruby swig
	make -C php swig
	make -C java swig
	make -C guile swig
	make -C tcl swig
	make -C R swig
	make -C go swig

#make -C clisp swig

