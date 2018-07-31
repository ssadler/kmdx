#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libevm/VMFace.h>

#include "ABI.h"
#include "NativeErrors.h"


using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;


namespace dev
{

namespace wing
{

uint32_t bytes2uint32(byte const* _data)
{
    return uint32_t(_data[0] << 24 | _data[1] << 16 | _data[2] << 8 | _data[3]);
}

bytes abiMethodId(string const method)
{
    bytes out = sha3(method).asBytes();
    out.resize(4);
    return out;
}

void ABI::safeRead(byte* _target, size_t _off, size_t _len)
{
    if (m_data.size() < _off + _len + m_offset)
        throwRevert(BadRequestABI);
    memcpy(_target, m_data.data()+m_offset+_off, _len);
    m_offset += _off + _len;
}

Address ABI::address()
{
    Address out;
    safeRead(out.data(), 12, 20);
    return out;
}

u256 ABI::uint()
{
    h256 out;
    safeRead(out.data(), 0, 32);
    return u256(out);
}

void ABIWriter::appendBytes(byte const* _mem, size_t _lpad, size_t _len)
{
    size_t off = m_data.size();
    ensureSpace(_lpad + _len);
    memcpy(&m_data[off]+_lpad, _mem, _len);
}

void ABIWriter::ensureSpace(size_t _needed)
{
    m_data.resize(m_data.size()+_needed);
}

}

}
