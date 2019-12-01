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

RoundState::RoundState(std::string chainID) : votes(chainID) {//FIXME
};

RoundState::RoundState(std::string chainID, height_t height, ValidatorSet validators) : votes(chainID, height,
                                                                                              validators) {//FIXME
};

void RoundState::updateRoundStep(int number, RoundStepType type) {
    roundNumber = number;
    stepType = type;
}

height_t RoundState::getHeight() const {
    return height;
}


void RoundState::setLockedRoundNumber(int lockedRoundNumber) {
    RoundState::lockedRoundNumber = lockedRoundNumber;
}

void RoundState::setHeight(height_t _height) {
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

const std::string RoundState::stateTypeString(RoundStepType stype) {
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

const EventDataRoundState RoundState::roundStateEvent() {
    return EventDataRoundState(height, roundNumber, stateTypeString(stepType));
}

void RoundState::setValidators(const ValidatorSet &validators) {
    RoundState::validators = validators;
}