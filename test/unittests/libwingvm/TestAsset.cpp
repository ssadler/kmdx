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
        {};

    std::unique_ptr<TransientDirectory> m_tempDirState;
    ChainParams m_params;
    std::unique_ptr<BlockChain> m_bc;
    OverlayDB m_db;
    Block m_genesisBlock;
};

class ERC20TestFixture : public NativeVMTestFixture
{
    DummyVM m_vm;
public:
    ERC20TestFixture(): NativeVMTestFixture(),
            m_vm(DummyVM(wing::MinersTokenAddress, m_genesisBlock.mutableState())) {
        sk = Secret(fromHex("0x3131313131313131313131313131313131313131313131313131313131313131"));
        sk2 = Secret(fromHex("0x1111111111111111111111111111111111111111111111111111111111111111"));
        addr = toAddress(sk);
        addr2 = toAddress(sk2);
        m_genesisBlock.mutableState().setBalance(addr2, 2 * u256(pow(10, 18)));
    }

    ExecutionResult doTx(bytes _data, Secret _sk, u256 _nonce, u256 _value=0)
    {
        Transaction tx(_value, 10, 100000, wing::MinersTokenAddress, _data, _nonce, _sk);
        return m_genesisBlock.execute(m_bc->lastBlockHashes(), tx);
    }

    ERC20 erc20()
    {
        return ERC20(m_vm);
    }

    Secret sk, sk2;
    Address addr, addr2;
};


BOOST_FIXTURE_TEST_SUITE(NativeContractsSuite, ERC20TestFixture)

namespace Methods
{
    std::string const
        TotalSupply = "totalSupply()",
        BalanceOf = "balanceOf(address)",
        Transfer = "transfer(address,uint)",
        Allowance = "allowance(address,address)",
        Approve = "approve(address,uint)",
        TransferFrom = "transferFrom(address,address,uint)";

    std::vector<std::string> All = {TotalSupply, BalanceOf, Transfer,
                                    Allowance, Approve, TransferFrom};
}

#define checkRevert(c, er) \
    { ExecutionResult r = er; \
      BOOST_CHECK_EQUAL(r.excepted, TransactionException::RevertInstruction); \
      BOOST_CHECK(r.output == bytes({c})); }

ExecutionResult checkOk(ExecutionResult r)
{
    BOOST_REQUIRE_EQUAL(r.excepted, TransactionException::None);
    return r;
}


BOOST_AUTO_TEST_CASE(TestERC20TotalSupply)
{
    ABIWriter callData(Methods::TotalSupply);
    ExecutionResult r = checkOk(doTx(callData, sk, 0));
    BOOST_REQUIRE_EQUAL(fromBigEndian<u256>(r.output), erc20().totalSupply());
}

BOOST_AUTO_TEST_CASE(TestERC20Balances)
{
    BOOST_REQUIRE_EQUAL(erc20().balanceOf(addr), u256(2*pow(10, 18)));
    // Check non existent balance 0
    BOOST_REQUIRE_EQUAL(erc20().balanceOf(Address()), u256(0));
}

BOOST_AUTO_TEST_CASE(TestERC20NoPayable)
{
    /// Check all methods reject payment
    int nonce = 0;
    BOOST_FOREACH(std::string &method, Methods::All)
    {
        auto callData = ABIWriter(method) << addr;
        checkRevert(MethodNotPayable, doTx(callData, sk, nonce++, 1));
    }
}

BOOST_AUTO_TEST_CASE(TestERC20Transfer)
{
    auto callData = ABIWriter(Methods::Transfer) << addr2 << u256(10000);
    ExecutionResult r = checkOk(doTx(callData, sk, 0));
    BOOST_CHECK_EQUAL(erc20().balanceOf(addr2), 10000);
}

BOOST_AUTO_TEST_CASE(TestERC20TransferZeroBalance)
{
    auto callData = ABIWriter(Methods::Transfer) << addr << u256(1);
    checkRevert(InsufficientBalance, doTx(callData, sk2, 0));
}

BOOST_AUTO_TEST_CASE(TestERC20TransferLowBalance)
{
    u256 transferAmount = erc20().balanceOf(addr) + 1;
    auto callData = ABIWriter(Methods::Transfer) << addr << transferAmount;
    checkRevert(InsufficientBalance, doTx(callData, sk, 0));
}

BOOST_AUTO_TEST_CASE(TestERC20Allowance)
{
    // addr2 will make a transfer using addr's funds
    u256 nonce2 = 0;

    // Verify 0 allowance
    auto callData = ABIWriter(Methods::Allowance) << addr << addr2;
    ExecutionResult r = checkOk(doTx(callData, sk2, nonce2++));
    BOOST_CHECK(r.output == bytes());

    // addr2 has no allowance to do this tx
    callData = ABIWriter(Methods::TransferFrom) << addr << addr2 << 10;
    checkRevert(InsufficientAllowance, doTx(callData, sk2, nonce2++));

    // addr will approve addr2
    callData = ABIWriter(Methods::Approve) << addr2 << 10;
    r = checkOk(doTx(callData, sk, 0));
    BOOST_CHECK(r.output == bytes({1}));

    // Verify 10 allowance
    callData = ABIWriter(Methods::Allowance) << addr << addr2;
    r = checkOk(doTx(callData, sk2, nonce2++));
    BOOST_CHECK(r.output == bytes({10}));

    // Verify transfer 11 is err
    callData = ABIWriter(Methods::TransferFrom) << addr << addr2 << 11;
    checkRevert(InsufficientAllowance, doTx(callData, sk2, nonce2++));

    // Verify transfer 5 is ok
    callData = ABIWriter(Methods::TransferFrom) << addr << addr2 << 5;
    r = checkOk(doTx(callData, sk2, nonce2++));
    BOOST_CHECK(r.output == bytes({1}));

    // Verify 5 allowance
    callData = ABIWriter(Methods::Allowance) << addr << addr2;
    r = checkOk(doTx(callData, sk2, nonce2++));
    BOOST_CHECK(r.output == bytes({5}));

    // addr will approve addr2 some more
    callData = ABIWriter(Methods::Approve) << addr2 << 10;
    r = checkOk(doTx(callData, sk, 1));
    BOOST_CHECK(r.output == bytes({1}));
    
    // Verify 10 allowance
    callData = ABIWriter(Methods::Allowance) << addr << addr2;
    r = checkOk(doTx(callData, sk2, nonce2++));
    BOOST_CHECK(r.output == bytes({10}));

    // Verify 5 balance for addr2
    BOOST_REQUIRE_EQUAL(erc20().balanceOf(addr2), u256(5));
}


BOOST_AUTO_TEST_SUITE_END()
