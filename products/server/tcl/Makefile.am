
SUBDIRS = . 

include_HEADERS =

tclscriptdir = ${datarootdir}/tcl/bin
tclscript_DATA = oeserver.tcl

install-data-hook:
	chmod +x ${tclscriptdir}/${tclscript_DATA}
	mkdir -p /var/oescript/data
	sed -e 's#__replace_me_with_data_dir__#/var/oescript/data#' ${tclscript_DATA} > ${tclscriptdir}/${tclscript_DATA}

