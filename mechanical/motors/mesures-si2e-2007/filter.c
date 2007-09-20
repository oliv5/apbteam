#include <stdio.h>
#include <errno.h>

int
main (void)
{
    int l, r;
    int ret;
    short unsigned int l_in, r_in, l_last, r_last;
    short int l_diff, r_diff, l_diff_last, r_diff_last, l_a, r_a;
    l = r = 0;
    l_last = r_last = 0;
    l_diff_last = r_diff_last = 0;
    do
    {
        ret = scanf ("%hd %hd\n", &l_in, &r_in);
        if (ret == EOF)
            break;
        if (ret != 2)
        {
            fprintf (stderr, "parse error\n");
            return 1;
        }
        l_diff = l_in - l_last;
        r_diff = r_in - r_last;
        l += l_diff;
        r += r_diff;
        l_last = l_in;
        r_last = r_in;
        l_a = l_diff - l_diff_last;
        r_a = r_diff - r_diff_last;
        l_diff_last = l_diff;
        r_diff_last = r_diff;
        printf ("%d %d %d %d %d %d\n", l, r, l_diff, r_diff, l_a, r_a);
    } while (1);
    if (ferror (stdin))
    {
        perror ("stdin");
        return 1;
    }
    return 0;
}
