
#include <memory>
#include <json/json.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libevm/VM.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "Native.h"
#include "NativeErrors.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::wing;

namespace dev
{

namespace wing
{

void NativeVM::bill(u256 _gas)
{
    if (m_io_gas < _gas)
        BOOST_THROW_EXCEPTION(OutOfGas());
    m_io_gas -= _gas;
}

u256 NativeVM::getWord(Key const& _key)
{
    bill(VMSchedule::sloadGas);
    return m_ext.store(_key);
}

void NativeVM::putWord(Key const& _key, u256 const& _val)
{
    bill(_val != 0 ? VMSchedule::sstoreSetGas : VMSchedule::sstoreResetGas);
    m_ext.setStore(_key, _val);
}


owning_bytes_ref NativeVM::exec()
{
    if (m_ext.depth != 1)
        BOOST_THROW_EXCEPTION(NativeContractCalledDirectly());

    NativeCall nativeCall(*this);
    call(nativeCall);
    return owning_bytes_ref(std::move(m_output), 0, m_output.size());
}


bool NativeCall::route(std::string const& _sig, unsigned _flags)
{
    if (abi.method() != ABI(_sig).method()) return false;

    if (!(_flags & METHOD_PAYABLE))
        if (m_vm.m_ext.value != 0)
            throwRevert(MethodNotPayable);

    return true;
}


}
}
