#include "stdio.h"
#include "Enclave_t.h"
#include "sgx_trts.h"

int secret_num = 0;
bool initialized = false;

#define TIME_LIMIT 30    //~seconds
#define ATTEMPT_LIMIT 8

bool init()
{
    unsigned char rand;
    sgx_status_t ret = SGX_SUCCESS;

    ret = sgx_read_rand(&rand , 1);
    if(ret != SGX_SUCCESS){oPrintStatus(ret); return false;}
    secret_num = (int) rand;

   initialized = true;
    return true; 
}

void ecall_Guess(int guess, int *response)
{
    sgx_status_t ret = SGX_SUCCESS;

    *response = 100; //initialization failure
    if( not initialized){
        if(not init()){
            return;
        }
    }

   *response = 160; //Unknown error
    if(guess < secret_num){ 
        *response = -1;
    } else if( guess == secret_num) { 
        *response = 0;
    } else if( guess > secret_num) {
        *response = 1;
    }
}

