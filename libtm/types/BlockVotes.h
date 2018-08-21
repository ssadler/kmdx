//
// Created by thpn on 17/08/18.
//

#ifndef AYETH_BLOCKVOTES_H
#define AYETH_BLOCKVOTES_H

#include "Vote.h"
#include <boost/optional.hpp>

/*
	Votes for a particular block
	There are two ways a *blockVotes gets created for a blockKey.
	1. first (non-conflicting) vote of a validator w/ blockKey (peerMaj23=false)
	2. A peer claims to have a 2/3 majority w/ blockKey (peerMaj23=true)
*/
class BlockVotes {
    bool peerMaj23;         // peer claims to have maj23
    int64_t sum;            // vote sum
    std::map<int, Vote> votes;     // valIndex -> *Vote

public:
    BlockVotes(bool _peerMaj23, std::map<int, Vote> _votes);;

    BlockVotes(bool _peerMaj23);;


    bool isPeerMaj23() const;

    void setPeerMaj23(bool peerMaj23);

    const std::map<int, Vote> &getVotes() const;

    int64_t getSum() const;

    void setSum(int64_t sum);

    const boost::optional<Vote> get(int n) const;

    void addVerifiedVote(Vote v, int64_t votingPower);
};


#endif //AYETH_BLOCKVOTES_H
