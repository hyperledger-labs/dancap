#include <iostream>
#include "sgx_urts.h"
#include "Enclave_u.h"


#define ENCLAVE_NAME "enclave.signed.so"
sgx_enclave_id_t global_eid = 0;

void oPrintStatus(sgx_status_t code){
    std::cout << "Enclave returned status: " << code << std::endl;;
}

void printErrorCode(int code){
    switch(code){
        case 100: std::cout << "Initialization Failure\n"; break;
        case 110: std::cout << "Increment Counter Failure\n"; break;
        case 120: std::cout << "Get Time Failure\n"; break;
        case 130: std::cout << "Attempt limit exceeded.\n"; break;
        case 140: std::cout << "Spacetime Anomaly check tardis diagnostics.\n"; break;
        case 150: std::cout << "Time limit exceeded. You are too slow.\n"; break;
        default: std::cout << "Unknown Error\n";
    }
}

int main()
{
    sgx_status_t ret = SGX_SUCCESS;
    sgx_launch_token_t token = { 0 };
    int updated = 0;

    int guess=0;
    int response=0;

    std::cout << "Loading secret enclave ...";

    ret = sgx_create_enclave(ENCLAVE_NAME, SGX_DEBUG_FLAG, &token, &updated, \
            &global_eid, NULL);

    if(ret != SGX_SUCCESS){
        std::cerr << "FAILED TO CREATE ENCLAVE. CODE: ";
        std::cerr << std::hex << ret << std::endl;
        return 1;
    }
    std::cout << "\nWelcome to Top Secret Number Guess\n";
    std::cout << "The number is between 0 and 255.\n";

    while(true){
        std::cout << "Enter Guess: ";
        std::cin >> guess; 
        ret =  ecall_Guess(global_eid, guess, &response);
        if( ret != SGX_SUCCESS){
            std::cerr << "ENCLAVE FAILURE!!!\n";
            std::cerr << ret << std::endl;
        }

        //Check failure condition
        if( response > 99 ) { 
            printErrorCode(response); 
            break;
        }
        //Check hints
        if( response < 0) {
            std::cout << "Bad Guess -- Too low.\n";
        } else if( response==0 ) {
            std::cout << "Good Guess!!!\n";
            break;
        } else if(response >0) {
            std::cout << "Bad Guess -- Too high.\n";  
        }
    }
    return 0;
}

