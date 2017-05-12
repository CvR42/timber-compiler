#include "symtable.h"

/*
  Important:
  Copies of the parameters to the add function are added to the database.
  Pointers to data in the database is returned, make copies when necessary.
  */
class symtable_entry
{
  public:
    symtable_entry *next;
    tmsymbol sym;
    origsymbol translated_to;
    int id;
    symtable_entry(tmsymbol theSym)
     : sym(theSym), translated_to(NULL), id(-1)
    {}
    ~symtable_entry()
    {
        rfre_tmsymbol(sym);
        if (translated_to != NULL)
            rfre_origsymbol(translated_to);
    }
};

class symtable
{
  public:
    symtable_entry *first;
    symtable()
     : first(NULL)
    {}
    ~symtable()
    {
        symtable_entry *next;
        for (;first != NULL; first = next)
        {
            next = first->next;
            delete first;
        }
    }
    symtable_entry* find(tmsymbol theSym)
    {
        for (symtable_entry *f = first; f != NULL; f=f->next)
            if (f->sym == theSym)
                return f;
        return NULL;
    }
    symtable_entry* add(tmsymbol theSym) // If present error
    {
        symtable_entry *s = find(theSym);
	assert (s == NULL);
        s = new symtable_entry(rdup_tmsymbol(theSym));
        s->next = first;
        first = s;
        return s;
    }
    void remove(tmsymbol theSym)
    {
	symtable_entry *prev = NULL;
	symtable_entry *walk = first;
	while (walk != NULL)
	{
	    if (walk->sym == theSym)
	    {
		symtable_entry *tmp = walk;
		walk = walk->next;
		if (prev == NULL)
		    first = walk;
		else
		    prev->next = walk;
		delete tmp;
	    }
	    else
	    {
		prev = walk;
		walk = walk->next;
	    }
	}
    }
};

/*
  Symboltable: tmsymbol to origsymbol translation.
  */
static symtable* theSymTable;

void symtable_init()
{
    theSymTable = new symtable();
}

void symtable_destroy()
{
    delete theSymTable;
}

void symtable_add_translation(tmsymbol sym, origsymbol target)
{
    symtable_entry *e = theSymTable->add(sym);
    e->translated_to = rdup_origsymbol(target);
}

origsymbol symtable_find_translation(tmsymbol sym)
{
    symtable_entry *e = theSymTable->find(sym);
    if (e == NULL)
        return NULL;
    else
        return e->translated_to;
}

void symtable_remove_translation(tmsymbol sym)
{
    theSymTable->remove(sym);
}

/*
  Tasktable: tmsymbol to int translation.
  */
static symtable* theTaskTable;

void tasktable_init()
{
    theTaskTable = new symtable();
}

void tasktable_destroy()
{
    delete theTaskTable;
}

void tasktable_add_translation(tmsymbol sym, int target)
{
    symtable_entry *e = theTaskTable->add(sym);
    e->id = target;
}

int tasktable_find_translation(tmsymbol sym)
{
    symtable_entry *e = theTaskTable->find(sym);
    if (e == NULL)
        return -1;
    else
        return e->id;
}
