//
// Created by utnso on 11/07/18.
//

#include "RoundState.h"

bool RoundState::operator==(const RoundState &other) {

    return (
            //TODO
            height == other.height &&
            roundNumber == other.roundNumber &&
            stepType == other.stepType /*&&
                one.startTime == other.startTime &&
                one.proposalBlock.Hash() == other.proposalBlock.Hash() &&
                one.lockedBlockHash.Hash() == other.lockedBlockHash.Hash() &&
                one.validBlockHash.Hash() == other.validBlockHash.Hash() &&
                one.votes == other.votes*/
    );
}

void RoundState::updateRoundStep(int number, RoundStepType type) {
    roundNumber = number;
    stepType = type;
}

int64_t RoundState::getHeight() const {
    return height;
}


void RoundState::setLockedRoundNumber(int lockedRoundNumber) {
    RoundState::lockedRoundNumber = lockedRoundNumber;
}

void RoundState::setHeight(int64_t _height) {
    height = _height;
}

void RoundState::setLockedBlock(const shared_ptr<Block> &lockedBlock) {
    RoundState::lockedBlock = lockedBlock;
}

void RoundState::setValidBlock(const shared_ptr<Block> &validBlock) {
    RoundState::validBlock = validBlock;
}

void RoundState::setProposalBlock(const shared_ptr<Block> &proposalBlock) {
    RoundState::proposalBlock = proposalBlock;
}

std::string RoundState::stateTypeString(RoundStepType stype) {
    switch (stype) {
        case RoundStepNewHeight:
            return "RoundStepNewHeight";
        case RoundStepNewRound:
            return "RoundStepNewRound";
        case RoundStepPropose:
            return "RoundStepPropose";
        case RoundStepPrevote:
            return "RoundStepPrevote";
        case RoundStepPrevoteWait:
            return "RoundStepPrevoteWait";
        case RoundStepPrecommit:
            return "RoundStepPrecommit";
        case RoundStepPrecommitWait:
            return "RoundStepPrecommitWait";
        case RoundStepCommit:
            return "RoundStepCommit";
        default:
            return "unknown step";
    }
}