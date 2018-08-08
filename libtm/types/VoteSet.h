//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_VOTESET_H
#define TM_LIGHT_VOTESET_H

#include <string>
#include <boost/optional.hpp>
#include "Vote.h"
#include "Error.h"
#include "HexBytes.h"
#include "Validator.h"

const unsigned int voteSetLength = 64;

/*
	Votes for a particular block
	There are two ways a *blockVotes gets created for a blockKey.
	1. first (non-conflicting) vote of a validator w/ blockKey (peerMaj23=false)
	2. A peer claims to have a 2/3 majority w/ blockKey (peerMaj23=true)
*/
class BlockVotes {
    bool peerMaj23;         // peer claims to have maj23
    vector<bool> bitArray;  // valIndex -> hasVote?
    vector<Vote> votes;     // valIndex -> *Vote
    int64_t sum;            // vote sum

public:
    BlockVotes(const vector<bool> &bitArray, const vector<Vote> &votes) : bitArray(bitArray), votes(votes) {}
};

/*
    VoteSet helps collect signatures from validators at each height+round for a
    predefined vote type.

    We need VoteSet to be able to keep track of conflicting votes when validators
    double-sign.  Yet, we can't keep track of *all* the votes seen, as that could
    be a DoS attack vector.

    There are two storage areas for votes.
    1. voteSet.votes
    2. voteSet.votesByBlock

    `.votes` is the "canonical" list of votes.  It always has at least one vote,
    if a vote from a validator had been seen at all.  Usually it keeps track of
    the first vote seen, but when a 2/3 majority is found, votes for that get
    priority and are copied over from `.votesByBlock`.

    `.votesByBlock` keeps track of a list of votes for a particular block.  There
    are two ways a &blockVotes{} gets created in `.votesByBlock`.
    1. the first vote seen by a validator was for the particular block.
    2. a peer claims to have seen 2/3 majority for the particular block.

    Since the first vote from a validator will always get added in `.votesByBlock`
    , all votes in `.votes` will have a corresponding entry in `.votesByBlock`.

    When a &blockVotes{} in `.votesByBlock` reaches a 2/3 majority quorum, its
    votes are copied into `.votes`.

    All this is memory bounded because conflicting votes only get added if a peer
    told us to track that block, each peer only gets to tell us 1 such block, and,
    there's only a limited number of peers.

    NOTE: Assumes that the sum total of voting power does not exceed MaxUInt64.
*/
class VoteSet {

    std::string chainID;
    int64_t height;
    int roundNumber;
    VoteType type;
    ValidatorSet valSet;

    //sync.Mutex mtx
    bool votesBitArray[voteSetLength];
    std::vector<Vote> votes;        // Primary votes to share
    int64_t sum;         // Sum of voting power for seen votes, discounting conflicts
    boost::optional<BlockID> maj23;        // First 2/3 majority seen
    unordered_map<std::string, BlockVotes> votesByBlock;  // string(blockHash|blockParts) -> blockVotes
//TODO    unordered_map<P2PID, BlockID> peerMaj23s; // Maj23 for each peer

private:

public:
    VoteSet(const string &chainID, int64_t blockHeight, int round, VoteType type, ValidatorSet valSet);

    boost::optional<Vote> getByAddress(HexBytes address) const;

    bool priv_addVote(Vote &v); // throw(PanicNullVoteSet);

    std::string toStringShort() const;

    bool addVerifiedVote(Vote, std::string blockKey, int64_t votingPower, boost::optional<Vote> conflicting);

    boost::optional<Vote> getVote(int valIndex, std::string blockKey) const;

    bool hasAllVotes() const;

    bool twoThirdMajority(BlockID &bytes) const;

    bool hasTwoThirdsAny() const;

    bool hasTwoThirdMajority() const;

    bool hasAll() const;

    bool addVote(Vote v); //throw(PanicNullVoteSet);

    const string &getChainID() const;

    int64_t getHeight() const;

    int getRoundNumber() const;

    VoteType getType() const;

    ValidatorSet getValSet() const;

    const vector<Vote> &getVotes() const;

    int64_t getSum() const;

    const boost::optional<BlockID> &getMaj23() const;

    const bytes getSignatures() const;
};


#endif //TM_LIGHT_VOTESET_H
