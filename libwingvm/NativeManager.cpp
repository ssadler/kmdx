
#include <memory>
#include <json/json.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "Asset.h"
#include "Native.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::wing;

namespace dev
{

namespace wing
{

static const Key NativeContracts = keyOn(0);

// TODO: Rename to hasActiveVM
bool NativeManager::isNativeContract(Address const& _addr)
{
    if (_addr == RootContractAddress)
        return true;

    std::string vmName;
    return m_vm.getData(NativeContracts[_addr], vmName);
}
 
void NativeManager::setNativeContract(Address const& _addr, std::string _name)
{
    m_vm.putData(NativeContracts[_addr], _name);
}

owning_bytes_ref NativeManager::callVM(Address const& _addr, u256& io_gas, ExtVM& _ext, OnOpFunc const& _onOp)
{
    //if (_addr == RootContractAddress)
    //    return std::unique_ptr<VMFace>(new RootContractVM());
    
    std::string vmName;
    if (m_vm.getData(NativeContracts[_addr], vmName)) {
        if (vmName == "asset")
            return AssetVM(io_gas, _ext, _onOp).exec();
    }

    auto ec = std::make_error_code(std::errc::invalid_seek);
    std::string what = "Native VM not found: " + vmName;
    BOOST_THROW_EXCEPTION(std::system_error(ec, what));
}

}

}
