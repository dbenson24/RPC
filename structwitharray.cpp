#include "structwitharray.idl"

int summation(s arrays) {
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += arrays.m1[i];
        for (int j = 0; j < 10; j++) {
            sum += arrays.m2[i][j];
            for (int k = 0; k < 100; k++) {
                sum += arrays.m3[i][j][k];
            }
        }
    }

    return sum;
}
