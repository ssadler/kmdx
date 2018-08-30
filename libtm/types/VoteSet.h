//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_VOTESET_H
#define TM_LIGHT_VOTESET_H

#include <string>
#include <mutex>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include "Vote.h"
#include "BlockVotes.h"
#include "Error.h"
#include "HexBytes.h"
#include "ValidatorSet.h"
#include "../helpers/Finally.h"
#include "AddVoteResult.h"

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
    height_t height;
    int roundNumber;
    VoteType type;
    ValidatorSet valSet;

    std::mutex mtx;
    map<int, Vote> votes;        // Primary votes to share
    int64_t sum;         // Sum of voting power for seen votes, discounting conflicts
    boost::optional<BlockID> maj23;        // First 2/3 majority seen
    map<P2PID, BlockID> peerMaj23s;     // Maj23 for each peer
    map<std::string, BlockVotes> votesByBlock;  // string(blockHash|blockParts) -> blockVotes

    friend class common_test;

private:
    AddVoteResult
    priv_addVote(Vote &vote, boost::optional<Vote> &conflicting); // throw(PanicNullVoteSet, __FILE__, __LINE__);

public:
    VoteSet(const string &chainID, height_t blockHeight, int round, VoteType type, ValidatorSet valSet);

    VoteSet(const VoteSet &);

    VoteSet &operator=(const VoteSet &);

    VoteSet();

    const boost::optional<Vote> getByAddress(AddressTm addresstm) const;

    const boost::optional<Vote> get(int index) const;

    boost::optional<Vote> getVote(int valIndex, std::string blockKey);

    BlockVotes *getBlockVotes(std::string blockKey);

    std::string toStringShort() const;

    AddVoteResult addVerifiedVote(Vote vote, std::string blockKey, int64_t votingPower,
                                  boost::optional<Vote> &conflicting);

    bool hasTwoThirdMajority();

    boost::optional<BlockID> twoThirdMajority();

    bool hasTwoThirdsAny();

    bool hasAll();

    AddVoteResult
    addVote(Vote &vote, boost::optional<Vote> &conflicting); //throw(PanicNullVoteSet, __FILE__, __LINE__);

    height_t getHeight() const;

    const std::vector<SignatureTm> getSignatures() const;

    const map<int, Vote> getVotes();

    void setMaj23(const BlockID &_maj23);

    AddVoteResult setPeerMaj23(const P2PID, BlockID);

    BlockID *getPeerMaj23s(P2PID bytes);

    Commit makeCommit();
};


#endif //TM_LIGHT_VOTESET_H
