
SUBDIRS = . 

include_HEADERS =

rubyscriptdir = ${bindir}/ruby
#rubyscript_DATA = oeserver.rb

install-data-hook:
#	chmod +x ${ruby_script_dir}/oeserver.rb

all-local:
	mkdir -p data
	mkdir -p bin/ruby
	mkdir -p scripts

clean-local:
	rm -rf data
	rm -rf bin
	rm -rf scripts

