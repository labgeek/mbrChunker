/* SVN FILE: $Id: mbr.c 5 2010-07-14 02:26:52Z jd $*/
/*
 * Source file : Program to parse the MBR of a dd image.
 * $Author: jd $
 * $Date: 2010-07-13 22:26:52 -0400 (Tue, 13 Jul 2010) $
 * $Revision: 5 $
 * $LastChangedBy: jd $
 * $URL: file:///data/codehole/mbrChunker/trunk/mbr.c $
 */

#include "mbrChunker.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define REVISIONDATE "2010-07-05 08:27:47 -0400 (Mon, 05 Jul 2010)"

/*
 * listing of the partition types similar to when you toggle while using
 * fdisk.  Pulled this from Tim Vidas and Brian Kaplan's Liveview java source code.
 */

const int validPartitionTypes[] = { 0x00, 0x10, 0x02, 0x03, 0x04, 0x05, 0x06,
		0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x14,
		0x16, 0x17, 0x18, 0x1b, 0x1c, 0x1e, 0x24, 0x39, 0x3c, 0x40, 0x41, 0x42,
		0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x5c, 0x61,
		0x63, 0x64, 0x65, 0x70, 0x75, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86,
		0x87, 0x88, 0x8e, 0x93, 0x94, 0x9f, 0xa0, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
		0xab, 0xb7, 0xb8, 0xbb, 0xbe, 0xbf, 0xc1, 0xc4, 0xc6, 0xc7, 0xda, 0xdb,
		0xde, 0xdf, 0xe1, 0xe3, 0xe4, 0xeb, 0xee, 0xef, 0xf0, 0xf1, 0xf4, 0xf2,
		0xfd, 0xfe, 0xff };
//const int startingByte = 0;
//const int partitionentrysize = 16;
//const int bootcodesize = 446;
/*
 * Input:  File pointer and source file pointer
 * Output:  boolean success or failure
 * Desc:   Attempts to open the file and read it,
 *         passes, back file pointer.
 */

void mbrAnalysis(FILE *f, char *sourcefile, char *output, long long size) {

	char *masterbootrecord;
	FILE *mdata_output_file;
	mdata_output_file = fopen(output, "w");

		printf("\nAuthor:   %s\n", AUTHOR);
		printf("Program:  %s\n", PROGRAM_NAME);
		printf("Website:  %s\n", WEBSITE);
		printf("Version:  %s\n", VERSION);
		printf("Revision date:  %s\n", REVISIONDATE);
		printf("\nMBR Analysis of %s\n", sourcefile);
		printf("Metadata file location:  %6s\n", output);
		printf("File size in bytes:  %lld\n", size);

		fprintf(mdata_output_file, "\nAuthor:   %s\n", AUTHOR);
		fprintf(mdata_output_file, "Program:  %s\n", PROGRAM_NAME);
		fprintf(mdata_output_file, "Website:  %s\n", WEBSITE);
		fprintf(mdata_output_file, "Version:  %s\n", VERSION);
		fprintf(mdata_output_file, "Revision date:  %s\n", REVISIONDATE);
		fprintf(mdata_output_file, "======================================\n");

		fprintf(mdata_output_file, "\nMBR Analysis of %s\n", sourcefile);
		fprintf(mdata_output_file, "Metadata file location:  %6s\n", output);
		fprintf(mdata_output_file, "File size in bytes:  %lld\n", size);

		/*
		 * Grabs the master boot record.  We pass
		 * the File pointer in first.
		 */
		masterbootrecord = getMBR(f);
		parsePartitionEntries(masterbootrecord, mdata_output_file);
		fclose(mdata_output_file);
		return;



}


/*
 * Gets the master boot record (in bytes)
 * input:  FILE pointer to opened file
 * output:  pointer to master boot record
 */

char *getMBR(FILE *fptr) {
	char *bfr;
	int bytes;

	bfr = malloc(512);
	if (bfr == NULL) {
		printf("Error, malloc failed, wtf\n");
		exit(EXIT_FAILURE);
	}

	(fseek(fptr, 0, SEEK_SET));
	bytes = fread(bfr, 1, 512, fptr);
	return (bfr);

}

void parsePartitionEntries(char *masterbootrec, FILE *outputptr) {

	int partitionentry1[16] = { };
	int partitionentry2[16] = { };
	int partitionentry3[16] = { };
	int partitionentry4[16] = { };
	int ptnum1 = 1;
	int ptnum2 = 2;
	int ptnum3 = 3;
	int ptnum4 = 4;

	int rtn = 0;
	char *hexvalue_p1;
	int i = 0;
	int counter = -1;

	for (i = 446; i < 446 + 16; i++) {
		counter++;
		hexvalue_p1 = dec2hex(masterbootrec[i] & 0xff);
		//printf("partition 1:  %d --> %02d ---> hex[%s]\n", counter,
		//		masterbootrec[i] & 0xff, hexvalue_p1);
		partitionentry1[counter] = masterbootrec[i] & 0xff;
		partitionentry2[counter] = masterbootrec[i + 16] & 0xff;
		partitionentry3[counter] = masterbootrec[i + 32] & 0xff;
		partitionentry4[counter] = masterbootrec[i + 48] & 0xff;

	}

	rtn = analyzePartitions(partitionentry1, ptnum1, outputptr);
	rtn = analyzePartitions(partitionentry2, ptnum2, outputptr);
	rtn = analyzePartitions(partitionentry3, ptnum3, outputptr);
	rtn = analyzePartitions(partitionentry4, ptnum4, outputptr);
	end_of_sector_marker(masterbootrec, outputptr);

	return;

}

int analyzePartitions(int p1[], int pnumber, FILE *oPtr) {

	fprintf(oPtr, "\nPARTITION %d:\n", pnumber);
	fprintf(oPtr, "============\n");
	int i = 0;
	int sectors_mbr_partition = 0;
	int isValid = 0;
	char *hexvalue = NULL;
	char *starthead, *systemid, *endhead;
	long total_sectors = 0;

	for (i = 0; i < 16; i++) {
		//printf("[%d] --> %d\n", i, p1[i]);
	}

	hexvalue = dec2hex(p1[0] & 0xff);
	if (p1[0] == 128) {
		fprintf(oPtr, "\nPartition %d has an active partition:  0x%s\n",
				pnumber, hexvalue);

	} else {
		fprintf(oPtr,
				"\nPartition %d does not have an active partition:  0x%s\n",
				pnumber, hexvalue);
	}

	/*
	 * get your starting head location
	 */
	starthead = dec2hex(p1[1]);
	fprintf(oPtr, "Starting Head:\t0x%s (%d)\n", starthead, p1[1]);

	/*
	 * get your system ID
	 * compare this with the array of
	 * valid partitions
	 */
	systemid = dec2hex(p1[4]);
	fprintf(oPtr, "System ID:\t0x%s\n", systemid);

	isValid = validatePartitionType(systemid, oPtr);
	if (isValid) {
		fprintf(oPtr, "Partition %d (VALID):  %s\n", pnumber, systemid);
	} else {
		fprintf(oPtr, "Partition %d (NOT VALID):  %s\n", pnumber, systemid);
	}

	endhead = dec2hex(p1[5]);
	fprintf(oPtr, "Ending head:\t0x%s (%d)\n", endhead, p1[5]);

	sectors_mbr_partition = (p1[9] << 8 | p1[8]);
	fprintf(oPtr, "Sectors between MBR and Partition %d:  %d\n", pnumber,
			sectors_mbr_partition);

	total_sectors = (p1[15] << 24 | p1[14] << 16 | p1[13] << 8 | p1[12]);
	fprintf(oPtr, "Total sectors in partition %d:  %ld\n", pnumber,
			total_sectors);

	return 0;

}

int validatePartitionType(char *code, FILE *out) {

	int i = 0;
	char *hx;
	char *unused = "00";

	for (i = 0; i < 94; i++) {
		hx = dec2hex(validPartitionTypes[i]);

		if (strcmp(hx, code) == 0) {
			if (strcmp(code, unused) == 0) {
				fprintf(out, "*** Unused partition table entry ***\n");
			}
			//printf("hx = %s and code = %s\n", hx, code);
			return 1;
		}
	}
	// if you got here, no match.
	return 0;

}
