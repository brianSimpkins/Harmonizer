#include <stdint.h>

//gets maximum index of a 64 length array of uint16_t
uint8_t getMax(uint16_t * arr)
{
    uint16_t max = arr[0];
    uint8_t maxIndex = 0;

    for (int i = 0; i < 63; i++) 
    {
        if (arr[i] > max)
        {
            max = arr[i];
            maxIndex = i;
        }
    }
    return maxIndex;
    
}

/*
TESTER CODE
*/
int main(){
    uint16_t arr[64] = {1,2,3,4,5};
    uint8_t m = getMax(arr);
}