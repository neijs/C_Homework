#include <stdio.h>
void sort_by_bits(size_t count, unsigned *data) {
    int i,j,k,bits1,bits2;
    unsigned mask;
    unsigned tmp, tmp1, tmp2;
    for (i = 0; i < count - 1; i++) {
        for (j = count - 1; j > i; j--) {
            mask = 0x8000;
            bits1 = 0;
            bits2 = 0;
            tmp1 = data[j - 1];
            tmp2 = data[j];
            for (k = 15; k >= 0; k--, mask >>= 1)
                bits1 += (tmp1 & mask) >> k;
            mask = 0x8000;  
            for (k = 15; k >= 0; k--, mask >>= 1)
                bits2 += (tmp2 & mask) >> k;
            if (bits1 > bits2) {
                tmp = data[j-1];
                data[j-1] = data[j];
                data[j] = tmp;
            } else if (bits1 == bits2) {
                if (data[j - 1] < data[j]) {
                        tmp = data[j-1];
                        data[j-1] = data[j];
                        data[j] = tmp;
                }
            }
        }
    }
}

int main() {
    unsigned data[15] = {5,4,3,2,1,7,8,9,10,56, 16, 32, 64, 128, 512};
    sort_by_bits(15, data);
    for (int i = 0; i < 15; i++) {
        printf ("%u\n", data[i]);
    }
    return 0;
}
