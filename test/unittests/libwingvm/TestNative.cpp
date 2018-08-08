
#include <boost/test/unit_test.hpp>
#include <test/tools/libtesteth/Options.h>
#include <test/tools/libtesteth/TestOutputHelper.h>
#include <test/tools/libtesteth/BlockChainHelper.h>
#include <test/tools/libtesteth/TestHelper.h>
#include <test/tools/libtestutils/TestLastBlockHashes.h>

#include <libdevcore/Exceptions.h>
#include <libevm/VM.h>
#include <libwing/Block.h>
#include <libwing/ExtVM.h>
#include <libwingvm/Native.h>

#include "TestNativeHelper.h"

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;
using namespace dev::test;


BOOST_AUTO_TEST_SUITE(Native)

BOOST_AUTO_TEST_CASE(EncodingAssumptions)
{
    h256 h(fromHex("62373e13aa45fc447c82f73fece1b5b1cda37a29223837524c06e57c319c6ea3"));
    u256 someHash(h);

    BOOST_CHECK_EQUAL(u256(keyOn(10, "hello world")), someHash);

    // Decode a length prefixed hash
    size_t firstTwo = static_cast<size_t>(someHash >> 240);
    BOOST_CHECK_EQUAL(firstTwo, 0x6237);
    bytes buf;
    buf.resize(30);
    memcpy(&buf[0], h.data()+2, 30);
    BOOST_CHECK_EQUAL(toHex(buf), "3e13aa45fc447c82f73fece1b5b1cda37a29223837524c06e57c319c6ea3");

    // Encode a length prefixed hash
    size_t someSize = 65533;
    h256 h2 = h256(static_cast<u256>(someSize) << 240);
    memcpy(h2.data()+2, h.data()+2, 30);
    BOOST_CHECK_EQUAL(toHex(h2), "fffd3e13aa45fc447c82f73fece1b5b1cda37a29223837524c06e57c319c6ea3");
}

BOOST_AUTO_TEST_CASE(NativeVMMultiWord)
{
    State s(State::Null);
    DummyVM ns(ManagerContractAddress, s);
    u256 k = keyOn(1, "a");
    std::string d(512, 'a');
    ns.putData(k, d);
    std::string after;
    BOOST_CHECK_EQUAL(512, ns.getData(k, after));
    BOOST_CHECK_EQUAL(d, after);
}

BOOST_AUTO_TEST_CASE(NativeVMMultiWordLarge)
{
    State s(State::Null);
    DummyVM ns(ManagerContractAddress, s);
    u256 k = keyOn(1, "a");
    std::string d(8190, 'a');
    ns.putData(k, d);
    std::string after;
    BOOST_CHECK_EQUAL(8190, ns.getData(k, after));
    BOOST_CHECK_EQUAL(d, after);
    d = std::string(8191, 'b');
    BOOST_CHECK_THROW(ns.putData(k, d), RevertInstruction);
    cout << "OK" << endl;
}

BOOST_AUTO_TEST_CASE(KomodoNetworkGenesisRootAccountState)
{
    State state(State::Null);
    ChainParams params(genesisInfo(eth::Network::KomodoNetwork), genesisStateRoot(eth::Network::KomodoNetwork));
    state.populateFrom(params.genesisState);
    DummyVM ns(ManagerContractAddress, state);
    //BOOST_CHECK_EQUAL(ns.get(0, "hello"), "world");
    //TODO
}


BOOST_FIXTURE_TEST_SUITE(Billing, NativeVMTestFixture)


BOOST_AUTO_TEST_CASE(NativeVMBill)
{
    bytes code;
    OnOpFunc onOp;
    h256 codeHash;
    EnvInfo info(m_genesisBlock.info(), m_bc->lastBlockHashes(), 0);
    ExtVM ext(m_genesisBlock.mutableState(), info, *m_bc->sealEngine(),
            addr2, addr, addr, u256(0), u256(10), &code, &code, codeHash, 0, false, true);
    u256 gas(VMSchedule::sloadGas * 3);
    NativeVM vm(gas, ext, onOp);
    auto get = [&](u256 k) { return vm.getWord(Key(k)); };
    get(u256());
    get(u256());
    get(u256());
    BOOST_CHECK_THROW(get(u256()), OutOfGas);
}


BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
