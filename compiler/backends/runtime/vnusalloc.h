// File: vnusalloc.h

#ifndef INC_VNUSALLOC_H
#define INC_VNUSALLOC_H

// The allocation/free routines.
class VnusBase;

extern void *vnus_alloc( size_t sz, bool nulled );
extern void vnus_free( void *p );
extern void *vnus_alloc_array( size_t elmsz, VnusInt nelm, bool nulled );
extern void vnus_free_array( size_t elmsz, VnusInt nelm, void *p );

// mark manipulation functions.
extern void vnus_alloc_setmark( void *p, bool val );
extern bool vnus_alloc_getmark( const void *p );

// functions to enumerate the list of blocks, and delete elements
extern void *vnus_first_block();
extern void *vnus_next_block();
extern void *vnus_first_unmarked_block();
extern void *vnus_next_unmarked_block();
extern void *vnus_delete_block();
extern void vnus_reset_marks();
extern VnusBoolean vnus_there_is_a_rootset();

extern VnusInt vnus_count_active_blocks();

// Allocator routines.
inline VnusByte *Java_spar_compiler_Allocator_allocate( VnusInt nelm, VnusInt elmsz ) { return (VnusByte *) calloc( nelm, elmsz ); }
inline void Java_spar_compiler_Allocator_free( VnusByte *elm ) { free( elm ); }

// Yes, it's sick to put an include in the middle of a header file,
// but this way we can avoid a problem with mutually dependent header files.
#include "vnusbaseclass.h"


// GC routines.
inline VnusBoolean Java_spar_compiler_GC_getMark( const VnusBase *obj )
{
    return vnus_alloc_getmark( obj );
}

inline void Java_spar_compiler_GC_setMark( VnusBase *obj, VnusBoolean val )
{
    vnus_alloc_setmark( obj, val );
}

extern VnusLong Java_spar_compiler_GC_freeMemory();
extern VnusLong Java_spar_compiler_GC_totalMemory();
extern VnusLong Java_spar_compiler_GC_memoryLimit();
extern void Java_spar_compiler_GC_setMemoryLimit( VnusLong val );
extern void Java_spar_compiler_GC_setTotalMemory( VnusLong val );

inline VnusBoolean Java_spar_compiler_GC_thereIsARootSet() { return vnus_there_is_a_rootset(); }

inline VnusBase *Java_spar_compiler_GC_firstBlock() { return (VnusBase *) vnus_first_block(); }
inline VnusBase *Java_spar_compiler_GC_nextBlock() { return (VnusBase *) vnus_next_block(); }
inline VnusBase *Java_spar_compiler_GC_firstUnmarkedBlock() { return (VnusBase *) vnus_first_unmarked_block(); }
inline VnusBase *Java_spar_compiler_GC_deleteBlock() { return (VnusBase *) vnus_delete_block(); }
inline void Java_spar_compiler_GC_resetActiveBlockMarks() { vnus_reset_marks(); }
inline VnusInt Java_spar_compiler_GC_countActiveBlocks() { return vnus_count_active_blocks(); }

extern VnusBase *Java_spar_compiler_GC_firstReference();
extern VnusBase *Java_spar_compiler_GC_nextReference();

extern void Java_spar_compiler_GC_pushRef( VnusBase *obj );
extern VnusBase *Java_spar_compiler_GC_popRef();
extern VnusBoolean Java_spar_compiler_GC_wasRefStackOverflow();
extern void Java_spar_compiler_GC_clearRefStackOverflow();
extern void verify_linkchain();
extern void vnus_report_garbagecollector();


#endif
