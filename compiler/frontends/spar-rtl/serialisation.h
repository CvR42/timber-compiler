#ifndef __VNUS_SERIALISATION_H__
#define __VNUS_SERIALISATION_H__

#include "vnustypes.h"
#include "vnusbaseclass.h"

class Handle_Base;
class Writer;
class Reader;

// CvR:
// Simple wrapper functions to keep the compiler and the implementation happy
// to keep the compiler happy.

extern Handle_Base* ser_impl_new_Handle_Base();
extern Writer* ser_impl_new_Writer();
extern Reader* ser_impl_new_Reader(int size);

extern void ser_impl_delete_Handle_Base(Handle_Base *h);
extern void ser_impl_delete_Writer(Writer *w);
extern void ser_impl_delete_Reader(Reader *r);

inline VnusBase* ser_new_Handle_Base() { return (VnusBase*) ser_impl_new_Handle_Base(); }
inline VnusBase* ser_new_Writer() { return (VnusBase*) ser_impl_new_Writer(); }
inline VnusBase* ser_new_Reader(int size) { return (VnusBase*) ser_impl_new_Reader(size); }

inline void ser_delete_Handle_Base(VnusBase *h) { ser_impl_delete_Handle_Base((Handle_Base*) h); }
inline void ser_delete_Writer(VnusBase *w) { ser_impl_delete_Writer((Writer*) w); }
inline void ser_delete_Reader(VnusBase *r) { ser_impl_delete_Reader((Reader*) r); }

extern void ser_impl_writeBoolean(Writer *w, Handle_Base *h, VnusBoolean data);
extern void ser_impl_writeChar(Writer *w, Handle_Base *h, VnusChar data);
extern void ser_impl_writeByte(Writer *w, Handle_Base *h, VnusByte data);
extern void ser_impl_writeShort(Writer *w, Handle_Base *h, VnusShort data);
extern void ser_impl_writeInt(Writer *w, Handle_Base *h, VnusInt data);
extern void ser_impl_writeLong(Writer *w, Handle_Base *h, VnusLong data);
extern void ser_impl_writeFloat(Writer *w, Handle_Base *h, VnusFloat data);
extern void ser_impl_writeDouble(Writer *w, Handle_Base *h, VnusDouble data);
extern void ser_impl_writeComplex(Writer *w, Handle_Base *h, VnusComplex data);
extern void ser_impl_writeObject(Writer *w, Handle_Base *h, VnusBase *data);
extern void ser_impl_writeString(Writer *w, Handle_Base *h, VnusString data);
extern void ser_impl_writeArray(Writer *w, Handle_Base *h, VnusBase *data);

extern VnusBoolean ser_impl_readBoolean(Reader *r, Handle_Base *h);
extern VnusChar ser_impl_readChar(Reader *r, Handle_Base *h);
extern VnusByte ser_impl_readByte(Reader *r, Handle_Base *h);
extern VnusShort ser_impl_readShort(Reader *r, Handle_Base *h);
extern VnusInt ser_impl_readInt(Reader *r, Handle_Base *h);
extern VnusLong ser_impl_readLong(Reader *r, Handle_Base *h);
extern VnusFloat ser_impl_readFloat(Reader *r, Handle_Base *h);
extern VnusDouble ser_impl_readDouble(Reader *r, Handle_Base *h);
extern VnusComplex ser_impl_readComplex(Reader *r, Handle_Base *h);
extern VnusBase* ser_impl_readObject(Reader *r, Handle_Base *h);
extern VnusString ser_impl_readString(Reader *r, Handle_Base *h);
extern VnusBase* ser_impl_readArray(Reader *r, Handle_Base *h);

inline void ser_writeBoolean(VnusBase *w, VnusBase *h, VnusBoolean data) { ser_impl_writeBoolean( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeChar(VnusBase *w, VnusBase *h, VnusChar data) { ser_impl_writeChar( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeByte(VnusBase *w, VnusBase *h, VnusByte data) { ser_impl_writeByte( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeShort(VnusBase *w, VnusBase *h, VnusShort data) { ser_impl_writeShort( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeInt(VnusBase *w, VnusBase *h, VnusInt data) { ser_impl_writeInt( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeLong(VnusBase *w, VnusBase *h, VnusLong data) { ser_impl_writeLong( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeFloat(VnusBase *w, VnusBase *h, VnusFloat data) { ser_impl_writeFloat( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeDouble(VnusBase *w, VnusBase *h, VnusDouble data) { ser_impl_writeDouble( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeComplex(VnusBase *w, VnusBase *h, VnusComplex data) { ser_impl_writeComplex( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeObject(VnusBase *w, VnusBase *h, VnusBase *data) { ser_impl_writeObject( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeString(VnusBase *w, VnusBase *h, VnusString data) { ser_impl_writeString( (Writer *) w, (Handle_Base *) h, data ); }
inline void ser_writeArray(VnusBase *w, VnusBase *h, VnusBase *data) { ser_impl_writeArray( (Writer *) w, (Handle_Base *) h, data ); }

inline VnusBoolean ser_readBoolean(VnusBase *r, VnusBase *h) { return ser_impl_readBoolean( (Reader *) r, (Handle_Base *) h ); }
inline VnusChar ser_readChar(VnusBase *r, VnusBase *h) { return ser_impl_readChar( (Reader *) r, (Handle_Base *) h ); }
inline VnusByte ser_readByte(VnusBase *r, VnusBase *h) { return ser_impl_readByte( (Reader *) r, (Handle_Base *) h ); }
inline VnusShort ser_readShort(VnusBase *r, VnusBase *h) { return ser_impl_readShort( (Reader *) r, (Handle_Base *) h ); }
inline VnusInt ser_readInt(VnusBase *r, VnusBase *h) { return ser_impl_readInt( (Reader *) r, (Handle_Base *) h ); }
inline VnusLong ser_readLong(VnusBase *r, VnusBase *h) { return ser_impl_readLong( (Reader *) r, (Handle_Base *) h ); }
inline VnusFloat ser_readFloat(VnusBase *r, VnusBase *h) { return ser_impl_readFloat( (Reader *) r, (Handle_Base *) h ); }
inline VnusDouble ser_readDouble(VnusBase *r, VnusBase *h) { return ser_impl_readDouble( (Reader *) r, (Handle_Base *) h ); }
inline VnusComplex ser_readComplex(VnusBase *r, VnusBase *h) { return ser_impl_readComplex( (Reader *) r, (Handle_Base *) h ); }
inline VnusBase* ser_readObject(VnusBase *r, VnusBase *h) { return ser_impl_readObject( (Reader *) r, (Handle_Base *) h ); }
inline VnusString ser_readString(VnusBase *r, VnusBase *h) { return ser_impl_readString( (Reader *) r, (Handle_Base *) h ); }
inline VnusBase* ser_readArray(VnusBase *r, VnusBase *h) { return ser_impl_readArray( (Reader *) r, (Handle_Base *) h ); }

#endif
