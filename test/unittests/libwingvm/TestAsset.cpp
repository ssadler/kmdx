#include <boost/test/unit_test.hpp>
#include <test/tools/libtesteth/Options.h>
#include <test/tools/libtesteth/TestOutputHelper.h>
#include <test/tools/libtesteth/BlockChainHelper.h>
#include <test/tools/libtesteth/TestHelper.h>

#include <libdevcore/Exceptions.h>
#include <libevm/Instruction.h>
#include <libwing/Block.h>
#include <libwing/ExtVM.h>
#include <libwingvm/Native.h>
#include <libwing/Executive.h>


using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;
using namespace dev::test;


class NativeAssetTestFixture : public TestOutputHelperFixture
{
public:
	NativeAssetTestFixture():
        m_tempDirState(std::unique_ptr<TransientDirectory>(new TransientDirectory())),
        m_params(ChainParams(genesisInfo(eth::Network::KomodoNetworkTest),
                             genesisStateRoot(eth::Network::KomodoNetworkTest))),
        m_bc(std::unique_ptr<BlockChain>(new BlockChain(m_params,
                m_tempDirState.get()->path(), WithExisting::Kill))),
        m_db(State::openDB(m_tempDirState.get()->path(), m_bc->genesisHash(), WithExisting::Kill)),
        m_genesisBlock(m_bc->genesisBlock(m_db))
        {};

    std::unique_ptr<TransientDirectory> m_tempDirState;
    ChainParams m_params;
    std::unique_ptr<BlockChain> m_bc;
    OverlayDB m_db;
    Block m_genesisBlock;
};


BOOST_FIXTURE_TEST_SUITE(NativeContractsSuite, NativeAssetTestFixture)

BOOST_AUTO_TEST_CASE(TestAsset)
{
    /// contract :: EvmAsm
    /// contract = do
    ///   push2 22
    ///   dup1
    ///   push2 14
    ///   push1 0
    ///   codecopy
    ///   push2 (14+22)
    ///   jump
    ///   push20 0x0301000000000000000000000000000000000000
    ///   delegatecall
    ///   jumpdest
    ///   push1 0
    ///   return

    Secret sk(fromHex("0x3131313131313131313131313131313131313131313131313131313131313131"));

    // TODO: make it so that contract checks output and returns it
    bytes createData(fromHex("61001f8061000e60003961002d5661ffff60003660007303010000000000000000000000000000000000005af45b6000f3"));
	Transaction createTx(1234, 10, 100000, createData, 0, sk);
    ExecutionResult r = m_genesisBlock.execute(m_bc->lastBlockHashes(), createTx);

    bytes callData(fromHex("1111")); // TODO
    Transaction callTx(0, 10, 100000, r.newAddress, callData, 1, sk);
    r = m_genesisBlock.execute(m_bc->lastBlockHashes(), callTx);
    cout << "r is: " << r << endl;
    BOOST_REQUIRE_EQUAL(r.output.size(), 5);
}

BOOST_AUTO_TEST_SUITE_END()
