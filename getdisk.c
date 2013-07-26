
#if defined (__OS2__)
 #define INCL_DOSFILEMGR
 #include <os2.h>

 #if defined (__WATCOMC__) && defined (__386__)
  #ifndef __32BIT__
   #define __32BIT__
  #endif
 #endif

#elif defined (WIN32) && !defined (__WATCOMC__)
 #include <direct.h>
#else
 #include <dos.h>
#endif

/*
 * returns: 0 = A, 1 = B etc.
 *
 */
unsigned int getdisk (void)
{
#if defined (__OS2__)
    ULONG  DriveMap;
#ifdef __32BIT__
    ULONG  CurDrive;
    DosQueryCurrentDisk (&CurDrive, &DriveMap);
#else
    USHORT CurDrive;
    DosQCurDisk (&CurDrive, &DriveMap);
#endif

#elif defined (WIN32)
    int CurDrive = _getdrive();
#else
    unsigned int CurDrive;
    _dos_getdrive (&CurDrive);
#endif

  return (unsigned int)(CurDrive - 1);
}

/*
 * drive: 0 = A, 1 = B etc.
 * returns: number of disks available.
 */
unsigned int setdisk (int drive)
{
    unsigned int total = 27;

#if defined (__OS2__)
#ifdef __32BIT__
    DosSetDefaultDisk (drive + 1);
#else
    DosSelectDisk (drive + 1);
#endif

#elif defined (WIN32) && !defined (__WATCOMC__)
    _chdrive (drive + 1);
#else
    _dos_setdrive (drive + 1, &total);
#endif

    return total;
}
