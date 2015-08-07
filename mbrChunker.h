/* SVN FILE: $Id: mbrChunker.h 19 2010-07-30 00:52:44Z jd $*/
/*
 * Project Name : mbrChunker.h header file
 * $Author: jd $
 * $Date: 2010-07-29 20:52:44 -0400 (Thu, 29 Jul 2010) $
 * $Revision: 19 $
 * $LastChangedBy: jd $
 * $URL: file:$
 */

#ifndef MBRCHUNKER_H_
#define MBRCHUNKER_H_

#define PROGRAM_NAME "mbrChunker"
#define AUTHOR "JD Durick <jd@vmforensics.org>"
#define WEBSITE "http://vmforensics.org"
#define VERSION "0.3 $Rev: 19 $"
#define DEFAULT_BLOCKSIZE 1024
#define HEXSTRING_MAX 16
#define HEX_SIZE 256

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>

/*
 * Function prototypes
 */

off_t getfilesize(FILE*fp);
void usage(char *);
void getpe1(int[]);
int getendcylinder(int[]);
int getsectorspertrack(int);
int getcylinders(int[]);
int getheadspertrack(int[]);
void printvmdkfile(char *, char *, long, int, int, int);
void parsePartitionEntries(char *masterbootrec, FILE *);
void mbrAnalysis(FILE *, char *, char *, long long);
char *getMBR(FILE *);
char* dec2hex(int);
int validatePartitionType(char *, FILE *);
int analyzePartitions(int[], int, FILE *);
void end_of_sector_marker(char *, FILE *);
void hxsearch(FILE *, char *, long long, long);
char *allocateBuffer(FILE *fptr, long long);

#endif /* MBRCHUNKER_H_ */

