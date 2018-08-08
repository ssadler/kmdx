
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



namespace dev
{

namespace wing
{



class ERC20 : public NativeContract
{
public:
    using NativeContract::NativeContract;
    void init();
    bytes call(NativeCall& _call) override;
    u256 totalSupply() const;
    u256 balanceOf(Address const& _addr) const;
    void setBalance(Address const& _addr, u256 _amount);
    bool transferFrom(Address const& _caller, Address const& _from, Address const& _to, u256 const& _amount);
    u256 allowance(Address const& _owner, Address const& _spender) const;
    bool approve(Address const& _owner, Address const& _spender, u256 const& _amount);
    bool transfer(Address const& _from, Address const& _to, u256 const& _amount);
};


class Asset : public NativeContract
{
public:
    using NativeContract::NativeContract;

    ERC20 erc20() const { return ERC20(m_vm); }

    void init()
    {
        erc20().init();
    }

    bytes call(NativeCall& _call) override
    {
        return erc20().call(_call);
    }
};

}
}
