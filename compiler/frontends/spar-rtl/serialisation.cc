#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "serialisation.h"
#include "vnusstd.h"
#include "fakeObject.h"

#define MALLOC_SIZE 256

// Copied from Introspection.cc
extern IntrospectionRecord *__spar_rte_typelist[];
extern VnusInt __spar_rte_typelist_length;

static void serialisation_error( const char *s )
{
    fprintf(stderr,"%s\n",s);
    exit(1);
}

static IntrospectionRecord *search_intro_record( VnusInt id )
{
    for( VnusInt ix=0; ix<__spar_rte_typelist_length; ix++ ){
        IntrospectionRecord *r = __spar_rte_typelist[ix];
        if( r->classno == id ){
            return r;
        }
    }
    return NULL;
}


// Database of previously encountered pointers
class Handle_Base: public VnusBase
{
  private:
    void **ptrs;
    unsigned sz;
    unsigned no_ptrs;
    
  public:
    Handle_Base()
    {
	ptrs = new void*[MALLOC_SIZE];
	sz = MALLOC_SIZE;
	no_ptrs = 0;
    }
    
    ~Handle_Base()
    {
	delete [] ptrs;
    }

    void check_buf()
    {
	if (sz == no_ptrs){
	    void **new_ptrs = new void*[2*sz];
	    memcpy(new_ptrs,ptrs,sizeof(void*)*sz);
	    delete [] ptrs;
	    ptrs = new_ptrs;
	    sz *= 2;
	}
    }
    
    int find_handle(void *ptr)
    {
	for (unsigned i=0; i<no_ptrs; i++)
	    if (ptrs[i] == ptr)
		return i;
	check_buf();
	ptrs[no_ptrs++] = ptr;
	return -1;
    }
    
    void* find_ptr(unsigned index)
    {
	assert(index<no_ptrs);
	return ptrs[index];
    }
    
    void reset()
    {
	no_ptrs = 0;
    }
};

class Writer: public VnusBase
{
  private:
    unsigned char *buf;
    unsigned sz;
    unsigned no_bytes;

  public:
    Writer()
    {
	buf = new unsigned char[MALLOC_SIZE];
	sz = MALLOC_SIZE;
	no_bytes = 0;
    }
    ~Writer()
    {
	if (buf != NULL)
	    delete [] buf;
    }

    void check_buf(int extra)
    {
	while (sz < no_bytes+extra){
	    unsigned char *new_buf = new unsigned char[2*sz];
	    memcpy(new_buf,buf,sizeof(unsigned char)*sz);
	    delete [] buf;
	    buf = new_buf;
	    sz *= 2;
	}
    }
    
    void writeBoolean(VnusBoolean data)
    {
	check_buf(sizeof(VnusInt));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusBoolean *)ptr = data;
	no_bytes += sizeof(VnusInt);
    }
    
    void writeByte(VnusByte data)
    {
	check_buf(sizeof(VnusInt));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusByte *)ptr = data;
	no_bytes += sizeof(VnusInt);
    }
    
    void writeChar(VnusChar data)
    {
	check_buf(sizeof(VnusInt));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusChar *)ptr = data;
	no_bytes += sizeof(VnusInt);
    }
    
    void writeShort(VnusShort data)
    {
	check_buf(sizeof(VnusInt));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusShort *)ptr = data;
	no_bytes += sizeof(VnusInt);
    }
    
    void writeInt(VnusInt data)
    {
	check_buf(sizeof(VnusInt));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusInt *)ptr = data;
	no_bytes += sizeof(VnusInt);
    }
    
    void writeLong(VnusLong data)
    {
	check_buf(sizeof(VnusLong));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusLong *)ptr = data;
	no_bytes += sizeof(VnusLong);
    }
    
    void writeFloat(VnusFloat data)
    {
	check_buf(sizeof(VnusFloat));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusFloat *)ptr = data;
	no_bytes += sizeof(VnusFloat);
    }
    
    void writeDouble(VnusDouble data)
    {
	check_buf(sizeof(VnusDouble));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusDouble *)ptr = data;
	no_bytes += sizeof(VnusDouble);
    }
    
    void writeComplex(VnusComplex data)
    {
	check_buf(sizeof(VnusComplex));
	unsigned char *ptr = &buf[no_bytes];
	*(VnusComplex *)ptr = data;
	no_bytes += sizeof(VnusComplex);
    }
    
    void writeObject(Handle_Base *h, VnusBase *data)
    {
	assert(data != NULL);
	if( data->isShape() ){
	    writeArray(h,data);
	}
	ObjectClass *o = (ObjectClass *) data;
	assert (o->vtable != NULL);
	assert (o->vtable->introspection != NULL);
	    // write object type
	writeInt(o->vtable->introspection->classno);
	    // check object has virtual function
	if (o->vtable->introspection->serwriter == NULL)
	    serialisation_error("writeObject can't find virtual function");
	    // write object(virtual function)
	o->vtable->introspection->serwriter((ObjectClass*)this,(ObjectClass*)h,o);
    }
    
    void writeString(VnusString data)
    {
	    // Write length(excluding trailing 0)
	    // memcpy content(including trailing 0)
	writeInt(strlen(data));
	check_buf(strlen(data) + 1);
	unsigned char *ptr = &buf[no_bytes];
	memcpy(ptr,data,strlen(data)+1);
	no_bytes += strlen(data) + 1;
	serialisation_error("writeString should not be called");
    }

	// This one is aproblem as we have 1d,2d,3d and nd arrays
    void writeArray(Handle_Base */*h*/, VnusBase */*data*/)
    {
	    // Write length
	    // Write number of dimensions
	    // Write dimensions
	    // Write array elements(via virtual function if not basic type)
	serialisation_error("writeArray not yet implemented");
    }

};


class Reader: public VnusBase
{
  private:
    unsigned char *buf;
    unsigned sz;
    unsigned no_bytes;

  public:
    Reader(int size)
    {
	buf = new unsigned char[size];
	sz = size;
	no_bytes = 0;
    }
    ~Reader()
    {
	if (buf != NULL)
	    delete [] buf;
    }

    VnusBoolean readBoolean()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusBoolean result = *(VnusBoolean *)ptr;
	no_bytes += sizeof(VnusInt);
	return result;
    }
    
    VnusByte readByte()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusByte result = *(VnusByte *)ptr;
	no_bytes += sizeof(VnusInt);
	return result;
    }
    
    VnusChar readChar()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusChar result = *(VnusChar *)ptr;
	no_bytes += sizeof(VnusInt);
	return result;
    }
    
    VnusShort readShort()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusShort result = *(VnusShort *)ptr;
	no_bytes += sizeof(VnusInt);
	return result;
    }
    
    VnusInt readInt()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusInt result = *(VnusInt *)ptr;
	no_bytes += sizeof(VnusInt);
	return result;
    }
    
    VnusLong readLong()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusLong result = *(VnusLong *)ptr;
	no_bytes += sizeof(VnusLong);
	return result;
    }
    
    VnusFloat readFloat()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusFloat result = *(VnusFloat *)ptr;
	no_bytes += sizeof(VnusFloat);
	return result;
    }
    
    VnusDouble readDouble()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusDouble result = *(VnusDouble *)ptr;
	no_bytes += sizeof(VnusDouble);
	return result;
    }
    
    VnusComplex readComplex()
    {
	unsigned char *ptr = &buf[no_bytes];
	VnusComplex result = *(VnusComplex *)ptr;
	no_bytes += sizeof(VnusComplex);
	return result;
    }
    
    VnusBase* readObject(Handle_Base *h)
    {
	VnusBase* result = NULL;
	int objType = readInt();
	assert(objType != -1);
	    // check object type?
	IntrospectionRecord *intro = search_intro_record(objType);
	if (intro == NULL)
	    serialisation_error("readObject encountered illegal object type");
	    // check object has virtual function
	if (intro->serread == NULL)
	    serialisation_error("readObject can't find virtual function");
	    // read object(virtual function)
	result = intro->serread((ObjectClass*)this,(ObjectClass*)h);
	return result;
    }
    
    VnusString readString()
    {
	VnusString result = NULL;
	int len = readInt();
	assert(len != -1);
	    // allocate string
	    // memcpy contents
	    // update no_bytes
	result = new char[len+1];
	memcpy((void*)result,&buf[no_bytes],len+1);
	no_bytes += len+1;
	serialisation_error("readString should not be called");
	return result;
    }
    
	// This one is aproblem as we have 1d,2d,3d and nd arrays
    VnusBase* readArray(Handle_Base */*h*/)
    {
	VnusBase* result = NULL;
	int len = readInt();
	assert(len != -1);
	    // check length
	    // read number of dimensions
	    // read dimensions
	    // allocate array
	    // read objects(virtual function)
	serialisation_error("readArray not yet implemented");
	return result;
    }
    
    void unreadInt()
    {
	assert(no_bytes >= sizeof(VnusInt));
	no_bytes -= sizeof(VnusInt);
    }
};


extern Handle_Base* ser_impl_new_Handle_Base()
{ return new Handle_Base(); }
extern Writer* ser_impl_new_Writer()
{ return new Writer(); }
extern Reader* ser_impl_new_Reader(int size)
{ return new Reader(size); }

extern void ser_impl_delete_Handle_Base(Handle_Base *h)
{ delete h; }
extern void ser_impl_delete_Writer(Writer *w)
{ delete w; }
extern void ser_impl_delete_Reader(Reader *r)
{ delete r; }

void ser_impl_writeBoolean(Writer *w, Handle_Base* /*h*/, VnusBoolean data)
{ w->writeBoolean(data); }

void ser_impl_writeChar(Writer *w, Handle_Base* /*h*/, VnusChar data)
{ w->writeChar(data); }

void ser_impl_writeByte(Writer *w, Handle_Base* /*h*/, VnusByte data)
{ w->writeByte(data); }

void ser_impl_writeShort(Writer *w, Handle_Base* /*h*/, VnusShort data)
{ w->writeShort(data); }

void ser_impl_writeInt(Writer *w, Handle_Base* /*h*/, VnusInt data)
{ w->writeInt(data); }

void ser_impl_writeLong(Writer *w, Handle_Base* /*h*/, VnusLong data)
{ w->writeLong(data); }

void ser_impl_writeFloat(Writer *w, Handle_Base* /*h*/, VnusFloat data)
{ w->writeFloat(data); }

void ser_impl_writeDouble(Writer *w, Handle_Base* /*h*/, VnusDouble data)
{ w->writeDouble(data); }

void ser_impl_writeComplex(Writer *w, Handle_Base* /*h*/, VnusComplex data)
{ w->writeComplex(data); }

void ser_impl_writeObject(Writer *w, Handle_Base* h, VnusBase *data)
{
    int handle = h->find_handle(data);
    if (handle == -1)
    {
	w->writeObject(h,data);
    }
    else
    {
	w->writeInt(-1);
	w->writeInt(handle);
    }
}

void ser_impl_writeString(Writer *w, Handle_Base* h, VnusString data)
{
    int handle = h->find_handle((void*)data);
    if (handle == -1)
    {
	w->writeString(data);
    }
    else
    {
	w->writeInt(-1);
	w->writeInt(handle);
    }
}

void ser_impl_writeArray(Writer *w, Handle_Base* h, VnusBase *data)
{
    int handle = h->find_handle(data);
    if (handle == -1)
    {
	w->writeArray(h,data);
    }
    else
    {
	w->writeInt(-1);
	w->writeInt(handle);
    }
}

VnusBoolean ser_impl_readBoolean(Reader *r, Handle_Base* /*h*/)
{ return r->readBoolean(); }

VnusChar ser_impl_readChar(Reader *r, Handle_Base* /*h*/)
{ return r->readChar(); }

VnusByte ser_impl_readByte(Reader *r, Handle_Base* /*h*/)
{ return r->readByte(); }

VnusShort ser_impl_readShort(Reader *r, Handle_Base* /*h*/)
{ return r->readShort(); }

VnusInt ser_impl_readInt(Reader *r, Handle_Base* /*h*/)
{ return r->readInt(); }

VnusLong ser_impl_readLong(Reader *r, Handle_Base* /*h*/)
{ return r->readLong(); }

VnusFloat ser_impl_readFloat(Reader *r, Handle_Base* /*h*/)
{ return r->readFloat(); }

VnusDouble ser_impl_readDouble(Reader *r, Handle_Base* /*h*/)
{ return r->readDouble(); }

VnusComplex ser_impl_readComplex(Reader *r, Handle_Base* /*h*/)
{ return r->readComplex(); }

/*
  The following three functions will have do some 'unreading' so that we
  do not have to pass information to the corresponding reader functions.
  */
VnusBase* ser_impl_readObject(Reader *r, Handle_Base* h)
{
    VnusBase* result = NULL;
    int objType = r->readInt();
    
    if (objType == -1)
    {
	int handle = r->readInt();
	result = (VnusBase*)h->find_ptr(handle);
    }
    else
    {
	r->unreadInt();
	result = r->readObject(h);
	    // add ptr to handle database
	h->find_handle(result);
    }
    return result;
}

VnusString ser_impl_readString(Reader *r, Handle_Base* h)
{
    VnusString result = NULL;
    int len = r->readInt();
    
    if (len == -1)
    {
	int handle = r->readInt();
	result = (VnusString)h->find_ptr(handle);
    }
    else
    {
	r->unreadInt();
	result = r->readString();
	    // add ptr to handle database
	h->find_handle((void*)result);
    }
    return result;
}

VnusBase* ser_impl_readArray(Reader *r, Handle_Base* h)
{
    VnusBase* result = NULL;
    int len = r->readInt();
    
    if (len == -1)
    {
	int handle = r->readInt();
	result = (VnusBase*)h->find_ptr(handle);
    }
    else
    {
	r->unreadInt();
	result = r->readArray(h);
	    // add ptr to handle database
	h->find_handle(result);
    }
    return result;
}
