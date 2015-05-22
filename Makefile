CXXFLAGS=-std=c++11 -Wall -g
LIBS=-llzma
TITLE=ROS PACK Firmware Archive Toolkit

# C++ LZMA library stream wrapper from Jim Brooks
# http://www.jimbrooks.org/programming/tools/cpp_stream_lzma_xz_compression.php
LZMA_S=lib/stream_lzma
LZMA_S_F=$(LZMA_S)/file.o
LZMA_S_IN=$(LZMA_S)/stream_input.o
LZMA_S_IN_ST=$(LZMA_S)/stream_input_storage_lzma.o
OBJS_UNPACK=$(LZMA_S_F) $(LZMA_S_IN) $(LZMA_S_IN_ST)

all: ros_unpack

stream_input:
	$(MAKE) -C $(LZMA_S) file.o
	$(MAKE) -C $(LZMA_S) stream_input.o
	$(MAKE) -C $(LZMA_S) stream_input_storage_lzma.o

stream_output:
	$(MAKE) -C $(LZMA_S) file.o
	$(MAKE) -C $(LZMA_S) stream_output.o
	$(MAKE) -C $(LZMA_S) stream_output_storage_lzma.o

ros_unpack: ros_unpack.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

ROS_Unpack: ros_unpack.cpp stream_input
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS_UNPACK) $(LIBS)

ROS_Pack: ros_pack.cpp stream_output
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) $(LIBS)

README.html: README.md
	pandoc --standalone --toc --title-prefix="$(TITLE)" --from markdown --to html5 -o $@ $<

html: README.html

clean:
	$(MAKE) -C $(LZMA_S) clean
	rm -f -v ros_unpack ros_pack *.o *.html

.PHONY: all clean

