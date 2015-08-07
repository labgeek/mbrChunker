/* SVN FILE: $Id: dd2vmdk.c 5 2010-07-14 02:26:52Z jd $*/
/*
 * Source file : Program to parse the MBR of a dd image.
 * $Author: jd $
 * $Date: 2010-07-13 22:26:52 -0400 (Tue, 13 Jul 2010) $
 * $Revision: 5 $
 * $LastChangedBy: jd $
 * $URL: file:///data/codehole/mbrChunker/trunk/dd2vmdk.c $
 */

#include <stdio.h>

const int mstr_boot_record = 512;

int getcylinders(int p1[]) {

	int begincylinder, endcyclinder, cylinders;
	/*
	 * starting cylinder is a 10 bit value,
	 * taking 8 bits from partition entry
	 *  and 2 bits from partition entry 3
	 */
	begincylinder = ((p1[3] << 8) | p1[2]);
	endcyclinder = ((p1[7] << 8) | p1[6]);

	/*
	 * code from PartitionEntry.java
	 * in the Liveview source code
	 */
	cylinders = ((endcyclinder & 65280) >> 8);

	// if the 6th bit of the 16 bit
	// structure is not zero
	if ((endcyclinder & 64) != 0) {
		cylinders += 256;
	}
	// set bit 8 of the 10 bit cylinder value

	if ((endcyclinder & 128) != 0) { // if the 7th bit of the 16 bit
		// structure is not zero
		cylinders += mstr_boot_record; // set bit 9 of the 10 bit cylinder value
	}

	/* loop until null is found */
	//for (n = 0; n < partitionentrysize; n++) {
	//	printf("%d and %02X\n", n, p1[n]);
	//}
	//printf("\n\n");

	return cylinders;

}

int getendcylinder(int partition1[]) {
	int endcyl = 0;
	endcyl = ((partition1[7] << 8) | partition1[6]);

	return endcyl;
}

int getsectorspertrack(int endcyl) {
	int sectors_per_track = 0;
	/* the ending sector is 6 bits which would be 00111111
	 * use the bitwise operator AND and compares in binary form
	 * in the case of 65535,  = 11111111 11111111
	 * Comparing the two would give us still 63 decimal
	 */
	sectors_per_track = endcyl & 63;
	return sectors_per_track;

}
int getheadspertrack(int part1[]) {

	int nheads = 0;
	nheads = part1[1] + part1[5];

	return nheads;

}

/*
 * Compliments to zapotek's template for creating a
 * baseline VMDK metadata file
 *
 */

void printvmdkfile(char *src, char *output, long tsectors, int cyls, int heads,
		int sectpertrack) {

	FILE *outputfile;
	outputfile = fopen(output, "w");
	fprintf(outputfile, "\nversion=1\n");
	fprintf(outputfile, "encoding=\"UTF-8\"\n");
	fprintf(outputfile, "CID=fffffffe\n");
	fprintf(outputfile, "parentCID=ffffffff\n");
	fprintf(outputfile, "isNativeSnapshot=\"no\"\n");
	fprintf(outputfile, "createType=\"monolithicFlat\"\n\n");
	fprintf(outputfile, "RW %ld FLAT \"%s\" 0\n\n", tsectors, src);
	fprintf(outputfile, "ddb.virtualHWVersion = \"7\"\n");
	fprintf(outputfile,
			"ddb.longContentID = \"29075898903f9855853610dffffffffe\"\n");
	fprintf(outputfile,
			"ddb.uuid = \"60 00 C2 91 8e 73 27 62-43 58 3b f8 05 ae 2e a0\"\n");
	fprintf(outputfile, "ddb.geometry.cylinders = \"%d\"\n", cyls);
	fprintf(outputfile, "ddb.geometry.heads = \"%d\"\n", heads);
	fprintf(outputfile, "ddb.geometry.sectors = \"%d\"\n", sectpertrack);
	fprintf(outputfile, "ddb.adapterType = \"ide\"\n");
	fclose(outputfile);

}

