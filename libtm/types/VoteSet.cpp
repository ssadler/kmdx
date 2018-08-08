#include "VoteSet.h"

//
// Created by utnso on 11/07/18.
//



boost::optional<Vote> VoteSet::getByAddress(HexBytes address) const {
    boost::optional<Vote> output;
    for (const Vote &v : votes) {
        if (v.getValidatorAddress() == address) {
            output = v;
            return output;
        }
    }
    return output;
}

bool VoteSet::priv_addVote(Vote &vote) {
    int valIndex = vote.getValidatorIndex();
    Address valAddr = vote.getValidatorAddress();
    std::string blockKey = vote.getBlockID().key();

// Ensure that validator index was set
    if (valIndex < 0) {
        throw new ErrAddingVote("Index < 0");
    } else if (valAddr.size() == 0) {
        throw new ErrAddingVote("Empty address");
    }

// Make sure the step matches.
    if (vote.getHeight() != this->height || vote.getRoundNumber() != this->roundNumber ||
        vote.getType() != this->type) {
        std::ostringstream out;
        out << "ErrVoteUnexpectedStep, Got %d/%d/%d, expected %d/%d/%d" <<
            this->height << this->roundNumber << this->type << vote.getHeight() << vote.getRoundNumber()
            << Vote::voteTypeToString(vote.getType());
        throw ErrAddingVote(out.str());
    }

// Ensure that signer is a validator.
    boost::optional<Validator> val = valSet.getByIndex(valIndex);
    if (!val.is_initialized()) {
        std::ostringstream out;
        out << "Cannot find validator %d in valSet of size %d" << valIndex << this->valSet.size();
        throw ErrAddingVote(out.str());
    }

// Ensure that the signer has the right address
    if (valAddr != val.get().getAddress()) {
        std::ostringstream out;
        out
                << "vote.ValidatorAddress (%s) does not match address (%s) for vote.ValidatorIndex (%d)\n Ensure the genesis file is correct across all validators."
                << valAddr.toString() << val.get().getAddress().toString() << valIndex;
        throw ErrAddingVote(out.str());
    }

// If we already know of this vote, return false.
    boost::optional<Vote> existing = this->getVote(valIndex, blockKey);
    if (existing.is_initialized()) {
        if (existing.get().getSignature() == vote.getSignature()) {
            return false; // duplicate
        }
        std::ostringstream out;
        out
                << "ErrVoteNonDeterministicSignature, Existing vote: %s; New vote: %s", existing.get().toString(), vote.toString();
        throw ErrAddingVote(out.str());
    }

// Check signature.
    try {
        vote.verify(this->chainID, val.get().getPubKey());
    } catch (Error &e) {
        std::ostringstream out;
        out << "Failed to verify vote with ChainID %s and PubKey %s" << this->chainID <<
            val.get().getPubKey().toString();
        throw ErrAddingVote(out.str());
    }

// Add vote and get conflicting vote if any
    boost::optional<Vote> conflicting;
    bool added = this->addVerifiedVote(vote, blockKey, val.get().getVotingPower(), conflicting);
    if (conflicting.is_initialized()) {
        std::ostringstream out;
        out << "Conflicting vote exists for validator %s, conflicting: %s, new: %s" << val.get().toString()
            << conflicting.get().toString()
            << vote.toString();
        throw ErrAddingVote(out.str());
    }
    if (!added) {
        throw Panic("Error Adding vote: Expected to add non-conflicting vote");
    }
    return added;
}

// Assumes signature is valid.
// If conflicting vote exists, returns it.
bool VoteSet::addVerifiedVote(Vote vote, std::string blockKey, int64_t votingPower, boost::optional<Vote> conflicting) {
    int valIndex = vote.getValidatorIndex();

    // Already exists in voteSet.votes?
    boost::optional<Vote> existing = this->votes[valIndex];
    if (existing.is_initialized()) {
        if (existing.get().getBlockID() == vote.getBlockID()) {
            throw new PanicSanity("addVerifiedVote does not expect duplicate votes");
        } else {
            conflicting = existing.get();
        }
        // Replace vote if blockKey matches voteSet.maj23.
        if (this->maj23.is_initialized() && this->maj23.get().key() == blockKey) {
            this->votes[valIndex] = vote;
            this->votesBitArray[valIndex] = true;
        }
        // Otherwise don't add it to voteSet.votes
    } else {
        // Add to voteSet.votes and incr .sum
        this->votes[valIndex] = vote;
        this->votesBitArray[valIndex] = true;
        this->sum += votingPower;
    }
//TODO pick up here
    /*BlockVotes votesByBlock = this->votesByBlock[blockKey];
        if ok {
                    if conflicting != nil && !votesByBlock.peerMaj23 {
                        // There's a conflict and no peer claims that this block is special.
                        return false, conflicting
                    }
                    // We'll add the vote in a bit.
            } else {
            // .votesByBlock doesn't exist...
            if conflicting != nil {
                        // ... and there's a conflicting vote.
                        // We're not even tracking this blockKey, so just forget it.
                        return false, conflicting
                }
            // ... and there's no conflicting vote.
            // Start tracking this blockKey
            votesByBlock = newBlockVotes(false, voteSet.valSet.Size())
            voteSet.votesByBlock[blockKey] = votesByBlock
            // We'll add the vote in a bit.
        }

        // Before adding to votesByBlock, see if we'll exceed quorum
        origSum := votesByBlock.sum
        quorum := voteSet.valSet.TotalVotingPower()*2/3 + 1

        // Add vote to votesByBlock
        votesByBlock.addVerifiedVote(vote, votingPower)

        // If we just crossed the quorum threshold and have 2/3 majority...
        if origSum < quorum && quorum <= votesByBlock.sum {
            // Only consider the first quorum reached
            if voteSet.maj23 == nil {
                        maj23BlockID := vote.BlockID
                        voteSet.maj23 = &maj23BlockID
                        // And also copy votes over to voteSet.votes
                        for i, vote := range votesByBlock.votes {
                            if vote != nil {
                                        voteSet.votes[i] = vote
                                }
                        }
                }
        }

        return true, conflicting*/
    return true;
}