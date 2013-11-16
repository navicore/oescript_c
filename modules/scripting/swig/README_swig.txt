=== java ===
note, needs a package and some tweaking to conform to normal java
classname=uppercase methondname=lowercase camalback stuff but these
methods will be wrapped in the true api which will leverage java idioms.


=== lua ===
note, lua swig on netbsd is broken.  gets
Program received signal SIGSEGV, Segmentation fault.
0xbba4b948 in pthread_create () from /usr/lib/libpthread.so.0

there are enough netbsd threads and bug reports claiming issues calling
pthread_create in various contexts (from sighandlers, from dynamically
loaded libs) that i've put fixing this on hold for now.

