
#pragma once

#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libwingvm/Native.h>
#include <libevm/VMFace.h>

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;


ETH_SIMPLE_EXCEPTION_VM(AssetContractCalledDirectly);


namespace dev
{

namespace wing
{


bytes abiMethodId(bytes const method);
/*
class ERC20
{
    enum State
    {
        Scalars,
        TotalSupply,
        Name,
        Symbol,
        Decimals,
        Balances
    }

    bytes const method_totalSupply = getMethodId("function totalSupply() public constant returns (uint);");

    void exec(bytes const& data)
    {
        bytes const methodId(data.begin(), data.begin()+4);
        if (methodId == method_totalSupply) {
            return m_ns.getWord(keyOn(Scalars, TotalSupply), 
        }
    }
}
*/

class AssetVM : public NativeVM
{
public:
    AssetVM(u256& io_gas, ExtVM& _ext, const OnOpFunc& _onOp):
        NativeVM(io_gas, _ext, _onOp) {};

    owning_bytes_ref exec() override
    {
        // First thing, ensure that depth is 1
        if (m_ext.depth != 1)
            BOOST_THROW_EXCEPTION(AssetContractCalledDirectly());

        return owning_bytes_ref{std::move(m_output), 0, 0};
    }
};

}
}
