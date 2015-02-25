SRC_HH = buffer_byte.hh buffer_string.hh file.hh stream_input.hh stream_input_storage.hh stream_input_storage_lzma.hh stream_output.hh stream_output_storage.hh stream_output_storage_lzma.hh
OBJS = file.o stream_input.o stream_input_storage_lzma.o stream_output.o stream_output_storage_lzma.o

#CXX = g++
CXXFLAGS = -std=c++11 -Wall

all: example

file.o: file.cc $(SRC_HH)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

stream_input.o: stream_input.cc $(SRC_HH)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

stream_input_storage_lzma.o: stream_input_storage_lzma.cc $(SRC_HH)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

stream_output.o: stream_output.cc $(SRC_HH)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

stream_output_storage_lzma.o: stream_output_storage_lzma.cc $(SRC_HH)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

example: example.cc $(SRC_HH) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJS) -llzma

clean:
	rm -f -v *.o example test.dat*
