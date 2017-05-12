/* File: global.c
 *
 * Global variables.
 * Obviously, there should be as few global variables as possible.
 * Main candidates are debugging and tracing switches.
 */

#include <stdlib.h>
#include <stdio.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "config.h"

/* debugging flags */
int dumpmangledtree = FALSE;
int dumptree = FALSE;
int givestat = FALSE;
int lextr = FALSE;
bool pref_no_boundscheck = false;
bool pref_no_inlining = false;
int showmangling = FALSE;
int showmethodbinding = FALSE;
int trace_analysis = FALSE;
int trace_gcadmin = FALSE;
int trace_bindings = FALSE;
int trace_files = FALSE;
int trace_generics = FALSE;
int trace_imports = FALSE;
int trace_inlining = FALSE;
int trace_innerclasses = FALSE;
int trace_symtab = FALSE;
int trace_typelist = FALSE;
int trace_usage = FALSE;
int trace_vtable = FALSE;
bool verify_mode = false;
int pref_no_codegeneration = FALSE;
int pref_generate_serialization = TRUE;
bool nullptr_is_caught = false;
bool boundviolation_is_caught = false;

int warn_ignoredvalue = FALSE;
int warn_semicolon = FALSE;
int inlinable_codesize = INLINABLE_CODESIZE;
unsigned int features = 0xffff & ~FEAT_INPRAGMA;
unsigned int spar_features = 0xffff & ~FEAT_INPRAGMA;
unsigned int java_features = 0xffff & ~FEAT_INPRAGMA;

bool pref_nonotnullassert = false;
bool pref_nogarbagecollection = false;
bool pref_strictanalysis = false;

tmstring_list sparpath = 0;
