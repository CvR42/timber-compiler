#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "mpihack.h"

class message
{
  public:
    char *buf;
    int sz;
    struct message *next;
};

static message *first = NULL;

void Hack_Bsend(const void *buf, int sz)
{
    message *next = new message();
    next->sz = sz;
    next->buf = new char[sz];
    next->next = first;
    memcpy(next->buf, buf, sz);
    first = next;
}


void Hack_Recv(void *buf, int *sz)
{
    assert (first != NULL);
    memcpy(buf, first->buf, first->sz);
    *sz = first->sz;
    delete [] first->buf;
    message *next = first->next;
    delete first;
    first = next;
}

