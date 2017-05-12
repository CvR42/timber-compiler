/*
 * All functions needed to do IO in sequential programs
 */

#include "io/ioseq.h"
#include "vnusstio.h"
#include "vnusrtl.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define  max_file 50

struct
{
    char isused;
    FILE *file;
} stio_file[max_file];


class Init
{
  public:
    Init();
};

static Init forceInit;

Init::Init ()
{
    stio_file[vnusin].isused = TRUE;
    stio_file[vnusin].file = stdin;
    stio_file[vnusout].isused = TRUE;
    stio_file[vnusout].file = stdout;
    stio_file[vnuserr].isused = TRUE;
    stio_file[vnuserr].file = stderr;
}

static inline void __check_fileHandle(VnusInt fileHandle, const char *function_name)
{
    if ((fileHandle < 0) || (fileHandle >= max_file))
      {
	char buf[256];
	sprintf(buf,"Illegal file handle in %s",function_name);
        runtime_error(buf);
      }
    if (!stio_file[fileHandle].isused)
      {
	char buf[256];
	sprintf(buf,"File not open in %s",function_name);
        runtime_error(buf);
      }
}    

/* opens a file and returns the file handle ( an int ). */
/* access string is according to the c-standard              */

VnusInt vs__open(VnusString filename, VnusString access)
{
    VnusInt i;

    i = 3;
    while ((i < max_file) && stio_file[i].isused)
        i++;
    if ( i == max_file)
        runtime_error("too many files opened in vs__open");
    stio_file[i].file = fopen( filename, access);
    if (stio_file[i].file == NULL)
        runtime_error("Error opening file in vs__open");
    stio_file[i].isused = TRUE;
    return i;
}


/* close a file with handle fileHandle */
/* ferror result is returned */

VnusInt vs__close(VnusInt fileHandle)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__vlose");
    result = fclose(stio_file[fileHandle].file);
    stio_file[fileHandle].isused = FALSE;
    if (result != 0)
    {
        perror("I/O error in vs__close");
        clearerr(stio_file[fileHandle].file);
    }
    return result;
}


/* Read a single element of a certain type from file with handle fileHandle */
/* Number of elements read or EOF is returned */
/* If result is not 1 a runtime warning message is generated */

VnusInt vs__readByte(VnusInt fileHandle, VnusByte *elt)
{
    VnusInt result;
    int dum;
    
    __check_fileHandle(fileHandle,"vs__readByte");
    result = fscanf(stio_file[fileHandle].file,"%d", &dum);
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readByte");
    if (result != 1)
        runtime_warning("No elements read in vs__readByte");
    return result;
}

VnusInt vs__readShort(VnusInt fileHandle, VnusShort *elt)
{
    VnusInt result;
    int dum;
    
    __check_fileHandle(fileHandle,"vs__readShort");
    result = fscanf(stio_file[fileHandle].file,"%d", &dum);
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readShort");
    if (result != 1)
        runtime_warning("No elements read in vs__readShort");
    return result;
}

VnusInt vs__readInt(VnusInt fileHandle, VnusInt *elt)
{
    VnusInt result;
    int dum;
    
    __check_fileHandle(fileHandle,"vs__readInt");
    result = fscanf(stio_file[fileHandle].file,"%d", &dum);
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readInt");
    if (result != 1)
        runtime_warning("No elements read in vs__readInt");
    return result;
}

VnusInt vs__readLong(VnusInt fileHandle, VnusLong *elt)
{
    VnusInt result;
    VnusLong dum;
    
    __check_fileHandle( fileHandle, "vs__readLong" );
#if defined( __GNUC__ )  || defined(__INTEL_COMPILER)
    result = __extension__ fscanf( stio_file[fileHandle].file, "%lli",  &dum );
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readLong");
    if (result != 1)
        runtime_warning("No elements read in vs__readLong");
    return result;
#else
    printf( "ERROR: vs__readLong not implemented.\n" );
    assert(0);
    exit(1);
    return 0;
#endif
}

VnusInt vs__readFloat(VnusInt fileHandle, VnusFloat *elt)
{
    VnusInt result;
    float dum;
    
    __check_fileHandle(fileHandle,"vs__readFloat");
    result = fscanf(stio_file[fileHandle].file,"%f",&dum);
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readFloat");
    if (result != 1)
        runtime_warning("No elements read in vs__readFloat");
    return result;
}

VnusInt vs__readDouble(VnusInt fileHandle, VnusDouble *elt)
{
    VnusInt result;
    double dum;
    
    __check_fileHandle(fileHandle,"vs__readDouble");
    result = fscanf(stio_file[fileHandle].file,"%lf",&dum);
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readDouble");
    if (result != 1)
        runtime_warning("No elements read in vs__readDouble");
    return result;
}

VnusInt vs__readComplex(VnusInt fileHandle, VnusComplex *elt)
{
    VnusInt result;
    double dumr;
    double dumi;
    
    __check_fileHandle(fileHandle,"vs__readComplex");
    result = fscanf(stio_file[fileHandle].file,"(%lf,%lf)",&dumr,&dumi);
    (*elt) = VnusComplex(dumr,dumi);
    if (result == EOF)
        runtime_warning("EOF reached in vs__readComplex");
    else
        if (result != 2)
            runtime_warning("No elements read in vs__readComplex");
        else
            result = 1;
    return result;
}


VnusInt vs__readBoolean(VnusInt fileHandle, VnusBoolean *elt)
{
    VnusInt result;
    
    runtime_warning("Reading a Boolean? You're kidding right?");
    __check_fileHandle(fileHandle,"vs__readBoolean");
    (*elt) = (VnusBoolean) getc(stio_file[fileHandle].file);
    if (ferror(stio_file[fileHandle].file) != 0)
    {
        result = EOF;
        runtime_warning("No element read in vs__readBoolean");
    }
    else
        result = 1;
    return result;
}

VnusInt vs__readChar(VnusInt fileHandle, VnusChar *elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__readChar");
    (*elt) = (VnusBoolean) getc(stio_file[fileHandle].file);
    if (ferror(stio_file[fileHandle].file) != 0)
    {
        result = EOF;
        runtime_warning("No element read in vs__readChar");
    }
    else
        result = 1;
    return result;
}

VnusInt vs__readString(VnusInt fileHandle, VnusString *elt)
{
    VnusInt result;
    char dum[256];
    
    __check_fileHandle(fileHandle,"vs__readString");
    result = fscanf(stio_file[fileHandle].file,"%256s",dum);
    (*elt) = dum;
    if (result == EOF)
        runtime_warning("EOF reached in vs__readString");
    if (result != 1)
        runtime_warning("No elements read in vs__readString");
    return result;
}


/* Write single element of a certain type to file with handle fileHandle */
/* Number of elements written is returned */
/* If result is not 1 a runtime warning message is generated */

VnusInt vs__writeByte(VnusInt fileHandle, const VnusByte elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeByte");
    result = fprintf(stio_file[fileHandle].file,"%d", (int)elt);
    if (result == 0)
        runtime_warning("Element not written in vs__writeByte");
    return result;
}

VnusInt vs__writeShort(VnusInt fileHandle, const VnusShort elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeShort");
    result = fprintf(stio_file[fileHandle].file,"%d", (int)elt);
    if (result == 0)
        runtime_warning("Element not written in vs__writeShort");
    return result;
}

VnusInt vs__writeInt(VnusInt fileHandle, const VnusInt elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeInt");
    result = fprintf(stio_file[fileHandle].file,"%d", (int)elt);
    if (result == 0)
        runtime_warning("Element not written in vs__writeInt");
    return result;
}

VnusInt vs__writeLong(VnusInt fileHandle, const VnusLong elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeLong");
#if defined( __GNUC__ ) || defined(__INTEL_COMPILER)
    result = __extension__ fprintf( stio_file[fileHandle].file, "%lld", elt );
    if (result == 0)
        runtime_warning("Element not written in vs__writeLong");
    return result;
#else
    printf("ERROR: vs__writeLong: unable to write 64-bit words.\n");
    assert(0);
    exit(1);
    return 0;
#endif
}

VnusInt vs__writeFloat(VnusInt fileHandle, const VnusFloat elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeFloat");
    result = fprintf(stio_file[fileHandle].file,"%.4g",(double) elt);
    if (result == 0)
        runtime_warning("Element not written in vs__writeFloat");
    return result;
}

VnusInt vs__writeDouble(VnusInt fileHandle, const VnusDouble elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeDouble");
    result = fprintf( stio_file[fileHandle].file, "%.4g", elt );
    if (result == 0)
        runtime_warning("Element not written in vs__writeDouble");
    return result;
}

VnusInt vs__writeComplex(VnusInt fileHandle, const VnusComplex elt)
{
//#if defined( __GNUC__ )
#if 1
    VnusInt result;
    
    __check_fileHandle( fileHandle, "vs__writeComplex" );
    double re = std::real( elt );
    double im = std::imag( elt );
    result = fprintf( stio_file[fileHandle].file, "(%.4g,%.4g)", re, im );
    if (result == 0)
        runtime_warning( "Element not written in vs__writeComplex" );
    else
        result = 1;
    return result;
#else
    printf("ERROR: vs__writeComplex not implemented.\n");
    assert(0);
    exit(1);
    return 0;
#endif
}

VnusInt vs__writeBoolean(VnusInt fileHandle, const VnusBoolean elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeBoolean");
    result = fprintf(stio_file[fileHandle].file,"%s",
                     (int)elt ? "TRUE" : "FALSE");
    if (result == 0)
        runtime_warning("Element not written in vs__writeBoolean");
    return result;
}

VnusInt vs__writeChar(VnusInt fileHandle, const VnusChar elt)
{
    VnusInt result;
    
    __check_fileHandle(fileHandle,"vs__writeChar");
    result = fprintf(stio_file[fileHandle].file,"%c",(char)elt);
    if (result == 0)
        runtime_warning("Element not written in vs__writeChar");
    return result;
}

VnusInt vs__writeString(VnusInt fileHandle, const VnusString elt)
{
    VnusInt result;
    int res;
    
    __check_fileHandle(fileHandle,"vs__writeString");
    res = fputs((const char *)elt,stio_file[fileHandle].file);
    if ( res<0 ){
	// There's  a problem.
        result = 0;
        runtime_warning("Element not written in vs__writeString");
    }
    else
        result = 1;
    (void) fflush(stio_file[fileHandle].file);
    return result;
}

/* Initialization of stio_file : all files set to not used */
void vs__initialize()
{       
    for (int i = 0; i < max_file; i++)
        stio_file[i].isused = FALSE;
}
