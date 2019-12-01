
#include <memory>
#include <regex>
#include <json/json.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "Asset.h"
#include "Native.h"
#include "Bridge.h"
#include "Manager.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::wing;

namespace dev
{

namespace wing
{

namespace manager
{
static const Key _K = keyOn("Manager"),
                 Bridges = _K["Bridges"];

std::string readSymbol(bytes _data)
{
    size_t idx;
    for (idx=0; idx<32; idx++)
        if (_data[idx] == 0) break;
    if (idx > 16 || idx == 0)
        return "";
    std::string out(_data.begin(), _data.begin()+idx);
    return std::regex_match(out, std::regex("^[A-Za-z0-9]+$")) ? out : "";
}

}

using namespace manager;

bytes Manager::call(NativeCall& _call)
{
    if (_call.route("getBridge(bytes16)"))
    {
        return getBridge(_call);
    }
    if (_call.route("createBridge(bytes16)"))
    {
        return createBridge(BridgeContractAddress, _call);
    }
    if (_call.route("createBridgeKMD(bytes16)"))
    {
        return createBridge(KMDBridgeContractAddress, _call);
    }
    throwRevert(InvalidMethod);
}

bytes Manager::getBridge(NativeCall& _call)
{
    // Get a string from the bytes
    std::string symbol = readSymbol(_call.abi.read(32));
    if (symbol == "")
        throwRevert(InvalidArgument);
    
    // See if exists
    bytes addr;
    Key k = Bridges[_call.ext.caller][symbol];
    m_vm.getData(k, addr);
    return addr;
}

bytes Manager::createBridge(Address const& _addr, NativeCall& _call)
{
    // Get a string from the bytes
    std::string symbol = readSymbol(_call.abi.read(32));
    if (symbol == "")
        throwRevert(InvalidArgument);

    // See if exists
    Key k = Bridges[_call.ext.caller][symbol];
    if (get(k) != u256())
        throwRevert(ResourceAlreadyExists);

    // Create it
    bytes proxy = delegateCallInitProxyCode(_addr);
    CreateResult r = _call.create(_call.ext.value, &proxy);

    // Check status
    if (r.status != EVMC_SUCCESS)
        throwRevert(ContractCreationFailed);

    // Write address
    bytes newAddr(r.address.asBytes());
    m_vm.putData(k, newAddr);
    return newAddr;
}

}
}
