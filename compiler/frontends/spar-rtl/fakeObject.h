// File: fakeObject.h
//
// Definitions that mimic the definition of java.lang.Object as closely
// as possible.

class IntrospectionRecord;

#ifndef HAVE_FAKEOBJECT_H
#define HAVE_FAKEOBJECT_H

class VTable: public VnusBase {
public:
    IntrospectionRecord *introspection;
};

class IVTable: public VnusBase {
public:
};

// A representation of the generated class Object
class ObjectClass: public VnusBase {
public:
    ObjectClass() {}
    ObjectClass(VTable *vt,IVTable *ivt,ObjectClass *n):
     vtable(vt), ivtable(ivt), next(n) {}
    VTable *vtable;
    IVTable *ivtable;
    ObjectClass *next;
};


class IntrospectionRecord: public VnusBase {
private:
    typedef void (*markfnptr)(ObjectClass *);
    typedef void (*serwriteptr)(ObjectClass *,ObjectClass *,ObjectClass *);
    typedef ObjectClass *(*serreadptr)(ObjectClass *,ObjectClass *);

public:
    VnusString classnm;
    VnusInt classno;
    VnusInt last_subclassno;
    VTable *vtable;
    void *ivtable;
    VnusInt flags;
    markfnptr mark;
    markfnptr finalizer;
    serwriteptr serwriter;
    serreadptr serread;
};

#else
class ObjectClass;
#endif
