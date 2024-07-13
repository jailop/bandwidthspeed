                             BANDWIDTH SPEED TESTER
                             ======================

To compile:

    $ make

In one host, run the server:

    $ ./server

In one or more other hosts, run the client:

    $ ./client SERVER_ADDRESS TIME
    
## Cross compilation using Zig

To compile for MacOS ARM64:

    $ make CC="zig cc -target aarch64-macos" \
      CXX="zig c++ -target aarch64-macos" AR="zig ar" \
      RANLIB="zig ranlib" USE_JEMALLOC=no USE_SYSTEMD=no
