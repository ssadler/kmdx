//
// Created by Maru on 7/9/2018.
//

#include "Validator.h"

Validator::Validator(PubKey _pubKey, int64_t _votingPower) : addresstm(_pubKey.getAddress()), pubKey(_pubKey) {
    votingPower = _votingPower;
    accum = 0;
}


bool ValidatorSet::hasAddress(const HexBytes bytes) {
    for (Validator v : validators) {
        if (v.getAddress() == bytes) return true;
    }
    return false;
}

boost::optional<Validator> ValidatorSet::getByIndex(int index) {
    if (index < 0 || (unsigned) index > validators.size())
        return boost::optional<Validator>();
    return this->validators.at((unsigned) index);
}

boost::optional<Validator> ValidatorSet::getByAddress(Address addresstm) {
//FIXME better way to return validadtor index?
    for (Validator &v: validators) {
        if (v.getAddress() == addresstm)
            return v;
    }
    return boost::optional<Validator>();
}

unsigned long ValidatorSet::size() {
    return this->validators.size();
}

unsigned int ValidatorSet::getTotalVotingPower() {
    return validators.size();
}

ValidatorSet::ValidatorSet() {}

ValidatorSet::ValidatorSet(const std::vector<Validator> &validators) : validators(validators) {}

const boost::optional<Validator> ValidatorSet::getProposer() {
    boost::optional<Validator> proposer;
    if (validators.empty()) return proposer;
    if (!this->proposer.is_initialized()) this->proposer = findProposer();
    return this->proposer;
}

boost::optional<Validator> ValidatorSet::findProposer() const {
    boost::optional<Validator> proposer;
    for (Validator const &val : validators) {
        if (!proposer.is_initialized() || val.getAddress() != proposer->getAddress()) {
            proposer = proposer->compareAccum(val);
        }
    }
    return proposer;
}

/* IncrementAccum increments accum of each validator and updates the
* proposer. Panics if validator set is empty.*/
void ValidatorSet::incrementAccum(int i) {
    i++;
/*// Add VotingPower * times to each validator and order into heap.
	validatorsHeap := cmn.NewHeap()
	for _, val := range valSet.Validators {
		// check for overflow both multiplication and sum
		val.Accum = safeAddClip(val.Accum, safeMulClip(val.VotingPower, int64(times)))
		validatorsHeap.PushComparable(val, accumComparable{val})
	}

	// Decrement the validator with most accum times times
	for i := 0; i < times; i++ {
		mostest := validatorsHeap.Peek().(*Validator)
		// mind underflow
		mostest.Accum = safeSubClip(mostest.Accum, valSet.TotalVotingPower())

		if i == times-1 {
			valSet.Proposer = mostest
		} else {
			validatorsHeap.Update(mostest, accumComparable{mostest})
		}
	}*/
}

HexBytes ValidatorSet::hash() const {
    //TODO
    return HexBytes();
}

Validator Validator::compareAccum(const Validator &other) const {
    if (this->accum > other.accum)
        return *this;
    if (this->accum < other.accum)
        return other;
    if (this->addresstm == other.addresstm)
        throw PanicSanity("Cannot compare identical validators", __FILE__, __LINE__);
    return this->addresstm < other.addresstm ? *this : other;
}

void Validator::signProposal(const std::string &chainID, Proposal &proposal) {
    if (chainID == "" || proposal.toString() == "")
        throw SignError("unable to sign proposal", __FILE__, __LINE__); //TODO
}

const Address &Validator::getAddress() const {
    return addresstm;
}

const PubKey &Validator::getPubKey() const {
    return pubKey;
}

int64_t Validator::getVotingPower() const {
    return votingPower;
}

int64_t Validator::getAccum() const {
    return accum;
}

std::string Validator::toString() const {
    return addresstm.toString();
}

int Validator::getIndex() const {
    return index;
}

