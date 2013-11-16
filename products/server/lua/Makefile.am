
SUBDIRS = . 

include_HEADERS =

luascriptdir = ${datarootdir}/lua/bin
luascript_DATA = oeserver.lua oeserver oeserver.lua_example

install-data-hook:
	chmod +x ${luascriptdir}/${luascript_DATA}
	mkdir -p /var/oescript/data
	sed -e 's#__replace_me_with_data_dir__#/var/oescript/data#' oeserver.lua > ${luascriptdir}/${luascript_DATA}

