/*
 * JLCD.CPP
 * JLCD Directory changer.
 *
 * ver 1.0, 05 Sep 1998
 *
 * Public domain by:
 *   Jari Laaksonen
 *   Arkkitehdinkatu 30 A 2
 *   FIN-33720 Tampere
 *   FINLAND
 *
 *   Fidonet : 2:220/855.20
 *   Internet: jla@to.icl.fi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>

#if defined (__OS2__)
 #define  INCL_NOPMAPI
 #define  INCL_DOS
 #define  INCL_DOSERRORS
 #include <os2.h>

 #if defined (__WATCOMC__) && defined (__386__)
  #ifndef __32BIT__
   #define __32BIT__
  #endif
 #endif

#endif

#include "ffbuff.h"
#include "vidmgr.h"
#include "vm_lb.h"

extern "C" {
unsigned int getdisk (void);
unsigned int setdisk (int drive);
char* stristr(const char *txt, const char *pattern);
}

#define VERSION "1.0"

struct StrEntry
{
    StrEntry()
    {
        m_szFileName = 0;
        m_next = 0;
    }

    ~StrEntry()
    {
        delete [] m_szFileName;
    }

    int setName (const char *psz)
    {
        m_szFileName = new char [strlen (psz) + 1];
        if (m_szFileName == 0)
            return 1;
        strcpy (m_szFileName, psz);
        return 0;
    }

    char *m_szFileName;
    StrEntry *m_next;
};

struct DirEntry
{
    DirEntry()
    {
        m_nLevel = 0;
        m_szFileName = 0;
        m_parent = 0;
        m_next = 0;
    }

    ~DirEntry()
    {
        delete [] m_szFileName;
    }

    int setName (const char *psz)
    {
        m_szFileName = new char [strlen (psz) + 1];
        if (m_szFileName == 0)
            return 1;
        strcpy (m_szFileName, psz);
        return 0;
    }

    void save(FILE *fp);
    int  read(FILE *fp);

    short m_nLevel;
    char *m_szFileName;
    DirEntry *m_parent;
    DirEntry *m_next;
};

void DirEntry::save (FILE *fp)
{
    short len;

    fwrite (&m_nLevel, sizeof m_nLevel, 1, fp);
    len = strlen (m_szFileName);
    fwrite (&len, sizeof len, 1, fp);
    fwrite (m_szFileName, len + 1, 1, fp);
}

int DirEntry::read (FILE *fp)
{
    char tmp[260];
    short len;
    int read = 0;

    read += fread (&m_nLevel, sizeof m_nLevel, 1, fp);
    read += fread (&len, sizeof len, 1, fp);
    read += fread (tmp, len + 1, 1, fp);
    if (setName (tmp))
        read = 0;

    return (feof (fp) ? 0 : read);
}

DirEntry *pNextFree = 0;
DirEntry *pLastNode = 0;

int g_nCount = 0;
short g_nMaxLevel = 0;

void pathNameFromNode (char *str, DirEntry *pNode)
{
    DirEntry *p;
    char tmp[260];

    p = pNode;
    if (p->m_szFileName[1] == '\\')
    {
        *str = '\0';
    }
    else
    {
        strcpy (str, p->m_szFileName);
        do
        {
            p = p->m_parent;
            if (p)
            {
                sprintf (tmp, "%s\\%s", p->m_szFileName, str);
                strcpy (str, tmp);
            }
        } while (p);
    }
}

int findFromList (
    DirEntry   *pRoot,
    const char *pszPath,
    int         nMatch,
    char        cDrive,
    StrEntry   *pList)
{
    if (pszPath == 0 || *pszPath =='\0')
    {
        return 0;
    }
    DirEntry *pNode = pRoot;
    char str[260];
    char tmp[260];
    int found = 0;
    int rc;
    int len = strlen (pszPath);
    StrEntry *pFree = pList;

    while (pNode)
    {
        if (nMatch == 0)       // prefix match
        {
            rc = (0 == strnicmp (pNode->m_szFileName, pszPath, len));
        }
        else if (nMatch == 1)  // exact match
        {
            rc = (0 == stricmp (pNode->m_szFileName, pszPath));
        }
        else                  // substring match
        {
            rc = (0 != stristr (pNode->m_szFileName, pszPath));
        }
        if (rc)
        {
            pathNameFromNode (str, pNode);

            found++;

            if (pList)
            {
                StrEntry *ne = new StrEntry();
                if (ne == 0)
                {
                    fprintf (stderr, "find: memory error in loop %d\n", found);
                    return found;
                }
                sprintf (tmp, "%c:\\%s", cDrive, str);
                if (pFree->setName (tmp))
                {
                    fprintf (stderr, "find: memory error in loop %d\n", found);
                    return found;
                }
                pFree->m_next = ne;
                pFree = ne;
            }
            else
            {
                fprintf (stderr, "%c:\\%s\n", cDrive, str);
            }
        }

        pNode = pNode->m_next;
    }

    return found;
}

int makeTreeList (
    DirEntry   *pRoot,
    char      **pList)
{
    DirEntry *pNode = pRoot;
    DirEntry *p;
    char str[260];
    short i;
    int count = 0;
    short flag;

    while (pNode)
    {
        str[0] = '\0';

        if (pNode->m_szFileName[1] != '\\')
        {
            // This loop checks in each level if there is a node later in list
            // that is in same level. If one can be found, print a leading bar,
            // if not, print leading space.
            for (i = 0; i < pNode->m_nLevel+1; i++)
            {
                flag = 0;
                p = pNode;
                do
                {
                    if (p->m_next && p->m_next->m_nLevel == i)
                    {
                        flag = 1;
                    }
                    p = p->m_next;
                } while (!flag && p && p->m_nLevel >= i);

                if (i == pNode->m_nLevel)
                {
                    if (flag)
                        strcat (str, " ÃÄÄ ");
                    else
                        strcat (str, " ÀÄÄ ");
                }
                else
                {
                    if (flag)
                        strcat (str, " ³  ");
                    else
                        strcat (str, "    ");
                }
            }
        }

        strcat (str, pNode->m_szFileName);

        pList[count] = new char [strlen (str) + 1];
        if (pList[count] == 0)
        {
            fprintf (stderr, "tree: memory error in loop %d\n", count);
            return count;
        }
        strcpy (pList[count], str);

        count++;

        pNode = pNode->m_next;
    }

    pList[count] = 0;
    return count;
}

int saveList (const char *szDataFile, DirEntry *pRoot)
{
    DirEntry *pNode = pRoot;
    FILE *fp;
    fp = fopen (szDataFile, "rb");
    if (fp != NULL)
    {
        // file exists - it must be deleted.
        fclose (fp);
        remove (szDataFile);
    }

    fp = fopen (szDataFile, "ab");  // open binary file for writing append mode.
    if (fp == NULL)
    {
        fprintf (stderr, "Error: cannot save file %s\n", szDataFile);
        return 1;
    }

    while (pNode)
    {
        pNode->save (fp);
        pNode = pNode->m_next;
    }

    fclose (fp);

    return 0;
}

void freeList (DirEntry *pRoot)
{
    DirEntry *p = pRoot;
    while (p)
    {
        DirEntry *pNode = p;
        p = p->m_next;
        delete pNode;
    }
}

void scanDirs(const char *pszSearchPath, short nLevel, DirEntry *parent)
{
    FFbuff Buff;
    int status;
    char szFileSpec[260];

    if (nLevel > g_nMaxLevel)
        g_nMaxLevel = nLevel;

    strcpy (szFileSpec, pszSearchPath);
    strcat (szFileSpec, "*");

    status = Buff.FindFirst (szFileSpec, FFbuff::ATTR_ONLY_DIR);

    while(!status)
    {
        if (Buff.IsDirectory() && ! Buff.IsDotName())
        {
            DirEntry *pNode = new DirEntry();
            if (pNode == 0)
            {
                fprintf (stderr, "scan: memory error in loop %d\n", g_nCount);
                return;
            }
            DirEntry *p = pNextFree;
            if (p->setName (Buff.GetName()))
            {
                fprintf (stderr, "scan: memory error in loop %d\n", g_nCount);
                return;
            }
            p->m_nLevel = nLevel;
            p->m_parent = parent;
            p->m_next = pNode;
            pLastNode = p;
            pNextFree = pNode;

            g_nCount++;

            sprintf(szFileSpec, "%s%s/", pszSearchPath, Buff.GetName());
            scanDirs(szFileSpec, nLevel + 1, p);
        }

        status = Buff.FindNext();
    }

    Buff.FindClose();
}

int scanList (
    DirEntry   *pRoot,
    const char *szDataFile,
    char        cDrive)
{
    int rc = 0;
    char szSearchPath[] = " :\\";
    szSearchPath[0] = cDrive;

    fprintf (stderr, "Scanning drive %c:\\\n", cDrive);

    pNextFree = pRoot;

    scanDirs (szSearchPath, 0, 0);

    pLastNode->m_next = 0;
    delete pNextFree;
    pNextFree = 0;

    if (g_nCount)
    {
        rc = saveList (szDataFile, pRoot);
    }

    return rc;
}

int readList (const char *szDataFile, DirEntry *pRoot)
{
    pLastNode = pRoot;
    pNextFree = pRoot;
    DirEntry data;
    DirEntry *pNode;
    DirEntry *parent = 0;
    size_t len;
    FILE *fp;

    g_nCount = 0;

    fp = fopen (szDataFile, "rb");
    if (fp == NULL)
    {
        fprintf (stderr, "file %s does not exist\n", szDataFile);
        return 1;
    }

    do
    {
        if (0 != (len = data.read (fp)))
        {
            pNode = new DirEntry();
            if (pNode == 0)
            {
                fprintf (stderr, "memory error in loop %d\n", g_nCount);
                return 1;
            }
            *pNextFree = data;
            pNextFree->m_next = pNode;

            if (data.m_nLevel == 0)
            {
                parent = 0;
            }
            else
            {
                if (data.m_nLevel > pLastNode->m_nLevel)
                {
                    parent = pLastNode;
                }
                else if (data.m_nLevel < pLastNode->m_nLevel)
                {
                    DirEntry *p = pLastNode;
                    while (p->m_nLevel != data.m_nLevel)
                    {
                        p = p->m_parent;
                    }
                    parent = p->m_parent;
                }

                pNextFree->m_parent = parent;
            }

            pLastNode = pNextFree;
            pNextFree = pNode;

            g_nCount++;
        }
    } while (len);

    fclose (fp);                  // close file

    pLastNode->m_next = 0;
    delete pNextFree;
    pNextFree = 0;

    return 0;
}

int showListBox (const char *pszTitle, int found, char **pList)
{
    vm_init();
    vm_setcursorstyle(CURSORHIDE);

    int left = 4;
    int top  = 4;
    int height = vm_getscreenheight() - (top  * 2);
    int width  = vm_getscreenwidth()  - (left * 2);

    if (height > found + 1)
    {
        top = (vm_getscreenheight() - found - 1) / 2;
        height = found + 1;
    }

    vm_listbox_init (vm_mkcolor(WHITE, CYAN), vm_mkcolor(YELLOW, CYAN), vm_mkcolor(BLACK, LIGHTGREY));
    int ret = vm_listbox(pszTitle, left, top, width, height, 0, pList);

    vm_done();

    return ret;
}

int main(int argc, char **argv)
{
    char szDir[260];
    char szPath[260] = "";
    char szDataFile[] = " :\\jlcd.dat";
    int ret = 0;
    int bRescan = 0;
    int bExactMatch = 0;
    int bSubstrMatch = 0;
    char cDrive = getdisk() + 'A';

    argv++;
    argc--;

    while (argc > 0 && (**argv == '-' || **argv == '/'))
    {
        (*argv)++;
        while (**argv != '\0')
        {
            switch (**argv)
            {
            case 'r': bRescan      = 1; break;
            case 'x': bExactMatch  = 1; break;
            case 's': bSubstrMatch = 1; break;

            case 'h':
            case '?':
                fprintf(stderr, "JLCD %s -- Change directory.\n", VERSION);
                fprintf(stderr, "USAGE: JLCD [-rxs] [d:][path]\n\n"
                    "-r   = rescan drive\n"
                    "-x   = exact match\n"
                    "-s   = substring match\n"
                    "d:   = drive to scan\n"
                    "path = partial path to change to\n"
                );
                return 1;

            default:
                fprintf (stderr, "JLCD: Unrecognized flag '%c' ignored.\n\n", **argv);
                break;
            }

            (*argv)++;
        }
        argv++;
        argc--;
    }

    if (argc)
    {
        strcpy (szDir, *argv);
    }
    else
    {
        strcpy (szDir, "");
    }

    char *pStr = szDir;
    if (szDir[1] == ':')
    {
        cDrive = szDir[0];
        pStr = szDir+2;
    }

    cDrive = toupper (cDrive);

    if (*pStr == '\\' || *pStr == '/')
        pStr++;

    strcpy (szPath, pStr);

    szDataFile[0] = cDrive;

    FILE *fp = fopen (szDataFile, "rb");
    if (fp == NULL)
    {
        fprintf (stderr, "File %s does not exist -- rescanning...\n", szDataFile);
        bRescan = 1;
    }
    else
    {
        fclose (fp);
    }

    DirEntry *pDirList = new DirEntry();

    if (bRescan)
    {
        ret = scanList (pDirList, szDataFile, cDrive);
    }
    else
    {
        ret = readList (szDataFile, pDirList);
    }

    if (ret == 0)
    {
        // add an extra node at top of the list.
        DirEntry *pNode = new DirEntry();
        pNode->setName (" \\");
        pNode->m_next = pDirList;
        pDirList = pNode;
        g_nCount++;

        int bTreeList = 0;
        char **pList = 0;
        int i;
        int found;

        if (szPath[0] == '\0')
        {
            pList = new char* [g_nCount + 1];

            found = makeTreeList (pDirList, pList);

            bTreeList = 1;
            ret = found;
        }
        else
        {
            StrEntry *p;
            StrEntry *pStrList = new StrEntry();

            found = findFromList (
                pDirList,
                szPath,
                (bExactMatch ? 1 : bSubstrMatch ? 2 : 0),
                cDrive,
                pStrList);

            if (found == 0)
            {
                fprintf (stderr, "None found\n");
                ret = -1;
            }
            else if (found == 1)
            {
                strcpy (szPath, pStrList->m_szFileName);
                ret = 0;
            }
            else
            {
                pList = new char* [found + 1];

                p = pStrList;
                for (i = 0; i < found; i++)
                {
                    pList[i] = new char [strlen (p->m_szFileName) + 1];
                    strcpy (pList[i], p->m_szFileName);
                    p = p->m_next;
                }
                pList[i] = 0;

                ret = found;
            }

            p = pStrList;
            while (p)
            {
                StrEntry *pNode = p;
                p = p->m_next;
                delete pNode;
            }
        }

        if (ret > 0)
        {
            char tmp[260] = "";

            sprintf (tmp, " JLCD %s -- %c: %d directories ", VERSION, cDrive, found);

            ret = showListBox (tmp, ret, pList);

            if (ret != -1)
            {
                if (bTreeList)
                {
                    i = 0;
                    DirEntry *p = pDirList;
                    while (p)
                    {
                        if (i == ret)
                        {
                            pathNameFromNode (tmp, p);
                            break;
                        }
                        i++;
                        p = p->m_next;
                    }
                    sprintf (szPath, "%c:\\%s", cDrive, tmp);
                }
                else
                {
                    strcpy (szPath, pList[ret]);
                }
            }

            for (i = 0; i < found; i++)
            {
                delete [] pList[i];
            }
            delete [] pList;
        }

        if (ret != -1)
        {
#if !defined (__386__) && !defined (WINNT)
            if (_osmode == 0)
            {
                setdisk (szPath[0] - 'A');
                ret = chdir (szPath);
                if (ret != 0)
                {
                    fprintf (stderr, "Cannot change to '%s'\n", szPath);
                }
            }
            else
#endif
            {
                printf ("%c: & cd \"%s\"\n", szPath[0], szPath);
            }
        }
    }

    freeList (pDirList);

#ifdef __IBMCPP__
#ifdef __DEBUG_ALLOC__
    _dump_allocated(-1);
#endif
#endif

    return (ret == -1);
}
