// File: arrayalloc.h

#ifndef IN_ARRAYALLOC_H
#define IN_ARRAYALLOC_H

template <class T> T *VnusAllocateFilledArray( T init, VnusInt sz )
{
    T *arr = (T *) calloc( (size_t) sz, sizeof( T ) );

    for( VnusInt ix=0; ix<sz; ix++ ){
	arr[ix] = init;
    }
    return arr;
}

template <class T> inline T *VnusAllocateArray( VnusInt sz, T *dummy )
{
    return (T *) calloc( (size_t) sz, sizeof( T ) );
}

#endif
