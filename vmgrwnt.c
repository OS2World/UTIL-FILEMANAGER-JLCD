/* +++Date last modified: 03-Oct-1996 */

/*
 *  VMGRWNT.C; VidMgr module for Windows 95/NT compilers.  Release 1.3.
 *
 *  This module written in May 1996 by Andrew Clarke and released to the
 *  public domain.  Last modified in October 1996.
 *
 * Modified 05 Sep 1998 by Jari Laaksonen
 * - does not close input and output handles in vm_vi_done().
 * - speed enhancement in vm_horizline: uses vm_xputs instead of vm_xputch.
 *
 */

#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "vidmgr.h"

#ifdef __CYGWIN32__
typedef WORD far *LPWORD;
#endif

static HANDLE HInput = INVALID_HANDLE_VALUE;
static HANDLE HOutput = INVALID_HANDLE_VALUE;
static HANDLE hConsole = INVALID_HANDLE_VALUE;
static DWORD  oldConsoleMode;
static int    codepage;
static unsigned long key_hit = 0xFFFFFFFFUL;

static int video_init = 0;

#define vi_init()  if (!video_init) vm_vi_init()
#define vi_done()  if (video_init) vm_vi_done()

void vm_vi_init(void)
{
    if (! video_init)
    {
        DWORD dwMode;
        BOOL rc;

        HInput = GetStdHandle(STD_INPUT_HANDLE);
        HOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        codepage = GetConsoleCP();
        hConsole = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            0, NULL,
            CONSOLE_TEXTMODE_BUFFER, NULL);

        dwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
        rc = SetConsoleMode(hConsole, dwMode);

        SetConsoleActiveScreenBuffer(hConsole);
        GetConsoleMode(HInput, &oldConsoleMode);
        SetConsoleMode(HInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
        SetConsoleOutputCP(codepage);
        SetConsoleCP(codepage);

        video_init = 1;
    }
}

void vm_vi_done(void)
{
    SetConsoleActiveScreenBuffer(HOutput);
    SetConsoleMode(HInput, oldConsoleMode);
    CloseHandle(hConsole);
    //CloseHandle(HInput);
    HInput = INVALID_HANDLE_VALUE;
    //CloseHandle(HOutput);
    HOutput = INVALID_HANDLE_VALUE;
    video_init = 0;
}

void vm_init(void)
{
    vi_init();
    vm_getinfo(&vm_startup);
    vm_setattr(vm_startup.attr);
}

void vm_done(void)
{
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = (DWORD) vm_startup.dwSize;
    cci.bVisible = (BOOL) vm_startup.bVisible;
    SetConsoleCursorInfo(hConsole, &cci);
    vi_done();
}

void vm_getinfo(struct vm_info *v)
{
    CONSOLE_CURSOR_INFO cci;
    v->ypos = vm_wherey();
    v->xpos = vm_wherex();
    v->attr = vm_getattrxy(v->xpos, v->ypos);
    v->height = vm_getscreenheight();
    v->width = vm_getscreenwidth();
    GetConsoleCursorInfo(hConsole, &cci);
    vm_startup.dwSize = (unsigned short)cci.dwSize;
    vm_startup.bVisible = (int)cci.bVisible;
}

char vm_getscreenwidth(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return (char)csbi.dwSize.X;
}

char vm_getscreenheight(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return (char)csbi.dwSize.Y;
}

short vm_getscreensize(void)
{
    return (short)(vm_getscreenwidth() * vm_getscreenheight() * 2);
}

char vm_wherex(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return (char)(csbi.dwCursorPosition.X + 1);
}

char vm_wherey(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return (char)(csbi.dwCursorPosition.Y + 1);
}

void vm_gotoxy(char x, char y)
{
    COORD dwCursorPosition;
    dwCursorPosition.X = (SHORT) (x - 1);
    dwCursorPosition.Y = (SHORT) (y - 1);
    SetConsoleCursorPosition(hConsole, dwCursorPosition);
}

int vm_kbhit(void)
{
    int iKey = 0;
    INPUT_RECORD irBuffer;
    DWORD pcRead;

    if (key_hit != 0xFFFFFFFFUL)
    {
        return (int)key_hit;
    }

    memset(&irBuffer, 0, sizeof irBuffer);

    if (WaitForSingleObject(HInput, 0L) == 0)
    {
        ReadConsoleInput(HInput, &irBuffer, 1, &pcRead);
        if (irBuffer.EventType == KEY_EVENT &&
            irBuffer.Event.KeyEvent.bKeyDown != 0 &&
            irBuffer.Event.KeyEvent.wRepeatCount <= 1)
        {
            WORD vk, vs, uc;
            BOOL fShift, fAlt, fCtrl;

            vk = irBuffer.Event.KeyEvent.wVirtualKeyCode;
            vs = irBuffer.Event.KeyEvent.wVirtualScanCode;
#ifdef __CYGWIN32__
            uc = irBuffer.Event.KeyEvent.AsciiChar;
#else
            uc = irBuffer.Event.KeyEvent.uChar.AsciiChar;
#endif

            fShift = (irBuffer.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED);
            fAlt = (irBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_ALT_PRESSED + LEFT_ALT_PRESSED));
            fCtrl = (irBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED + LEFT_CTRL_PRESSED));

            if (uc == 0)
            {                   /* function keys */
                switch (vk)
                {
                case 0x21:     /* PgUp */
                    if (fCtrl)
                    {
                        vs = 0x84;  /* Ctrl+PgUp */
                    }
                    break;

                case 0x22:     /* PgDn */
                    if (fCtrl)
                    {
                        vs = 0x76;  /* Ctrl+PgDn */
                    }
                    break;

                case 0x23:     /* End */
                    if (fCtrl)
                    {
                        vs = 0x75;  /* Ctrl+End */
                    }
                    break;

                case 0x24:     /* Home */
                    if (fCtrl)
                    {
                        vs = 0x77;  /* Ctrl+Home */
                    }
                    break;

                case 0x25:     /* Left Arrow */
                    if (fCtrl)
                    {
                        vs = 0x73;  /* Ctrl+Left Arrow */
                    }
                    break;

                case 0x26:     /* Up Arrow */
                    if (fCtrl)
                    {
                        vs = 0x8d;  /* Ctrl+Up Arrow */
                    }
                    break;

                case 0x27:     /* Right Arrow */
                    if (fCtrl)
                    {
                        vs = 0x74;  /* Ctrl+Right Arrow */
                    }
                    break;

                case 0x28:     /* Down Arrow */
                    if (fCtrl)
                    {
                        vs = 0x91;  /* Ctrl+Down Arrow */
                    }
                    break;

                case 0x70:     /* F-Keys */
                case 0x71:
                case 0x72:
                case 0x73:
                case 0x74:
                case 0x75:
                case 0x76:
                case 0x77:
                case 0x78:
                case 0x79:
                    if (fAlt)
                    {
                        vs += 0x2d;  /* Alt+F-Key */
                    }
                    else if (fShift)
                    {
                        vs += 0x19;  /* Shift+F-Key */
                    }
                    break;
                }

                if (vk > 0x20 && vk < 0x92)  /* If it's OK use scan code */
                {
                    iKey = vs << 8;
                }
            }
            else
            {
                if (fAlt)       /* Alt+Key */
                {
                    iKey = vs << 8;
                }
                else if (fCtrl) /* Ctrl+Key */
                {
                    iKey = vk & 0xbf;
                }
                else
                {
                    iKey = uc;
                }
            }
        }
    }

    if (iKey != 0)
    {
        key_hit = iKey;
    }

    return (int)iKey;
}

int vm_getch(void)
{
    int iKey;
    while (key_hit == 0xFFFFFFFFUL)
    {
        vm_kbhit();
    }
    iKey = key_hit;
    key_hit = 0xFFFFFFFFUL;
    return (int)iKey;
}

char vm_getchxy(char x, char y)
{
    char ch;
    DWORD len;
    COORD coord;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    ReadConsoleOutputCharacterA(hConsole, &ch, 1, coord, &len);
    return ch;
}

char vm_getattrxy(char x, char y)
{
    DWORD len;
    COORD coord;
    WORD wattr;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    ReadConsoleOutputAttribute(hConsole, &wattr, 1, coord, &len);
    return (char)wattr;
}

void vm_xgetchxy(char x, char y, char *attr, char *ch)
{
    DWORD len;
    COORD coord;
    WORD wattr;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    ReadConsoleOutputCharacterA(hConsole, ch, 1, coord, &len);
    ReadConsoleOutputAttribute(hConsole, &wattr, 1, coord, &len);
    *attr = (char)wattr;
}

void vm_setcursorstyle(int style)
{
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hConsole, &cci);
    switch (style)
    {
    case CURSORHALF:
        cci.bVisible = 1;
        cci.dwSize = 49;
        SetConsoleCursorInfo(hConsole, &cci);
        break;
    case CURSORFULL:
        cci.bVisible = 1;
        cci.dwSize = 99;
        SetConsoleCursorInfo(hConsole, &cci);
        break;
    case CURSORNORM:
        cci.bVisible = 1;
        cci.dwSize = 12;
        SetConsoleCursorInfo(hConsole, &cci);
        break;
    case CURSORHIDE:
        cci.bVisible = 0;
        SetConsoleCursorInfo(hConsole, &cci);
        break;
    default:
        break;
    }
}

void vm_putch(char x, char y, char ch)
{
    DWORD len;
    COORD coord;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    WriteConsoleOutputCharacterA(hConsole, &ch, 1, coord, &len);
}

void vm_puts(char x, char y, char *str)
{
    DWORD len;
    COORD coord;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    WriteConsoleOutputCharacterA(hConsole, str, (DWORD) strlen(str), coord, &len);
}

void vm_xputch(char x, char y, char attr, char ch)
{
    DWORD len;
    COORD coord;
    WORD wattr;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    wattr = attr;
    WriteConsoleOutputCharacterA(hConsole, &ch, 1, coord, &len);
    WriteConsoleOutputAttribute(hConsole, &wattr, 1, coord, &len);
}

void vm_xputs(char x, char y, char attr, char *str)
{
    DWORD i, len;
    COORD coord;
    LPWORD pwattr;
    pwattr = malloc(strlen(str) * sizeof(*pwattr));
    if (!pwattr)
    {
        return;
    }
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    for (i = 0; i < strlen(str); i++)
    {
        *(pwattr + i) = attr;
    }
    WriteConsoleOutputCharacterA(hConsole, str, (DWORD) strlen(str), coord, &len);
    WriteConsoleOutputAttribute(hConsole, pwattr, (DWORD) strlen(str), coord, &len);
    free(pwattr);
}

void vm_putattr(char x, char y, char attr)
{
    DWORD len;
    COORD coord;
    WORD wattr;
    coord.X = (DWORD) (x - 1);
    coord.Y = (DWORD) (y - 1);
    wattr = attr;
    WriteConsoleOutputAttribute(hConsole, &wattr, 1, coord, &len);
}

void vm_paintclearbox(char x1, char y1, char x2, char y2, char attr)
{
    COORD coord;
    LPWORD pwattr;
    char y, *pstr;
    DWORD i, len, width;
    width = (x2 - x1 + 1);
    pwattr = malloc(width * sizeof(*pwattr));
    if (!pwattr)
    {
        return;
    }
    pstr = malloc(width);
    if (!pstr)
    {
        free(pwattr);
        return;
    }
    for (i = 0; i < width; i++)
    {
        *(pwattr + i) = attr;
        *(pstr + i) = ' ';
    }
    for (y = y1; y <= y2; y++)
    {
        coord.X = (DWORD) (x1 - 1);
        coord.Y = (DWORD) (y - 1);
        WriteConsoleOutputCharacterA(hConsole, pstr, width, coord, &len);
        WriteConsoleOutputAttribute(hConsole, pwattr, width, coord, &len);
    }
    free(pwattr);
    free(pstr);
}

void vm_paintbox(char x1, char y1, char x2, char y2, char attr)
{
    DWORD i, len, width;
    COORD coord;
    LPWORD pwattr;
    char y;
    width = (x2 - x1 + 1);
    pwattr = malloc(width * sizeof(*pwattr));
    if (!pwattr)
    {
        return;
    }
    for (i = 0; i < width; i++)
    {
        *(pwattr + i) = attr;
    }
    for (y = y1; y <= y2; y++)
    {
        coord.X = (DWORD) (x1 - 1);
        coord.Y = (DWORD) (y - 1);
        WriteConsoleOutputAttribute(hConsole, pwattr, width, coord, &len);
    }
    free(pwattr);
}

void vm_clearbox(char x1, char y1, char x2, char y2)
{
    vm_fillbox(x1, y1, x2, y2, ' ');
}

void vm_fillbox(char x1, char y1, char x2, char y2, char ch)
{
    DWORD i, len, width;
    COORD coord;
    char y, *pstr;
    width = (x2 - x1 + 1);
    pstr = malloc(width);
    if (!pstr)
    {
        return;
    }
    for (i = 0; i < width; i++)
    {
        *(pstr + i) = ch;
    }
    for (y = y1; y <= y2; y++)
    {
        coord.X = (DWORD) (x1 - 1);
        coord.Y = (DWORD) (y - 1);
        WriteConsoleOutputCharacterA(hConsole, pstr, width, coord, &len);
    }
    free(pstr);
}

void vm_gettext(char x1, char y1, char x2, char y2, char *dest)
{
    DWORD i, len, width;
    COORD coord;
    LPWORD pwattr;
    char y, *pstr;
    width = (x2 - x1 + 1);
    pwattr = malloc(width * sizeof(*pwattr));
    if (!pwattr)
    {
        return;
    }
    pstr = malloc(width);
    if (!pstr)
    {
        free(pwattr);
        return;
    }
    for (y = y1; y <= y2; y++)
    {
        coord.X = (DWORD) (x1 - 1);
        coord.Y = (DWORD) (y - 1);
        ReadConsoleOutputCharacterA(hConsole, pstr, width, coord, &len);
        ReadConsoleOutputAttribute(hConsole, pwattr, width, coord, &len);
        for (i = 0; i < width; i++)
        {
            *dest = *(pstr + i);
            dest++;
            *dest = (char)*(pwattr + i);
            dest++;
        }
    }
    free(pwattr);
    free(pstr);
}

void vm_puttext(char x1, char y1, char x2, char y2, char *srce)
{
    DWORD i, len, width;
    COORD coord;
    LPWORD pwattr;
    char y, *pstr;
    width = (x2 - x1 + 1);
    pwattr = malloc(width * sizeof(*pwattr));
    if (!pwattr)
    {
        return;
    }
    pstr = malloc(width);
    if (!pstr)
    {
        free(pwattr);
        return;
    }
    for (y = y1; y <= y2; y++)
    {
        for (i = 0; i < width; i++)
        {
            *(pstr + i) = *srce;
            srce++;
            *(pwattr + i) = *srce;
            srce++;
        }
        coord.X = (DWORD) (x1 - 1);
        coord.Y = (DWORD) (y - 1);
        WriteConsoleOutputCharacterA(hConsole, pstr, width, coord, &len);
        WriteConsoleOutputAttribute(hConsole, pwattr, width, coord, &len);
    }
    free(pwattr);
    free(pstr);
}

void vm_horizline(char x1, char x2, char row, char attr, char ch)
{
    char *str;
    char x;
    int width = x2 - x1 + 1;

    str = malloc (width+1);
    if (str)
    {
        memset (str, ch, width);
        str[width] = '\0';
        vm_xputs (x1, row, attr, str);
        free (str);
    }

    /*for (x = x1; x <= x2; x++)
    {
        vm_xputch(x, row, attr, ch);
    } */
}
