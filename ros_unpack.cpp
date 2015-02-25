/* VxWorks ROS Firmware extractor
 * (c) Copyright 2015 TJ <hacker@iam.tj>
 * https://github.com/iam-TJ/ros_pack
 *
 * Extracts payload of firmware updates for many switches, routers and other devices
 * that use VxWorks Realtime Operating System (ROS) firmware update files
 *
 * The header signature of such files is the string "PACK" at offset 0x18 (24). 
 * Licensed on the terms of the GMU General Public License version 2
 * contained in the file COPYRIGHT
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <string.h>
#include <sstream>
#include "ros_pack.hpp"

using namespace std;

const struct _version version = { 0, 5};

struct data_sig data_sigs[] = {
  { { '7', 'z', (char)0xBC, (char)0xAF, (char)0x27, (char)0x1C}, 6, "7z archive"},
  { {(char)0x5D, (char)0x00}, 2, "LZMA compressed"}
};

const unsigned int buffer_size = 1024*1024; // payload data read/write buffer size

// command-line switches
const char *switch_verbose = "--verbose";
const char *switch_extract = "--extract";
const char *switch_uncompress = "--uncompress";
const char *switch_help = "--help";

bool verbose = false;
bool extract = false;
bool uncompress = false;

unsigned int payload_checksum = 0;

unsigned int
checksum_calc(const char *data, unsigned int length)
{
  for (const char *p = data; p && p < data + length; ++p)
    payload_checksum += static_cast<unsigned char>(*p);

  return payload_checksum;
}

void
usage(char *prog_name)
{
  cout << "Usage: " << prog_name << " ["
       << " " << switch_verbose
       << " " << switch_extract
       << " " << switch_uncompress
       << " " << switch_help
       <<  " ] FILENAME" << endl
       << switch_verbose << ": be verbose about progress" << endl
       << switch_extract << ": extract archive contents to current directory" << endl
       << switch_uncompress << ": uncompress payload data files to current directory" << endl
       << switch_help    << ": display this help text" << endl
       << "FILENAME: the ROS PACK archive file to process" << endl;
}


int
main(int argc, char **argv, char **env)
{
  char *target_file = nullptr;
  unsigned long target_length;
  fstream target;
  fstream payload;

  cout << "ROS PACK firmware archive payload extractor" << endl
       << "Version " << version.major << "." << version.minor << endl
       << "(c) Copyright 2015 TJ <hacker@iam.tj>" << endl
       << "Licensed on the terms of the GNU General Public License version 2" << endl << endl;

  if (argc < 2) {
    usage(argv[0]);
    return 1;
  }

  for (unsigned i = 1; i < static_cast<unsigned>(argc); ++i) {
    if (strncmp(argv[i], switch_help, 3) == 0) {
      usage(argv[0]);
      return 0;
    }
    else if (strncmp(argv[i], switch_verbose, 3) == 0) {
      verbose = true;
    }
    else if (strncmp(argv[i], switch_extract, 3) == 0) {
      extract = true;
    }
    else if(strncmp(argv[i], switch_uncompress, 3) == 0) {
      uncompress = true;
      if (!extract) // uncompress infers extract
        extract = true;
    }
    else {
      target_file = argv[i];
      target.open(target_file, ios_base::in);
      if (!target.good()) {
        cerr << "Error opening " << target_file << " for reading" << endl;
        return 2;
      }
      else {
        target.seekg(0, target.end);
        if (!target.good()) {
          cerr << "Error seeking to end of " << target_file << endl;
          target.close();
          return 3;
        }
        target_length = target.tellg();
      }
    }
  }

  if (target_file && target.good()) {
    struct ros_header header;
    target.seekg(0, target.beg);
    target.read(reinterpret_cast<char *>(&header), sizeof(struct ros_header));
    if (!target.good()) {
      cerr << "Error reading header from " << target_file << endl;
      target.close();
      return 4;
    }

    // extract fixed-width char arrays for output via ostream
    string arc_magic(header.arc_magic, sizeof(((ros_header*)0)->arc_magic));
    string arc_index(header.arc_index, sizeof(((ros_header*)0)->arc_index));
    string arc_signature(header.signature, sizeof(((ros_header*)0)->signature));

    // Give a summary from the primary header
    cout << "Filename:         " << target_file << endl
         << "File length:      " << target_length <<  " (" << showbase << hex << target_length << dec << ")" << endl
         << "ARC Magic:        " << arc_magic << endl
         << "ARC Index:        " << arc_index << endl
         << "Payload length:   " << header.payload_length << " (" << showbase << hex << header.payload_length << dec << ")" << endl
         << "Header length:    " << sizeof(struct ros_header) << endl;
    cout.fill('0');
    cout << "Link Time:        " << setw(2) << static_cast<int>(header.link_hour) << ":" << setw(2) << static_cast<int>(header.link_minute) << ":" << setw(2) << static_cast<int>(header.link_second) << endl 
         << "Link Date:        " << setw(4) << static_cast<int>(header.link_year) << "-" << setw(2) << static_cast<int>(header.link_month) << "-" << setw(2) << static_cast<int>(header.link_day) << endl;
    // cout.fill(cout_prev_fill);
    cout << "Payload Checksum: " << showbase << hex << header.payload_checksum << dec << " (" << header.payload_checksum << ")" << endl
         << "Signature:        " << arc_signature << endl 
         << "Dir Entries:      " << dec << header.dir_entries_qty << endl;

    // Iterate over the payload directory entries
    struct ros_dirent *dirents = new struct ros_dirent[header.dir_entries_qty];
    for (unsigned i = 0; i < header.dir_entries_qty; ++i) {
      target.read(reinterpret_cast<char *>(&dirents[i]), sizeof(struct ros_dirent));
      checksum_calc(reinterpret_cast<const char *>(&dirents[i]), sizeof(struct ros_dirent));
    }

    // now read and interpret the payload contents
    char *buffer = new char[buffer_size];
    if (!buffer) {
      cout << "Error allocating memory for read buffer" << endl;
      delete[] dirents;
      target.close();
      return 5;
    }
    unsigned int total_extracted = (header.dir_entries_qty * sizeof(struct ros_dirent));
    for (unsigned i = 0; i < header.dir_entries_qty; ++i) {
      target.seekg(dirents[i].offset, target.beg);
      if (extract) {
        payload.open(dirents[i].filename, ios_base::out);
      }
      // read buffer-sized chunks of payload data
      unsigned int remaining = dirents[i].length;
      while (remaining > 0 && target.good() && payload.good()) {
        unsigned int chunk_length = remaining > buffer_size ? buffer_size : remaining;

        unsigned real_offset = 0; // adjustments for writing payload
        int real_chunk_offset = 0;

        target.read(buffer, chunk_length);
        checksum_calc(reinterpret_cast<const char *>(buffer), chunk_length);
        if (remaining == dirents[i].length) { // first chunk - may need to disgard a header
          struct ros_arc_header *arc_header = reinterpret_cast<ros_arc_header *>(buffer);

          if (verbose) cout << endl 
               << "Entry:               " << i << endl
               << "Filename:            " << dirents[i].filename << endl
               << "Length:              " << showbase << hex << dirents[i].length << " (" << dec << dirents[i].length << ")" << endl
               << "Payload Offset:      " << showbase << hex << dirents[i].offset << " (" << dec << dirents[i].offset << ")" << endl
               << "Next Offset:         " << showbase << hex << dirents[i].offset + dirents[i].length << dec << endl;

          // examine the ARC sub-header
          if (strncmp(arc_header->arc_magic, header.arc_magic, sizeof(((ros_header*)0)->arc_magic)) == 0) {
            // found an ARC sub-header - adjust the offset and length of data to be written
            real_offset = sizeof(struct ros_arc_header);
            real_chunk_offset = -sizeof(struct ros_arc_header);
            dirents[i].offset += real_offset;
            dirents[i].length -= real_offset;
 
            string arc_sub_index(arc_header->arc_index, sizeof(((ros_arc_header*)0)->arc_index));

            if (verbose)
              cout << "  Sub-header found" << endl
                   << "  Magic Index:         " << arc_sub_index << endl
                   << "  Uncompressed length: " << arc_header->uncompressed_length << endl
                   << "  Link Time:           " << setw(2) << static_cast<int>(arc_header->link_hour) << ":" << setw(2) << static_cast<int>(arc_header->link_minute) << ":" << setw(2) << static_cast<int>(arc_header->link_second) << endl; 
              int link_year = static_cast<int>(arc_header->link_year);
              stringstream ss;
              if (link_year > 2100) { // probably need to swap byte order
                ss << " (Swapping big-endian year value, unlikely to be " << link_year << ")";
                link_year = ((link_year & 0xFF) << 8) | ((link_year & 0xFF00) >> 8);
              }

              if (verbose)
                cout << "  Link Date:           " << setw(4) << link_year << "-" << setw(2) << static_cast<int>(arc_header->link_month) << "-" << setw(2) << static_cast<int>(arc_header->link_day) << (verbose && ss.str().length() ? ss.str() : "") << endl;

          }
          
          // try to identify the payload data type
          for (unsigned j = 0; j < sizeof(data_sigs); ++j) {
            if (strncmp(buffer + real_offset, data_sigs[j].magic, sizeof(data_sigs[j].magic)) == 0) {
              if (verbose)
                cout << "Data type:           " << data_sigs[j].title << endl;
              break;
            }
          }

        }
        if (extract)
          payload.write(buffer + real_offset, chunk_length + real_chunk_offset);

        remaining -= chunk_length;
        total_extracted += chunk_length;
      }
      if (extract) {
        payload.close();
        cout << "Extracted " << dirents[i].filename << " from offset " << dirents[i].offset;
        cout << " (" << dec << dirents[i].length << " bytes)" << endl;
      }

    }
    delete[] buffer;
    cout << endl
         << "Payload length:      " << header.payload_length << endl
         << "Payload extracted:   " << total_extracted << endl
         << "Payload Checksum:    " << showbase << hex << header.payload_checksum << endl
         << "Calculated Checksum: " << showbase << hex << payload_checksum << endl << endl;
    target.close();

  }

  return 0;
}

