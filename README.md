OeScript
--------

What and Why

This is an experiment from a few years ago where I wanted to work out some ideas about coordination languages as APIs.  

It is an effort to become a better C programmer and implement an async tuple space.

I also wanted to do real work in anything except Java :) so I use C, autotools, and swig and embed berkeleyDB.  The weak point is BerkelyDB's incompatability with the Apache2 license so I hope to re-implement it someday with only Apache2 compatable licensed parts.


tobuild: run:

    ./autogen.sh [skip if building from a dowloaded platform specific tarball]
    ./configure
    make

to test:

    make check

if you have trouble linking to the external libs (libevent, libyaml,
berkeleydb) see the README_yourdist file and run ./configure --help

to specify exacly where your extern libs and includes are you need to set
environment variables CFLAGS and LDFLAGS.

to specify the names of your includes and libs, use --with-APP-lib= and --with-APP-header.  see README_yourdist for examples.

---------------------------------------------------------------------
the layout of files is:

root (this dir, root autotools stuff like configure.ac)

    core (shared libs to make stuff out of)
        common (some typedefs and malloc utils)
        net
        store
            bdb (berkeleydb)
            more...
        dispatcher (threads and events and timers)
        kernel_api (the space api)
        kernel (the space implementation)

    modules (implementations of net and store core apis)
        net (networking with libevent or internal api or optional helpers)
        protocol (wire formats)
        scripting (lang support)
        store (berkeleydb, others...)
    
    products (stuff people use.  ie: things that get run from a bin dir,
            like a server.  or things that get untar'ed for embedding in
            user code like api toolkits)
        server
        oescript_bin (cmdline stuff)
        oescript_lib (for things like luarock)

each node has a test dir that supports the 'check' target

'make install' will install to /opt/onextent/__________ ...product name

