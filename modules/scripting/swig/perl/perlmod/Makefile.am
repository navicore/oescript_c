SUBDIRS = .

include_HEADERS = 

EXTRA_DIST = 

all:
	cat oescriptimpl.pod ../perllib/oescriptimpl.pm > lib/oescriptimpl.pm
	perl ./Build.PL 
	./Build build

clean-local:
	cat oescriptimpl.pod ../perllib/oescriptimpl.pm > lib/oescriptimpl.pm
	perl ./Build.PL 
	test ! -e ./Build || ./Build clean
	rm -f ./Build
	rm -f ./lib/*impl*.pm
	rm -rf ./data

check:
	cat oescriptimpl.pod ../perllib/oescriptimpl.pm > lib/oescriptimpl.pm
	mkdir -p data
	perl ./Build.PL
	./Build test

install-data-hook:
if OE_INSTALL_PERL_MODULE
	cat oescriptimpl.pod ../perllib/oescriptimpl.pm > lib/oescriptimpl.pm
	perl ./Build.PL 
	./Build build
	cp ../perllib/oescriptimpl.pm lib/
	./Build install
endif

