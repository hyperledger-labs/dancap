#include "Enclave_t.h"
#include "sgx_trts.h"
#include "sgx_utils.h"

int ecall_Hello(sgx_target_info_t* targetInfo, sgx_report_t* report)
{
    sgx_status_t ret = SGX_SUCCESS;
    sgx_create_report(targetInfo, NULL, report);
    if(ret != SGX_SUCCESS){ oPrintStatus(ret);}
    return ret;
}
