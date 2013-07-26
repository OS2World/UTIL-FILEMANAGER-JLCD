/*
 * VM_LB.C
 * VidMgr Listbox
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
#include <ctype.h>

#include "vidmgr.h"

#define VM_KEY_DOWN  0x5000
#define VM_KEY_UP    0x4800
#define VM_KEY_RIGHT 0x4D00
#define VM_KEY_LEFT  0x4B00
#define VM_KEY_PGDN  0x5100
#define VM_KEY_PGUP  0x4900
#define VM_KEY_HOME  0x4700
#define VM_KEY_END   0x4F00

static char lb_text    = vm_mkcolor(LIGHTCYAN, BLUE);
static char lb_frame   = vm_mkcolor(WHITE,     BLUE);
static char lb_hilight = vm_mkcolor(BLACK,     LIGHTGREY);

static void vm_listbox_frame(int left, int top, int width, int height, int attr)
{
  //enum ListBoxframe {TLC = 218, TL = 196, TRC = 191, BLC = 212, BL = 205, BRC = 190, UPR = 179};
  //enum ListBoxframe {TLC = 'Ú', TL = 'Ä', TRC = '¿', BLC = 'Ô', BL = 'Í', BRC = '¾', UPR = '³'};
    enum ListBoxframe {TLC = 'É', TL = 'Í', TRC = '»', BLC = 'È', BL = 'Í', BRC = '¼', UPR = 'º'};
    int ct = 0;

    vm_xputch (left, top, attr, TLC);
    vm_horizline (left + 1, left + width - 1, top, attr, TL);
    vm_xputch (left + width, top , attr, TRC);
    for (ct = top + 1; ct < top + height; ct++)
    {
        vm_xputch (left, ct, attr, UPR);
        vm_xputch (left + width, ct, attr, UPR);
    }
    vm_xputch (left, top + height, attr, BLC);
    vm_horizline (left + 1, left + width, top + height, attr, BL);
    vm_xputch (left + width, top + height, attr, BRC);

}

static void vm_listbox_paint_list(int left, int top, int width, int height, int top_item, int shift, char *list[])
{
    char **p;
    char *str;
    int cury;
    int len;

    cury = top;
    p = &list[top_item];
    while(*p && cury <= top + height)
    {
        vm_horizline(left, left + width - 1, cury, lb_text, ' ');
        str = *p;
        len = strlen (str);
        vm_xprintf(left, cury, lb_text, "%.*s", width, (shift > len ? "" : str + shift));
        cury++;
        p++;
    }
}

int vm_listbox(const char *title, int x1, int y1, int w1, int h1, int def, char *list[])
{
    char **p;
    int left, top, width, height, right;
    int cury = 0, done = -2, ch, top_item = 0, cursel = 0;
    int shift = 0;
    int longest = 0;
    int items = 0;
    char *savebuf = (char*) malloc(sizeof(char) * ((w1 + 1) * (h1 + 1)) * 2);

    p = list;
    while(*p)
    {
        if (strlen (*p) > longest)
            longest = strlen (*p);
        p++;
        items++;
    }

    vm_gettext(x1, y1, x1 + w1, y1 + h1, savebuf);
    vm_listbox_frame(x1, y1, w1, h1, lb_frame);

    if (title && *title)
    {
        int len = strlen (title);
        vm_xputs (x1+(w1-len)/2, y1, lb_frame, title);
    }

    left   = x1 + 1;
    top    = y1 + 1;
    height = h1 - 2;
    width  = w1 - 1;
    right  = left + width - 1;

    top_item = def > height ? def - height : 0;
    vm_listbox_paint_list(left, top, width, height, top_item, 0, list);
    cursel = def;
    cury = def > height ? top + height : top + def;
    vm_paintbox(left, cury, right, cury, lb_hilight);

    while(done == -2)
    {
        ch = vm_getch();

        switch(ch)
        {
        case  13:
            done = cursel;
            break;

        case  27:
            done = -1;
            break;

        case VM_KEY_PGDN:
            if (cursel < items - 1)
            {
                cury = top;
                top_item += height;
                cursel = top_item;
                if (top_item + height > items - 1)
                {
                    cury = top + height;
                    top_item = items - height - 1;
                    cursel = top_item + height;
                }
                vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                vm_paintbox(left, cury, right, cury, lb_hilight);
            }
            break;

        case VM_KEY_PGUP:
            if(cursel > 0)
            {
                cury = top;
                top_item -= height;
                cursel = top_item;
                if (top_item < 0)
                {
                    top_item = cursel = 0;
                }
                vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                vm_paintbox(left, cury, right, cury, lb_hilight);
            }
            break;

        case  VM_KEY_LEFT:
            if(shift > 0)
                shift--;
            vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
            vm_paintbox(left, cury, right, cury, lb_hilight);
            break;

        case  VM_KEY_RIGHT:
            if (shift < longest-width)
                shift++;
            vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
            vm_paintbox(left, cury, right, cury, lb_hilight);
            break;

        case VM_KEY_UP:
            if(cursel > 0)  /* process up arrow */
            {
                if(cury > top)  /* move up */
                {
                    vm_paintbox(left, cury, right, cury, lb_text);
                    cury--;
                    cursel--;
                }
                else /* scroll */
                {
                    top_item--;
                    cursel--;
                    vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                    cury = top;
                }
                vm_paintbox(left, cury, right, cury, lb_hilight);
                break;
            }
            /* roll over, Beethoven! */

        case VM_KEY_END:
            if (cursel < items - 1)
            {
                cury = top + height;
                top_item = items - height - 1;
                cursel = top_item + height;
                vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                vm_paintbox(left, cury, right, cury, lb_hilight);
            }
            break;

        case VM_KEY_DOWN:
            if(list[cursel + 1] != NULL)
            {
                if(cursel - top_item < height)
                {
                    vm_paintbox(left, cury, right, cury, lb_text);
                    cury++;
                    cursel++;
                }
                else  /* scroll */
                {
                    top_item++;
                    cursel++;
                    vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                    cury = top + height;
                }
                vm_paintbox(left, cury, right, cury, lb_hilight);
                break;
            }
            /* roll over, Beethoven! */

        case VM_KEY_HOME:
            if(cursel > 0)
            {
                cury = top;
                top_item = cursel = 0;
                vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                vm_paintbox(left, cury, right, cury, lb_hilight);
            }
            break;

        default:
            if (ch >= 32 && ch <= 255)
            {
                int found = 0;
                int oldsel = cursel;
                int oldcury = cury;

                p = &list[cursel+1];
                while(*p)
                {
                    if (0 != strchr (*p, ch) || 0 != strchr (*p, toupper(ch)))
                    {
                        cury++;
                        cursel++;
                        found = 1;
                        break;
                    }
                    cury++;
                    cursel++;
                    p++;
                }

                if (found)
                {
                    if (cursel - top_item > height)
                    {
                        top_item = top_item + (cursel - oldsel);
                        if (top_item + height > items - 1)
                        {
                            top_item = items - height - 1;
                        }
                        cursel = top_item + height;
                        cury = top + height;
                    }
                }
                else
                {
                    cursel = oldsel;
                    cury = oldcury;
                }

                vm_listbox_paint_list(left, top, width, height, top_item, shift, list);
                vm_paintbox(left, cury, right, cury, lb_hilight);
            }

            break;
        }
    }

    vm_puttext(x1, y1, x1 + w1, y1 + h1, savebuf);
    free(savebuf);

    return done;
}

void vm_listbox_init (char t, char f, char h)
{
    lb_text    = t;
    lb_frame   = f;
    lb_hilight = h;
}

#ifdef TEST_LB

int main()
{
    char *s[] =
    {
        "Daffy",
        "Donald",
        "Sylvester",
        "Tweetie",
        "Garfield",
        "Bart",
        "Homer",
        "Jerry",
        "Tom",
        "Gotta stop watching cartoons",
        NULL
    };

    int done = 0, i = 0;
    int def = 0;

    vm_init();
    vm_setcursorstyle(CURSORHIDE);

    while(!done)
    {
        vm_printf(1, 1, "Test #%d ", ++i);
        vm_fillbox(1, 3, 80, 20, '.');
        done = vm_listbox("Title", 5, 5, 15, 8, def, s);
        def = done;
        vm_paintclearbox(1, 1, 60, 2, LIGHTGRAY | BLACK);
        vm_printf(1, 2, "ListBox : %02d = %s", done, done >= 0 ? s[done] : "ESC         ");

        if(done != -1)
            done = 0;
    }

    vm_done();

    return 0;
}

#endif
