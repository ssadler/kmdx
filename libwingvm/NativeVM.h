
#pragma once

#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libwing/ExtVM.h>
#include <libevm/VMFace.h>
#include "ABI.h"
#include "NativeErrors.h"

using namespace std;
using namespace dev;
using namespace dev::eth;

namespace dev
{
namespace wing
{


ETH_SIMPLE_EXCEPTION_VM(NativeContractCalledDirectly);
ETH_SIMPLE_EXCEPTION_VM(MethodNotFound);

struct VMNotConfigured : Exception {};

string getAccountStorageJson(Account const* a);
bytes delegateCallProxyCode(Address const& _addr);

u256 as256(int _i);
u256 as256(std::string _s);
std::string from256(u256 _h);


class Key
{
    u256 m_key;
public:
    Key(u256 u): m_key(u) {};
    template <typename K>
    Key operator [](K _key) const;
    operator u256() const { return m_key; }
};

template <typename A, typename B>
Key keyOn(A _a, B _b)
{
    RLPStream rlp;
    rlp.appendList(2);
    rlp << _a << _b;
    return Key(sha3(rlp.out()));
}

template <typename A>
Key keyOn(A _a)
{
    RLPStream rlp;
    rlp.appendList(1);
    rlp << _a;
    return Key(sha3(rlp.out()));
}

template <typename K>
Key Key::operator [](K _key) const { return keyOn(m_key, _key); }

class NativeVMFace
{
public:
    virtual u256 getWord(Key const& _key) = 0;
    virtual void putWord(Key const& _key, u256 const& _val) = 0;
    template <typename V> size_t getData(u256 _key, V& out);
    template <typename V> void putData(u256 _key, V const& in);
};


class NativeCall;

/*
 * Invocation of VM, contract execution backend
 */
class NativeVM : public NativeVMFace
{
    friend class NativeCall;
public:
    NativeVM(u256& io_gas, ExtVM& _ext, const OnOpFunc& _onOp):
        m_io_gas(io_gas),
        m_ext(_ext),
        m_onOp(_onOp)
    {};

    owning_bytes_ref exec();
    virtual void call(NativeCall& _call) = 0;

    void bill(u256 _gas);

    u256 getWord(Key const& _key) override;
    void putWord(Key const& _key, u256 const& _val) override;
protected:
    u256& m_io_gas;
    ExtVM& m_ext;
    OnOpFunc const& m_onOp;
    bytes m_output;
};


unsigned constexpr METHOD_PAYABLE = 0;


class DummyVM : public NativeVMFace
{
public:
    DummyVM(Address _addr, State& _s): m_addr(_addr), m_s(_s) {};
    u256 getWord(Key const& _key) override { return m_s.storage(m_addr, _key); }
    void putWord(Key const& _key, u256 const& _val) override { m_s.setStorage(m_addr, _key, _val); }
    std::string toJsonMap() { return getAccountStorageJson(m_s.account(m_addr)); }
protected:
    Address m_addr;
    State& m_s;
};


class NativeCall
{
    NativeVM& m_vm;
public:
    ABI abi;
    NativeCall(NativeVM& _vm): m_vm(_vm), abi(_vm.m_ext.data) {};
    Address const& caller() const { return m_vm.m_ext.caller; }
    bool route(std::string const& _sig) { return route(_sig, 0); }
    bool route(std::string const& _sig, unsigned _flags);
};


class NativeContract
{
public:
    NativeContract(NativeVMFace &_vm): m_vm(_vm) {};
    u256 get(Key const& _key) const { return m_vm.getWord(_key); }
    void put(Key const& _key, u256 const& _val) { m_vm.putWord(_key, _val); }

    static void require(bool _pass, NativeVMErrorCode code)
    {
        if (!_pass)
            throwRevert(code);
    }
protected:
    NativeVMFace &m_vm;
};


}
}
