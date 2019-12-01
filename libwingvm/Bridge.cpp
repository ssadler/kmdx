
#include "Asset.h"
#include "Bridge.h"


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
static const Key Bridge = keyOn("Bridge"),
                 LastReceipt = Bridge["LastReceipt"];
}

void BridgeContract::init()
{

}

bytes BridgeContract::dispatch(NativeCall& _call)
{
    if (_call.route("lastReceipt()"))
    {
        bytes out;
        m_vm.getData(LastReceipt, out);
        return out;
    }
    throwRevert(InvalidMethod);
}


}
}
