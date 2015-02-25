CXXFLAGS=-std=c++11 -Wall -g
LIBS=-llzma

# C++ LZMA library stream wrapper from Jim Brooks
# http://www.jimbrooks.org/programming/tools/cpp_stream_lzma_xz_compression.php
LZMA_S=lib/stream_lzma
LZMA_S_F=$(LZMA_S)/file.o
LZMA_S_IN=$(LZMA_S)/stream_input.o
LZMA_S_IN_ST=$(LZMA_S)/stream_input_storage_lzma.o
OBJS_UNPACK=$(LZMA_S_F) $(LZMA_S_IN) $(LZMA_S_IN_ST)

all: ros_unpack ros_pack

stream_input:
	$(MAKE) -C $(LZMA_S) file.o
	$(MAKE) -C $(LZMA_S) stream_input.o
	$(MAKE) -C $(LZMA_S) stream_input_storage_lzma.o

stream_output:
	$(MAKE) -C $(LZMA_S) file.o
	$(MAKE) -C $(LZMA_S) stream_output.o
	$(MAKE) -C $(LZMA_S) stream_output_storage_lzma.o

ros_unpack: ros_unpack.cpp stream_input
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS_UNPACK) $(LIBS)

ros_pack: ros_pack.cpp stream_output
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LIBS)

clean:
	$(MAKE) -C $(LZMA_S) clean
	rm -f -v ros_unpack ros_pack *.o

.PHONY: all clean

