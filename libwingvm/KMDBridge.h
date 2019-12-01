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


class KMDBridgeContract : public NativeContract
{
public:
    using NativeContract::NativeContract;
    void init();
    bytes call(NativeCall& _call) override;
};


}
}
