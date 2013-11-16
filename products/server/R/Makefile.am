
SUBDIRS = . 

include_HEADERS =

Rscriptdir = ${datarootdir}/R/bin
Rscript_DATA = oeserver.R

install-data-hook:
	chmod +x ${Rscriptdir}/${Rscript_DATA}
	mkdir -p /var/oescript/data
	sed -e 's#__replace_me_with_data_dir__#/var/oescript/data#' ${Rscript_DATA} > ${Rscriptdir}/${Rscript_DATA}

