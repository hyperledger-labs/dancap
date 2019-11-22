#include <iostream>
#include "sgx_urts.h"
#include "Enclave_u.h"
#include "sgx_uae_quote_ex.h"


#define ENCLAVE_NAME "enclave.signed.so"
#define PRINTERR std::cerr << "ERR: "
sgx_enclave_id_t global_eid = 0;

const uint8_t g_ecdsa_p256_att_key_id_list[] = {
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x8c, 0x4f,
    0x57, 0x75, 0xd7, 0x96, 0x50, 0x3e, 0x96, 0x13,
    0x7f, 0x77, 0xc6, 0x8a, 0x82, 0x9a, 0x00, 0x56,
    0xac, 0x8d, 0xed, 0x70, 0x14, 0x0b, 0x08, 0x1b,
    0x09, 0x44, 0x90, 0xc5, 0x7b, 0xff, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void oPrintStatus(sgx_status_t code){
    std::cout << "Enclave returned status: " << code << std::endl;;
}

int main()
{
    sgx_status_t ret = SGX_SUCCESS;
    sgx_launch_token_t token = { 0 };
    int updated = 0;

    std::cout << "Loading secret enclave ...\n";
    ret = sgx_create_enclave(ENCLAVE_NAME, SGX_DEBUG_FLAG, &token, &updated, \
            &global_eid, NULL);

    if (ret != SGX_SUCCESS) {
        PRINTERR << "FAILED TO CREATE ENCLAVE. CODE: ";
        PRINTERR << std::hex << ret << std::endl;
        return 1;
    }
    std::cout << "Enclave loaded\n";

    std::cout << "\nSelecting Attestation Key...\n";
    sgx_att_key_id_t key_id = {};
    ret = sgx_select_att_key_id(
        g_ecdsa_p256_att_key_id_list,
        (uint32_t) sizeof(g_ecdsa_p256_att_key_id_list),
        &key_id);

    switch(ret){
        case SGX_SUCCESS:
            std::cout << "Key selected\n"; break;
        case SGX_ERROR_INVALID_PARAMETER:
            PRINTERR << "Malformed Key List\n"; break;
        case SGX_ERROR_UNSUPPORTED_ATT_KEY_ID:
            PRINTERR << "Unsupported Key\n"; break;
        default:
            PRINTERR << "Unknown error\n";
    }

    std::cout << "Discovering pubkey ID size...\n";
    sgx_target_info_t target_info = {0};
    size_t key_size = 0;

    ret = sgx_init_quote_ex(&key_id, &target_info, &key_size, NULL);
    switch(ret){
        case SGX_SUCCESS:
            std::cout << "Pubkey ID size " << key_size << "\n"; break;
        case SGX_ERROR_INVALID_PARAMETER:
            PRINTERR << "Check key inputs\n"; break;
        case SGX_ERROR_BUSY:
        case SGX_ERROR_SERVICE_UNAVAILABLE:
        case SGX_ERROR_SERVICE_TIMEOUT:
            PRINTERR << "SGX/AE service error\n"; break;
        case SGX_ERROR_OUT_OF_MEMORY:
        case SGX_ERROR_OUT_OF_EPC:
            PRINTERR << "Out of memory/EPC\n"; break;
        case SGX_ERROR_NETWORK_FAILURE:
            PRINTERR << "Network error\n"; break;
        case SGX_ERROR_UPDATE_NEEDED:
        case SGX_ERROR_UNRECOGNIZED_PLATFORM:
            PRINTERR << "TCB problem\n"; break;
        case SGX_ERROR_UNSUPPORTED_ATT_KEY_ID:
        case SGX_ERROR_ATT_KEY_CERTIFICATION_FAILURE:
            PRINTERR << "Attestation key problem\n"; break;
        default:
            PRINTERR << "Unknown error\n";
            PRINTERR << "Failed to discover pubkey size\n";
        return 1;
    }
    uint8_t *pubkey_id = new uint8_t[key_size];

    std::cout << "Initializing quote...\n";
    ret = sgx_init_quote_ex(&key_id, &target_info, &key_size, pubkey_id);
    switch(ret){
        case SGX_SUCCESS:
            std::cout << "Quote initialization succeeded\n"; break;
        case SGX_ERROR_INVALID_PARAMETER:
            PRINTERR << "Check key inputs\n"; break;
        case SGX_ERROR_BUSY:
        case SGX_ERROR_SERVICE_UNAVAILABLE:
        case SGX_ERROR_SERVICE_TIMEOUT: 
            PRINTERR << "SGX/AE service error\n"; break;
        case SGX_ERROR_OUT_OF_MEMORY:
        case SGX_ERROR_OUT_OF_EPC:
            PRINTERR << "Out of memory/EPC\n"; break;
        case SGX_ERROR_NETWORK_FAILURE:
            PRINTERR << "Network error\n"; break;
        case SGX_ERROR_UPDATE_NEEDED:
        case SGX_ERROR_UNRECOGNIZED_PLATFORM:
            PRINTERR << "TCB problem\n"; break;
        case SGX_ERROR_UNSUPPORTED_ATT_KEY_ID:
        case SGX_ERROR_ATT_KEY_CERTIFICATION_FAILURE:
            PRINTERR << "Attestation key problem\n"; break;
        default:
            PRINTERR << "Unknown error\n";
            return 1;
    }

    std::cout << "Requesting report from enclave...\n";
    sgx_report_t report = {0};
    int retval = 0;
    ret =  ecall_Hello(global_eid, &retval, &target_info, &report);

    if (ret == SGX_SUCCESS) {
        std::cout << "Enclave call returned without internal error\n";
        if (retval == 1) {
            std::cout << "Report created successfully\n";
        } else {
            PRINTERR << "Report creation failed within enclave";
        }
    } else {
        PRINTERR << "Enclave call failed\n";
        PRINTERR << ret << std::endl;
    }

    std::cout << "Getting quote size...\n";
    uint32_t quote_size = 0;
    ret = sgx_get_quote_size_ex(&key_id, &quote_size);
    switch(ret){
        case SGX_SUCCESS:
            std::cout << "Retrieved quote size\n"; break;
        case SGX_ERROR_INVALID_PARAMETER:
            PRINTERR << "Invalid parameter - Check inputs\n"; break;
        case SGX_ERROR_ATT_KEY_UNINITIALIZED:
            PRINTERR << "Attestation key uninitialized - init quote again\n";
            //TODO: retry sgx_init_quote_ex(..)
            break;
        case SGX_ERROR_UNSUPPORTED_ATT_KEY_ID:
            PRINTERR << "Unsupported attestation key\n"; break;
        default:
            PRINTERR << "Unknown error\n";
            return 1;
    }

    std::cout << "Getting quote...\n";
    uint8_t *quote = new uint8_t[quote_size];
    ret = sgx_get_quote_ex(&report, &key_id, NULL,
            quote, quote_size);
            //reinterpret_cast<sgx_quote_t *>(&quote[0]), quote_size);
    switch(ret){
        case SGX_SUCCESS:
            std::cout << "Retrieved quote successfully\n"; break;
        case SGX_ERROR_INVALID_PARAMETER:
            PRINTERR << "Check inputs\n"; break;
        case SGX_ERROR_BUSY:
        case SGX_ERROR_SERVICE_UNAVAILABLE:
        case SGX_ERROR_SERVICE_TIMEOUT:
            //TODO: retry
            PRINTERR << "SGX/AE service error\n"; break;
        case SGX_ERROR_OUT_OF_MEMORY:
        case SGX_ERROR_OUT_OF_EPC:
            PRINTERR << "Out of memory/EPC\n"; break;
        case SGX_ERROR_NETWORK_FAILURE:
            PRINTERR << "Network error\n"; break;
        case SGX_ERROR_UPDATE_NEEDED:
        case SGX_ERROR_UNRECOGNIZED_PLATFORM:
            PRINTERR << "TCB problem\n"; break;
        case SGX_ERROR_UNSUPPORTED_ATT_KEY_ID:
        case SGX_ERROR_ATT_KEY_CERTIFICATION_FAILURE:
            PRINTERR << "Attestation key problem\n"; break;
        default:
            PRINTERR << "Unknown error\n";
            return 1;
    }

    delete pubkey_id;
    delete quote;
    std::cout << "\nCOMPLETED\n\n";
    return 0;
}

