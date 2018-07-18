
#pragma once

#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libwingcore/Common.h>
#include <libwing/State.h>
#include <libevm/VMFace.h>

using namespace dev;
using namespace dev::eth;


ETH_SIMPLE_EXCEPTION_VM(RootContractCalledIndirectly);


class RootContractVM : public NativeVM
{
public:
    owning_bytes_ref exec(u256& io_gas, ExtVMFace& _ext, const OnOpFunc& _onOp)
    {
        if (_ext.depth != 1)
            BOOST_THROW_EXCEPTION(RootContractCalledIndirectly());
        bool l = io_gas == u256() || _ext.depth == 0 || &_onOp == NULL;
        if (l || !l) m_output = bytes();
        return owning_bytes_ref(std::move(m_output), 0, 0);
    }
};
