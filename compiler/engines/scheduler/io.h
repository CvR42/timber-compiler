#ifndef _a_IO_H
#define _a_IO_H

#include <stdio.h>
#include <tmc.h>
#include "vnusbasetypes.h"
#include "tmadmin.h"
#include "error.h"

extern tmstring infilename;
extern tmstring outfilename;
extern tmstring errfilename;
extern tmstring proc_file_name;

/* Table of debugging flags plus associated information. */
extern dbflag flagtab[];


/* Scan command line parameters and options. */
void scanargs (int argc, char *argv[]);

/* Open the input, output and error files. */
void open_iofiles (void);
void close_iofiles (void);

/* Read the tm representation of the program from input file. */
void read_tm_file (vnusprog *p_the_prog);
void print_tm_file (vnusprog *p_the_prog);

void clean_memory (vnusprog the_prog);

/* Same as fopen, but give error message if file can't be opened */
FILE *ckfopen (const char *nm, const char *acc);

#endif
