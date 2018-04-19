#include "approxmatch.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int getListLen(string list)
{
    return list.length();
}

char get_char_by_index(string head, int n)
{
    return head[n];
}

int approxmatch(string  pattern, string  text, int k, int type)
{

    if(k == 0)
    {
        return exactmatch(pattern, text);
    }

    return 0;
}

int exactmatch(string  pattern, string  text)
{
    if(pattern == text)
        return 0;

    else return -1;
}

int dynamicprogramming(string  pattern, string  text, int k)
{
    int psz = getListLen(pattern);
    int tsz = getListLen(text);

    int ** D = new int*[psz +1];

    D[0] = new int[tsz +1];
    
    for(int j =0; j <= tsz; j++)
    {
        D[0][j] = 0;
    }

    for(int i =1; i <= psz; i++)
    {
        D[i] = new int[tsz +1];
        for(int j =0; j <= tsz; j++)
        {
            if(j < 1 )  D[i][j] = D[i-1][j] + 1;
            else 
            {
                int o = (get_char_by_index(pattern, i) == get_char_by_index(text, j)) ? 0 : 1;
                int a = D[i-1][j] + 1;
                int b = D[i-1][j-1] + o;
                int c = D[i][j - 1] + 1;

                D[i][j] = MIN( a, b );
                D[i][j] = MIN( D[i][j], c  );
            }

        }
    }

    for(int i = 0; i <= psz; i++)
    {
        for(int j =0; j <= tsz; j++)
        {
            printf("%d", D[i][j]);
        }

        printf("\n");

    }

    for(int i = 0; i <= psz; i++)
    {
        delete[] D[i];
    }

    delete[] D;

    return 0;

}


