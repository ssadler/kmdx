//
// Created by thpn on 28/08/18.
//

#include "ValidatorSetTest.h"

void ValidatorSetTest::testValidatorSetBasic() {
    ValidatorSet vset;
    //for (Validator& val : vset.validators) {
    BOOST_CHECK_THROW(vset.incrementAccum(1), Panic);
    BOOST_REQUIRE(!vset.hasAddress(AddressTm("some val")));
    BOOST_REQUIRE(!vset.getByAddress(AddressTm("some val")).is_initialized());
    BOOST_REQUIRE(!vset.getByIndex(-100).is_initialized());
    BOOST_REQUIRE(!vset.getByIndex(0).is_initialized());
    BOOST_REQUIRE(!vset.getByIndex(100).is_initialized());
    BOOST_REQUIRE_EQUAL(0, vset.size());
    BOOST_REQUIRE_EQUAL(0, vset.getTotalVotingPower());
    BOOST_REQUIRE(!vset.getProposer());
    BOOST_REQUIRE(vset.hash().empty());

    // add
    Validator val = common_test::randValidator(false, 1);
    BOOST_REQUIRE(vset.add(val));
    BOOST_REQUIRE(vset.hasAddress(val.address));
    BOOST_REQUIRE_EQUAL(0, vset.getIndex(val));
    BOOST_REQUIRE(vset.getByAddress(val.address).is_initialized());
    BOOST_REQUIRE(val == vset.getByAddress(val.address).get());
    BOOST_REQUIRE(val == vset.getByIndex(0).get());
    BOOST_REQUIRE_EQUAL(1, vset.size());
    BOOST_REQUIRE_EQUAL(val.votingPower, vset.getTotalVotingPower());
    BOOST_REQUIRE(val == *vset.getProposer());
    BOOST_REQUIRE(!vset.hash().empty());
    cout << vset.hash().toString();
    BOOST_REQUIRE_NO_THROW(vset.incrementAccum(1));

    // update
    Validator randomUpdate = common_test::randValidator(false, 1);
    randomUpdate.address[9] += 4;//val.address[9]+9;
    BOOST_REQUIRE(!vset.update(randomUpdate));
    val.votingPower = 100;
    BOOST_REQUIRE(vset.update(val));

    // remove
    Validator randomRmv = common_test::randValidator(false, 1);
    randomRmv.address[9] += 3;//val.address[9]+9;
    BOOST_REQUIRE(!vset.remove(randomRmv));
    BOOST_REQUIRE(vset.remove(val));
}

void ValidatorSetTest::testProposerSelection1() {
    std::vector<Validator> validators;
    Validator foo(PrivKey(dev::Secret("ffff")).getKey(), 1000, 0);
    Validator bar(PrivKey(dev::Secret("bbba")).getKey(), 300, 1);
    Validator baz(PrivKey(dev::Secret("bbbb")).getKey(), 330, 2);
    // Validator foo(PubKey(AddressTm("foo")), 1000);
    // Validator 1(PubKey(AddressTm("1")), 300);
    // Validator baz(PubKey(AddressTm("baz")), 330);
//    ValidatorStub foo("ffffffffffffffffffffffffffffffff", 1000);
//    ValidatorStub 1("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba", 300);
//    ValidatorStub baz("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", 330);
    validators.push_back(foo);
    validators.push_back(bar);
    validators.push_back(baz);
    ValidatorSet vset(validators);
    vset.incrementAccum(1);
    ostringstream proposers;
    for (int i = 0; i < 99; i++) {
        Validator *prop = vset.getProposer();
        proposers << " " << prop->index;
        vset.incrementAccum(1);
    }
    cout << proposers.str();
    std::string expected(
            " 0 2 0 1 0 0 2 0 1 0 0 2 0 0 1 0 2 0 0 1 0 0 2 0 1 0 0 2 0 1 0 0 2 0 0 1 0 2 0 0 1 0 2 0 0 1 0 2 0 0 1 0 2 0 0 0 2 1 0 0 0 2 0 1 0 0 2 0 1 0 0 2 0 1 0 0 2 0 1 0 0 2 0 0 1 0 2 0 0 1 0 2 0 0 1 0 2 0 0");
    // " foo baz foo bar foo foo baz foo bar foo foo baz foo foo bar foo baz foo foo bar foo foo baz foo bar foo foo baz foo bar foo foo baz foo foo bar foo baz foo foo bar foo baz foo foo bar foo baz foo foo bar foo baz foo foo foo baz bar foo foo foo baz foo bar foo foo baz foo bar foo foo baz foo bar foo foo baz foo bar foo foo baz foo foo bar foo baz foo foo bar foo baz foo foo bar foo baz foo foo");
//            " ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbba ffffffffffffffffffffffffffffffff bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc ffffffffffffffffffffffffffffffff ffffffffffffffffffffffffffffffff");
    BOOST_REQUIRE_EQUAL(expected, proposers.str());
}
// when all voting power is same, we go in order of addresses

void ValidatorSetTest::testProposerSelectionByAddress() {
    std::vector<Validator> validators;
//    Validator addr0(PubKey(AddressTm(HexBytes("0"))), 100);
//    Validator addr1(PubKey(AddressTm(HexBytes("1"))), 100);
//    Validator addr2(PubKey(AddressTm(HexBytes("2"))), 100);
    Validator addr0(PrivKey(dev::Secret("1111")).getKey(), 100, 0);
    Validator addr1(PrivKey(dev::Secret("2222")).getKey(), 100, 1);
    Validator addr2(PrivKey(dev::Secret("3333")).getKey(), 100, 2);
    validators.push_back(addr0);
    validators.push_back(addr1);
    validators.push_back(addr2);
    ValidatorSet vset(validators);
    vset.incrementAccum(1);
    for (unsigned int i = 0; i < vset.size() * 5; i++) {
        int ii = (i) % vset.size();
        //            int ii = i;
        Validator *prop = vset.getProposer();
        BOOST_CHECK_EQUAL(ii, prop->getIndex());
//        BOOST_CHECK_EQUAL(vset.getByIndex(ii)->getAddress().toString(), prop->getAddress().toString());
        vset.incrementAccum(1);
    }
}

// One validator has more than the others, but not enough to propose twice in a row
void ValidatorSetTest::testProposerSelectionNotTwice() {
    std::vector<Validator> validators;
//        Validator addr0(PubKey(AddressTm(HexBytes("0"))), 100);
//        Validator addr1(PubKey(AddressTm(HexBytes("1"))), 100);
//        Validator addr2(PubKey(AddressTm(HexBytes("2"))), 400);
    Validator addr0(PrivKey(dev::Secret("0")).getKey(), 100);
    Validator addr1(PrivKey(dev::Secret("1")).getKey(), 100);
    Validator addr2(PrivKey(dev::Secret("2")).getKey(), 400);
    validators.push_back(addr0);
    validators.push_back(addr1);
    validators.push_back(addr2);
    ValidatorSet vset(validators);
    vset.incrementAccum(1);

    Validator *prop = vset.getProposer();
    BOOST_CHECK_EQUAL(prop->getAddress().toString(),
                      addr2.getAddress().toString()); //"Expected address with highest voting power to be first proposer
    vset.incrementAccum(1);
    prop = vset.getProposer();
    BOOST_CHECK_EQUAL(prop->getAddress().toString(),
                      addr0.getAddress().toString()); //"Expected smallest address to be validator"
}

// One validator has more than the others, and enough to be proposer twice in a row
void ValidatorSetTest::testProposerSelectionTwice() {

    std::vector<Validator> validators;
//        Validator addr0(PubKey(AddressTm(HexBytes("0"))), 100);
//        Validator addr1(PubKey(AddressTm(HexBytes("1"))), 100);
//        Validator addr2(PubKey(AddressTm(HexBytes("2"))), 401);
    Validator addr0(PrivKey(dev::Secret("0")).getKey(), 100);
    Validator addr1(PrivKey(dev::Secret("1")).getKey(), 100);
    Validator addr2(PrivKey(dev::Secret("2")).getKey(), 401);
    validators.push_back(addr0);
    validators.push_back(addr1);
    validators.push_back(addr2);
    ValidatorSet vset(validators);
    vset.incrementAccum(1);
    Validator *prop = vset.getProposer();
    //"Expect address with highest voting power to be first proposer
    BOOST_CHECK_EQUAL(prop->getAddress().toString(), addr2.getAddress().toString());
    vset.incrementAccum(1);
    prop = vset.getProposer();
    //"Expect address with highest voting power to be second proposer"
    BOOST_CHECK_EQUAL(prop->getAddress().toString(), addr2.getAddress().toString());
    vset.incrementAccum(1);
    prop = vset.getProposer();
    //"Expect smallest address to be validator"
    BOOST_CHECK_EQUAL(prop->getAddress().toString(), addr0.getAddress().toString());
    vset.incrementAccum(1);
}

// each validator should be the proposer a proportional number of times
void ValidatorSetTest::testProposerSelectionProportional() {
    std::vector<Validator> validators;
//        Validator addr0(PubKey(AddressTm(HexBytes("0"))), 4);
//        Validator addr1(PubKey(AddressTm(HexBytes("1"))), 5);
//        Validator addr2(PubKey(AddressTm(HexBytes("2"))), 3);
    Validator addr0(PrivKey(dev::Secret("0")).getKey(), 4, 0);
    Validator addr1(PrivKey(dev::Secret("1")).getKey(), 5, 1);
    Validator addr2(PrivKey(dev::Secret("2")).getKey(), 3, 2);
    validators.push_back(addr0);
    validators.push_back(addr1);
    validators.push_back(addr2);
    ValidatorSet vset(validators);
    vset.incrementAccum(1);
    int proposalCount[]{0, 0, 0};
    int n = 1;
    for (int i = 0; i < 120 * n; i++) {
        Validator *prop = vset.getProposer();
        proposalCount[prop->getIndex()]++;
        vset.incrementAccum(1);
    }
    // Expect prop count for validator with 4/12 of voting power to be 40
    BOOST_REQUIRE_EQUAL(40, proposalCount[0]);
    // Expect prop count for validator with 5/12 of voting power to be 50
    BOOST_REQUIRE_EQUAL(50, proposalCount[1]);
    // Expect prop count for validator with 3/12 of voting power to be 30
    BOOST_REQUIRE_EQUAL(30, proposalCount[2]);
}


void ValidatorSetTest::testProposerSelection3() {
    std::vector<Validator> validators;
//    Validator addr0(PubKey(AddressTm(HexBytes("a"))), 1);
//    Validator addr1(PubKey(AddressTm(HexBytes("b"))), 1);
//    Validator addr2(PubKey(AddressTm(HexBytes("c"))), 1);
//    Validator addr3(PubKey(AddressTm(HexBytes("d"))), 1);
    Validator addr0(PrivKey(dev::Secret("a")).getKey(), 1);
    Validator addr1(PrivKey(dev::Secret("b")).getKey(), 1);
    Validator addr2(PrivKey(dev::Secret("c")).getKey(), 1);
    Validator addr3(PrivKey(dev::Secret("d")).getKey(), 1);
    validators.push_back(addr0);
    validators.push_back(addr1);
    validators.push_back(addr2);
    validators.push_back(addr3);
    ValidatorSet vset(validators);
    AddressTm proposerOrder[4]{};
    //vset.incrementAccum(1);
    for (int i = 0; i < 4; i++) {
        Validator *prop = vset.getProposer();
        proposerOrder[i] = prop->getAddress();
        vset.incrementAccum(1);
    }

    // i for the loop
    // j for the times
    // we should go in order for ever, despite some IncrementAccums with times > 1
    int i = 0;
    int j = 0;
    for (; i < 10; i++) {
        AddressTm got = vset.getProposer()->address;
        AddressTm expected = proposerOrder[j % 4];
        BOOST_REQUIRE_EQUAL(expected.toString(), got.toString());

        // serialize, deserialize, check proposer
//    dev::RLP rlp = vset.toRLP();
//    ValidatorSet vset = ValidatorSet::fromBytes(rlp);

        Validator *computed = vset.getProposer(); // findGetProposer()
        if (i != 0) {
            BOOST_REQUIRE_EQUAL(computed->address.toString(),
                                got.toString()); // "vset.proposer (%X) does not match computed proposer (%X) for (%d, %d)", got, computed.address, i, j))"
        }
        std::srand(std::time(nullptr));
        // times is usually 1
        int times = (std::rand() % 5) + 1;
        vset.incrementAccum(times);
        j += times;
    }
};

/*
func newValidator(address []byte, power int64) *Validator {
return &Validator{Address: address, VotingPower: power}
}

func randPubKey() crypto.pubKey {
var pubKey [32]byte
        copy(pubKey[:], cmn.randBytes(32))
return ed25519.pubKeyEd25519(pubKey)
}

func randValidator_() *Validator {
val = NewValidator(randPubKey(), cmn.randInt64())
val.accum = cmn.randInt64()
return val
}

func randValidatorSet(numValidators int) *ValidatorSet {
validators = make([]*Validator, numValidators)
for i = 0; i < numValidators; i++ {
validators[i] = randValidator_()
}
return NewValidatorSet(validators)
}

func (valSet *ValidatorSet) toBytes() []byte {
bz, err = cdc.marshalBinary(valSet)
if err != nil {
panic(err)
}
return bz
}

func (valSet *ValidatorSet) fromBytes(b []byte) {
err = cdc.unmarshalBinary(b, &valSet)
if err != nil {
// DATA HAS BEEN CORRUPTED OR THE SPEC HAS CHANGED
panic(err)
}
}

//-------------------------------------------------------------------

func TestValidatorSetTotalVotingPowerOverflows(t *testing.t) {
vset = NewValidatorSet([]*Validator{
        {Address: []byte("a"), VotingPower: math.maxInt64, Accum: 0},
        {Address: []byte("b"), VotingPower: math.maxInt64, Accum: 0},
        {Address: []byte("c"), VotingPower: math.maxInt64, Accum: 0},
})

assert.equalValues(t, math.maxInt64, vset.totalVotingPower())
}

func TestValidatorSetIncrementAccumOverflows(t *testing.t) {
    // NewValidatorSet calls IncrementAccum(1)
    vset = NewValidatorSet([]*Validator{
            // too much voting power
            0: {Address: []byte("a"), VotingPower: math.maxInt64, Accum: 0},
            // too big accum
            1: {Address: []byte("b"), VotingPower: 10, Accum: math.maxInt64},
            // almost too big accum
            2: {Address: []byte("c"), VotingPower: 10, Accum: math.maxInt64 - 5},
    })

    BOOST_REQUIRE_EQUAL(int64(0), vset.validators[0].accum, "0") // because we decrement val with most voting power;
    assert.equalValues(t, math.maxInt64, vset.validators[1].accum, "1")
    assert.equalValues(t, math.maxInt64, vset.validators[2].accum, "2")
}

func TestValidatorSetIncrementAccumUnderflows(t *testing.t) {
    // NewValidatorSet calls IncrementAccum(1)
    vset = NewValidatorSet([]*Validator{
            0: {Address: []byte("a"), VotingPower: math.maxInt64, Accum: math.minInt64},
            1: {Address: []byte("b"), VotingPower: 1, Accum: math.minInt64},
    })

    vset.incrementAccum(5)

    assert.equalValues(t, math.minInt64, vset.validators[0].accum, "0")
    assert.equalValues(t, math.minInt64, vset.validators[1].accum, "1")
}

func TestSafeMul(t *testing.t) {
    f = func(a, b int64) bool {
            c, overflow = safeMul(a, b)
            return overflow || (!overflow && c == a*b)
    }
    if err = quick.check(f, nil); err != nil {
            t.error(err)
    }
}

func TestSafeAdd(t *testing.t) {
    f = func(a, b int64) bool {
            c, overflow = safeAdd(a, b)
            return overflow || (!overflow && c == a+b)
    }
    if err = quick.check(f, nil); err != nil {
            t.error(err)
    }
}

func TestSafeMulClip(t *testing.t) {
    assert.equalValues(t, math.maxInt64, safeMulClip(math.minInt64, math.minInt64))
    assert.equalValues(t, math.minInt64, safeMulClip(math.maxInt64, math.minInt64))
    assert.equalValues(t, math.minInt64, safeMulClip(math.minInt64, math.maxInt64))
    assert.equalValues(t, math.maxInt64, safeMulClip(math.maxInt64, 2))
}

func TestSafeAddClip(t *testing.t) {
    assert.equalValues(t, math.maxInt64, safeAddClip(math.maxInt64, 10))
    assert.equalValues(t, math.maxInt64, safeAddClip(math.maxInt64, math.maxInt64))
    assert.equalValues(t, math.minInt64, safeAddClip(math.minInt64, -10))
}

func TestSafeSubClip(t *testing.t) {
    assert.equalValues(t, math.minInt64, safeSubClip(math.minInt64, 10))
    assert.equalValues(t, 0, safeSubClip(math.minInt64, math.minInt64))
    assert.equalValues(t, math.minInt64, safeSubClip(math.minInt64, math.maxInt64))
    assert.equalValues(t, math.maxInt64, safeSubClip(math.maxInt64, -10))
}

//-------------------------------------------------------------------
*/
void ValidatorSetTest::testValidatorSetVerifyCommit() {
    PrivKey privKey;
    PubKey pubKey = privKey.getPubKey();
    Validator v1(pubKey, 1000);
    ValidatorSet vset({v1});

    std::string chainID = "mychainID";
    BlockID blockID(HexBytes("hello"));
    height_t height(5);

    Vote vote(v1.address,
              0,
              height,
              0,
              boost::posix_time::second_clock::local_time(),
              VoteTypePrecommit,
              blockID
    );
    SignatureTm sig;
    BOOST_REQUIRE_NO_THROW(sig = privKey.sign(vote.signBytes(chainID)));
    vote.setSignature(sig);
    std::map<int, Vote> precommits;
    precommits.emplace(0, vote);
    Commit commit(blockID, precommits);

    Vote nullVote(v1.address,
                  0,
                  height,
                  0,
                  boost::posix_time::second_clock::local_time(),
                  VoteTypePrecommit,
                  BlockID()
    );

    std::string badChainID = "notmychainID";
    BlockID badBlockID(HexBytes("goodbye"));
    height_t badHeight = height + 1;
    std::map<int, Vote> badPrecommits;
    badPrecommits.emplace(0, nullVote);
    Commit badCommit(badBlockID, badPrecommits);

    BOOST_CHECK_THROW(vset.verifyCommit(badChainID, blockID, height, commit), Error);
    BOOST_CHECK_THROW(vset.verifyCommit(chainID, badBlockID, height, commit), Error);
    BOOST_CHECK_THROW(vset.verifyCommit(chainID, blockID, badHeight, commit), Error);
    BOOST_CHECK_THROW(vset.verifyCommit(chainID, blockID, height, badCommit), Error);

    // test a good one
    BOOST_CHECK_NO_THROW(vset.verifyCommit(chainID, blockID, height, commit));
}