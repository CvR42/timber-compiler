/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.io;

public class ByteToCharDefault extends ByteToCharConverter {

// CvR:
// Given an array of bytes 'from' and an array of bytes 'to', convert
// the appropriate range of bytes in 'from' to characters and put them in
// the appropriate range of 'to'.
// This method is specific to Kaffe, and is not well-documented. The
// behavior above is inferred.
public int convert ( byte[] from, int fpos, int flen, char[] to, int tpos, int tlen )
{
     final int len = Math.min( flen, tlen );

     for( int i=0; i<len; i++ ){
         to[tpos+i] = (char) ((int) from[fpos+i] & 0xFF);
     }
     if( len<flen ){
         carry( from, fpos+len, flen-len );
     }
     return len;
}

public int getNumberOfChars ( byte[] from, int fpos, int flen ) {
	return flen;
}

}
