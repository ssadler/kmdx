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

bytes abiMethodId(bytes const method)
{
    h256 h = sha3(method);
    return bytes(h.begin(), h.begin()+4);
}

}
}
