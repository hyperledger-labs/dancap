# Overview
This is a proof of concept project to explore attestation options for
Hyperledger Avalon.

The code can be used to learn the call flow for ECDSA based attestation. To a
lesser degree, the project can also be used to verify the availability of
attestation components on a host (e.g. SGX and libs).

The code is written first and foremost to make the attestation API readable and
simple. It is not written to be fully secure or robust. You can treat this as a
simplified example before looking at official SGX examples.

The main attestation logic is in App/App.cpp. It loads a minimal enclave and
uses the SGX SDK methods for creating an ECDSA based attestation of that
enclave.

This process has been verified on Azure ACC nodes only.

The project also includes a Docker-based dev environment. The app also works in
simulation mode within that environment. That container is meant for development
not for deployment. It may be possible to modify the container, mapping in
/dev/sgx and the quote provider library (libdcap_quoteprov.so) for deployment
but this has not been tested.

## TODO
Quote verification

# Build
## Docker From project root
1. build the container
    ```./build-docker```

1. run the container (this drops you into a shell)
    ```./run-container```

1. build in hardware mode (default) or simulation mode
    ```make clean && make```
    or
    ```SGX_MODE=SIM make clean && make```

1. run (Note: when run within the container only simulation mode will work.
However if you build in HW mode the binary will work on Ubuntu natively.)
    ```./attestor```

## Ubuntu 18.04
1. Install machine per instructions in Cloud Provisioning

1. build in hardware mode (default) or simulation mode
    ```make clean && make```
    or
    ```SGX_MODE=SIM make clean && make```

1. run
    ```./attestor```

# Cloud provisioning
## Azure Confidential Compute
1. Provision an ACC node with
    * Ubuntu 18.04
    * Open Enclave SDK 
    ** This is necessary to get the attestation dependencies installed correctly within Azure.
    See:
    https://software.intel.com/en-us/articles/get-started-with-azure-confidential-computing
    for more details.

1. Upgrade the machine
    ```apt-get update && apt-get upgrade```

1. Install the Intel(r) SGX SDK
    ```wget
       https://download.01.org/intel-sgx/sgx-linux/2.7.1/distro/ubuntu18.04-server/sgx_linux_x64_sdk_2.7.101.3.bin \
       && chmod +x sgx_linux_x64_sdk_2.7.101.3.bin \
       && echo "yes" | ./sgx_linux_x64_sdk_2.7.101.3.bin \
       && rm sgx_linux_x64_sdk_2.7.101.3.bin \
       && echo "source /opt/intel/sgxsdk/environment" >> /etc/environment```

1. Set library search order to pick up Azure's quote provider library rather
    than the one available in the SGX common path. You can do this the right
    way by managing /etc/ld.so.conf Or you can do it the expedient way by
    removing the one we don't want.
    We want this one:  
    ```$ /usr/lib/libdcap_quoteprov.so```

    we do NOT want this one:  
    ```~$ /opt/intel/libsgx-enclave-common/aesm/libdcap_quoteprov.so~```

# Misc
The project has a helper function to print SGX error codes and messages.
This helper function can be regenerated if the SGX SDK changes using the
python script in App/:
```App/generate_handle_sgx_error.py > App/handle_sgx_error.h```

