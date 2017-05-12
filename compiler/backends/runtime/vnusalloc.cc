// File: vnusalloc.cc
//
// Memory allocation and administration for garbage collection.

#include <stdlib.h>

#include "vnustypes.h"
#include "garbagecollect.h"
#include "outofmemory.h"
#include "vnusalloc.h"
#include "vnusservice.h"

#include <string.h>

// Estimated overhead in bytes of each block.
#define OVERHEAD 4

// Amount of memory used for emergency purposes, namely the construction
// of an out-of-memory event.
#define EMERGENCY_SIZE 10000

// The lowest amount of maxmem we consider reasonable.
#define MIN_MAXMEM 50000

#define GCMAGIC 0x7251fe55

// Are we currently in emergency mode?
static bool in_emergency = false;
static bool initialized = false;

// Statistics variables.
static long long int consumed_memory = 0ll;
static int n_alloc = 0;
static int n_free = 0;
static int n_gc = 0;

// A block of memory that has been squirreled away for emergencies.
static void *emergency_block;

// Maximum amount of allocated memory in bytes.
// In practice this is not a hard limit, but a threshold above which we
// will do garbage collection to try and free up memory.
static unsigned long int max_mem = 32000000;

static unsigned long int hard_max_mem = 0;

// After a garbage collect has been done, at least collect_interval
// bytes should be allocated before a new collect is done. If 
// the used memory is over the max_mem threshold, and garbage collection
// does not work, enforcing this collect_interval ensures that
// subsequent garbage collection is not done immediately, since
// it is probably pointless anyway.
static unsigned long collect_interval = 320000;
static unsigned long current_collect_interval;

// The flags of the memory blocks..
#define GCFLAG_MARK		0x0001
#define GCFLAG_EMERGENCY	0x0002
#define GCFLAG_SANE		0x0004		// Sanity check flag

typedef struct str_header {
    unsigned int flags;
    unsigned int size;
    struct str_header *next;
    struct str_header *prev;
} header;

// Pointer to the first element in the doubly linked list of elements.
static header gc_blocks = { 0, 0l, NULL, NULL };

// The outstanding memory in bytes.
static unsigned long allocated_mem = 0l;

static void dump_chain( FILE *f, header *it )
{
    header *p = &gc_blocks;

    while( p != NULL ){
	const char *mk = "  ";
	if( p == &gc_blocks ){
	    mk = "# ";
	}
	if( p == it ){
	    mk = "->";
	}
	fprintf( f, "%sblk:%9p next:%9p prev:%9p\n", mk, (void *) p, (void *) p->next, (void *) p->prev ); 
	p = p->next;
    }
}

// As long as we are not in an emergency, try to ensure that there is
// emergency memory allocatate.
static void maintain_emergency_memory()
{
    if(
	VNUS_UNLIKELY(
	    in_emergency ||
	    emergency_block != NULL ||
	    2*(allocated_mem+EMERGENCY_SIZE+OVERHEAD)>hard_max_mem
	)
    ){
	// We need not, or cannot, allocate emergency memory.
	return;
    }
    // We can try to allocate an emergency block..
    emergency_block = malloc( EMERGENCY_SIZE );
    if( emergency_block != NULL ){
	allocated_mem += EMERGENCY_SIZE;
    }
}

// We are now officially in an emergency. Free the emergency block.
static void declare_emergency()
{
    if( VNUS_UNLIKELY( in_emergency ) ){
	fputs( "Double memory emergency. I give up.\n", stderr );
	exit( 5 );
    }
#if 0
    fprintf( stderr, "GC: declaring emergency\n" );
#endif
    in_emergency = true;
    current_collect_interval = (3*EMERGENCY_SIZE)/4;
    free( emergency_block );
    allocated_mem -= EMERGENCY_SIZE;
    emergency_block = NULL;
}

static void end_emergency()
{
    fprintf( stderr, "GC: emergency ended\n" );
    in_emergency = false;
}

// Initialize allocator.
// For the moment, just get the maximal memory to use to from an environment
// variable.
static void initialize_allocator()
{
    char *str = getenv( "SPAR_MAXMEM" );
    if( str != NULL ){
        // The environment variable is there.
	long newsz = atol( str );
	if( newsz>MIN_MAXMEM ){
	    max_mem = newsz;
	    collect_interval = newsz/100;
	    if( hard_max_mem != 0 && hard_max_mem<max_mem ){
		hard_max_mem = max_mem;
	    }
	}
	else {
	    fprintf( stderr, "Warning: SPAR_MAXMEM value too small; ignored\n" );
	}
    }
    initialized = true;
}

void *vnus_alloc( size_t sz, bool nulled )
{
    n_alloc++;
    if( VNUS_UNLIKELY( !initialized ) ){
        initialize_allocator();
    }
    maintain_emergency_memory();
    sz += sizeof( header );

    // Enforce hard_max_memory iff it is not null.
    if(
	VNUS_UNLIKELY(
	    !in_emergency &&
	    hard_max_mem>0 &&
	    (allocated_mem+sz+OVERHEAD)>hard_max_mem
	)
    ){
	// We really cannot do this allocation. See if garbage collection
	// improves matters.
#if 0
	fprintf( stderr, "Out of memory, collecting garbage\n" );
#endif
	n_gc++;
	if(
	    VNUS_UNLIKELY(
		VnusEventGarbageCollect() == 0 ||
		(allocated_mem+sz+OVERHEAD)>hard_max_mem
	    )
	){
	    declare_emergency();
	    VnusEventOutOfMemory();
	    return NULL;
	}
    }
    if( current_collect_interval == 0 && (allocated_mem+sz+OVERHEAD)>max_mem ){
	// We're using too much memory. Do a garbage collect.
	n_gc++;
	if( VnusEventGarbageCollect() == 0 ){
	    // Collection didn't help. Don't do it again immediately.
	    current_collect_interval = collect_interval;
	}
    }
    if( VNUS_UNLIKELY( current_collect_interval != 0 ) ){
	if( sz>current_collect_interval ){
	    current_collect_interval = 0;
	}
	else {
	    current_collect_interval -= sz;
	}
    }
    void *p = malloc( sz );
    if( VNUS_UNLIKELY( p == NULL ) ){
	n_gc++;
	// We're truely out of memory. See if garbage collection helps.
	if( VnusEventGarbageCollect() != 0 ){
	    p = malloc( sz );
	}
    }
    if( VNUS_UNLIKELY( p == NULL ) ){
	declare_emergency();
	VnusEventOutOfMemory();
	return p;
    }
    if( nulled ){
	memset( p, 0, sz );
    }
    else {
	memset( p, 0, sizeof( header ) );
    }
    header *hp = (header *) p;
    hp->next = gc_blocks.next;
    if( VNUS_UNLIKELY( hp->next != NULL ) ){
	hp->next->prev = hp;
    }
    hp->prev = &gc_blocks;
    gc_blocks.next = hp;
    //dump_chain( stdout, hp );
    hp->size = sz+OVERHEAD;
    hp->flags = GCFLAG_SANE;
    if( VNUS_UNLIKELY( in_emergency ) ){
	// This block is allocated during a low-memory emergency.
	// Presumably this means it is needed to throw an out-of-memory
	// exception. Freeing of a block with this flag
	// indicates that the emergency is over.
	hp->flags |= GCFLAG_EMERGENCY;
    }
    allocated_mem += hp->size;
    consumed_memory += hp->size;
    return ((char *) p) + sizeof( header );
}

// Remove the given block from the gc_blocks list.
void vnus_free( void *p )
{
    n_free++;
    header *hp = (header *) (((char *) p) - sizeof( header ));
    //dump_chain( stdout, hp );
    if( hp->next != NULL ){
	hp->next->prev = hp->prev;
    }
    if( hp->prev != NULL ){
	hp->prev->next = hp->next;
    }
    allocated_mem -= hp->size;
    if( VNUS_UNLIKELY( hp->flags & GCFLAG_EMERGENCY ) ){
	// Someone is freeing blocks that were declared during an
	// emergency. Good, that means that the emergency is over.
	end_emergency();
    }
    free( hp );
    maintain_emergency_memory();
}

// Given an element size, the number of elements, and a flag 'nulled'
// indicating wether the array should be nulled after allocation,
// allocate an array of the given size.
//
// This could be handled directly by malloc, but we want to keep track
// of the amount of memory that is spent on arrays.
// Allocated arrays should not be visible to the garbage collection
// administration, and are therefore not registered in a list.
void *vnus_alloc_array( size_t elmsz, VnusInt nelm, bool nulled )
{
    size_t sz = elmsz*nelm;
    maintain_emergency_memory();
    void *p = malloc( sz );

    n_alloc++;
    if( VNUS_UNLIKELY( p == NULL ) ){
	n_gc++;
	// We're out of memory. See if garbage collection helps.
	if( VnusEventGarbageCollect() != 0 ){
	    // And try to allocate again.
	    p = malloc( sz );
	}
    }
    if( VNUS_UNLIKELY( p == NULL ) ){
	declare_emergency();
	VnusEventOutOfMemory();
	return NULL;
    }
    if( nulled ){
	memset( p, 0, sz );
    }
    allocated_mem += sz+OVERHEAD;
    consumed_memory += sz;
    return p;
}

// Remove the given block from the gc_blocks list.
void vnus_free_array( size_t elmsz, VnusInt nelm, void *p )
{
    n_free++;
    allocated_mem -= elmsz*nelm+OVERHEAD;
    free( p );
    maintain_emergency_memory();
}

void vnus_alloc_setmark( void *p, bool val )
{
    header *hp = (header *) (((char *) p) - sizeof( header ));
    //dump_chain( stdout, hp );
    if( val ){
	hp->flags |= GCFLAG_MARK;
    }
    else {
	hp->flags &= ~GCFLAG_MARK;
    }
}

bool vnus_alloc_getmark( const void *p )
{
    const header *hp = (const header *) (((const char *) p) - sizeof( header ));
    return (hp->flags & GCFLAG_MARK) != 0;
}

static header *walker_block = NULL;

void *vnus_first_block()
{
    walker_block = gc_blocks.next;
    if( walker_block == NULL ){
	return NULL;
    }
    return ((char *) walker_block)+sizeof( header );
}

void *vnus_next_block()
{
    if( VNUS_LIKELY( walker_block != NULL ) ){
	walker_block = walker_block->next;
    }
    else {
	return NULL;
    }
    if( walker_block == NULL ){
	return NULL;
    }
    return ((char *) walker_block)+sizeof( header );
}

void *vnus_next_unmarked_block()
{
    do {
	if( VNUS_LIKELY( walker_block != NULL ) ){
	    walker_block = walker_block->next;
	}
	else {
	    return NULL;
	}
	if( walker_block == NULL ){
	    return NULL;
	}
    }
    while( (walker_block->flags & GCFLAG_MARK) != 0 );

    return ((char *) walker_block)+sizeof( header );
}

void *vnus_first_unmarked_block()
{
    walker_block = &gc_blocks;
    return vnus_next_unmarked_block();
}

void *vnus_delete_block()
{
    header *hp = walker_block;
    if( hp == NULL ){
	return NULL;
    }
    void *res = vnus_next_unmarked_block();

    VnusBase *p = (VnusBase *) (((char *) hp) + sizeof( header ));
    delete p;
    return res;
}

void vnus_reset_marks()
{
    header *p = &gc_blocks;

    while( p != NULL ){
	p->flags &= ~GCFLAG_MARK;
	p = p->next;
    }
}

VnusInt vnus_count_active_blocks()
{
    header *p = gc_blocks.next;
    VnusInt n = 0;

    while( p != NULL ){
	n++;
	p = p->next;
    }
    return n;
}

class MarkLink: public VnusBase {
public:
    MarkLink *next;
    VnusInt nelm;
    VnusBase ***elm;
    VnusInt serial;
    VnusInt magic;
};

extern MarkLink *__gc_reflink_chain;

// These two variables form the enumeration state.
static MarkLink *refchain_ptr;
static VnusInt refchain_ix;	// Index of the next element to return.

VnusBoolean vnus_there_is_a_rootset()
{
    return __gc_reflink_chain != NULL;
}

VnusBase *Java_spar_compiler_GC_nextReference()
{
again:
    if( refchain_ptr == NULL ){
	return NULL;
    }
    if( refchain_ptr->magic != GCMAGIC ){
        fprintf(
	    stderr,
	    "GC refchain block with serial %ld has bad magic number %ld\n",
	    (long int) refchain_ptr->serial,
	    (long int) refchain_ptr->magic
	);
    }
    if( refchain_ptr->nelm<=refchain_ix ){
	// The references at this level have been exhausted, go to the
	// next level.
	MarkLink *nextchain_ptr = refchain_ptr->next;
	if( nextchain_ptr != NULL && nextchain_ptr->magic != GCMAGIC ){
	    fprintf(
		stderr,
		"GC refchain block with serial %ld points to a refchain block with serial %ld with bad magic number %ld\n",
		(long int) refchain_ptr->serial,
		(long int) nextchain_ptr->serial,
		(long int) nextchain_ptr->magic
	    );
	}
	refchain_ptr = refchain_ptr->next;
	refchain_ix = 0;
	goto again;
    }
    VnusBase **p = refchain_ptr->elm[refchain_ix++];
    if( *p == NULL ){
	// This is a null refence, return the next one.
	goto again;
    }
    return *p;
}

void verify_linkchain()
{
    for( MarkLink *p=__gc_reflink_chain; p!=NULL; p=p->next ){
        if( p->magic != GCMAGIC ){
	    fprintf(
		stderr,
		"GC refchain block %9p with serial %ld has bad magic number %ld\nBlocks before that are:\n",
		(void *) p,
		(long int) p->serial,
		(long int) p->magic
	    );
	    for( MarkLink *e=__gc_reflink_chain; e!=NULL; e=e->next ){
		if( e->magic != GCMAGIC ){
		    break;
		}
		fprintf(
		    stderr,
		    "block %9p with serial %ld nelm %4d\n",
		    (void *) e,
		    (long int) e->serial,
		    (int) e->nelm
		);
	    }
	}
    }
}

VnusBase *Java_spar_compiler_GC_firstReference()
{
    refchain_ptr = __gc_reflink_chain;
    refchain_ix = 0;
    verify_linkchain();
    return Java_spar_compiler_GC_nextReference();
}

// The size of the GC reference stack.
#define STACKSIZE 200000
static VnusBase *stack[STACKSIZE];
static int stackptr = 0;
static VnusBoolean stackoverflow = false;

void Java_spar_compiler_GC_pushRef( VnusBase *obj )
{
    if( obj == NULL || vnus_alloc_getmark( obj ) ){
	// already marked, so don't bother to push it.
	return;
    }
    if( stackptr<STACKSIZE ){
	stack[stackptr++] = obj;
	vnus_alloc_setmark( obj, true );
    }
    else {
	// We have no room for this object. Drop it on the floor, and
	// set the overflow flag.
	//fprintf( stderr, "Warning: GC object stack overflowed, recovery code will be used\n" );
	stackoverflow = true;
    }
}

VnusBase *Java_spar_compiler_GC_popRef()
{
    if( VNUS_UNLIKELY( stackptr == 0 ) ){
	return NULL;
    }
    stackptr--;
    return stack[stackptr];
}


VnusBoolean Java_spar_compiler_GC_wasRefStackOverflow()
{
    return stackoverflow;
}

void Java_spar_compiler_GC_clearRefStackOverflow()
{
    stackoverflow = false;
}

VnusLong Java_spar_compiler_GC_freeMemory()
{
    if( allocated_mem>max_mem ){
	return (VnusLong) 0;
    }
    return (unsigned long) max_mem-allocated_mem;
}

VnusLong Java_spar_compiler_GC_totalMemory()
{
    return (VnusLong) max_mem;
}

VnusLong Java_spar_compiler_GC_memoryLimit()
{
    return (VnusLong) hard_max_mem;
}

void Java_spar_compiler_GC_setTotalMemory( VnusLong val )
{
    if( val<MIN_MAXMEM ){
	// TODO: protest more loudly about this unacceptable value.
	return;
    }
    max_mem = val;
    collect_interval = val/100;
    if( hard_max_mem != 0 && hard_max_mem<max_mem ){
	hard_max_mem = max_mem;
    }
    if( allocated_mem>max_mem ){
	n_gc++;
	if( VnusEventGarbageCollect() == 0 ){
	    declare_emergency();
	    VnusEventOutOfMemory();
	}
    }
}

void Java_spar_compiler_GC_setMemoryLimit( VnusLong val )
{
    if( val == (VnusLong) 0 ){
	hard_max_mem = 0;
	return;
    }
    if( val<1000 ){
	// TODO: protest more loudly about this unacceptable value.
	return;
    }
    hard_max_mem = val;
    if( max_mem>hard_max_mem ){
	max_mem = hard_max_mem;
	collect_interval = val/100;
    }
    if( allocated_mem>hard_max_mem ){
	n_gc++;
	if( VnusEventGarbageCollect() == 0 ){
	    declare_emergency();
	    VnusEventOutOfMemory();
	}
    }
}

void vnus_report_garbagecollector()
{
    fprintf(
	stderr,
	"Total consumed memory: %lld\nallocations: %d\nfrees: %d\ngarbage collections: %d\n", 
	consumed_memory,
	n_alloc,
	n_free,
	n_gc
    );
}
