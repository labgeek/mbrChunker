
                             mbrChunker v0.3.17
                              by JD Durick 
                           <labgeek@gmail.com>
                          <http://vmforensics.org>

               Licensed under the GNU General Public License v2

General
---------- 
mbrChunker is a *nix-based program that allows you to mount raw disk images (created by dd, dcfldd, dc3dd, ftk imager, etc) by taking the
 raw image, analyzing the master boot record (physical sector 0), and getting specific information that is needed to create a vmdk file.
 Version 0.3.17 has the ability to extract data from the master boot record of any raw image, extracted information such as heads
 cylinders, sectors per track, etc. and create a working vmdk file (monolithic flat disk).  The tool now has the ability to search 
 for hex byte patterns within any type of binary files (ie.  dd images of VMFS partitions). 
 
 Compile:
 1.  Download mbrChunker-0.3.17.tar.gz (tar zxvf mbrChunker-0.3.17.tar.gz)
 2.  cd mbrChunker-0.3.17
 3.  make
 
root@redbox:/data/projects/C/mbrChunker-0.3.17# make
gcc -g -Wall -o mbrChunker mbrChunker.c dd2vmdk.c mbr.c
  
 4.  run mbrChunker:  ./mbrChunker (to see options)
 
 
 Below is your options for using mbrChunker:
 
root@redbox:/data/projects/C/mbrChunker-0.3-release# ./mbrChunker -h
Program: mbrChunker
Author:  JD Durick <jd@vmforensics.org>
Website: http://vmforensics.org

Description:  mbrChunker is a *nix-based program that allows you to mount raw disk images
as well as perform analysis on partition tables within the master boot record.  Furthermore,
mbrChunker can search for hex patterns within any type of binary file
Usage: ./mbrChunker -i <path to dd image> -v <output path of vmdk>...

OPTIONS:
   -i image file name that you will be analyzing
   -v VMDK metadata file that you will be creating for your dd2vmdk conversion
   -a creation of a metadata file to be created (file location required)
   -x search for hex code patterns within your file (not 0x80, just 80)
   -b blocksize you will be using during your hex pattern search [allows for huge files to be searched]
   
Examples:
./mbrChunker -i <raw image path location> -x <hex pattern> -b <blocksize for buffer>
./mbrChunker -i <raw image path location> -v <vmdk output file location>
./mbrChunker -i <raw image path location> -a <output of analysis file>
 
 

Features
-----------
For all *nix users:
    o Converts raw dd images to vmdk files
    o handles large files (over the 2GB limit)
    o Quick and easy to use
    o Metadata file gives you additional information about the image file you are converting/analzying
    o searches for hex strings within a dd image or any binary file
    o matches number of hex strings within large file (<4gb)
    o For VMFS partitions that have been dd'd:  searches for all MBR of all VM's within the dd image
    	 - after it grabs the hex offset for each string, then gets the full 512 master boot record 
    	   and then parses it.  This applies to VMFS partitions so far.
    o With the -x and -i features, you can now find hex byte offsets within your dd image or file.  I 
      was tired of using hexedit(great tool) so I figured I would integrate it into the mbrChunker tool.
      This allows the user to enter the hex pattern via the command line such as hex bytes:  4939614747 
      as well as your image, it will then find the byte location within that image.  I thought it was useful 
      to me.  What I will end up doing is going through a VMFS partition and finding the MBR partitions
      and grabbing all 512 bytes, analyzing them, then printing out those results.  We also have a -b option
      that allows the user to predefine the buffer block size for which you are searching.  This allow you to 
      search files greater than 4GB in size.
    

Limitation(s):
 - The DD to vmdk functionality only handles images that use Partition #1 (not dual booted) or using
 any of the other primary partition tables.
 - lack of error checking throughout
 
Usage
------

./mbrChunker -i <raw image path location> -v <vmdk output file location>
./mbrChunker -i <raw image path location> -a <output of analysis file>
./mbrChunker -x 4939614747 -i /data/images/testsector.img 

TODO
----
 - clean up

EXAMPLE #1
---------
root@redbox:/data/projects/C/mbrChunker/Debug# ./mbrChunker -i /data/images/sixgbimage.img -v /tmp/output.vmdk

Author:   JD Durick <jd@vmforensics.org>
Program:  mbrChunker
Website:  http://vmforensics.org
Version:  0.2
Revision date:  2010-07-05 08:27:47 -0400 (Mon, 05 Jul 2010)

Image Geometry specifications [/data/images/sixgbimage.img]
================================
1.  Image location:  /data/images/sixgbimage.img
1.  VMDK destination:  /tmp/output.vmdk
2.  Number of sectors = 12594960
3.  Number of cylinders:  1023
4.  Number of heads per track:  64
5.  Number of sectors per track: 63
6.  File size in bytes: 6448619520
7.  Size of each sector: 512

EXAMPLE #2
----------
root@redbox:/data/projects/C/mbrChunker/Debug# ./mbrChunker -i /data/images/firstsector.img -a /tmp/output_analysis.txt

Author:   JD Durick <jd@vmforensics.org>
Program:  mbrChunker
Website:  http://vmforensics.org
Version:  0.2
Revision date:  2010-07-05 08:27:47 -0400 (Mon, 05 Jul 2010)
======================================

MBR Analysis of /data/images/firstsector.img
Metadata file location:  /tmp/output_analysis.txt
File size in bytes:         512

PARTITION 1:
============

Partition 1 has an active partition:  0x80
Starting Head:	0x01 (1)
System ID:	0x07
Partition 1 (VALID):  07
Ending head:	0xfe (254)
Sectors between MBR and Partition 1:  63
Total sectors in partition 1:  429690492

PARTITION 2:
============

Partition 2 does not have an active partition:  0x00
Starting Head:	0xfe (254)
System ID:	0x0f
Partition 2 (VALID):  0f
Ending head:	0xfe (254)
Sectors between MBR and Partition 2:  36539
Total sectors in partition 2:  547077510

PARTITION 3:
============

Partition 3 does not have an active partition:  0x00
Starting Head:	0x00 (0)
System ID:	0x00
*** Unused partition table entry ***
Partition 3 (VALID):  00
Ending head:	0x00 (0)
Sectors between MBR and Partition 3:  0
Total sectors in partition 3:  0

PARTITION 4:
============

Partition 4 does not have an active partition:  0x00
Starting Head:	0x00 (0)
System ID:	0x00
*** Unused partition table entry ***
Partition 4 (VALID):  00
Ending head:	0x00 (0)
Sectors between MBR and Partition 4:  0
Total sectors in partition 4:  0

Two byte signature word:  55AA (End of MBR)
Exiting...

EXAMPLE #3
----------
In the following example, from the hex signature offset, I grab the next 5 bytes - however, 
I will enable that later until I get more time to work on it.  For now, you will be just getting the 
byte offset location:

root@redbox:/data/projects/C/mbrChunker-0.3.17# ./mbrChunker -i /data/images/image57.img -x 3961 -b 25

Total size of /data/images/image57.img file:  57 bytes
Parsing Hex string "3961" now.

Results:

Hex string 3961 was found at starting byte location:  2
Hex string 3961 was found at starting byte location:  17
Hex string 3961 was found at starting byte location:  33
Hex string 3961 was found at starting byte location:  52


Quick statistics:
=====================
Number of bytes that have been read:  57
Number of signature matches found:  4
Total number of bytes in hex string:  2
Block size of each buffer:  25
Number of buffers searched:  3

Requirements
------------
GCC:
root@redbox:/data/projects/C/mbrChunker-0.3-release# gcc -v
Using built-in specs.
Target: i486-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 4.3.3-5ubuntu4' --with-bugurl=file:///usr/share/doc/gcc-4.3/README.Bugs --enable-languages=c,c++,fortran,objc,obj-c++ --prefix=/usr --enable-shared --with-system-zlib --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --enable-nls --with-gxx-include-dir=/usr/include/c++/4.3 --program-suffix=-4.3 --enable-clocale=gnu --enable-libstdcxx-debug --enable-objc-gc --enable-mpfr --enable-targets=all --with-tune=generic --enable-checking=release --build=i486-linux-gnu --host=i486-linux-gnu --target=i486-linux-gnu
Thread model: posix
gcc version 4.3.3 (Ubuntu 4.3.3-5ubuntu4) 

               
