#include "stdio.h"
#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_tae_service.h"

int secret_num = 4;
bool initialized = false;
sgx_mc_uuid_t mcid;
uint32_t mc=0;
sgx_time_t timer_start;
sgx_time_source_nonce_t nonce;

#define TIME_LIMIT 10    //~seconds
#define ATTEMPT_LIMIT 3

bool init()
{
    sgx_status_t ret = SGX_SUCCESS;

    ret = sgx_create_pse_session();
    if(ret != SGX_SUCCESS){oPrintStatus(ret); return false;}
    
    ret = sgx_create_monotonic_counter(&mcid, &mc); 
    if(ret != SGX_SUCCESS){oPrintStatus(ret); return false;}

    ret = sgx_get_trusted_time(&timer_start, &nonce);
    if(ret != SGX_SUCCESS){oPrintStatus(ret); return false;}

    initialized = true;
    return true; 
}

void ecall_Guess(int guess, int *response)
{
    sgx_status_t ret = SGX_SUCCESS;
    sgx_time_t now = 0;

    *response = 100; //initialization failure
    if( not initialized){
        if(not init()){
            return;
        }
    }

    *response = 110; //increment counter failed
    ret = sgx_increment_monotonic_counter(&mcid, &mc);
    if(ret != SGX_SUCCESS){oPrintStatus(ret); return;}

    *response = 120; //get time failed
    ret = sgx_get_trusted_time(&now, &nonce);
    if(ret != SGX_SUCCESS){oPrintStatus(ret); return;}

    *response = 130; //Attempt limit exceeded
    if(mc > ATTEMPT_LIMIT) {return;}

    *response = 140; //Spacetime anomaly check Tardis diagnostics
    if(now < timer_start) {return;}

    *response = 150; //Time expired
    if(now - timer_start > TIME_LIMIT) {return;}

    *response = 160; //Unknown error
    if(guess < secret_num){ 
        *response = -1;
    } else if( guess == secret_num) { 
        *response = 0;
    } else if( guess > secret_num) {
        *response = 1;
    }
}

