//
// Created by utnso on 11/07/18.
//
#include "HeightVoteSet.h"

VoteSet *HeightVoteSet::getPrevotes(int round) {
    std::lock_guard<std::mutex> lock(mtx);
    return getVoteSet(round, VoteTypePrevote);
}

VoteSet *HeightVoteSet::getPrecommits(int round) {
    std::lock_guard<std::mutex> lock(mtx);
    return getVoteSet(round, VoteTypePrecommit);
}

RoundVoteSet *HeightVoteSet::getRoundVoteSet(int round) {
    return roundVoteSets.count(round) ? &roundVoteSets.at(round) : nullptr;
}

VoteSet *HeightVoteSet::getVoteSet(int round, VoteType type) {
    RoundVoteSet *rvs = getRoundVoteSet(round);
    if (!rvs)
        return nullptr;
    switch (type) {
        case VoteTypePrevote:
            return &rvs->getPrevotes();
        case VoteTypePrecommit:
            return &rvs->getPrecommits();
        default:
            std::ostringstream outString;
            outString << "Unexpected vote type %s", Vote::voteTypeToString(type);
            throw PanicSanity(outString.str(), __FILE__, __LINE__);
    }
}

HeightVoteSet::HeightVoteSet(const string &chainID) : mtx(), chainID(chainID), valSet() {}

HeightVoteSet::HeightVoteSet(const string &chainID, height_t height, const ValidatorSet &valSet)
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
        VoteSet *rvs = getVoteSet(r, VoteTypePrevote);
        if (rvs) {
            if (rvs->hasTwoThirdMajority()) {
                blockID = rvs->twoThirdMajority().get();
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

bool HeightVoteSet::addVote(Vote &vote, HexBytes bytes) {
    return (vote.getType() == VoteTypeFirstCommit || bytes.empty());
    //TODO unimplemented
}

AddVoteResult HeightVoteSet::setPeerMaj23(int round, VoteType _type, P2PID peerID, BlockID blockID) {
    lock_guard<std::mutex> lock(mtx);
    if (!Vote::isVoteTypeValid(_type)) {
        std::string str("SetPeerMaj23: Invalid vote type: ");
        str += Vote::voteTypeToString(_type);
        Error(str, __FILE__, __LINE__);
        return AddVoteResult(false, true, str);
    }
    VoteSet *voteSet = getVoteSet(round, _type);
    if (!voteSet) {
        return AddVoteResult(false, false, "we dont know about this yet."); // something we don't know about yet
    }
    return voteSet->setPeerMaj23(P2PID(peerID), blockID);
}