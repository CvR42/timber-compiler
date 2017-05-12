/* File: symbol_table.h
 *
 * Header file for symbol_table.c
 */

/* The global variables below describe the structures of the
 * symbol table.
 */
#ifndef __VNUS_SYMBOL_TABLE_H__
#define __VNUS_SYMBOL_TABLE_H__


extern void register_variable_scopes( const declaration_list decls );
extern void clear_scopes_register( void );
extern void register_scope( const tmsymbol scope );
extern void unregister_scope( const tmsymbol scope );
extern origsymbol_list get_scope_locals( const tmsymbol scope );
extern unsigned int count_scope_locals( const tmsymbol scope );
extern bool check_known_scopes( void );

/* initialization */
extern void init_symbol_table( void );
extern void end_symbol_table( void );

#endif
