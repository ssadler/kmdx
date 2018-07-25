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
    InvalidMethod = 0x03,
    BadRequestABI = 0x04,
    MethodNotPayable = 0x10,
    InsufficientBalance = 0x20,
    InsufficientAllowance = 0x21
};

#define throwRevert(code) \
    { bytes msg = {code}; \
      owning_bytes_ref obr{std::move(msg), 0, msg.size()}; \
      throw RevertInstruction(std::move(obr)); }

}
}
