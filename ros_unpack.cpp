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

const struct _version version = { 0, 6};

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
checksum_calc(unsigned int checksum, const char *data, unsigned int length)
{
  for (const unsigned char *p = reinterpret_cast<const unsigned char *>(data); p && p < reinterpret_cast<const unsigned char *>(data) + length; ++p)
    checksum += *p;

  return checksum;
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

  struct ros_header_version version;
  if (target_file && target.good()) {
    target.seekg(0, target.beg);
    target.read(reinterpret_cast<char *>(&version), sizeof(struct ros_header_version));
    if (!target.good()) {
      cerr << "Error reading header version from " << target_file << endl;
      target.close();
      return 4;
    }

    // extract fixed-width char arrays for output via ostream
    string arc_magic(version.arc_magic, sizeof(((ros_header_version*)0)->arc_magic));
    string arc_index(version.arc_index, sizeof(((ros_header_version*)0)->arc_index));
    unsigned int ros_header_version = 0;

    union header_v1_v2 {
       ros_header_v1 v1;
       ros_header_v2 v2;
    } header;

    target.seekg(0, target.beg);
    target.read(reinterpret_cast<char *>(&header), sizeof(union header_v1_v2));

    struct ros_header_timestamp timestamp;
    struct ros_header_directory directory;
    struct ros_header_checksum payload_hdr_checksum;

    switch (arc_index.substr(0,1)[0]) {
        case '1':
            ros_header_version = 1;
            version = header.v1.version;
            timestamp = header.v1.timestamp;
            directory = header.v1.directory;
            payload_hdr_checksum = header.v1.payload_checksum_v1;
            target.seekg(sizeof(struct ros_header_v1), target.beg);
          break;
        case '2':
            ros_header_version = 2;
            version = header.v2.version;
            timestamp = header.v2.timestamp;
            directory = header.v2.directory;
            payload_hdr_checksum = header.v2.payload_checksum_v2;
            target.seekg(sizeof(struct ros_header_v2), target.beg);
          break;
        default:
          cerr << "Error: Unknown header version: " << arc_magic << arc_index << endl;
          target.close();
          return 5;
    }

    string arc_signature(header.v1.signature.signature, sizeof(ros_header_signature));

    // Give a summary from the primary header
    cout << "Filename:          " << target_file << endl
         << "File length:       " << target_length <<  " (" << showbase << hex << target_length << dec << ")" << endl
         << "ARC Magic:         " << arc_magic << endl
         << "ARC Index:         " << arc_index << endl
         << "Header     length: " << (ros_header_version >= 2 ? sizeof(struct ros_header_v2) : sizeof(struct ros_header_v1)) << endl;
    if ( ros_header_version > 1) {
      cout
         << "         checksum: " << header.v2.header_checksum.checksum << " (" << showbase << hex << header.v2.header_checksum.checksum << ")" << endl;
        header.v2.header_checksum.checksum = 0;
        unsigned int checksum_header = 0xFFFFFFFF - checksum_calc(0, reinterpret_cast<const char *>(&header), sizeof(struct ros_header_v2));
        cout
         << "       calculated: " << checksum_header << " (" << showbase << hex << checksum_header << ")" << endl;
    }
    cout << "Payload" << (ros_header_version > 1 ? " (outer)" : "") << endl
         << " Payload length:   " << dec << header.v1.payload_checksum_v1.length << " (" << showbase << hex << header.v1.payload_checksum_v1.length << ")" << endl
         << " Payload Checksum: " << dec << header.v1.payload_checksum_v1.checksum << " (" << showbase << hex << header.v1.payload_checksum_v1.checksum << ")" << endl;
    switch (ros_header_version) {
      case 2:
        cout
         << "Payload (inner)" << endl
         << " Payload length:   " << dec << header.v2.payload_checksum_v2.length << " (" << showbase << hex << header.v2.payload_checksum_v2.length << dec << ")" << endl
         << " Payload Checksum: " << dec << header.v2.payload_checksum_v2.checksum << " (" << showbase << hex << header.v2.payload_checksum_v2.checksum << ")" << endl
         << " Firmware version: " << header.v2.firmware_version << endl;
        break;
    }
    cout.fill('0');
    cout << dec
         << "Link Time:         " << setw(2) << static_cast<int>(timestamp.link_hour) << ":" << setw(2) << static_cast<int>(timestamp.link_minute) << ":" << setw(2) << static_cast<int>(timestamp.link_second) << endl
         << "Link Date:         " << setw(4) << static_cast<int>(timestamp.link_year) << "-" << setw(2) << static_cast<int>(timestamp.link_month) << "-" << setw(2) << static_cast<int>(timestamp.link_day) << endl
    // cout.fill(cout_prev_fill);
         << "Signature:         " << arc_signature << endl
         << "Dir Entries:       " << dec << directory.dir_entries_qty << endl;

    // Iterate over the payload directory entries
    struct ros_dirent *dirents = new struct ros_dirent[directory.dir_entries_qty];
    for (unsigned i = 0; i < directory.dir_entries_qty; ++i) {
      target.read(reinterpret_cast<char *>(&dirents[i]), sizeof(struct ros_dirent));
      payload_checksum = checksum_calc(payload_checksum, reinterpret_cast<const char *>(&dirents[i]), sizeof(struct ros_dirent));
    }

    // now read and interpret the payload contents
    char *buffer = new char[buffer_size];
    if (!buffer) {
      cout << "Error allocating memory for read buffer" << endl;
      delete[] dirents;
      target.close();
      return 5;
    }
    unsigned int total_extracted = (directory.dir_entries_qty * sizeof(struct ros_dirent));
    for (unsigned i = 0; i < directory.dir_entries_qty; ++i) {
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
        payload_checksum = checksum_calc(payload_checksum, reinterpret_cast<const char *>(buffer), chunk_length);
        if (remaining == dirents[i].length) { // first chunk - may need to disregard a header
          struct ros_arc_header *arc_header = reinterpret_cast<ros_arc_header *>(buffer);

          if (verbose) cout << endl
               << "Entry:               " << i << endl
               << "Filename:            " << dirents[i].filename << endl
               << "Length:              " << showbase << hex << dirents[i].length << " (" << dec << dirents[i].length << ")" << endl
               << "Payload Offset:      " << showbase << hex << dirents[i].offset << " (" << dec << dirents[i].offset << ")" << endl
               << "Next Offset:         " << showbase << hex << dirents[i].offset + dirents[i].length << dec << endl;

          // examine the ARC sub-header
          if (strncmp(arc_header->version.arc_magic, version.arc_magic, sizeof ros_header_version::arc_magic) == 0) {
            // found an ARC sub-header - adjust the offset and length of data to be written
            real_offset = sizeof(struct ros_arc_header);
            real_chunk_offset = -sizeof(struct ros_arc_header);
            dirents[i].offset += real_offset;
            dirents[i].length -= real_offset;

            string arc_sub_index(arc_header->version.arc_index, sizeof ros_arc_header::version.arc_index);

            if (verbose)
              cout << "  Sub-header found" << endl
                   << "  Magic Index:         " << arc_sub_index << endl
                   << "  Uncompressed length: " << arc_header->uncompressed_length << endl
                   << "  Link Time:           " << setw(2) << static_cast<int>(arc_header->timestamp.link_hour) << ":" << setw(2) << static_cast<int>(arc_header->timestamp.link_minute) << ":" << setw(2) << static_cast<int>(arc_header->timestamp.link_second) << endl;
              int link_year = static_cast<int>(arc_header->timestamp.link_year);
              stringstream ss;
              if (link_year > 2100) { // probably need to swap byte order
                ss << " (Swapping big-endian year value, unlikely to be " << link_year << ")";
                link_year = ((link_year & 0xFF) << 8) | ((link_year & 0xFF00) >> 8);
              }

              if (verbose)
                cout << "  Link Date:           " << setw(4) << link_year << "-" << setw(2) << static_cast<int>(arc_header->timestamp.link_month) << "-" << setw(2) << static_cast<int>(arc_header->timestamp.link_day) << (verbose && ss.str().length() ? ss.str() : "") << endl;

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
         << "Payload length:      " << dec << payload_hdr_checksum.length << " (" << showbase << hex << payload_hdr_checksum.length << ")" << endl
         << "Payload extracted:   " << dec << total_extracted << " (" << showbase << hex << total_extracted << ")" << endl
         << "Payload Checksum:    " << dec << payload_hdr_checksum.checksum << " (" << showbase << hex << payload_hdr_checksum.checksum << ")" << endl
         << "Calculated Checksum: " << dec << payload_checksum << " (" << showbase << hex << payload_checksum << ")" << endl
         << endl;
    target.close();

  }

  return 0;
}

