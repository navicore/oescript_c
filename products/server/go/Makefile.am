
SUBDIRS = . 

include_HEADERS =

goscriptdir = ${datarootdir}/go/bin
goscript_DATA = oeserver.go

install-data-hook:
	chmod +x ${goscriptdir}/${goscript_DATA}
	mkdir -p /var/oescript/data
	sed -e 's#__replace_me_with_data_dir__#/var/oescript/data#' ${goscript_DATA} > ${goscriptdir}/${goscript_DATA}

