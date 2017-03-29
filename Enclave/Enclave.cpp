#include "Enclave_t.h"
#include "sgx_trts.h"

int secret_num = 3;

void ecall_Guess(int guess, int *answer)
{
    if(guess < secret_num){ 
        *answer = -1;
    } else if( guess == secret_num) { 
        *answer = 0;
    } else if( guess > secret_num) {
        *answer = 1;
    }
}

