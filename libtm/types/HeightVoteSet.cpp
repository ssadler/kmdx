//
// Created by utnso on 11/07/18.
//
#include "../helpers/Logger.h"
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
void HeightVoteSet::addRound(int round) {
    if (roundVoteSets.find(round)!=roundVoteSets.end())
        throw PanicSanity("addRound() for an existing round", __FILE__, __LINE__);

    VoteSet pv(chainID, height, roundNumber, VoteTypePrevote, valSet);
    VoteSet pc(chainID, height, roundNumber, VoteTypePrecommit, valSet);
    RoundVoteSet rvs(pv, pc);
    roundVoteSets.emplace(std::make_pair(round, rvs));
    clog(dev::VerbosityInfo, Logger::channelTm)<<"addRound(round)" << " round:" << round;
}

void HeightVoteSet::setRoundNumber(int _roundNumber) {
    this->roundNumber = _roundNumber;
}

bool HeightVoteSet::isEmpty() {
    return roundVoteSets.empty();//TODO review
}

AddVoteResult HeightVoteSet::addVote(Vote &vote, P2PID peerID) {
    lock_guard<std::mutex> lock(mtx);
    if (!Vote::isVoteTypeValid(vote.getType())) {
        return AddVoteResult(false, false, "Vote type invalid");
    }
    VoteSet* voteSet = getVoteSet(vote.getRoundNumber(), vote.getType());
    if (!voteSet) {
        if (peerCatchupRounds.count(peerID) < 2) {
                addRound(vote.getRoundNumber());
                voteSet = getVoteSet(vote.getRoundNumber(), vote.getType());
//                peerCatchupRounds[peerID] = append(rndz, vote.getRoundNumber());
                peerCatchupRounds.emplace(peerID, vote.getRoundNumber());
            } else {
                // punish peer
                throw ErrorGotVoteFromUnwantedRound(__FILE__, __LINE__);
            }
        }
        boost::optional<Vote> conflicting;
    return voteSet->addVote(vote, conflicting);
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