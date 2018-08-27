//
// Created by thpn on 17/08/18.
//

#include "BlockVotes.h"

bool BlockVotes::isPeerMaj23() const {
    return peerMaj23;
}

void BlockVotes::setPeerMaj23(bool peerMaj23) {
    BlockVotes::peerMaj23 = peerMaj23;
}

const std::map<int, Vote> &BlockVotes::getVotes() const {
    return votes;
}

int64_t BlockVotes::getSum() const {
    return sum;
}

void BlockVotes::setSum(int64_t sum) {
    BlockVotes::sum = sum;
}

const boost::optional<Vote> BlockVotes::get(int n) const {
    return votes.count(n) ? votes.at(n) : boost::optional<Vote>();
}

void BlockVotes::addVerifiedVote(Vote vote, int64_t votingPower) {
    int valIndex = vote.getValidatorIndex();
    boost::optional<Vote> existing = get(valIndex);
    if (!existing.is_initialized()) {
        votes.emplace(std::pair<int, Vote>(valIndex, vote));
        sum += votingPower;
    }
}

boost::optional<Vote> BlockVotes::get(int i) {
    return votes.count(i) ? votes.at(i) : boost::optional<Vote>();
}

bool BlockVotes::getPeerMaj23() {
    return peerMaj23;
}
