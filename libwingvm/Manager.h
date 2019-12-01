
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

Address const ManagerContractAddress   = Address("0300000000000000000000000000000000000000");
Address const AssetContractAddress     = Address("0301000000000000000000000000000000000000");
Address const BridgeContractAddress    = Address("0302000000000000000000000000000000000000");
Address const KMDBridgeContractAddress = Address("0303000000000000000000000000000000000000");
Address const MinersTokenAddress       = Address("7777777777777777777777777777777777777777");



bool isNativeContract(State&, Address const&);


class Manager : public NativeContract
{
public:
    using NativeContract::NativeContract;
    bool hasContract(Address const& _addr);
    void setContract(Address const& _addr, std::string _name);
    bytes call(NativeCall&) override;

    bytes createBridge(Address const& _addr, NativeCall& _call);
    bytes getBridge(NativeCall& _call);
};

owning_bytes_ref execNative(Address const& _addr, u256& _io_gas, ExtVM& _ext, OnOpFunc const& _onOp);

}
}
