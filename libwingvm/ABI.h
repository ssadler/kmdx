#pragma once

#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libevm/VMFace.h>

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;


namespace dev
{

namespace wing
{

uint32_t bytes2uint32(byte const* _data);
bytes abiMethodId(string const method);


class ABI
{
public:
    ABI() {};
    ABI(std::string _sig): m_data(abiMethodId(_sig)) {};
    ABI(bytesConstRef _data): m_data(_data.toVector()) {};

    uint32_t method() {
        if (m_data.size() < 4) return 0;
        return bytes2uint32(m_data.data());
    }

    Address address();
    u256 uint();
    bytes read(size_t n);
    bytes bytesLong();
private:
    void safeRead(byte* _target, size_t _off, size_t _len);
    size_t m_offset = 4;
    bytes m_data;
};


class ABIWriter
{
public:
    ABIWriter(std::string _sig): m_data(abiMethodId(_sig)) {};
	
	/// Shift operators for appending data items.
    template <class T> ABIWriter& operator<<(T _data) { append(_data); return *this; }

    void appendBytes(byte const* _mem, size_t _lpad, size_t _len);

    void append(Address const& _addr) { appendBytes(_addr.data(), 12, 20); }
    void append(h256 const& _uint)    { appendBytes(_uint.data(), 0, 32); }
    void append(u256 const& _uint)    { append(h256(_uint)); }

    operator bytes() const { return m_data; }
private:
    void ensureSpace(size_t _needed);
    bytes m_data;
};

}
}
