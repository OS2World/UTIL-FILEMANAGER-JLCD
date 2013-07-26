/*
 * FFBUFF.H
 * Portable class for FindFirst/FindNext operations.
 *
 * ver 1.0, 28 Mar 1997
 *
 * ver 1.1, 30 Mar 1997
 *   Added IsDirectory.
 *
 * ver 1.2, 07 Apr 1997
 *   Added WIN32 specific stuff.
 *
 * Public domain by:
 *   Jari Laaksonen
 *   Arkkitehdinkatu 30 A 2
 *   FIN-33720 Tampere
 *   FINLAND
 *
 *   Fidonet : 2:221/360.20
 *   Internet: jla@to.icl.fi
 */

#ifndef _FFBUFF_H_
#define _FFBUFF_H_

#if defined (__OS2__)

  #ifndef OS2_INCLUDED
    #error "os2.h must be included before ffbuff.h"
  #endif

  #if defined (__WATCOMC__) && defined (__386__)
    #ifndef __32BIT__
      #define __32BIT__
    #endif
  #endif

  #if defined (__WATCOMC__) && !defined (__32BIT__)
    #define BUFF_NAME(buff)   buff ## . ## name
    #define BUFF_NAME0(buff)  buff ## . ## name[0]
    #define BUFF_ATTR(buff)   buff ## . ## attrib
    #define BUFF_SIZE(buff)   buff ## . ## size
  #else
    #define BUFF_NAME(buff)   buff ## . ## achName
    #define BUFF_NAME0(buff)  buff ## . ## achName[0]
    #define BUFF_ATTR(buff)   buff ## . ## attrFile
    #define BUFF_SIZE(buff)   buff ## . ## cbFile
  #endif

#elif defined (WIN32)

  #define BUFF_NAME(buff)   buff ## . ## cFileName
  #define BUFF_NAME0(buff)  buff ## . ## cFileName[0]
  #define BUFF_ATTR(buff)   buff ## . ## dwFileAttributes
  #define BUFF_SIZE(buff)   buff ## . ## nFileSizeLow

#else

  #ifdef __BORLANDC__
    #define BUFF_NAME(buff)   buff ## . ## ff_name
    #define BUFF_NAME0(buff)  buff ## . ## ff_name[0]
    #define BUFF_ATTR(buff)   buff ## . ## ff_attrib
    #define BUFF_SIZE(buff)   buff ## . ## ff_fsize
  #else
    #define BUFF_NAME(buff)   buff ## . ## name
    #define BUFF_NAME0(buff)  buff ## . ## name[0]
    #define BUFF_ATTR(buff)   buff ## . ## attrib
    #define BUFF_SIZE(buff)   buff ## . ## size
  #endif

#endif // __OS2__

#if defined (__OS2__)

  #ifdef __32BIT__
    typedef FILEFINDBUF3  FIND_BUFF;
    #define INFOLEVEL FIL_STANDARD
  #else
    #ifdef __WATCOMC__
      #include <dos.h>
      typedef struct find_t FIND_BUFF;
    #else
      typedef FILEFINDBUF   FIND_BUFF;
    #endif
    #define INFOLEVEL 0L
  #endif

#elif defined (WIN32)

  //#include <io.h>
  //typedef struct _finddata_t FIND_BUFF;
  #include <windows.h>
  typedef WIN32_FIND_DATA FIND_BUFF;

#else // !__OS2__

  #include <dos.h>

  #ifdef __BORLANDC__
    #include <dir.h>
    typedef struct ffblk  FIND_BUFF;
  #else
    typedef struct find_t FIND_BUFF;
  #endif

#endif


class FFbuff
{
public:

    enum
    {
#if defined (__OS2__)
        ATTR_NORMAL    = FILE_NORMAL,
        ATTR_READONLY  = FILE_READONLY,
        ATTR_HIDDEN    = FILE_HIDDEN,
        ATTR_SYSTEM    = FILE_SYSTEM,
        ATTR_DIRECTORY = FILE_DIRECTORY,
#ifdef __32BIT__
        ATTR_ONLY_DIR  = MUST_HAVE_DIRECTORY,
#else
        ATTR_ONLY_DIR  = FILE_DIRECTORY,
#endif
        ATTR_ARCHIVED  = FILE_ARCHIVED
#elif defined (WIN32)
        ATTR_NORMAL    = FILE_ATTRIBUTE_NORMAL,
        ATTR_READONLY  = FILE_ATTRIBUTE_READONLY,
        ATTR_HIDDEN    = FILE_ATTRIBUTE_HIDDEN,
        ATTR_SYSTEM    = FILE_ATTRIBUTE_SYSTEM,
        ATTR_DIRECTORY = FILE_ATTRIBUTE_DIRECTORY,
        ATTR_ONLY_DIR  = FILE_ATTRIBUTE_DIRECTORY,
        ATTR_ARCHIVED  = FILE_ATTRIBUTE_ARCHIVE
#else
        ATTR_NORMAL    = _A_NORMAL,
        ATTR_READONLY  = _A_RDONLY,
        ATTR_HIDDEN    = _A_HIDDEN,
        ATTR_SYSTEM    = _A_SYSTEM,
        ATTR_DIRECTORY = _A_SUBDIR,
        ATTR_ONLY_DIR  = _A_SUBDIR,
        ATTR_ARCHIVED  = _A_ARCH
#endif
    };

    FFbuff();

    int             FindFirst (char *filename, unsigned int attr);
    int             FindNext();
    int             FindClose();

    void            SetHandle (unsigned long h)
    {
#if defined (__OS2__)
        handle = (HDIR) h;
#elif defined (WIN32)
        handle = h;
#else
        h = h;
#endif
    }

    char           *GetName();
    char            GetName0();
    unsigned short  GetAttr();
    unsigned long   GetSize();

    short           IsDirectory();
    short           IsDotName();

    unsigned long   SizeOfBuff() { return sizeof (buff); }

    FIND_BUFF      *PtrToBuff()  { return &buff; }

protected:

private:
    FIND_BUFF       buff;

#if defined (__OS2__)
    HDIR            handle;
#ifdef __32BIT__
    ULONG           count;
#else
    USHORT          count;
#endif
#elif defined (WIN32)
    long            handle;
#endif

};

inline char* FFbuff::GetName()
{
    return BUFF_NAME (buff);
}

inline char FFbuff::GetName0()
{
    return BUFF_NAME0 (buff);
}

inline unsigned short FFbuff::GetAttr()
{
    return BUFF_ATTR (buff);
}

inline unsigned long FFbuff::GetSize()
{
    return BUFF_SIZE (buff);
}

inline short FFbuff::IsDirectory()
{
    return 0 != (BUFF_ATTR (buff) & ATTR_DIRECTORY);
}

#endif // _FFBUFF_H_
