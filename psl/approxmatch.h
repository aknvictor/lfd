#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

int exactmatch(string pattern, string text);

//if k=0 exactmatch
int dynamicprogramming(string pattern, string text, int k);

int galilpark(char** pattern, char** text, int k);

int ukkonenwood(char** pattern, char** text, int k);

int boyermoore(char** pattern, char** text, int k);

int exactmatch_(string  pattern, string  text);

int getListLen(string list);

int approxmatch(string  pattern, string  text, int k, int type);
