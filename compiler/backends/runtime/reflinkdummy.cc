// File: reflinkdummy.cc
//
// Provide a dummy __gc_reflink_chain to keep Vnus programs that are
// not generated by the Spar frontend happy.

#include "vnusbaseclass.h"

VnusBase*__gc_reflink_chain = NULL;
