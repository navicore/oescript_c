
SUBDIRS = . 

include_HEADERS =

pythonscriptdir = ${datarootdir}/python/bin
pythonscript_DATA = oeserver.py

install-data-hook:
	chmod +x ${pythonscriptdir}/${pythonscript_DATA}
	mkdir -p /var/oescript/data
	sed -e 's#__replace_me_with_data_dir__#/var/oescript/data#' ${pythonscript_DATA} > ${pythonscriptdir}/${pythonscript_DATA}

