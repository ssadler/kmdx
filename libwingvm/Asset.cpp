#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libevm/VMFace.h>

#include "Asset.h"
#include "ABI.h"
#include "NativeVM.h"
#include "NativeErrors.h"

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;


namespace dev
{
namespace wing
{

namespace
{
static const Key e2k = keyOn("ERC20"),
                 TotalSupply = e2k["TotalSupply"],
                 Name = e2k["Name"],
                 Symbol = e2k["Symbol"],
                 Decimals = e2k["Decimals"],
                 Balances = e2k["Balances"],
                 Allowance = e2k["Allowance"];
}

void ERC20::init()
{
    // TODO: make contract parameterisable
    u256 decimals = 18;
    u256 n = 1000000;
    put(Decimals, decimals);
    put(TotalSupply, n * (10 ^ decimals));
    m_vm.putData(Name, string() + "Miners");
    m_vm.putData(Symbol, string() + "MNR");
}


bytes ERC20::dispatch(NativeCall& _call)
{
    if (_call.route("totalSupply()"))
    {
        return toCompactBigEndian(totalSupply());
    }
    else if (_call.route("balanceOf(address)"))
    {
        return toCompactBigEndian(balanceOf(_call.abi.address()));
    }
    else if (_call.route("transfer(address,uint)"))
    {
        Address to = _call.abi.address();
        bool r = transfer(_call.caller(), to, _call.abi.uint());
        return toCompactBigEndian(r);
    }
    else if (_call.route("allowance(address,address)"))
    {
        Address a = _call.abi.address(), b = _call.abi.address();
        return toCompactBigEndian(allowance(a, b));
    }
    else if (_call.route("approve(address,uint)"))
    {
        Address spender = _call.abi.address();
        bool r = approve(_call.caller(), spender, _call.abi.uint());
        return toCompactBigEndian(r);
    }
    else if (_call.route("transferFrom(address,address,uint)"))
    {
        Address from = _call.abi.address(), to = _call.abi.address();
        u256 r = transferFrom(_call.caller(), from, to, _call.abi.uint());
        return toCompactBigEndian(r);
    }
    
    throwRevert(InvalidMethod);
}

u256 ERC20::totalSupply() const
{
    return get(TotalSupply);
}

u256 ERC20::balanceOf(Address const& _addr) const
{
    return get(Balances[_addr]);
}

void ERC20::setBalance(Address const& _addr, u256 _amount)
{
    put(Balances[_addr], _amount);
}

u256 ERC20::allowance(Address const& _owner, Address const& _spender) const
{
    return get(Allowance[_owner][_spender]);
}

bool ERC20::approve(Address const& _owner, Address const& _spender, u256 const& _amount)
{
    put(Allowance[_owner][_spender], _amount);
    // TODO: emit Approval(msg.sender, spender, tokens);
    return true;
}


bool ERC20::transfer(Address const& _from, Address const& _to, u256 const& _amount)
{
    Key fromBalanceKey = Balances[_from],
        toBalanceKey = Balances[_to];

    u256 fromBalance = get(fromBalanceKey);

    require(fromBalance >= _amount, InsufficientBalance);

    put(fromBalanceKey, fromBalance - _amount);
    put(toBalanceKey, get(toBalanceKey) + _amount);

    // TODO: emit Transfer(m_vm.caller(), _to, _amount);

    return true;
}

bool ERC20::transferFrom(Address const& _caller, Address const& _from, Address const& _to, u256 const& _amount)
{
    Key allowedKey = Allowance[_from][_caller],
        fromBalanceKey = Balances[_from],
        toBalanceKey = Balances[_to];

    u256 fromBalance = get(fromBalanceKey);
    require(fromBalance >= _amount, InsufficientBalance);

    u256 fromApproved = get(allowedKey);
    require(fromApproved >= _amount, InsufficientAllowance);

    put(allowedKey, fromApproved - _amount);
    put(fromBalanceKey, fromBalance - _amount);
    put(toBalanceKey, get(toBalanceKey) + _amount);

    // TODO: emit("Transfer");
    return true;
}

}
}
