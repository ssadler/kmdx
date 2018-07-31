
#include <memory>
#include <json/json.h>
#include <libdevcore/Log.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Hash.h>
#include <libdevcrypto/Common.h>
#include <libevm/ExtVMFace.h>
#include <libwingcore/Common.h>
#include "NativeVM.h"

using namespace dev;
using namespace dev::wing;

namespace dev
{
namespace wing
{


bytes delegateCallProxyCode(Address const& _addr)
{
    /// hath delegatecallProxy 0x0000000000000000000000000000000000000000
    bytes out(fromHex("366000600037600060003660007300000000000000000000000000000000000000005af43d6000803e600014610034573d6000f35b3d6000fd"));
    memcpy(&out[14], _addr.data(), 20);
    return out;
}


}
}
