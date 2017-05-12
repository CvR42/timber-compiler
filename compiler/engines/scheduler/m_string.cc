#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "data/arguments.h"
#include "m_string.h"
#include "error.h"
#include "io.h"


static void add_room (m_string m_str);


m_string get_new_m_string (void)
{
  m_string m_str = (m_string) malloc (sizeof (struct str_m_string));

  m_str->buf = (char*) malloc ((M_STRING_INIT_LENGTH + 1) * sizeof (char));
  m_str->max_length = M_STRING_INIT_LENGTH;
  m_str->length = 0;

  return m_str;
}

void free_m_string (m_string m_str)
{
  free (m_str->buf);
}


void clear_m_string (m_string m_str)
{
  m_str->buf[0] = '\0';
  m_str->length = 0;
}


void copy_m_string (m_string m_str_dest, const m_string m_str_src)
{
  clear_m_string (m_str_dest);
  append_string (m_str_dest, m_str_src->buf);
}


void append_string (m_string m_str, const char *str)
{
  int str_len = strlen (str);

  while (str_len > (m_str->max_length - m_str->length))
  {
    add_room (m_str);
  }

  strcat (m_str->buf, str);
  m_str->length += str_len;
}

void append_int (m_string m_str, const int val)
{
  char buf[M_STRING_INIT_LENGTH + 1];
  int str_len = snprintf (buf, M_STRING_INIT_LENGTH * sizeof(char), "%d", val);

  assert (str_len >= 0);

  while (str_len > (m_str->max_length - m_str->length))
  {
    add_room (m_str);
  }

  strcat (m_str->buf, buf);
  m_str->length += str_len;
}

void append_char (m_string m_str, const char c)
{
  int str_len = 1;

  while (str_len > (m_str->max_length - m_str->length))
  {
    add_room (m_str);
  }

  m_str->buf[m_str->length] = c;
  m_str->buf[m_str->length + 1] = '\0';
  m_str->length++;
}

char* get_string (m_string m_str)
{
  return (m_str->buf);
}

void print_m_string (m_string m_str)
{
  fputs (m_str->buf, outfile);
}


static void add_room (m_string m_str)
{
  int new_max_length = m_str->max_length + M_STRING_INIT_LENGTH;
  char *buf = (char*) malloc (new_max_length * sizeof (char));

  memcpy (buf, m_str->buf, (m_str->max_length + 1) * sizeof (char));
  free (m_str->buf);
  m_str->buf = buf;
}
