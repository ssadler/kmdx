
#include "VoteSet.h"

//
// Created by utnso on 11/07/18.
//



const boost::optional<Vote> VoteSet::getByAddress(HexBytes address) const {
    for (auto const &iterator : votes) {
        Vote const &vote = iterator.second;
        if (vote.getValidatorAddress() == address)
            return boost::optional<Vote>(vote);

    }
    return boost::optional<Vote>();
}

VoteSet::VoteSet(const string &_chainID, int64_t _blockHeight, int _round, VoteType _type, ValidatorSet _valSet)
        : chainID(_chainID),
          height(_blockHeight),
          roundNumber(_round),
          type(_type),
          valSet(_valSet) {};

VoteSet::VoteSet(const VoteSet &other) :
        chainID(other.chainID),
        height(other.height),
        roundNumber(other.roundNumber),
        type(other.type),
        valSet(other.valSet) {
    votes = other.votes;
    sum = other.sum;
    maj23 = other.maj23;
    votesByBlock = other.votesByBlock;
}

VoteSet &VoteSet::operator=(const VoteSet &other) {
    if (&other == this)
        return *this;
    chainID = other.chainID;
    height = other.height;
    roundNumber = other.roundNumber;
    type = other.type;
    valSet = ValidatorSet(other.valSet);
    votes = std::map<int, Vote>(other.votes);
    sum = other.sum;
    maj23 = other.maj23;
    votesByBlock = std::map<std::string, BlockVotes>(other.votesByBlock);
    return *this;
}

const boost::optional<Vote> VoteSet::get(int n) const {
    return votes.count(n) ? votes.at(n) : boost::optional<Vote>();
}

bool VoteSet::priv_addVote(Vote &vote) {
    int valIndex = vote.getValidatorIndex();
    Address valAddr = vote.getValidatorAddress();
    std::string blockKey = vote.getBlockID().key();

// Ensure that validator index was set
    if (valIndex < 0) {
        throw ErrAddingVote("Index < 0", __FILE__, __LINE__);
    } else if (valAddr.empty()) {
        throw ErrAddingVote("Empty address", __FILE__, __LINE__);
    }

// Make sure the step matches.
    if (vote.getHeight() != this->height || vote.getRoundNumber() != this->roundNumber ||
        vote.getType() != this->type) {
        std::ostringstream out;
        out << "ErrVoteUnexpectedStep, Got %d/%d/%d, expected %d/%d/%d" <<
            this->height << this->roundNumber << Vote::voteTypeToString(this->type) << vote.getHeight()
            << vote.getRoundNumber()
            << Vote::voteTypeToString(vote.getType());
        throw ErrAddingVote(out.str(), __FILE__, __LINE__);
    }

// Ensure that signer is a validator.
    boost::optional<Validator> val = valSet.getByIndex(valIndex);
    if (!val.is_initialized()) {
        std::ostringstream out;
        out << "Cannot find validator %d in valSet of size %d" << valIndex << this->valSet.size();
        throw ErrAddingVote(out.str(), __FILE__, __LINE__);
    }

// Ensure that the signer has the right address
    if (valAddr != val.get().getAddress()) {
        std::ostringstream out;
        out
                << "vote.ValidatorAddress (%s) does not match address (%s) for vote.ValidatorIndex (%d)\n Ensure the genesis file is correct across all validators."
                << valAddr.toString() << val.get().getAddress().toString() << valIndex;
        throw ErrAddingVote(out.str(), __FILE__, __LINE__);
    }

// If we already know of this vote, return false.
    boost::optional<Vote> existing = this->getVote(valIndex, blockKey);
    if (existing.is_initialized()) {
        if (existing.get().getSignature() == vote.getSignature()) {
            return false; // duplicate
        }
        std::ostringstream out;
        out
                << "ErrVoteNonDeterministicSignature, Existing vote: %s; New vote: %s", existing.get().toString(), vote.toString();
        throw ErrAddingVote(out.str(), __FILE__, __LINE__);
    }

// Check signature.
    try {
        vote.verify(this->chainID, val.get().getPubKey());
    } catch (Error &e) {
        std::ostringstream out;
        out << "Failed to verify vote with ChainID %s and PubKey %s" << this->chainID <<
            val.get().getPubKey().toString();
        throw ErrAddingVote(out.str(), __FILE__, __LINE__);
    }

// Add vote and get conflicting vote if any
    boost::optional<Vote> conflicting;
    bool added = this->addVerifiedVote(vote, blockKey, val.get().getVotingPower(), conflicting);
    if (conflicting.is_initialized()) {
        std::ostringstream out;
        out << "Conflicting vote exists for validator %s, conflicting: %s, new: %s" << val.get().toString()
            << conflicting.get().toString()
            << vote.toString();
        throw ErrAddingVote(out.str(), __FILE__, __LINE__);
    }
    if (!added) {
        throw Panic("Error Adding vote: Expected to add non-conflicting vote", __FILE__, __LINE__);
    }
    return added;
}

// Assumes signature is valid.
// If conflicting vote exists, returns it.
bool VoteSet::addVerifiedVote(Vote vote, std::string blockKey, int64_t votingPower, boost::optional<Vote> conflicting) {
    int valIndex = vote.getValidatorIndex();

    // Already exists in voteSet.votes?
    boost::optional<Vote> existing = get(valIndex);
    if (existing.is_initialized()) {
        if (existing.get().getBlockID() == vote.getBlockID()) {
            throw PanicSanity("addVerifiedVote does not expect duplicate votes", __FILE__, __LINE__);
        } else {
            conflicting = existing.get();
        }
        // Replace vote if blockKey matches voteSet.maj23.
        if (this->maj23.is_initialized() && this->maj23.get().key() == blockKey) {
            this->votes.emplace(std::make_pair(valIndex, vote));
        }
        // Otherwise don't add it to voteSet.votes
    } else {
        // Add to voteSet.votes and incr .sum
        this->votes.emplace(std::make_pair(valIndex, vote));
        this->sum += votingPower;
    }

    boost::optional<BlockVotes> votesByBlock = this->votesByBlock.at(blockKey);
    if (votesByBlock.is_initialized()) {
        if (conflicting.is_initialized() && !votesByBlock.get().isPeerMaj23()) {
            // There's a conflict and no peer claims that this block is special.
            return false;
        }
        // We'll add the vote in a bit.
    } else {
        // .votesByBlock doesn't exist...
        if (conflicting.is_initialized()) {
            // ... and there's a conflicting vote.
            // We're not even tracking this blockKey, so just forget it. //FIXME how do we know this?
            return false;
        }
        // ... and there's no conflicting vote.
        // Start tracking this blockKey
        this->votesByBlock.insert({blockKey, BlockVotes(false)});
        // We'll add the vote in a bit.
    }

    // Before adding to votesByBlock, see if we'll exceed quorum
    int64_t origSum = votesByBlock.get().getSum();
    float quorum = valSet.getTotalVotingPower() * 2 / 3 + 1;

    // Add vote to votesByBlock
    votesByBlock.get().addVerifiedVote(vote, votingPower);

    // If we just crossed the quorum threshold and have 2/3 majority...
    if (origSum < quorum && quorum <= votesByBlock.get().getSum()) {
        // Only consider the first quorum reached
        if (!maj23.is_initialized()) {
            this->maj23 = vote.getBlockID();
            // And also copy votes over to voteSet.votes
            for (auto const &iterator : votesByBlock.get().getVotes()) {
                const Vote &voteInBlock = iterator.second;
                this->votes.emplace(std::make_pair(valIndex, voteInBlock));
            }
        }
    }
    return true;
}

std::string VoteSet::toStringShort() const {
    std::ostringstream output;
    output << "VoteSet size: %d " << votes.size();
    for (auto const &iterator : votes) {
        output << iterator.second.toString();
        //const Vote &Vote =
        //iterator.second.toString;
        //output<<vote.toString();
    }
    return output.str();
}

boost::optional<Vote> VoteSet::getVote(int valIndex, std::string blockKey) {
    boost::optional<Vote> existing = get(valIndex);
    if (existing.is_initialized() && existing.get().getBlockID().key() == blockKey)
        return existing;
    existing = votesByBlock.at(blockKey).getVotes().at(valIndex);
    return existing;
}

const boost::optional<BlockVotes> VoteSet::getBlockVotes(std::string n) const {
    return votesByBlock.count(n) ? votesByBlock.at(n) : boost::optional<BlockVotes>();
}

boost::optional<BlockID> VoteSet::twoThirdMajority() { //
    if (!this->votes.empty()) {
        std::lock_guard<std::mutex> lock(mtx);

        if (this->maj23.is_initialized()) {
            return boost::optional<BlockID>(this->maj23.get());
        }
    }
    return boost::optional<BlockID>();
}

bool VoteSet::hasTwoThirdsAny() {
    if (this->votes.empty()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(mtx);
    return this->sum > this->valSet.getTotalVotingPower() * 2 / 3;
}

bool VoteSet::hasTwoThirdMajority() {
    if (this->votes.empty()) return false;
    std::lock_guard<std::mutex> lock(mtx);

    return this->maj23.is_initialized();
}

bool VoteSet::hasAll() {
    std::lock_guard<std::mutex> lock(mtx);
    return this->sum == valSet.getTotalVotingPower();
}

bool VoteSet::addVote(Vote vote) {
    /*if (!votes.is_initialized()) {
        throw PanicNullVoteSet__FILE__, __LINE__);
    }*/
    std::lock_guard<std::mutex> lock(mtx);
    return priv_addVote(vote);
}

int64_t VoteSet::getHeight() const {
    return height;
};

const std::vector<Signature> VoteSet::getSignatures() const {
    std::vector<Signature> output;
    for (auto const &iterator : votes) {
        output.push_back(iterator.second.getSignature());
    }
    return output;
}

BlockVotes::BlockVotes(bool _peerMaj23, std::map<int, Vote> _votes) : peerMaj23(_peerMaj23), votes(_votes) {
}

BlockVotes::BlockVotes(bool _peerMaj23) : peerMaj23(_peerMaj23) {

}

const map<int, Vote> VoteSet::getVotes() {
    lock_guard<std::mutex> lock(mtx);
    return map<int, Vote>();
}

