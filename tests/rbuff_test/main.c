#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "../../rbuff.h"


int main()
{
    rbuff_t rbuff;
    uint8_t buff[30];

    uint8_t *set_a;
    uint8_t *set_b;

    uint32_t read;
    uint32_t write;
    uint32_t i, n;

    printf("Test byl spusten\n");

    set_a = (uint8_t*) malloc(8000000);
    if(set_a == NULL) exit (1);

    set_b = (uint8_t*) malloc(8000000);
    if(set_b == NULL) exit (1);

    srand (time(NULL));

    for(i = 0; i < 8000000; i++)
    {
        set_a[i] = rand() % 256;
    }

    printf("Testovaci sada byla vytvorena\n");

    rbuff_init(&rbuff, buff, 30);

    read = 0;
    write = 0;

    do
    {
        n = rand() % 40;
        if(read + n >= 8000000) n = 8000000 - read;
        n = rbuff_write(&rbuff, set_a + read, n);
        read += n;

        n = rand() % 40;
        if(write + n >= 8000000) n = 8000000 - write;

        n = rbuff_read(&rbuff, set_b + write, n);
        write += n;
    }
    while(write < 8000000);

    for(i = 0; i < 8000000; i++)
    {
        if(set_a[i] != set_b[i])
        {
            printf("Testovaci mnoziny nejsou stejne!\n");
            break;
        }
    }
    if(i == 8000000) printf("Testovaci mnoziny jsou stejne\n");

    printf("Test byl dokoncen\n");

    free(set_a);
    free(set_b);

    return 0;
}
