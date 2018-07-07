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
    if(type == 0)
    {
        return exactmatch(pattern, text);
    }

    if(type == 1)
    {
        int distance = levDistance(pattern, text);
        if(distance > k) return -1;
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


//http://www.talkativeman.com/levenshtein-distance-algorithm-string-comparison/
//source - base string, target - string to be modified
int levDistance(string source, string target)
{
    const int slen = source.length();
    const int tlen = target.length();

    if(slen == 0 ) return tlen;
    if(tlen == 0) return slen;


    typedef vector< vector<int> > TMatrix;

    TMatrix matrix(slen + 1);

    for(int i = 0; i <= slen; i++ )
    {
        matrix[i].resize( tlen + 1 );
        
    }

    for(int i = 0; i <= slen; i++ )
    {
        matrix[i][0] = i;
    }

    for(int i = 0; i <= tlen; i++ )
    {
        matrix[0][i] = i;
    }

    for(int i = 1; i <= slen; i++ )
    {
        const char s_ = source[i-1];
        for(int j = 1; j <= tlen; j++ )
        {
            const char t_ = target[j-1];

            int cost;

            if(s_ == t_) cost = 0;
            else cost = abs( (int) s_ - t_ );


            int above  = matrix[i-1][j];
            int left = matrix[i][j-1];
            int diag = matrix[i-1][j-1];

            int cell = min( above + 1, min (left + 1, diag + cost));


            if( i > 2 && j > 2){
                int transposition = matrix[i-2][j-2] + 1;

                if(source[i-2] != t_) transposition++;
                if(s_ != target[j-2]) transposition ++;

                if(cell > transposition) cell = transposition;
            }

            matrix[i][j] = cell;
        }
    }

    return matrix[slen][tlen];

}

