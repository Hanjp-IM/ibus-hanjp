#include "hanjp.h"
#include <stdio.h>
#include <wchar.h>

void TestChar();
void TestConv();

int main()
{
    //TestChar();
    //TestConv();
    return 0;
}

/*할 일:
문자 함수 검증
-TestChar
변환 함수 검증
-TestConv
*/

void TestChar()
{
    FILE *fp;

    fp = fopen("chartest.txt", "r");

    if(!fp)
    {
        printf("Fail to open file\n");
        return;
    }

    fclose(fp);
}

void TestChar()
{
    FILE *fp;

    fp = fopen("convtest.txt", "r");

    if(!fp)
    {
        printf("Fail to open file\n");
        return;
    }

    fclose(fp);
}