//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_HEIGHTVOTESET_H
#define TM_LIGHT_HEIGHTVOTESET_H

#include <boost/optional.hpp>
#include <memory>
#include <mutex>
#include "VoteSet.h"

class RoundVoteSet {
    VoteSet &prevotes;
    VoteSet &precommits;
public:
    RoundVoteSet(VoteSet &prevotes, VoteSet &precommits);

    VoteSet &getPrevotes() const;

    VoteSet &getPrecommits() const;
};

class HeightVoteSet {
private:
    std::mutex mtx;
    std::string chainID;
    int64_t height;
    ValidatorSet valSet;
    int roundNumber;                   // max tracked round
    std::map<int, RoundVoteSet> roundVoteSets;
    std::map<P2PID, int> peerCatchupRounds; // keys: peer.ID; values: at most 2 rounds

public :
    HeightVoteSet(const HeightVoteSet &);

    HeightVoteSet(const string &chainID);

    HeightVoteSet(const string &chainID, int64_t height, const ValidatorSet &valSet);

    boost::optional<VoteSet> getPrevotes(int round);

    boost::optional<VoteSet> getPrecommits(int round);

    boost::optional<VoteSet> getVoteSet(int round, VoteType type);

    /*returns int = polRound & sets blockID */
    int polInfo(BlockID &blockID);

    bool addVote(Vote vote, HexBytes bytes);

    void setRoundNumber(int roundNumber);

    bool isEmpty();


};


#endif //TM_LIGHT_HEIGHTVOTESET_H
