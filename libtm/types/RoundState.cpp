//
// Created by utnso on 11/07/18.
//

#include "RoundState.h"

bool RoundState::operator==(const RoundState &one, const RoundState &other) {
    bool result;
    return (
            //TODO
            one.height == other.height &&
            one.roundNumber == other.roundNumber &&
            one.stepType == other.stepType /*&&
                one.startTime == other.startTime &&
                one.proposalBlock.Hash() == other.proposalBlock.Hash() &&
                one.lockedBlockHash.Hash() == other.lockedBlockHash.Hash() &&
                one.validBlockHash.Hash() == other.validBlockHash.Hash() &&
                one.votes == other.votes*/
    );
}

int64_t RoundState::getHeight() const {
    return height;
}

int RoundState::getRoundNumber() const {
    return roundNumber;
}

RoundStepType RoundState::getStepType() const {
    return stepType;
}

time_t RoundState::getStartTime() const {
    return startTime;
}

time_t RoundState::getCommitTime() const {
    return commitTime;
}

const vector<Validator> &RoundState::getValidators() const {
    return validators;
}

Proposal *RoundState::getProposal() const {
    return proposal;
}

Block *RoundState::getProposalBlock() const {
    return proposalBlock;
}

int RoundState::getLockedRoundNumber() const {
    return lockedRoundNumber;
}

Block *RoundState::getLockedBlock() const {
    return lockedBlock;
}

int RoundState::getValidRoundNumber() const {
    return validRoundNumber;
}

Block *RoundState::getValidBlock() const {
    return validBlock;
}

HeightVoteSet *RoundState::getVotes() const {
    return votes;
}

int RoundState::getCommitRoundNumber() const {
    return commitRoundNumber;
}

VoteSet *RoundState::getLastCommit() const {
    return lastCommit;
}

ValidatorSet *RoundState::getLastValidators() const {
    return lastValidators;
}

void RoundState::setLockedRoundNumber(int lockedRoundNumber) {
    RoundState::lockedRoundNumber = lockedRoundNumber;
}

void RoundState::setLockedBlock(Block *lockedBlock) {
    RoundState::lockedBlock = lockedBlock;
}

void RoundState::setValidBlock(Block *validBlock const) {
    //TODO
}

void RoundState::setRoundNumber(const int _roundNumber) {
    roundNumber = _roundNumber;
}

void RoundState::setStepType(RoundStepType type) {
    stepType = type;
}
