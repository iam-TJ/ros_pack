# ROS PACK Firmware Archive Toolkit

  * ros_unpack
  * ros_pack (coming soon)

These tools will examine and optionally extract or build the payload of ROS firmware update files
commonly used on switches, routers, and other devices which use Marvell chipsets and
Marvell Software Solutions Israel (formerly RADLAN) OpENS (Open and portable Embedded
Networking System), and in particualar its Op-ROS (Router Operating System) on VxWorks RTOS.

Many equipment manufacturers provide firmware updates of the operating system in the form of
.ros files. These files are identified by the string "PACK" at offset 0x18 (24).

These tools will not process the associated .rfb (firmware boot-loader) files.

The payload can contain many individual payload data files. Each data file may be stored
as-is or compressed, and may be another form of archive container (7z, zip, etc.).

---

## Roadmap

An overview of what functionality exists and is planned. Help welcome in all areas, especially binary
packaging. Programming is C++ 11 with C style and library interface fall-backs.

  * understand data structures ✔
  * list payload files ✔
  * extract payload files ✔
  * reproduce checksum algorithm ✔
  * collect wide range of example ROS files covering many devices (see known_devices.csv)
  * add unit test suite to ensure continued accuracy
  * uncompress payload file data (LZMA)
  * uncompress and unpack payload archives (7z, zip)
  * create 100% identical compressed payload entry
  * create 100% identical compressed archive entry (7z, zip)
  * add ros_pack archive builder tool
  * add support for i18n and L10n (language and locale)
  * add documentation (HTML)
  * refactor build tooling to use autoconf/automake and friends
  * binary package for Debian/Ubuntu and other compatible Linux distros
  * binary package for RPM and other Redhat/Centos compatible Linux distros (help needed)
  * binary package for OSX (help needed)
  * code refactoring to support Microsoft Windows (help needed)
  * binary installer package for Microsoft Windows (help needed)
  * GUI (cross-platform if possible - maybe simple embedded HTTP server) (help needed)

## Changelog

```
2015-02-26 v0.5 is 100% checksum-accurate, paving way for building ROS PACK archives
2015-02-25 v0.4 able to extract bit-accurate payloads
2015-02-22 v0.1 able to read most of PACK header and Directory entries
```

## Usage

    $ ros_unpack --help
    ROS PACK firmware archive payload extractor
    Version 0.5
    (c) Copyright 2015 TJ <hacker@iam.tj>
    Licensed on the terms of the GNU General Public License version 2

    Usage: ros_unpack [ --verbose --extract --help ] FILENAME
    --verbose: be verbose about progress
    --extract: extract archive contents to current directory
    --help: display this help text
    FILENAME: the ROS PACK archive file to process


Example run using a Netgear GS748TP firmware file:

    $ ros_unpack --verbose --extract GS7xxTP-V5.2.0.11.ros

    ROS PACK firmware archive payload extractor
    Version 0.5
    (c) Copyright 2015 TJ <hacker@iam.tj>
    Licensed on the terms of the GNU General Public License version 2

    Filename:         GS7xxTP-V5.2.0.11.ros
    File length:      3850801 (0x3ac231)
    ARC Magic:        NG01
    ARC Index:        1.01
    Payload length:   3850753 (0x3ac201)
    Header length:    48
    Link Time:        11:01:59
    Link Date:        2014-05-04
    Payload Checksum: 0x1d186dc8 (488140232)
    Signature:        PACK
    Dir Entries:      6

    Entry:               0
    Filename:            DATETIME_C
    Length:              0xa2 (162)
    Payload Offset:      0xf0 (240)
    Next Offset:         0x192
    Extracted DATETIME_C from offset 240 (162 bytes)

    Entry:               1
    Filename:            RSCODE
    Length:              0x2c0668 (2885224)
    Payload Offset:      0x192 (402)
    Next Offset:         0x2c07fa
      Sub-header found
      Magic Index:         2.00
      Uncompressed length: 11458320
      Link Time:           11:01:27
      Link Date:           2014-05-04 (Swapping big-endian year value, unlikely to be 56839)
    Data type:           LZMA compressed
    Extracted RSCODE from offset 434 (2885192 bytes)

    Entry:               2
    Filename:            CLI_FILE
    Length:              0x12b4 (4788)
    Payload Offset:      0x2c07fa (2885626)
    Next Offset:         0x2c1aae
      Sub-header found
      Magic Index:         2.00
      Uncompressed length: 14752
      Link Time:           11:01:56
      Link Date:           2014-05-04 (Swapping big-endian year value, unlikely to be 56839)
    Data type:           LZMA compressed
    Extracted CLI_FILE from offset 2885658 (4756 bytes)

    Entry:               3
    Filename:            DELSCRF
    Length:              0x47e58 (294488)
    Payload Offset:      0x2c1aae (2890414)
    Next Offset:         0x309906
      Sub-header found
      Magic Index:         2.00
      Uncompressed length: 1263279
      Link Time:           11:01:56
      Link Date:           2014-05-04 (Swapping big-endian year value, unlikely to be 56839)
    Data type:           LZMA compressed
    Extracted DELSCRF from offset 2890446 (294456 bytes)

    Entry:               4
    Filename:            EWS_FILE
    Length:              0x9eb99 (650137)
    Payload Offset:      0x309906 (3184902)
    Next Offset:         0x3a849f
    Data type:           7z archive
    Extracted EWS_FILE from offset 3184902 (650137 bytes)

    Entry:               5
    Filename:            UPNP_FILE
    Length:              0x3d92 (15762)
    Payload Offset:      0x3a849f (3835039)
    Next Offset:         0x3ac231
      Sub-header found
      Magic Index:         2.00
      Uncompressed length: 55672
      Link Time:           11:01:59
      Link Date:           2014-05-04 (Swapping big-endian year value, unlikely to be 56839)
    Data type:           LZMA compressed
    Extracted UPNP_FILE from offset 3835071 (15730 bytes)

    Payload length:      3850753
    Payload extracted:   3850753
    Payload Checksum:    0x1d186dc8
    Calculated Checksum: 0x1d186dc8

