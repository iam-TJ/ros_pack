C++ I/O stream classes that use LZMA/XZ for compression and decompression.

These classes were designed to support different compression algorithms.
StreamInput and StreamOutput are generic.
Compose a StreamInput/Output with a *Storage[Compression]* object
which has the specific compression algorithm code (LZMA).
(shptr is a smart pointer, replace with your own smart pointers, perhaps boost::intrusive_ptr.)

    shptr<StreamOutputStorageLZMA> streamOutputStorageLZMA = new StreamOutputStorageLZMA( pathname );
    StreamOutput out( streamOutputStorageLZMA.PTR() );
    out << s0 << ' ' << s1 << std::endl;

Developed with xz-5.0.5 on Linux Debian 7 with C++11 (GNU gcc) compiler.

License: GNU GPL 2

Jim Brooks
http://www.jimbrooks.org/programming/
