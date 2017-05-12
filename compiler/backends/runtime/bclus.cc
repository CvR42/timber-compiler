/* File: bclus.cc
 *
 * Service routines for efficient local set enumeration and
 * aggregate communication.
 *
 * These sources are based on routines written by Will Denissen, but
 * extensively changed by C. van Reeuwijk
 */

#include <assert.h>
#include <stdio.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "vnusservice.h"
#include "shape.h"

/* This function calculates the necessary parameters to enumerate the
 * elements of the loop
 * for [i:ni] {
 *    if( owner A[a*i+b] ){
 *       ...
 *    }
 * }
 * efficiently.
 *
 * As input parameters are given the processor number this is running
 * on 'p' (note that this is the last parameter, since that's the convention
 * with our implementation), and the 'a' and 'b' of the linear function
 * in the owner test. For the moment also a block size 'm' is given.
 *
 * The code requires shapes that are block-cyclic distributed.
 */
