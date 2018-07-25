#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <test/tools/libtesteth/Options.h>
#include <test/tools/libtesteth/TestOutputHelper.h>
#include <test/tools/libtesteth/BlockChainHelper.h>
#include <test/tools/libtesteth/TestHelper.h>

#include <libdevcore/CommonData.h>
#include <libdevcore/Exceptions.h>
#include <libevm/Instruction.h>
#include <libwing/Block.h>
#include <libwing/ExtVM.h>
#include <libwingvm/Native.h>
#include <libwingvm/NativeErrors.h>
#include <libwingvm/Asset.h>
#include <libwing/Executive.h>


using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;
using namespace dev::test;


class NativeVMTestFixture : public TestOutputHelperFixture
{
public:
	NativeVMTestFixture():
        m_tempDirState(std::unique_ptr<TransientDirectory>(new TransientDirectory())),
        m_params(ChainParams(genesisInfo(eth::Network::KomodoNetworkTest),
                             genesisStateRoot(eth::Network::KomodoNetworkTest))),
        m_bc(std::unique_ptr<BlockChain>(new BlockChain(m_params,
                m_tempDirState.get()->path(), WithExisting::Kill))),
        m_db(State::openDB(m_tempDirState.get()->path(), m_bc->genesisHash(), WithExisting::Kill)),
        m_genesisBlock(m_bc->genesisBlock(m_db))
    {
        sk = Secret(fromHex("0x3131313131313131313131313131313131313131313131313131313131313131"));
        sk2 = Secret(fromHex("0x1111111111111111111111111111111111111111111111111111111111111111"));
        addr = toAddress(sk);
        addr2 = toAddress(sk2);
        m_genesisBlock.mutableState().setBalance(addr2, 2 * u256(pow(10, 18)));
    }

    std::unique_ptr<TransientDirectory> m_tempDirState;
    ChainParams m_params;
    std::unique_ptr<BlockChain> m_bc;
    OverlayDB m_db;
    Block m_genesisBlock;

    Secret sk, sk2;
    Address addr, addr2;
};


