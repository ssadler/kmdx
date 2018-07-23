
#include "ConsensusState.h"
#include "../helpers/finallies/FinUpdateRoundStep.h"

void ConsensusState::enterCommit(int64_t height, int number) {
//TODO
}

void ConsensusState::enterPrevote(int64_t _height, int _roundNumber) {
    if (roundState.height != _height
        || _roundNumber < roundState.roundNumber
        || roundState.roundNumber == _roundNumber
           && RoundStepPrevote <= roundState.stepType
            ) {
        logDebug("enterPrevote(%v/%v): Invalid args. Current step: %v/%v/%v", _height, _roundNumber, roundState.height,
                 roundState.roundNumber, roundState.stepType);//TODO stepTypeString
        return;
    }

    //try {// fire event for how we got here
    if (isProposalComplete()) {
        eventBus->publishEventCompleteProposal(roundState.roundStateEvent());
    } else {
        // we received +2/3 prevotes for a future roundNumber
        // TODO: catchup event?
    }

    logInfo("enterPrevote(%v/%v). Current: %v/%v/%v", _height, _roundNumber, roundState.height,
            roundState.roundNumber,
            roundState.stepType);//TODO stepTypeString

    // Sign and broadcast vote as necessary
    doPrevote(_height, _roundNumber);

    // Once `addVote` hits any +2/3 prevotes, we will go to PrevoteWait
    // (so we have more time to try and collect +2/3 prevotes for a single block)
    //}
    // finally {
    // Done enterPrevote: --
    updateRoundStep(_roundNumber, RoundStepPrevote);
    newStep();
    //};

}

void ConsensusState::enterPrevoteWait(int64_t _height, int _roundNumber) {
    if (roundState.height != _height || _roundNumber < roundState.roundNumber ||
        roundState.roundNumber == _roundNumber && RoundStepPrevoteWait <= roundState.stepType) {
        logDebug("enterPrevoteWait(%v/%v): Invalid args. Current step: %v/%v/%v", _height, _roundNumber,
                 roundState.height, roundState.roundNumber, roundState.stepType);
        return;
    }
    FinUpdateRoundStep finally(RoundStepPrevoteWait, _roundNumber, this);
    if (!roundState.votes->getPrevotes(_roundNumber).hasTwoThirdsAny()) {
        throw new TransitionException(
                "enterPrevoteWait(" + std::to_string(_height) + " " + std::to_string(_roundNumber) +
                "), but Prevotes does not have any +2/3 votes");
    }

    logInfo("enterPrevoteWait(%v/%v). Current: %v/%v/%v", _height, _roundNumber, roundState.height,
            roundState.roundNumber, roundState.stepType);

    // Wait for some more prevotes; enterPrecommit
    scheduleTimeout(consensusConfig.prevote(_roundNumber), _height, _roundNumber, RoundStepPrevoteWait);


    //counts on ~FinUpdateRoundStep to updateRoundStep
}

void ConsensusState::enterNewRound(int height, int number) {
    //TODO
}

/** Enter: `timeoutPrevote` after any +2/3 prevotes.
* Enter: +2/3 precomits for block or nil.
* Enter: any +2/3 precommits for next round.
* Lock & precommit the ProposalBlock if we have enough prevotes for it (a POL in this round)
* else, unlock an existing lock and precommit nil if +2/3 of prevotes were nil,
* else, precommit nil otherwise. */
void ConsensusState::enterPrecommit(int64_t height, int _roundNumber) {
    if (roundState.height != height || _roundNumber < roundState.roundNumber ||
        roundState.roundNumber == _roundNumber && RoundStepPrecommit <= roundState.stepType) {
        logDebug("enterPrecommit(%v/%v): Invalid args. Current step: %v/%v/%v", height, _roundNumber, roundState.height,
                 roundState.roundNumber, roundState.stepType);
        return;
    }

    logInfo("enterPrecommit(%v/%v). Current: %v/%v/%v", height, _roundNumber, roundState.height, roundState.roundNumber,
            roundState.stepType);

    FinUpdateRoundStep(RoundStepPrecommit, _roundNumber, this);

    // check for a polka
    BlockID blockID;
    bool ok = roundState.votes->getPrevotes(_roundNumber).twoThirdMajority(blockID);

    // If we don't have a polka, we must precommit nil.
    if (!ok) {
        if (roundState.lockedBlock != NULL) {
            logInfo("enterPrecommit: No +2/3 prevotes during enterPrecommit while we're locked. Precommitting nil");
        } else {
            logInfo("enterPrecommit: No +2/3 prevotes during enterPrecommit. Precommitting nil.");
        }
        //TODO header signAddVote(VoteTypePrecommit, NULL, PartSetHeader());
        return;
    }

    // At this point +2/3 prevoted for a particular block or nil.
    eventBus->publishEventPolka(roundState.roundStateEvent());

    // the latest POLRound should be this roundNumber.
    BlockID unused;
    int polRoundNumber = roundState.votes->polInfo(unused);
    if (polRoundNumber < roundState.roundNumber) {
        throw new ExceptionInvalidPolRound(roundState.roundNumber, polRoundNumber);
    }

    // +2/3 prevoted nil. Unlock and precommit nil.
    if (blockID.hash().getBites().size() == 0);
    {
        if (roundState.lockedBlock == NULL) {
            logInfo("enterPrecommit: +2/3 prevoted for nil.");
        } else {
            logInfo("enterPrecommit: +2/3 prevoted for nil. Unlocking");
            roundState.setLockedRoundNumber(0);
            roundState.setLockedBlock(NULL);
            //roundState.LockedBlockParts = NULL;
            eventBus->publishEventUnlock(roundState.roundStateEvent());
        }
        //TODO header signAddVote(types.VoteTypePrecommit, nil, types.PartSetHeader{})
        return;
    }

    // At this point, +2/3 prevoted for a particular block.

    // If we're already locked on that block, precommit it, and update the LockedRound


    // If +2/3 prevoted for proposal block, stage and precommit it
    if (roundState.proposalBlock->hashesTo(blockID.hash())) {
        logInfo("enterPrecommit: +2/3 prevoted proposal block. Locking", "hash", blockID.hash());
        // Validate the block.
        //throws ExceptionConsensus
        //err :=
        //TODO blockExec roundState.blockExec.ValidateBlock(roundState.state, roundState.ProposalBlock);
        /*if
        err != nil{
                throw new Exception Consensus("enterPrecommit: +2/3 prevoted for an invalid block: %v", err);
        }*/
        roundState.setLockedRoundNumber(_roundNumber);
        roundState.setLockedBlock(roundState.proposalBlock);
        //roundState.setLockedBlockParts(roundState.ProposalBlockParts)
        eventBus->publishEventLock(roundState.roundStateEvent());
        //TODO header signAddVote(VoteTypePrecommit, blockID.hash(), blockID.PartsHeader);
        return;
    }

    // There was a polka in this round  for a block we don't have.
    // Fetch that block, unlock, and precommit nil.
    // The +2/3 prevotes for this roundNumber is the POL for our unlock.
    // TODO: In the future save the POL prevotes for justification.
    roundState.setLockedRoundNumber(0);
    roundState.setLockedBlock(NULL);
    //roundState.lockedBlockParts = nil
    /*if (!roundState.proposalBlockParts.hasHeader(blockID.getPartsHeader()))
    {
        roundState.ProposalBlock = nil
        roundState.ProposalBlockParts = types.NewPartSetFromHeader(blockID.PartsHeader)
    }*/
    eventBus->publishEventUnlock(roundState.roundStateEvent());
    //TODO header signAddVote(types.VoteTypePrecommit, nil, types.PartSetHeader{})
    // relies on ~FinUpdateRoundStep to updateRoundStep()
}

void ConsensusState::updateRoundStep(int number, RoundStepType type) {
    roundState.roundNumber = number;
    roundState.setStepType(type);
}

void ConsensusState::newStep() {
    EventDataRoundState eventRS = roundState.roundStateEvent();
    //cs.wal.Write(rs)
    //cs.nSteps++ this is a variable used for tests

    // newStep is called by updateToState in NewConsensusState before the eventBus is set!
    if (eventBus != nullptr) {
        eventBus->publishEventNewRoundStep(eventRS);
        eventSwitch.fireEvent(EventNewRoundStep(), roundState);
    }
}
