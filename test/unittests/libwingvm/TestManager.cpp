
#include <boost/test/unit_test.hpp>
#include <test/tools/libtesteth/Options.h>
#include <test/tools/libtesteth/TestOutputHelper.h>
#include <test/tools/libtesteth/BlockChainHelper.h>
#include <test/tools/libtesteth/TestHelper.h>
#include <test/tools/libtestutils/TestLastBlockHashes.h>

#include <libwingvm/Native.h>

#include "TestNativeHelper.h"

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;
using namespace dev::test;


class ManagerTestFixture : public NativeVMTestFixture
{
    DummyVM m_vm;
public:
    ManagerTestFixture(): NativeVMTestFixture(),
            m_vm(DummyVM(wing::ManagerContractAddress, m_genesisBlock.mutableState())) {}

    ExecutionResult doTx(bytes _data, Secret _sk, u256 _nonce, u256 _value=0)
    {
        Transaction tx(_value, 10, 100000, wing::ManagerContractAddress, _data, _nonce, _sk);
        return m_genesisBlock.execute(m_bc->lastBlockHashes(), tx);
    }
};

#define checkOk(r) \
    BOOST_REQUIRE_EQUAL(r.excepted, TransactionException::None);

BOOST_AUTO_TEST_SUITE(Native)

BOOST_FIXTURE_TEST_SUITE(Manager, ManagerTestFixture)

namespace Methods
{
    std::string const
        CreateBridge = "createBridge(bytes16)";
}

//BOOST_AUTO_TEST_CASE(CreateBridgeContract)
//{
//    auto callData = ABIWriter(Methods::CreateBridge) << fromBigEndian<u256>("KMD");
//    ExecutionResult r = doTx(callData, sk, 0);
//    checkOk(r);
//    //romBigEndian<u256>(r.output), erc20().totalSupply());
//}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
