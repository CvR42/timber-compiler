/* file: error.c
 *
 * error handling
 */

#include <stdio.h>

/* the tm support library */
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"

extern int sys_nerr;

char errpos[ERRARGLEN] = "";
char errarg[ERRARGLEN] = "";


/* Given an origin 'org', fill 'errpos' with the origin info from 'org'. */
void orig_errpos (const origin org)
{
  sprintf (errpos, "%s(%d)", org->file->name, org->line);
}


/* General error printing routine: print error message 'msg' possibly preceded
 * by string in 'errpos', and possibly followed by string in 'errarg'.
 *
 * set a flag to indicate an error has occurred. If the function
 * errcheck() is called, it will cause an exit (1) if 'goterr' is now
 * TRUE.
 */
void error (const char *msg)
{
#if 0
  fputs ("Vnus: ", stderr);
#endif
  if (errpos[0] != '\0')
    fprintf (stderr, "%s: ", errpos);
  fputs (msg, stderr);

  if (errarg[0] != '\0')
  {
    if (strlen (msg)>40)
    {
      fputs (":\n", stderr);
      if (errpos[0] != '\0')
        fprintf (stderr, "%s: ", errpos);
      fprintf (stderr, " %s", errarg);
    }
    else
    {
      fprintf (stderr, ": %s", errarg);
    }
  }

  fputs ("\n", stderr);
  errarg[0] = '\0';
  errpos[0] = '\0';
  goterr = true;
}


/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate error
 * message 'msg'.
 */
void origin_error (const origsymbol s, const char *msg)
{
  tmsymbol sym;

  orig_errpos (s->orig);
  sym = s->sym;

  if (sym == tmsymbolNIL)
    sprintf (errarg, "<NIL symbol>");
  else
    sprintf (errarg, "'%s'", sym->name);

  error (msg);
}


/* System error handler. Note that there is supposed to be a
 * function 'strerror()' that simplifies this, but at least under
 * SunOS 4.1.4 it isn't there.
 */
void sys_error (int no)
{
  if (no>sys_nerr)
  {
    (void) sprintf (errarg, "%d", no);
    error ("unknown system error");
  }
  else
  {
    error (sys_errlist[no]);
  }
}


/* Handle an internal botch. */
void internal_botch (const char *msg)
{
  char msg1[500];

  sprintf (msg1, "Internal botch: %s", msg);
  error (msg1);
  exit (2);
}


/* Given an origsymbol 's' and a message 'msg', fill errpos and errarg
 * with the position and name of the symbol respectively, and generate an
 * internal botch message 'msg'.
 */
void origin_internal_botch (const origsymbol s, const char *msg)
{
  tmsymbol sym;

  orig_errpos (s->orig);
  sym = s->sym;

  if (sym == tmsymbolNIL)
    sprintf (errarg, "<NIL symbol>");
  else
    sprintf (errarg, "'%s'", sym->name);

  internal_botch (msg);
}


/* Check if 'goterr' flag is set, and do exit(1) if it is. */
void errcheck (void)
{
  if (!goterr)
    return;

  exit (1);
}


/* Given a character 'c' and a flag table 'flagtab',
   return a pointer to the entry of character 'c', or return
   dbflagNIL if not found.
 */
dbflag *finddbflag (int c, dbflag *flagtab)
{
  dbflag *p;

  p = flagtab;
  while (p->flagchar != '\0')
  {
    if (p->flagchar == c)
      return p;

    p++;
  }

  return dbflagNIL;
}


/* Give help information on debugging flags */
void helpdbflags (FILE *f, const dbflag *flagtab)
{
  const dbflag *p;

  fputs ("Debugging flags:\n", f);
  p = flagtab;

  while (p->flagchar != '\0')
  {
    fprintf (f, " %c - %s.\n", p->flagchar, p->flagdescr);
    p++;
  }
  fputs (" . - all flags.\n", f);
}


/* Given a string 's' containing debug flags, a flag table 'flagtab' and
   a value 'val', look up all flags given in 's', and set the associated
   value to 'val'. The flag character '.' means that all flags must
   be set.
 */
void setdbflags (const char *s, dbflag *flagtab, int val)
{
  int c;
  dbflag *e;

  while (*s != '\0')
  {
    c = *s++;
    if (c == '.')
    {
      e = flagtab;
      while (e->flagchar != '\0')
      {
        *e->flagadr = val;
        e++;
      }
    }
    else
    {
      e = finddbflag (c, flagtab);
      if (e == dbflagNIL)
      {
        fprintf (stderr, "Unknown debug flag: '%c'\n", c);
        exit (1);
      }
      *e->flagadr = val;
    }
  }
}


void tm_fatal (const char *file, const int line, const char *s)
{
  if (file[0] == '\0' && line == 0)
    fprintf (stderr, "*** fatal error: %s\n", s);
  else
    fprintf (stderr, "%s(%d): *** fatal error: %s\n", file, line, s);

  exit (1);
}
