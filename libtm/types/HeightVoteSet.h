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
    height_t height;
    ValidatorSet valSet;
    int roundNumber;                   // max tracked round
    std::map<int, RoundVoteSet> roundVoteSets;
    std::multimap<P2PID, int> peerCatchupRounds; // keys: peer.ID; values: at most 2 rounds

public :
    HeightVoteSet(const HeightVoteSet &);

    HeightVoteSet(const string &chainID);

    HeightVoteSet(const string &chainID, height_t height, const ValidatorSet &valSet);

    VoteSet *getPrevotes(int round);

    VoteSet *getPrecommits(int round);

    RoundVoteSet *getRoundVoteSet(int round);

    VoteSet *getVoteSet(int round, VoteType type);

    AddVoteResult setPeerMaj23(int round, VoteType _type, P2PID peerID, BlockID blockID);

    /*returns int = polRound & sets blockID */
    int polInfo(BlockID &blockID);

    AddVoteResult addVote(Vote &, P2PID);

    void addRound(int round);

    void setRoundNumber(int roundNumber);

    bool isEmpty();


};


#endif //TM_LIGHT_HEIGHTVOTESET_H
