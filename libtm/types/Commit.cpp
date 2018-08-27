//
// Created by utnso on 11/07/18.
//
#include "Commit.h"


/*// FirstPrecommit returns the first non-nullptr precommit in the commit.;
// If all precommits are nullptr, it returns an empty precommit with height 0.;
shared_ptr<Vote> Commit::getFirstPrecommit() {//TODO return value or reference
    if (precommits.empty()) {
        return nullptr;
    }
    if (firstPrecommit != nullptr) {
        return firstPrecommit;
    }

    for (uint i = 0; i < precommits.size(); ++i) {
        if (precommits[i] != nullptr) {
            firstPrecommit = precommits[i];
            return firstPrecommit;
        }
    }

    return shared_ptr<Vote>(new Vote(VoteTypeFirstCommit));

}*/

// Height returns the height of the commit;
height_t Commit::height() {
    if (precommits.empty()) {
        return 0;
    }
    return firstPrecommit->getHeight();
}

/** Round returns the round of the commit */
int Commit::round() {
    if (precommits.empty()) {
        return 0;
    }
    return firstPrecommit->getRoundNumber();
}


// Type returns the vote type of the commit, which is always VoteTypePrecommit;
VoteType Commit::type() {
    return VoteTypePrecommit;
}

// Size returns the number of votes in the commit;
ulong Commit::size() {
    //TODO how to fix this?!
    /*if (commit == nullptr) {
        return 0;
    }*/
    return precommits.size();
}

//TOdo bitarray

//TOdo GetByIndex returns the vote corresponding to a given validator index;

// IsCommit returns true if there is at least one vote;
bool Commit::isCommit() {
    return !precommits.empty();
}

// ValidateBasic performs basic validation that doesn't involve state data.
void Commit::validateBasic() { //throw(ErrInvalidVoteSet) {
    if (blockID.getBytes().empty()) {
        throw ErrInvalidVoteSet("Commit cannot be for nullptr block", __FILE__, __LINE__);
    }
    if (precommits.empty()) {
        throw ErrInvalidVoteSet("No precommits in commit", __FILE__, __LINE__);
    }
    height_t height = this->height();
    int round = this->round();

// validate the precommits
    //for (uint i = 0; i < precommits.size(); ++i) {

    for (auto const &iterator : precommits) {
        const Vote &precommit = iterator.second;
// Ensure that all votes are precommits
        if (precommit.getType() != VoteTypePrecommit) {
            throw ErrInvalidVoteSet(
                    "Invalid commit vote. Expected precommit, got " + Vote::voteTypeToString(precommit.getType()),
                    __FILE__, __LINE__);
        }
// Ensure that all heights are the same
        if (precommit.getHeight() != height) {
            throw ErrInvalidVoteSet(
                    "Invalid commit precommit height. Expected " + to_string(height) + ", got " +
                    to_string(precommit.getHeight()), __FILE__, __LINE__);
        }
// Ensure that all rounds are the same
        if (precommit.getRoundNumber() != round) {
            throw ErrInvalidVoteSet("Invalid commit precommit round. Expected " + to_string(round) + ", got " +
                                    to_string(precommit.getRoundNumber()), __FILE__, __LINE__);
        }


    }
}

std::map<int, Vote> Commit::getPrecommits() const {
    return precommits;
}

// Hash returns the hash of the commit;
HexBytes Commit::getHash() {
    return HexBytes(vector<uint8_t>());    //TODO
}

const boost::optional<Vote> Commit::getFirstPrecommit() {
    boost::optional<Vote> output;
    if (precommits.empty() == 0) {
        return output;
    }
    if (firstPrecommit.is_initialized()) {
        return firstPrecommit;
    }
    for (auto const &iterator : precommits) {
        const Vote &precommit = iterator.second;
        firstPrecommit = precommit;
        return firstPrecommit;
    }


    output = Vote(VoteTypePrecommit);
    return output;
}

Commit::Commit() {}

Commit::Commit(const BlockID &blockID, const map<int, Vote> &precommits) : blockID(blockID), precommits(precommits) {}

