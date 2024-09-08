#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum parseSta {
    idle = 0,
    dir_get,
    num1_get,
    num_all_get,
    err,
};


int main()
{
    char ch;
    char dir = 'A';
    int sta = idle;
    int val = 0;
    int x = 0;
    int y = 0;

    while ((ch = getchar()) != EOF)
    {
        switch (sta)
        {
        default:
        case idle:
            if (ch == ';')
            {
                continue;
            }
            if (ch == 'A' || ch == 'S' || ch == 'D' || ch == 'W')
            {
                dir = ch;
                sta = dir_get;
            }
            else
            {
                goto ERR;
            }
            break;

        case dir_get:
            if (ch >= '0' && ch <= '9')
            {
                val = ch - '0';
                sta = num1_get;
            }
            else
            {
                goto ERR;
            }
            break;

        case num1_get:
            if (ch >= '0' && ch <= '9')
            {
                val = val * 10 + ch - '0';
                sta = num_all_get;
            }
            else if (ch == ';')
            {
                goto FIN;
            }
            else
            {
                goto ERR;
            }
            break;

        case num_all_get:
            if (ch == ';')
            {
            FIN:
                sta = idle;
                switch (dir)
                {
                case 'A':
                    x -= val;
                    break;

                case 'S':
                    y -= val;
                    break;

                case 'D':
                    x += val;
                    break;

                case 'W':
                    y += val;
                    break;
                }
            }
            else
            {
                goto ERR;
            }
            break;

        ERR:
        case err:
            sta = err;
            if (ch == ';')
            {
                sta = idle;
            }
            break;
        }
    }

    printf("%d,%d", x, y);

    return 0;
}
