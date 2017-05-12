/* File: symbol_support.h
 *
 * Header file for symbol_support.c
 */

/* The global variables below describe the structures of the
 * symbol table.
 */
#ifndef __VNUS_SYMBOL_SUPPORT_H__
#define __VNUS_SYMBOL_SUPPORT_H__

extern bool is_readonly( const_declaration_list decls, const_origsymbol name, const_Pragma_list gl );
extern bool is_location( const_declaration_list decls, const_origsymbol name, const_Pragma_list gl );

/* record administration. */
extern tmsymbol get_record_name( const_declaration_list decls, const_origin org, const_field_list fields );
extern void register_record( const_declaration_list decls, const_field_list fields );
extern void generate_record_definitions( FILE *f, const_declaration_list decls );

/* initialization */
extern void init_symbol_support( void );
extern void end_symbol_support( void );

#endif
