
#include "Asset.h"
#include "KMDBridge.h"


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
static const Key KMDBridge = keyOn("Bridge"),
                 Nonce = KMDBridge["Nonce"],
                 LastState = KMDBridge ["LastState"];
}

void KMDBridgeContract::init()
{
    put(Nonce, u256(0));
}

bytes KMDBridgeContract::call(NativeCall& _call)
{
    if (_call.route("getNonce()"))
    {
        return toCompactBigEndian(get(Nonce));
    }
    if (_call.route("lastState()"))
    {
        bytes out;
        if (!m_vm.getData(LastState, out))
        {
            out.resize(2);
            memcpy(&out[0], (const void*)"{}", 2);
        }
        return out;
    }
    if (_call.route("setState(bytes)"))
    {
        m_vm.putData(LastState, _call.abi.bytesLong());
    }
    throwRevert(InvalidMethod);
}


}
}
