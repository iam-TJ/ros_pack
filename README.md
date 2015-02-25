# ROS PACK Firmware Archive Payload Manipulator

  * ros_unpack
  * ros_pack (coming soon)

These tools will examine and optionally extract or build the payload of ROS firmware update files
commonly used on switches, routers, and other devices which use VxWorks Realtime Operating System.

Many equipment manufacturers provide firmware updates of the operating system in the form of
.ros files. These files are identified by the string "PACK" at offset 0x18 (24).

These tools will not process the associated .rfb (firmware boot-loader) files.

The payload can contain many individual payload data files. Each data file may be stored
as-is or compressed, and may be another form of archive container (7z, zip, etc.).

Usage:

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

    $ ros_unpack --verbose GS7xxTP-V5.2.0.11.ros

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

    Entry:               4
    Filename:            EWS_FILE
    Length:              0x9eb99 (650137)
    Payload Offset:      0x309906 (3184902)
    Next Offset:         0x3a849f
    Data type:           7z archive

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

    Payload length:      3850753
    Payload extracted:   3850753
    Payload Checksum:    0x1d186dc8
    Calculated Checksum: 0x1d186dc8

