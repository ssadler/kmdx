#include <libdevcrypto/Common.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "Asset.h"
#include "Native.h"
#include "Manager.h"
#include "KMDBridge.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::wing;

namespace dev
{

namespace wing
{

bool isNativeContract(State& m_s, Address const& _addr)
{
    (void)m_s;
    return _addr == ManagerContractAddress || _addr == AssetContractAddress
        || _addr == KMDBridgeContractAddress;
}

owning_bytes_ref execNative(Address const& _addr, u256& _io_gas, ExtVM& _ext, OnOpFunc const& _onOp)
{
    auto f = [&]()
    {
        NativeVM vm(_io_gas, _ext, _onOp);
        NativeCall call(vm);

        if (_addr == ManagerContractAddress)
            return Manager(vm).call(call);

        // Should we actually be testing if the addr is the same as the
        // storage location in _ext?
        if (_ext.depth != 1)
            throwRevert(IllegalContractCall);

        if (_addr == AssetContractAddress)
            return Asset(vm).call(call);
        
        if (_addr == KMDBridgeContractAddress)
            return KMDBridgeContract(vm).call(call);

        // This should not happen... It should be made impossible really
        auto ec = std::make_error_code(std::errc::invalid_seek);
        std::string what = "Contract not found: " + toHexPrefixed(_addr);
        BOOST_THROW_EXCEPTION(std::system_error(ec, what));
    };

    // TODO: Check that not operating on own storage
    //       Check min call depth
    bytes output = f();
    return owning_bytes_ref(std::move(output), 0, output.size());
}

}
}
