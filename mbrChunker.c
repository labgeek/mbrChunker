/* SVN FILE: $Id: mbrChunker.c 19 2010-07-30 00:52:44Z jd $*/
/*
 * Project Name : Program to parse the MBR of a dd image.
 * $Author: jd $
 * $Date: 2010-07-29 20:52:44 -0400 (Thu, 29 Jul 2010) $
 * $Revision: 19 $
 * $LastChangedBy: jd $
 * $URL: file:///data/codehole/mbrChunker/trunk/mbrChunker.c $
 */

/*
 ##########################################################################################
 # Created on:  19 July 2010
 # File(s):     mbrChunker.c
 # Version:	    0.3.16
 #
 # @author: JD Durick <labgeek@gmail.com>
 #
 # This program is free software; you can redistribute it and/or
 # modify it under the terms of the GNU General Public License
 # as published by the Free Software Foundation, using version 2
 # of the License.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 # 02110-1301, USA.
 #
 ##########################################################################################
 */

/* Support Large File Use */
#define _LARGEFILE_SOURCE 1
#define _LARGEFILE64_SOURCE 1
#define _FILE_OFFSET_BITS   64
#define REVISIONDATE "2010-07-05 08:27:47 -0400 (Mon, 05 Jul 2010)"
#define _BSD_SOURCE
#define DEBUG

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

const int bootcodesize = 446;
const int mbr = 512;

#include "mbrChunker.h"

int main(int argc, char** argv) {
	char *cmd = argv[0];
	char *image = NULL;
	char *fileoutput = NULL;
	char *vmdkoutput = NULL;
	char *analysis_output = NULL;
	char opt = 0;
	char *hexstring = NULL;

	long long fSize = 0;
	long blocksize = 0;

	FILE *fp;
	long totalsectors = 0;
	int i, bytecount, cylinders, sectorspertrack, numheads, endcylinder;
	unsigned char *buffer;
	int pecounter = 0;

	/*
	 * Series of arrays that hold the
	 * partition entry data for all four
	 * primary partitions
	 */

	int part1[16] = { };
	int part2[16] = { };
	int part3[16] = { };
	int part4[16] = { };

	/*
	 * flag switch for users to pick
	 * what options they want.  Right
	 * now, it is limited to just
	 * source and output flags..
	 */
	while (opt != -1) {
		opt = getopt(argc, argv, "hi:o:a:x:v:b:");
		switch (opt) {
		case '?':
			usage(cmd);
			break;
		case 'i':
			image = optarg;
			break;
		case 'v':
			vmdkoutput = optarg;
			break;
		case 'o':
			fileoutput = optarg;
			break;
		case 'a':
			analysis_output = optarg;
		case 'b':
			blocksize = atoi(optarg); /* block size */
			break;
		case 'x':
			hexstring = optarg;
			break;
		case 'h':
			usage(cmd);
			return 0;
		case -1:
			break;
		default:
			usage(cmd);
			return (1);
		}
	}

	/*
	 * The -s and the -o (source and output)
	 * flags must be set (equal to 1) to proceed
	 * further into this block of code.  Initially,
	 * this tool was just created to make take the
	 * geometry configuration from a dd image to
	 * a flat vmdk file.
	 */

	if ((image) && (vmdkoutput)) { //This is our dd to vmdk code here

		if ((fp = fopen(image, "rb")) == NULL) {
			printf("%s could not be opened - here\n", image);
			exit(0);
		}

		buffer = malloc(512);
		if (buffer == NULL) {
			printf("Error, malloc failed, wtf\n");
			exit(EXIT_FAILURE);
		}

		/*
		 * Total sectors is found by dividing the total
		 * number of bytes in the file by the sector size
		 * which in this case is 512.  It will be interesting
		 * to see what happens when we move to 2k or 4k
		 * sector sizes
		 */
		printf("sizeof(off_t) = %d.\n", sizeof(off_t));

		off_t file_size = getfilesize(fp);

		printf("total size:  %lld\n", file_size);

		totalsectors = file_size / 512;
		(fseek(fp, 0, SEEK_SET));
		bytecount = fread(buffer, 1, 512, fp);

		/*
		 * Partition table:  64 byte entries from byte offsets 446 - 509
		 * Partition 1:  446 - 461
		 * Partition 2:  462 - 477
		 * Partition 3:  478 - 493
		 * Partition 4:  494 - 509
		 *
		 * Disk Signature marker is from byte offset 510 and 511 (55AA for windows)
		 */

		/*
		 * bootcodesize is 0-445 or 446 bytes.  In the situation below
		 * we increment by 16 in order to get each of the 4 primary
		 * partition data entries.
		 */
		for (i = bootcodesize; i < bootcodesize + 16; i++) {
			//	printf("[%d]: %02X\n", i, buffer[i] & 0xff);
			part1[pecounter] = buffer[i] & 0xff;
			part2[pecounter] = buffer[i + 16] & 0xff;
			part3[pecounter] = buffer[i + 32] & 0xff;
			part4[pecounter] = buffer[i + 48] & 0xff;
			pecounter++;
		}

		/*
		 * Gets the total number of cylinders
		 */
		cylinders = getcylinders(part1);
		endcylinder = getendcylinder(part1);

		/*
		 * gets the number of sectors per track
		 */
		sectorspertrack = getsectorspertrack(endcylinder);
		numheads = getheadspertrack(part1);

		printf("\nAuthor:   %s\n", AUTHOR);
		printf("Program:  %s\n", PROGRAM_NAME);
		printf("Website:  %s\n", WEBSITE);
		printf("Version:  %s\n", VERSION);
		printf("Revision date:  %s\n", REVISIONDATE);
		printf("\nImage Geometry specifications [%s]\n", image);
		printf("================================\n");

		printf("1.  Image location:  %s\n", image);
		printf("1.  VMDK destination:  %s\n", vmdkoutput);
		printf("2.  Number of sectors = %ld\n", totalsectors);
		printf("3.  Number of cylinders:  %d\n", cylinders);
		printf("4.  Number of heads per track:  %d\n", numheads);
		printf("5.  Number of sectors per track: %d\n", sectorspertrack);
		printf("6.  File size in bytes: %lld\n", file_size);
		printf("7.  Size of each sector: %d\n", mbr);

		/*
		 * prints out the vmdk file based on the -o flag
		 */
		printvmdkfile(image, vmdkoutput, totalsectors, cylinders, numheads,
				sectorspertrack);

		free(buffer);
		fclose(fp);

	}

	/*
	 *  Code block to handle flag for image file location
	 *  and analysis output file location
	 */

	else if ((image) && (analysis_output)) {

		if ((fp = fopen(image, "rb")) == NULL) {
			printf("%s could not be opened - here\n", image);
			exit(0);
		}

		long long sz = 0;

		off_t fs = getfilesize(fp);
		printf("total size:  %lld\n", fs);
		sz = fs;
		//printf("total size:  %lld\n", sz);

		mbrAnalysis(fp, image, analysis_output, sz);
		printf("Exiting...\n");
		fclose(fp);
		exit(0);
	}

	else if (((image && hexstring) || blocksize)) {

		if ((fp = fopen(image, "rb")) == NULL) {
			printf("%s could not be opened - here\n", image);
			exit(0);
		}

		off_t file_size = getfilesize(fp);

		if (blocksize < 1) {
			blocksize = DEFAULT_BLOCKSIZE;
		}

		if (file_size == 0) {
			fprintf(stderr, "ERROR -> File size is %lld bytes\n", file_size);
			exit(1);
		}
		fSize = file_size;
		printf("\nTotal size of %s file:  %lld bytes\n", image, fSize);
		hxsearch(fp, hexstring, fSize, blocksize);
		exit(0);
	}

	else {
		usage(cmd);
		return 0;
	}
	return (0);

}

/*
 * Input:  file pointer, hex string from the command line,
 * and the file size.
 * Output:  Nothing (stdout)
 */
void hxsearch(FILE *fptr, char *hexstring, long long filesize_bytes,
		long blocksize) {

	char pattern_buffer[HEXSTRING_MAX] = { 0 };
	char chunk[blocksize];
	unsigned int byte;
	int res;
	int z = 0;
	int current_search = 0;
	// char *parsebuffer;
	long long int master_counter = 0;
	int arraysize = 0;
	int len = 0;
	int pos = 0;
	int counter = 0;
	int bytes_read = 0;
	int runs = 0;
	int matches = 0;
	size_t offsetctr = 0;
	char *chunkptr;
	long long sz = 0;
	len = strlen(hexstring);

	arraysize = sizeof(hexstring) / sizeof(char);
	off_t fs = getfilesize(fptr);
	sz = fs;

	printf("Parsing Hex string \"%s\" now.\n\n", hexstring);
	while ((res = sscanf(&hexstring[pos], "%2x", &byte)) == 1) {
		//printf("\nres=%d, byte=%d(%02x)\n", res, byte, byte);
		pattern_buffer[counter] = byte;
		pos += 2;
		counter++;
	}

	printf("Results:\n\n");
	while ((bytes_read = fread(chunk, sizeof(unsigned char), sizeof(chunk),
			fptr)) > 0) {
		chunkptr = chunk;
		for (z = 0; z < bytes_read; z++) {
			if (*chunkptr == pattern_buffer[current_search]) {
				current_search++;
				if (current_search > (counter - 1)) {
					current_search = 0;
					printf(
							"Hex string %s was found at starting byte location:  %lld\n",
							hexstring, (long long int) (offsetctr - 1));
					matches++;
				}
			} else {
				current_search = 0;
			}
			chunkptr++;
			// printf("[%d]: %02X\n", offsetctr, chunk[z] & 0xff);
			offsetctr++;
		}
		master_counter += bytes_read;
		runs++;
		//printf("buffer run = %d\n", runs);

	}
	printf("\n\nQuick statistics:\n");
	printf("=====================\n");
	printf("Number of bytes that have been read:  %lld\n", master_counter);
	printf("Number of signature matches found:  %d\n", matches);
	printf("Total number of bytes in hex string:  %d\n", counter);
	printf("Block size of each buffer:  %ld\n", blocksize);
	printf("Number of buffers searched:  %d\n", runs);

	fclose(fptr);
	exit(0);

}

char *allocateBuffer(FILE *fptr, long long size) {
	char *bfr;
	size_t result;

	printf("size of file in get buffer:  %lld\n", size);

	bfr = (char*) malloc(sizeof(char) * size);
	if (bfr == NULL) {
		printf("Error, malloc failed, wtf\n");
		exit(EXIT_FAILURE);
	}

	fseek(fptr, 0, SEEK_SET);
	result = fread(bfr, sizeof(char), size, fptr);
	printf("result = %lld\n", (long long) result);
	printf("errno = %d.\n", errno);

	if (result != size) {
		printf("File failed to read\n");
		exit(5);
	}

	if (result > 0) {
		printf("Number of bytes read:  %lu\n", (unsigned long) result);
		return (bfr);
	} else {
		printf("Number of bytes read:  %lu\n", (unsigned long) result);
		printf("We should of got a number greater than 0\n");
		exit(3);
	}

	return (bfr);

}

void usage(char *prog_name) {
	fprintf(stderr, "\nProgram: mbrChunker v0.3.17\n");
	fprintf(stderr, "Author:  JD Durick <jd@vmforensics.org>\n");
	fprintf(stderr, "Website: http://vmforensics.org\n\n");
	fprintf(
			stderr,
			"Description:  mbrChunker is a *nix-based program that allows you to mount raw disk images\n"
				"as well as perform analysis on partition tables within the master boot record.  Furthermore,\n"
				"mbrChunker can search for hex patterns within any type of binary file\n");
	fprintf(
			stderr,
			"Usage: %s -i <path to dd image> -v <output path of vmdk>...\n"
				"\nOPTIONS:\n"
				"   -i image file name that you will be analyzing\n"
				"   -v VMDK metadata file that you will be creating for your dd2vmdk conversion\n"
				"   -a creation of a metadata file to be created (file location required)\n"
				"   -x search for hex code patterns within your file (not 0x80, just 80)\n",
			prog_name);
	fprintf(stderr, "Examples:\n"
		"%s -x 493961 -i /data/images/testsector.img\n", prog_name);
	fprintf(stderr,
			"%s -i <raw image path location> -v <vmdk output file location>\n",
			prog_name);
	fprintf(stderr,
			"%s -i <raw image path location> -a <output of analysis file>\n\n",
			prog_name);

}

void end_of_sector_marker(char *mastboot, FILE *output) {
	/*
	 * gets byte offset 510 and 511 which hold
	 the two byte signature.  Without the end-of-sector
	 marker, this sector would not be interpreted as a
	 valid MBR.*/

	fprintf(output, "\nTwo byte signature word:  %02X%02X (End of MBR)\n",
			mastboot[510] & 0xff, mastboot[511] & 0xff);
	return;

}

char* dec2hex(int decimal) {
	static char hx[256];
	sprintf(hx, "%02x", decimal);
	//printf("Hex value of Decimal value %d  is %s\n",decimal,hx);
	return hx;

}

off_t getfilesize(FILE *f) {
	struct stat xstat;
	fstat(fileno(f), &xstat);
	return xstat.st_size;
}

//old code
/*	tptr = strtok(hexstring, "0x");
 for (z = 0; z < arraysize; z++) {

 strcpy(newstring, tptr + (z * 2));
 //	printf("newstring = %s\n", newstring);
 strncpy(hexbuffer, newstring, 2);
 printf("first two of the command line = [%d]:  0x%s\n", z, hexbuffer);
 //strcpy(finalbuff[z], hexbuffer);
 }*/
