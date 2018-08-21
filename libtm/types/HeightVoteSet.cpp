//
// Created by utnso on 11/07/18.
//
#include "HeightVoteSet.h"

boost::optional<VoteSet> HeightVoteSet::getPrevotes(int round) {
    std::lock_guard<std::mutex> lock(mtx);
    return getVoteSet(round, VoteTypePrevote);
}

boost::optional<VoteSet> HeightVoteSet::getPrecommits(int round) {
    std::lock_guard<std::mutex> lock(mtx);
    return getVoteSet(round, VoteTypePrecommit);
}

boost::optional<VoteSet> HeightVoteSet::getVoteSet(int round, VoteType type) {
    boost::optional<RoundVoteSet> rvs = roundVoteSets.at(round);
    boost::optional<VoteSet> votes;
    if (!rvs.is_initialized())
        return votes;
    switch (type) {
        case VoteTypePrevote:
            return rvs.get().getPrevotes();
        case VoteTypePrecommit:
            return rvs.get().getPrecommits();
        default:
            std::ostringstream outString;
            outString << "Unexpected vote type %s", Vote::voteTypeToString(type);
            throw PanicSanity(outString.str(), __FILE__, __LINE__);
    }
}

HeightVoteSet::HeightVoteSet(const string &chainID) : mtx(), chainID(chainID), valSet() {}

HeightVoteSet::HeightVoteSet(const string &chainID, int64_t height, const ValidatorSet &valSet)
        : mtx(), chainID(chainID),
          height(height),
          valSet(valSet) {}

HeightVoteSet::HeightVoteSet(const HeightVoteSet &other) : mtx(), chainID(other.chainID), height(other.height),
                                                           valSet(other.valSet), roundNumber(other.roundNumber),
                                                           roundVoteSets(other.roundVoteSets),
                                                           peerCatchupRounds(other.peerCatchupRounds) {}

RoundVoteSet::RoundVoteSet(VoteSet &prevotes, VoteSet &precommits) : prevotes(prevotes), precommits(precommits) {}

VoteSet &RoundVoteSet::getPrevotes() const {
    return prevotes;
}

VoteSet &RoundVoteSet::getPrecommits() const {
    return precommits;
}

int HeightVoteSet::polInfo(BlockID &blockID) {
    std::lock_guard<std::mutex> lock(mtx);
    for (int r = roundNumber; r >= 0; r--) {
        boost::optional<VoteSet> rvs = getVoteSet(r, VoteTypePrevote);
        if (rvs.is_initialized()) {
            if (rvs.get().hasTwoThirdMajority()) {
                blockID = rvs.get().twoThirdMajority().get();
                return r;
            }
        }
    }
    return -1;
}

void HeightVoteSet::setRoundNumber(int _roundNumber) {
    this->roundNumber = _roundNumber;
}

bool HeightVoteSet::isEmpty() {
    return roundVoteSets.empty();//TODO review
}

bool HeightVoteSet::addVote(Vote vote, HexBytes bytes) {
    return (vote.getType() == VoteTypeFirstCommit || bytes.empty());
    //TODO unimplemented
}
