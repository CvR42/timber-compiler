#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"
#include "service.h"

extern void symtable_init();
extern void symtable_destroy();
extern void symtable_add_translation(tmsymbol sym, origsymbol target);
extern origsymbol symtable_find_translation(tmsymbol sym);
extern void symtable_remove_translation(tmsymbol sym);

extern void tasktable_init();
extern void tasktable_destroy();
extern void tasktable_add_translation(tmsymbol sym, int target);
extern int tasktable_find_translation(tmsymbol sym);

#endif
