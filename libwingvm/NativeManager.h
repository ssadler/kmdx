
#pragma once

#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libwing/ExtVM.h>
#include <libevm/VMFace.h>
#include "NativeVM.h"

using namespace std;
using namespace dev;
using namespace dev::eth;


namespace dev
{
namespace wing
{

Address const RootContractAddress = Address("0300000000000000000000000000000000000000");


class NativeManager
{
public:
    NativeManager(State& _s): m_vm(DummyVM(RootContractAddress, _s)) {};
    bool isNativeContract(Address const& _addr) const;
    void setNativeContract(Address const& _addr, std::string _name);
    owning_bytes_ref callVM(Address const& _addr, u256& io_gas, ExtVM& _ext, OnOpFunc const& _onOp);
    DummyVM& vm() { return m_vm; }
private:
    DummyVM m_vm;
};

}

}


