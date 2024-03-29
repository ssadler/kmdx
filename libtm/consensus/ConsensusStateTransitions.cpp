
#include "ConsensusState.h"

#include "../helpers/Finally.h"

void ConsensusState::enterCommit(int64_t height, int commitRound) {
//logger = logWith("height", height, "commitRound", commitRound);

    if (roundState.height != height || RoundStepCommit <= roundState.stepType) {
        logDebug("enterCommit(%v/%v): Invalid args. Current step: %v/%v/%v", height, commitRound, roundState.height,
                 roundState.roundNumber, roundState.stepType);
        return;
    }
    logInfo("enterCommit(%v/%v). Current: %v/%v/%v", height, commitRound, roundState.height, roundState.roundNumber,
            roundState.stepType);

    // Done enterCommit:
    // keep roundState.roundNumber the same, commitRound points to the right Precommits set.
    auto guard = finally([&]() {
        roundState.updateRoundStep(roundState.roundNumber, RoundStepCommit);
        roundState.commitRoundNumber = commitRound;
        roundState.commitTime = Time();
        newStep();

        // Maybe finalize immediately.
        tryFinalizeCommit(height);
    });
    BlockID blockID;

    if (!roundState.votes.getPrecommits(commitRound).get()->twoThirdMajority(blockID)) {
        throw PanicEnterRound("RunActionCommit() expects +2/3 precommits");
    }

    // The Locked* fields no longer matter.
    // Move them over to ProposalBlock if they match the commit hash ,
    // otherwise they'll be cleared in updateToState.
    /*if (LockedBlock.HashesTo(blockID.Hash) ) {
        logInfo("Commit is for locked block. Set ProposalBlock=LockedBlock", "blockHash", blockID.Hash);
        ProposalBlock = LockedBlock
        ProposalBlockParts = LockedBlockParts
    }*/
/* //TODO get block if we dont have it
    // If we don't have the block being committed, set up to get it .
    if (!roundState.proposalBlock->hashesTo(blockID.getHash()) ) {
        if (!ProposalBlockParts.HasHeader(blockID.PartsHeader) ) {
            logInfo("Commit is for a block we don't know about. Set ProposalBlock=nil", "proposal", ProposalBlock.Hash(), "commit", blockID.Hash);
            // We're getting the wrong block.
            // Set up ProposalBlockParts and keep waiting.
            ProposalBlock = nil
            ProposalBlockParts = types.NewPartSetFromHeader(blockID.PartsHeader)
        } else {
            // We just need to keep waiting.
        }
    }*/
}

void ConsensusState::enterPrevote(int64_t _height, int _roundNumber) {
    if (roundState.height != _height
        || _roundNumber < roundState.roundNumber
        || (roundState.roundNumber == _roundNumber
            && RoundStepPrevote <= roundState.stepType)
            ) {
        logDebug("enterPrevote(%v/%v): Invalid args. Current step: %v/%v/%v", _height, _roundNumber, roundState.height,
                 roundState.roundNumber, roundState.stepType);//TODO stepTypeString
        return;
    }
    auto guard = finally([&]() {
        roundState.updateRoundStep(_roundNumber, RoundStepPrevote);
        newStep();
    });
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
}

void ConsensusState::enterPrevoteWait(int64_t _height, int _roundNumber) {
    if (roundState.height != _height || _roundNumber < roundState.roundNumber ||
        (roundState.roundNumber == _roundNumber && RoundStepPrevoteWait <= roundState.stepType)) {
        logDebug("enterPrevoteWait(%v/%v): Invalid args. Current step: %v/%v/%v", _height, _roundNumber,
                 roundState.height, roundState.roundNumber, roundState.stepType);
        return;
    }
    auto guard = finally([&]() {
        roundState.updateRoundStep(_roundNumber, RoundStepPrevoteWait);
        newStep();
    });

    if (!roundState.votes.getPrevotes(_roundNumber).hasTwoThirdsAny()) {
        throw TransitionException(
                "enterPrevoteWait(" + std::to_string(_height) + " " + std::to_string(_roundNumber) +
                "), but Prevotes does not have any +2/3 votes");
    }

    logInfo("enterPrevoteWait(%v/%v). Current: %v/%v/%v", _height, _roundNumber, roundState.height,
            roundState.roundNumber, roundState.stepType);

    // Wait for some more prevotes; enterPrecommit
    scheduleTimeout(consensusConfig.prevote(_roundNumber), _height, _roundNumber, RoundStepPrevoteWait);
}

void ConsensusState::enterNewRound(int64_t height, int round) {
    //logger := cs.logWith("height", height, "round", round);

    if (roundState.height != height || round < roundState.roundNumber ||
        (roundState.roundNumber == round && roundState.stepType != RoundStepNewHeight)) {
        logDebug("enterNewRound(%v/%v): Invalid args. Current step: %v/%v/%v", height, round, roundState.height,
                 roundState.roundNumber, roundState.stepType);
        return;
    }

    if (roundState.startTime.after(Time())) {
        logInfo("Need to set a buffer and log message here for sanity.", "startTime", roundState.startTime,
                "now"); //TODO now to string
    }

    logInfo("enterNewRound(%v/%v). Current: %v/%v/%v", height, round, roundState.height, roundState.roundNumber,
            roundState.stepType);

    // Increment validators if necessary
    ValidatorSet validators = roundState.validators;
    if (roundState.roundNumber < round) {
        validators = validators.copy();
        validators.incrementAccum(round - roundState.roundNumber);
    }

    // Setup new round
    // we don't fire newStep for this step,
    // but we fire an event, so update the round step first
    roundState.updateRoundStep(round, RoundStepNewRound);
    validators = roundState.validators;
    if (round == 0) {
        // We've already reset these upon new height,
        // and meanwhile we might have received a proposal
        // for round 0.
    } else {
        logInfo("Resetting Proposal info");
        clearProposal();
        //setProposalBlock(nullptr);
        //setProposalBlockParts(nullptr);
    }
    roundState.votes.setRoundNumber(round + 1); // also track next round (round+1) to allow round-skipping

    eventBus->publishEventNewRound(roundState.roundStateEvent());

    // Wait for txs to be available in the mempool
    // before we enterPropose in round 0. If (the last block changed the app has)h,
    // we may need an empty "proof" block, and enterPropose immediately.
    /*waitForTxs = (consensusConfig.waitForTxs() && round == 0 && !needProofBlock(height));
    if (waitForTxs) {
        if (cs.config.CreateEmptyBlocksInterval > 0) {
            cs.scheduleTimeout(cs.config.EmptyBlocksInterval(), height, round, cstypes.RoundStepNewRound)
        }
        go cs.proposalHeartbeat(height, round)
    } else {
        cs.enterPropose(height, round)
    }*/
}

/** Enter: `timeoutPrevote` after any +2/3 prevotes.
* Enter: +2/3 precomits for block or nil.
* Enter: any +2/3 precommits for next round.
* Lock & precommit the ProposalBlock if we have enough prevotes for it (a POL in this round)
* else, unlock an existing lock and precommit nil if +2/3 of prevotes were nil,
* else, precommit nil otherwise. */
void ConsensusState::enterPrecommit(int64_t height, int _roundNumber) {
    if (roundState.height != height || _roundNumber < roundState.roundNumber ||
        (roundState.roundNumber == _roundNumber && RoundStepPrecommit <= roundState.stepType)) {
        logDebug("enterPrecommit(%v/%v): Invalid args. Current step: %v/%v/%v", height, _roundNumber, roundState.height,
                 roundState.roundNumber, roundState.stepType);
        return;
    }

    logInfo("enterPrecommit(%v/%v). Current: %v/%v/%v", height, _roundNumber, roundState.height, roundState.roundNumber,
            roundState.stepType);

    auto guard = finally([&]() {
        roundState.updateRoundStep(roundState.roundNumber, roundState.stepType);
        newStep();
    });

    // check for a polka
    BlockID blockID;
    bool ok = roundState.votes.getPrevotes(_roundNumber).twoThirdMajority(blockID);

    // If we don't have a polka, we must precommit nil.
    if (!ok) {
        if (roundState.lockedBlock != nullptr) {
            logInfo("enterPrecommit: No +2/3 prevotes during enterPrecommit while we're locked. Precommitting nil");
        } else {
            logInfo("enterPrecommit: No +2/3 prevotes during enterPrecommit. Precommitting nil.");
        }
        signAddVote(VoteTypePrecommit);
        return;
    }

    // At this point +2/3 prevoted for a particular block or nil.
    eventBus->publishEventPolka(roundState.roundStateEvent());

    // the latest POLRound should be this roundNumber.
    BlockID unused;
    int polRoundNumber = roundState.votes.polInfo(unused);
    if (polRoundNumber < roundState.roundNumber) {
        throw ExceptionInvalidPolRound(roundState.roundNumber, polRoundNumber);
    }

    // +2/3 prevoted nil. Unlock and precommit nil.
    if (blockID.isEmpty()) {
        if (roundState.lockedBlock == nullptr) {
            logInfo("enterPrecommit: +2/3 prevoted for nil.");
        } else {
            logInfo("enterPrecommit: +2/3 prevoted for nil. Unlocking");
            roundState.setLockedRoundNumber(0);
            roundState.setLockedBlock(nullptr);
            //roundState.LockedBlockParts = NULL;
            eventBus->publishEventUnlock(roundState.roundStateEvent());
        }
        signAddVote(VoteTypePrecommit);
        return;
    }
    // At this point, +2/3 prevoted for a particular block.

    // If we're already locked on that block, precommit it, and update the LockedRound


    // If +2/3 prevoted for proposal block, stage and precommit it
    if (roundState.proposalBlock->hashesTo(blockID.getHash())) {
        logInfo("enterPrecommit: +2/3 prevoted proposal block. Locking", "hash", blockID.getHash());
        // Validate the block.
        //throws ExceptionConsensus
        //err :=
        //TODO blockExec roundState.blockExec.ValidateBlock(roundState.state, roundState.ProposalBlock);
        /*if
        err != nil{
                throw Exception Consensus("enterPrecommit: +2/3 prevoted for an invalid block: %v", err);
        }*/
        roundState.setLockedRoundNumber(_roundNumber);
        roundState.setLockedBlock(roundState.proposalBlock);
        //roundState.setLockedBlockParts(roundState.ProposalBlockParts)
        eventBus->publishEventLock(roundState.roundStateEvent());
        signAddVote(VoteTypePrecommit, blockID.getHash());
        return;
    }

    // There was a polka in this round  for a block we don't have.
    // Fetch that block, unlock, and precommit nil.
    // The +2/3 prevotes for this roundNumber is the POL for our unlock.
    // TODO: In the future save the POL prevotes for justification.
    roundState.setLockedRoundNumber(0);
    roundState.setLockedBlock(nullptr);
    //roundState.lockedBlockParts = nil
    /*if (!roundState.proposalBlockParts.hasHeader(blockID.getPartsHeader()))
    {
        roundState.ProposalBlock = nil
        roundState.ProposalBlockParts = types.NewPartSetFromHeader(blockID.PartsHeader)
    }*/
    eventBus->publishEventUnlock(roundState.roundStateEvent());
    signAddVote(VoteTypePrecommit);
    // relies on ~FinUpdateRoundStep to updateRoundStep()

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
