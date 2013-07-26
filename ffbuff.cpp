/*
 * FFBUFF.CPP
 * Portable class for FindFirst/FindNext operations.
 *
 * ver 1.0, 28 Mar 1997
 *
 * ver 1.1, 30 Mar 1997
 *   Added IsDirectory.
 *
 * ver 1.2, 07 Apr 1997
 *   Added WIN32 specific stuff.
 *   OS/2 version of FindClose resets handle and count.
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

#include <string.h>

#if defined __OS2__
  #define INCL_DOSFILEMGR
  #define INCL_ERRORS
  extern "C" {
    #include <os2.h>
  };
#endif

#if defined (WIN32)
#include <stdlib.h>
#endif

#include "ffbuff.h"

FFbuff::FFbuff()
#if defined (__OS2__)
        : handle ((HDIR) HDIR_CREATE)
        , count (1)
#elif defined (WIN32)
        : handle (0)
#endif
{
#if defined (WIN32)
    memset (&buff, 0, sizeof (buff));
#endif
}

int FFbuff::FindFirst (char *filename, unsigned int attr)
{
#if defined (__OS2__)

#if defined (__WATCOMC__) && !defined (__32BIT__)
    return _dos_findfirst (filename, attr, &buff);
#else
    int rc = DosFindFirst (
        (PSZ) filename,
        &handle,
        attr,
        &buff,
        sizeof (buff),
        &count,
        INFOLEVEL
    );
    if (rc == ERROR_PATH_NOT_FOUND)  /* 3 = path not found */
    {
        DosFindClose (handle);
        rc = -1;
    }
    return rc;
#endif

#elif defined (WIN32)

    //int rc = _findfirst (filename, &buff);
    int rc = (int) FindFirstFile (filename, &buff);
    if (rc == -1)
    {
        //_findclose (handle);
        //::FindClose((HANDLE)rc);
        handle = rc;
        return rc;
    }
    handle = rc;
    return 0;

#else

#ifdef __BORLANDC__
    return findfirst (filename, &buff, attr);
#else
    return _dos_findfirst (filename, attr, &buff);
#endif

#endif
}

int FFbuff::FindNext()
{
#if defined (__OS2__)

#if defined (__WATCOMC__) && !defined (__32BIT__)
    return _dos_findnext (&buff);
#else
    return DosFindNext (handle, &buff, sizeof (buff), &count);
#endif

#elif defined (WIN32)

    //return _findnext (handle, &buff);
    return (0 == FindNextFile((HANDLE)handle, &buff));

#else

#ifdef __BORLANDC__
    return findnext (&buff);
#else
    return _dos_findnext (&buff);
#endif

#endif
}

int FFbuff::FindClose()
{
    int rc = 0;

#if defined (__OS2__)
    count = 1;
#if defined (__WATCOMC__) && !defined (__32BIT__)
    rc = _dos_findclose (&buff);
#else
    rc = DosFindClose (handle);
    handle = (HDIR) HDIR_CREATE;
#endif

#elif defined (WIN32)
    //rc = _findclose (handle);
    rc = ::FindClose((HANDLE)handle);
#endif

    return rc;
}

short FFbuff::IsDotName()
{
    return 0 == (strcmp (BUFF_NAME (buff), ".") && strcmp (BUFF_NAME (buff), ".."));
}
