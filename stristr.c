/*
 From : Mark Kimes                  1:380/16        19 Feb 95  11:52:20
 To   : Fred Cole
 Subj : Re: Wildcards
*/

/************** Standard Bob Stout Notice ******************/
/*                                                         */
/* Below code released to public domain.                   */
/*                                                         */
/* Modified 05 Sep 1998 by Jari Laaksonen                  */
/* - checks NULL and empty strings                         */
/*                                                         */
/***********************************************************/

#include <stdlib.h>
#include <ctype.h>

char *stristr (const char *t, const char *s)
{
    char *t1,*tt;
    char *s1,*ss;

    if (!t || !s)
        return NULL;

    if (*s == '\0')
        return (char *)t;

    tt = t;
    ss = s;
    while(*tt)
    {
        t1 = tt;
        s1 = ss;
        while(*s1)
        {
            if (toupper(*s1) != toupper(*tt))
                break;
            else
            {
                s1++;
                tt++;
            }
        }
        if (!*s1)
            return t1;
        tt = ++t1;
    }
    return NULL;
}

#ifdef TEST

#include <stdio.h>
#include <string.h>

int main (void)
{
    char buffer[80] = "heLLo, HELLO, hello, hELLo, HellO";
    char *sptr = buffer;

    while (0 != (sptr = strstr(sptr, "hello")))
    {
        printf("Found '%5.5s'\n", sptr++);
    }

    sptr = strstr ("hello", "");
    printf ("'%s'\n", sptr);

    sptr = buffer;
    while (0 != (sptr = stristr(sptr, "hello")))
    {
        printf("Found %5.5s!\n", sptr++);
    }

    sptr = stristr ("hello", "");
    printf ("'%s'\n", sptr);

    sptr = stristr ("hello", 0);
    printf ("'%s'\n", sptr);

    sptr = stristr (0, 0);
    printf ("'%s'\n", sptr);

    return 0;
}
#endif
