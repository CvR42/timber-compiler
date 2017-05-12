#ifndef _a_M_STRING_H
#define _a_M_STRING_H


#define M_STRING_INIT_LENGTH 80

typedef struct str_m_string
{
  char *buf;
  int length;
  int max_length;
} *m_string;


m_string get_new_m_string (void);
void free_m_string (m_string m_str);

void clear_m_string (m_string m_str);
void copy_m_string (m_string m_str_dest, const m_string m_str_src);

void append_string (m_string m_str, const char *str);
void append_int (m_string m_str, const int val);
void append_char (m_string m_str, const char c);

char* get_string (m_string m_str);
void print_m_string (m_string m_str);

#endif
