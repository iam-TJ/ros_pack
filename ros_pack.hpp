/* VxWorks ROS Firmware Toolkit
 * (c) Copyright 2015 TJ <hacker@iam.tj>
 * https://github.com/iam-TJ/ros_pack
 *
 * Understands the payload of firmware updates for many switches, routers and other devices
 * that use VxWorks Realtime Operating System (ROS) firmware update files.
 *
 * The header signature of such files is the string "PACK" at offset 0x18 (24).
 *
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

#if !defined __ROS_PACK_HPP__
#define __ROS_PACK_HPP__

struct _version {
  unsigned major;
  unsigned minor;
};


/* The following structures map directly onto the stored file data.
 * All fields are the same size in memory as in storage.
 */

/* Primary header of the entire ROS ARChive PACK file */
struct ros_header { // 32 bytes
  char arc_magic[4];
  char arc_index[4];
  unsigned char link_second;
  unsigned char link_minute;
  unsigned char link_hour;
  unsigned char unknown0;
  unsigned char link_day;
  unsigned char link_month;
  unsigned short link_year;
  unsigned int payload_length; // offset 16
  unsigned int payload_checksum;
  char signature[4];
  unsigned int unknown1;
  unsigned int dir_entries_qty; // offset 32
  unsigned int extended_header_length;
  unsigned int unknown3;
  unsigned int unknown4;
};

/* Directory Entries for payload files */
struct ros_dirent { // 32 bytes
  char filename[16];
  unsigned int offset; // offset 16
  unsigned int length;
  unsigned int unknown1;
  unsigned int unknown2;
};

/* Sub-header at start of payload file data
 * identified by its link_arc magic signature prefix */
struct ros_arc_header {
  char arc_magic[4];
  char arc_index[4];
  unsigned char link_second;
  unsigned char link_minute;
  unsigned char link_hour;
  unsigned char unknown0;
  unsigned char link_day;
  unsigned char link_month;
  unsigned short link_year;
  unsigned int unknown1; // offset 16
  unsigned int uncompressed_length;
  unsigned int unknown2;
  unsigned int unknown3;
};

/* Simple structure to specify commonly found payload data types */
struct data_sig {
  char magic[16];
  unsigned int  magic_length;
  const char *title;
};

#endif

