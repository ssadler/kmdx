//
// Created by thpn on 28/08/18.
//

#include "ValidatorSet.h"

boost::optional<Validator> ValidatorSet::getByAddress(AddressTm addresstm) {
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

int64_t ValidatorSet::getTotalVotingPower() {
    int64_t sum = 0;
    for (Validator &val:validators) {
        sum += val.getVotingPower();
    }
    return sum;
}

ValidatorSet::ValidatorSet() {}

ValidatorSet::ValidatorSet(const std::vector<Validator> &validators) : validators(validators), proposer(nullptr) {}

Validator *ValidatorSet::getProposer() {
    if (validators.empty())
        return nullptr;
    if (!proposer)
        proposer = findProposer();
    return proposer;

}

Validator *ValidatorSet::findProposer() {
    if (validators.empty()) return nullptr;
    if (validators.size() == 1)
        proposer = &validators[0];
    if (!proposer) proposer = &validators[0];
    for (Validator &val : validators) {
        if (val.getAddress() != proposer->getAddress()) {
            proposer = proposer->compareAccum(val);
        }
    }
    return proposer;
}


/* IncrementAccum increments accum of each validator and updates the
* proposer. Panics if validator set is empty.*/
void ValidatorSet::incrementAccum(int times) {
    if (validators.empty()) throw Panic("Empty validator set", __FILE__, __LINE__);
// Add VotingPower * times to each validator and order into heap.
    for (Validator &val : validators) {
        val.incrementAccum(times);
    }
    auto cmp = [](const Validator &a, const Validator &b) {
        if (a.getAccum() != b.getAccum())
            return a.getAccum() < b.getAccum();
        return a.getAddress() > b.getAddress();
    };
// Decrement the validator with most accum times times
    for (int i = 0; i < times; ++i) {
        auto m = max_element(validators.begin(), validators.end(),
                             cmp);//, cmp);//, []( Validator a, Validator b ) { return a > b; });
        m.base()->decrementAccum(getTotalVotingPower());
        if (i == times - 1)
            proposer = (Validator *) m.base();
    }
/*	validatorsHeap := cmn.NewHeap()
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
    std::ostringstream hash;
    for (const Validator &v: validators) {
        hash << v.hash().toString();
    }
    return HexBytes(hash.str());
    //TODO
}

bool ValidatorSet::hasAddress(const HexBytes bytes) {
    for (Validator v : validators) {
        if (v.getAddress() == bytes) return true;
    }
    return false;
}

boost::optional<Validator> ValidatorSet::getByIndex(int index) {
    if (validators.empty() || index < 0 || (unsigned) index > validators.size())
        return boost::optional<Validator>();
    return validators[index];
}


bool ValidatorSet::add(Validator val) {
    std::lock_guard<std::mutex> lock(mtx);
    if (getByAddress(val.getAddress()).is_initialized())
        return false;
    validators.push_back(val);
    proposer = nullptr;
//        totalVotingPower = 0;
    return true;
}

ValidatorSet::ValidatorSet(const ValidatorSet &other) :
        ValidatorSet(other.validators, other.proposer) {
}

ValidatorSet &ValidatorSet::operator=(const ValidatorSet &other) {
    this->validators = other.validators;
    this->proposer = other.proposer;
    return *this;
}

ValidatorSet::ValidatorSet(const std::vector<Validator> &_validators, Validator *_proposer) :
        validators(_validators), proposer(_proposer) {
}

int ValidatorSet::getIndex(Validator &val) {
    for (unsigned int j = 0; j < validators.size(); ++j) {
        if (validators[j].getAddress() == val.getAddress()) return j;
    }
    return -1;
}

bool ValidatorSet::remove(Validator &val) {
    std::lock_guard<std::mutex> lock(mtx);
    int index = getIndex(val);
    if (index < 0) return false;
    validators.erase(validators.begin() + ((unsigned) index));
    proposer = nullptr;
    //totalVotingPower = 0
    return true;
}

bool ValidatorSet::update(Validator &val) {
    std::lock_guard<std::mutex> lock(mtx);
    int i = getIndex(val);
    if (i < 0) return false;
    validators[i] = val;
    // Invalidate cache
    proposer = nullptr;
    //totalVotingPower = 0;
    return true;
}
