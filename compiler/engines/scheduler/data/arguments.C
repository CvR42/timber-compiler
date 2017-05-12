#include "arguments.h"

int NETWORK_TYPE = MYRINET;
bool HIERARCHICAL = false;
bool DEBUG = false;

int output_format = T2D_NUMERIC | T2D_SCHEDULE_TIME | T2D_RUNNING_TIME;
FILE* outfile = stdout;
FILE* infile;

bool code_gen = false;
