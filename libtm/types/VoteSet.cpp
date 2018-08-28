
#include "VoteSet.h"

//
// Created by utnso on 11/07/18.
//

const boost::optional<Vote> VoteSet::getByAddress(Address addresstm) const {
    for (auto const &iterator : votes) {
        Vote const &vote = iterator.second;
        if (vote.getValidatorAddress() == addresstm)
            return boost::optional<Vote>(vote);

    }
    return boost::optional<Vote>();
}

VoteSet::VoteSet(const string &_chainID, height_t _blockHeight, int _round, VoteType _type, ValidatorSet _valSet)
        : chainID(_chainID),
          height(_blockHeight),
          roundNumber(_round),
          type(_type),
          valSet(_valSet), sum(0) {};

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

// NOTE: Validates as much as possible before attempting to verify the signature.
AddVoteResult VoteSet::priv_addVote(Vote &vote, boost::optional<Vote> &conflicting) {

    cout << vote.getValidatorIndex();
    int valIndex = vote.getValidatorIndex();
    Address valAddr = vote.getValidatorAddress();
    std::string blockKey = vote.getBlockID().key();

// Ensure that validator index was set
    if (valIndex < 0) {
        return AddVoteResult(false, true, "Index < 0");
    } else if (valAddr.empty()) {
        return AddVoteResult(false, true, "Empty addresstm");
    }

// Make sure the step matches.
    if (vote.getHeight() != this->height || vote.getRoundNumber() != this->roundNumber ||
        vote.getType() != this->type) {
        std::string str;
        boost::format f("ErrVoteUnexpectedStep, expected height/round/stepType %i /%i /%s, Got %i /%i /%s, ");
        f % (int) vote.getHeight() % vote.getRoundNumber() %
        Vote::voteTypeToString(vote.getType())
        % (int) this->height %
        this->roundNumber % Vote::voteTypeToString(this->type);
        str = f.str();
        return AddVoteResult(false, true, str);
    }

// Ensure that signer is a validator.
    boost::optional<Validator> val = valSet.getByIndex(valIndex);
    if (!val.is_initialized()) {
        std::ostringstream out;
        out << "Cannot find validator %d in valSet of size %d" << valIndex << this->valSet.size();
        return AddVoteResult(false, true, out.str());
    }

// Ensure that the signer has the right addresstm
    if (valAddr != val->getAddress()) {
        std::ostringstream out;
        out
                << "vote.ValidatorAddress (%s) does not match addresstm (%s) for vote.ValidatorIndex (%d)\n Ensure the genesis file is correct across all validators."
                << valAddr.toString() << val->getAddress().toString() << valIndex;
        return AddVoteResult(false, true, out.str());
    }

// If we already know of this vote, return false.
    boost::optional<Vote> existing = this->getVote(valIndex, blockKey);
    if (existing.is_initialized()) {
        if (existing->getSignature() == vote.getSignature()) {
            cout << "existing: " << existing->getSignature().toString() << " new vote: "
                 << vote.getSignature().toString();
            return AddVoteResult(false, true, "duplicate vote");
        }
        std::ostringstream out;
        out
                << "ErrVoteNonDeterministicSignature, Existing vote: %s; New vote: %s", existing->toString(), vote.toString();
        return AddVoteResult(false, true, out.str());
    }

// Check signature.
    try {
        vote.verify(this->chainID, val->getPubKey());
    } catch (Error &e) {
        std::ostringstream out;
        out << "Failed to verify vote with ChainID %s and PubKey %s" << this->chainID <<
            val->getPubKey().toString();
        return AddVoteResult(false, true, out.str());
    }

// Add vote and get conflicting vote if any
//    boost::optional<Vote> conflicting;
    AddVoteResult added = this->addVerifiedVote(vote, blockKey, val->getVotingPower(), conflicting);
    if (conflicting.is_initialized()) {
        std::ostringstream out;
        out << "Conflicting vote exists for validator %s, conflicting: %s, new: %s" << val->toString()
            << conflicting->toString()
            << vote.toString();
        return AddVoteResult(added.isAdded(), true, out.str());
    }
    if (!added.isAdded()) {
        throw Panic("Error Adding vote: Expected to add non-conflicting vote", __FILE__, __LINE__);
    }
    return added;
}

// Assumes signature is valid.
// If conflicting vote exists, returns it.
AddVoteResult
VoteSet::addVerifiedVote(Vote vote, std::string blockKey, int64_t votingPower, boost::optional<Vote> &conflicting) {
    int valIndex = vote.getValidatorIndex();

    // Already exists in voteSet.votes?
    boost::optional<Vote> existing = get(valIndex);
    if (existing.is_initialized()) {
        if (existing->getBlockID() == vote.getBlockID()) {
            throw PanicSanity("addVerifiedVote does not expect duplicate votes", __FILE__, __LINE__);
        } else {
            conflicting = existing.get();
        }
        // Replace vote if blockKey matches voteSet.maj23.
        if (this->maj23.is_initialized() && this->maj23->key() == blockKey) {
            this->votes.emplace(std::make_pair(valIndex, vote));
        }
        // Otherwise don't add it to voteSet.votes
    } else {
        // Add to voteSet.votes and incr .sum
        this->votes.emplace(std::make_pair(valIndex, vote));
        this->sum += votingPower;
    }

    BlockVotes *votesByBlock = getBlockVotes(blockKey);
    if (votesByBlock) {
        if (conflicting.is_initialized() && !votesByBlock->isPeerMaj23()) {
            // There's a conflict and no peer claims that this block is special.
            return AddVoteResult(false, true, "vote conflict - no peer maj 23");
        }
        // We'll add the vote in a bit.
    } else {
        // .votesByBlock doesn't exist...
        if (conflicting.is_initialized()) {
            // ... and there's a conflicting vote.
            // We're not even tracking this blockKey, so just forget it. //FIXME how do we know this?
            return AddVoteResult(false, true, "vote conflict - will ignore");
        }
        // ... and there's no conflicting vote.
        // Start tracking this blockKey
        this->votesByBlock.insert({blockKey, BlockVotes(false)});
        votesByBlock = getBlockVotes(blockKey);
        // We'll add the vote in a bit.
    }

    // Before adding to votesByBlock, see if we'll exceed quorum
    int64_t origSum = votesByBlock->getSum();
    int quorum = (int) (valSet.getTotalVotingPower() * 2 / 3) + 1;
    // Add vote to votesByBlock
    votesByBlock->addVerifiedVote(vote, votingPower);

    // If we just crossed the quorum threshold and have 2/3 majority...
    if (origSum < quorum && quorum <= votesByBlock->getSum()) {
        // Only consider the first quorum reached
        if (!maj23.is_initialized()) {
            setMaj23(vote.getBlockID());
            // And also copy votes over to voteSet.votes
            for (auto const &iterator : votesByBlock->getVotes()) {
                const Vote &voteInBlock = iterator.second;
                this->votes.emplace(std::make_pair(valIndex, voteInBlock));
            }
        }
    }
    return AddVoteResult();
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
    if (existing.is_initialized() && existing->getBlockID().key() == blockKey)
        return existing;
    if (getBlockVotes(blockKey))
        existing = getBlockVotes(blockKey)->get(valIndex);
    return existing;
}

BlockVotes *VoteSet::getBlockVotes(std::string n) {
    return votesByBlock.count(n) ? &votesByBlock.at(n) : nullptr;
}

boost::optional<BlockID> VoteSet::twoThirdMajority() {
    if (!this->votes.empty()) {
        std::lock_guard<std::mutex> lock(mtx);

        if (maj23.is_initialized()) {
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

    return this->maj23.is_initialized();// && !maj23->isEmpty();
}

bool VoteSet::hasAll() {
    std::lock_guard<std::mutex> lock(mtx);
    return this->sum == valSet.getTotalVotingPower();
}

AddVoteResult VoteSet::addVote(Vote &vote, boost::optional<Vote> &conflicting) {
    /*if (!votes.is_initialized()) {
        throw PanicNullVoteSet__FILE__, __LINE__);
    }*/
    std::lock_guard<std::mutex> lock(mtx);
    return priv_addVote(vote, conflicting);
}

height_t VoteSet::getHeight() const {
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
    sum = 0;
}

BlockVotes::BlockVotes(bool _peerMaj23) : peerMaj23(_peerMaj23) {
    sum = 0;
}

const map<int, Vote> VoteSet::getVotes() {
    lock_guard<std::mutex> lock(mtx);
    return votes;
}

BlockID *VoteSet::getPeerMaj23s(const P2PID bytes) {
    return peerMaj23s.count(bytes) ? &peerMaj23s.at(bytes) : nullptr;
}

AddVoteResult VoteSet::setPeerMaj23(const P2PID peerID, BlockID blockID) {
    lock_guard<std::mutex> lock(mtx);

    std::string blockKey = blockID.key();

    // Make sure peer hasn't already told us something.
    BlockID *existing = getPeerMaj23s(peerID);
    if (existing) {
        if (*existing == blockID) {
            return AddVoteResult(false, false, "we already know this");
        }

        std::string out("SetPeerMaj23: Received conflicting blockID from peer ");
        out += peerID.toString();
        out += ". Got ";
        out += blockID.toString();
        out += ", expected ";
        out += existing->toString();
        return AddVoteResult(false, true, out);
    }
    peerMaj23s.emplace(std::make_pair(peerID, blockID));

    // Create .votesByBlock entry if needed.
    BlockVotes *votesByBlock = getBlockVotes(blockKey);
    if (votesByBlock) {
        if (votesByBlock->getPeerMaj23()) {
            return AddVoteResult(false, false, "nothing to do");// Nothing to do
        }
        votesByBlock->setPeerMaj23(true);
        // No need to copy votes, already there.
    } else {
        this->votesByBlock.emplace(std::make_pair(blockKey, BlockVotes(true)));//, valSet.size());
        // No need to copy votes, no votes to copy over.
    }
    return AddVoteResult(true, false, "");
}

Commit VoteSet::makeCommit() {
    if (type != VoteTypePrecommit) {
        throw PanicSanity("Cannot MakeCommit() unless VoteSet.Type is VoteTypePrecommit", __FILE__, __LINE__);
    }
    lock_guard<std::mutex> lock(mtx);

    // Make sure we have a 2/3 majority
    if (!maj23.is_initialized()) {
        throw PanicSanity("Cannot MakeCommit() unless a blockhash has +2/3", __FILE__, __LINE__);
    }

    // For every validator, get the precommit
    std::map<int, Vote> precommits;
    for (unsigned int i = 0; i < votes.size(); i++) {
        boost::optional<Vote> vote = get(i);
        if (vote.is_initialized())
            precommits.emplace(std::make_pair(i, vote.get()));
    }
    BlockID maj23id = BlockID(maj23.get());
    return Commit(maj23id, precommits);
}

void VoteSet::setMaj23(const BlockID &_maj23) {
    maj23 = _maj23;
}

