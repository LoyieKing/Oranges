#include "string.h"
#include "type.h"

uint32 strlen(char *string)
{
    uint32 len = 0;
    for (; string[len]; len++)
        ;
    return len;
}
int32 strcmp(char *s1, char *s2)
{
    char *p1;
    char *p2;
    p1 = s1;
    p2 = s2;
    while ((*p1) && (*p2))
    {
        if (*p1 == *p2)
        {
            p1++;
            p2++;
        }
        else
        {
            return (*p1 - *p2);
        }
    }
    return (*p1 - *p2);
}