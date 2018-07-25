
#include <boost/test/unit_test.hpp>
#include <test/tools/libtesteth/Options.h>
#include <test/tools/libtesteth/TestOutputHelper.h>
#include <test/tools/libtesteth/BlockChainHelper.h>
#include <test/tools/libtesteth/TestHelper.h>
#include <test/tools/libtestutils/TestLastBlockHashes.h>

#include <libdevcore/Exceptions.h>
#include <libwing/Block.h>
#include <libwing/ExtVM.h>
#include <libwingvm/Native.h>


using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::wing;
using namespace dev::test;


BOOST_AUTO_TEST_SUITE(Native)

BOOST_AUTO_TEST_CASE(RootContractIsNative)
{
    State state(State::Null);
    NativeManager m(state);
    BOOST_REQUIRE(m.isNativeContract(RootContractAddress));
    BOOST_REQUIRE(!m.isNativeContract(Address("1234000000000000000000000000000000000000")));
}

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

BOOST_AUTO_TEST_CASE(EncodingStringAs256)
{
    std::string d, s = "hello world";

    /// Check string value encode / decode
    d = from256(as256(s));
    BOOST_CHECK_EQUAL(s, d);

    /// Now with 0 bytes
    s = "";
    d = from256(as256(s));
    BOOST_CHECK_EQUAL(s, d);

    /// Now with 32 bytes
    s = "monkeys monkeys monkeys monkeys ";
    d = from256(as256(s));
    BOOST_CHECK_EQUAL(s, d);

    /// Check exception for oversize
    s = "monkeys monkeys monkeys monkeys m";
    BOOST_CHECK_THROW(as256(s), dev::ValueTooLarge);
}

BOOST_AUTO_TEST_CASE(NativeVMMultiWord)
{
    State s(State::Null);
    DummyVM ns(RootContractAddress, s);
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
    DummyVM ns(RootContractAddress, s);
    u256 k = keyOn(1, "a");
    std::string d(8190, 'a');
    ns.putData(k, d);
    std::string after;
    BOOST_CHECK_EQUAL(8190, ns.getData(k, after));
    BOOST_CHECK_EQUAL(d, after);
    d = std::string(8191, 'b');
    BOOST_CHECK_THROW(ns.putData(k, d), dev::ValueTooLarge);
}

BOOST_AUTO_TEST_CASE(KomodoNetworkGenesisRootAccountState)
{
    State state(State::Null);
    ChainParams params(genesisInfo(eth::Network::KomodoNetwork), genesisStateRoot(eth::Network::KomodoNetwork));
    state.populateFrom(params.genesisState);
    DummyVM ns(RootContractAddress, state);
    //BOOST_CHECK_EQUAL(ns.get(0, "hello"), "world");
}

BOOST_AUTO_TEST_SUITE_END()
