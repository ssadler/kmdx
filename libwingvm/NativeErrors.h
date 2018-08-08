#pragma once

using namespace std;
using namespace dev;
using namespace dev::eth;

namespace dev
{
namespace wing
{

enum NativeVMErrorCode: byte
{
    IllegalContractCall = 0x02,
    InvalidMethod = 0x03,
    BadRequestABI = 0x04,
    InvalidArgument = 0x05,
    MethodNotPayable = 0x10,
    InsufficientBalance = 0x20,
    InsufficientAllowance = 0x21,
    ResourceAlreadyExists = 0x30,
    ValueOversize = 0x50,

    ContractCreationFailed = 0x60
};

#define throwRevert(code) \
    { bytes msg = {code}; \
      owning_bytes_ref obr{std::move(msg), 0, msg.size()}; \
      throw RevertInstruction(std::move(obr)); }

}
}
