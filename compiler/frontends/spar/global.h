/* File: global.h
 *
 * Header file for global.c.
 */

extern int dumpmangledtree;
extern int dumptree;
extern int givestat;
extern int lextr;
extern int showmangling;
extern int showmethodbinding;
extern int trace_bindings;
extern int trace_gcadmin;
extern int trace_files;
extern int trace_imports;
extern int trace_symtab;
extern int trace_typelist;
extern int trace_usage;
extern int trace_vtable;
extern int trace_generics;
extern int trace_innerclasses;
extern int trace_analysis;
extern int trace_inlining;
extern bool pref_no_boundscheck;
extern bool pref_no_inlining;
extern bool verify_mode;
extern int pref_no_codegeneration;
extern int pref_generate_serialization;
extern bool nullptr_is_caught;
extern bool boundviolation_is_caught;

extern int warn_ignoredvalue;
extern int warn_semicolon;
extern int inlinable_codesize;

extern unsigned int features;
extern unsigned int spar_features;
extern unsigned int java_features;

extern bool pref_nonotnullassert;
extern bool pref_nogarbagecollection;
extern bool pref_strictanalysis;

extern tmstring_list sparpath;
