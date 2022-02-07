#include <stdio.h>
#include <string.h>
#define MAX_DIGITS 25
#define INPUT 999
const int BASE = 1000000000;
/*Every unsigned integer array in the structure stands for a number which is less than 10^9 (or it will overflow).
  fib(999) needs about 209 digits, so we set Max_Digits to be 25 that can represent 25*9 = 225 digits at most.*/

typedef struct {
    unsigned int big[MAX_DIGITS + 1];
} Big_Number;

// Addition of Big Numbers which are less or equal than 9 digits
void add(Big_Number *front, Big_Number *past, Big_Number *temp) {
    unsigned int sum = 0;
    int i = 0;
    memset(temp, 0, sizeof(Big_Number)); // c has to store the highest carry digit 
    for (i = 0; i < MAX_DIGITS; i++) {
        temp->big[i] = front->big[i] + past->big[i] + temp->big[i];
        temp->big[i + 1] = temp->big[i] / BASE;           // Give the carry number to the next digit
        temp->big[i] = temp->big[i] % BASE;               // Save the remainder
  }
}

void fib(int n) {
    Big_Number a, b, c;
    int i = 0;
    for (i = 0; i < MAX_DIGITS; i++) {  // Initialize the value
        a.big[i] = 0;
        b.big[i] = 0;
        c.big[i] = 0;
    }
    // First two fibonacci number
    a.big[0] = 0;
    b.big[0] = 1;

    for (i = 2; i <= n; i++) {
        add(&a, &b, &c);
        memcpy(&a, &b, sizeof(Big_Number));
        memcpy(&b, &c, sizeof(Big_Number));
    }
    for (i = MAX_DIGITS - 1; i > 0 && c.big[i] == 0; i--)
        ; // Search from highest digit to find which is not 0
    printf("%u", c.big[i]); i--; //print the highest digit which is not 0
    while (i--)
        printf("%09u", c.big[i]);  // print by every nine digits
    printf("\n");
}

int main(void)
{
    if (INPUT < 2) {
        printf("fib(%d) = %d\n", INPUT, INPUT);
    } else {
        printf("fib(%d) = ", INPUT);
        fib(INPUT);
    }
    return 0;
}
