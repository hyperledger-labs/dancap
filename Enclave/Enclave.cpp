#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tae_service.h"

int secret_num = 3;
bool initialized = false;
sgx_mc_uuid_t mcid;
uint32_t mc=88;

bool init()
{
    sgx_status_t ret= SGX_SUCCESS;
    ret = sgx_create_pse_session();
    if(ret != SGX_SUCCESS){return false;}

    ret = sgx_create_monotonic_counter(&mcid, &mc); 
    if(ret != SGX_SUCCESS){return false;}

    ret = sgx_increment_monotonic_counter(&mcid, &mc);
    if(ret != SGX_SUCCESS){return false;}

    initialized = true;
    return true; 
}

void ecall_Guess(int guess, int *answer)
{
    *answer = 99;
    if( not initialized){
        if(not init()){
            return;
        }
    }
    sgx_increment_monotonic_counter(&mcid, &mc);
    *answer = mc;
    return;

    if(guess < secret_num){ 
        *answer = -1;
    } else if( guess == secret_num) { 
        *answer = 0;
    } else if( guess > secret_num) {
        *answer = 1;
    }
}

