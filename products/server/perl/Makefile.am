
SUBDIRS = . 

include_HEADERS =

perlscriptdir = ${datarootdir}/perl/bin
perlscript_DATA = oeserver.pl

install-data-hook:
	chmod +x ${perlscriptdir}/${perlscript_DATA}
	mkdir -p /var/oescript/data
	sed -e 's#__replace_me_with_data_dir__#/var/oescript/data#' ${perlscript_DATA} > ${perlscriptdir}/${perlscript_DATA}

