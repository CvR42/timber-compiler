/* File: global.h
 *
 * Header file for global.c.
 */
#ifndef __VNUS_GLOBAL_H__
#define __VNUS_GLOBAL_H__

extern bool trace_struct_register;
extern bool trace_analysis;
extern bool trace_scope_admin;
extern bool dontflattenselection;
extern bool lextr;
extern bool givestat;
extern bool dumptree;
extern bool generateTimingCode;
extern bool longAsClass;
extern unsigned int numberOfProcessors;
extern PROGTYPE program_type;
extern tmuint_list processor_array;

#endif
