// Copyright contributors to Hyperledger Avalon
// SPDX-License-Identifier: Apache-2.0
#include <iostream>
#include <fstream>
#include <time.h>
#include "sgx_dcap_quoteverify.h"

int main() {
    using namespace std;

    ifstream quote_file ("attestation.bytes", ifstream::binary); 
    if (!quote_file) {
        cerr << "ERROR: Unable to read attestation file\n";
        return(1);
    } 

    // get quote size
    quote_file.seekg(0, quote_file.end);
    int quote_size = quote_file.tellg();
    quote_file.seekg(0, quote_file.beg);

    char * quote = new char [quote_size];

    cout << "Reading " << quote_size << " bytes... ";
    quote_file.read(quote, quote_size);

    if (quote_file)
        cout << "Read attestation file successfully.\n";
    else
        cerr << "ERROR: only " << quote_file.gcount() << " could be read";
    quote_file.close();


    // verify quote

    time_t now = time(NULL);
    uint32_t expiration_status = 0;
       
    sgx_ql_qv_result_t verification_result = SGX_QL_QV_RESULT_UNSPECIFIED;
    quote3_error_t result = SGX_QL_ERROR_UNEXPECTED;

    result = sgx_qv_verify_quote( 
            (uint8_t*) quote,       // Quote
            quote_size,             // Quote size
            NULL,                   // Collateral. NULL asks the library to find it.
            now,                    // Check expiration as of this date
            &expiration_status,     // Receive expiration indication;
            &verification_result,   // Result of verification check
            NULL,                   // Optional Report Info
            0,                      // Optional supplemental data size
            NULL                    // Optional supplemental data
            );

    if (result != SGX_QL_SUCCESS){
            cerr << "ERROR: Quote verification FAILED with error: " << hex << result;
            cerr << "\n Verification error code: " << verification_result << endl; 
            cerr << dec;
            return 1;
    } else {
            cout << "Success: Quote verification PASSED\n";
            cout << "\n Verification result code: " << hex << verification_result << endl;
            cout << dec;
    }
 

    // TODO: Verify report data

    delete[] quote;
    return 0;
}

