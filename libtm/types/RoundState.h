//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_ROUNDSTATE_H
#define TM_LIGHT_ROUNDSTATE_H

#include "Block.h"
#include "Validator.h"
#include "VoteSet.h"
#include "HeightVoteSet.h"
#include "event/Event.h"
#include "../helpers/Time.h"


class Proposal;

typedef enum RoundStepType {
    RoundStepNewHeight = 0x01, // Wait til CommitTime + timeoutCommit
    // NOTE: RoundStepNewHeight acts as RoundStepCommitWait.
            RoundStepNewRound = 0x02, // Setup new roundNumber and go to RoundStepPropose
    RoundStepPropose = 0x03, // Did propose, gossip proposal
    RoundStepPrevote = 0x04, // Did prevote, gossip prevotes
    RoundStepPrevoteWait = 0x05, // Did receive any +2/3 prevotes, start timeout
    RoundStepPrecommit = 0x06, // Did precommit, gossip precommits
    RoundStepPrecommitWait = 0x07, // Did receive any +2/3 precommits, start timeout
    RoundStepCommit = 0x08, // Entered commit state machine
} RoundStepType;

/**RoundState defines the internal consensus state.
 NOTE: Not thread safe. Should only be manipulated by functions downstream
 of the cs.receiveRoutine */
class RoundState {
    friend class ConsensusState;

    int64_t height; // Height we are working on
    int roundNumber;
    RoundStepType stepType;
    Time startTime;
    Time commitTime; // Subjective time when +2/3 precommits for Block at Round were found
    ValidatorSet validators;
    shared_ptr<Proposal> proposal;
    shared_ptr<Block> proposalBlock;
    //PartSet* proposalBlockParts;
    int lockedRoundNumber;
    shared_ptr<Block> lockedBlock;
    //PartSet* lockedBlockParts;
    int validRoundNumber; // Last known roundNumber with POL for non-nil valid block.
    shared_ptr<Block> validBlock; // Last known block of POL mentioned above.
    //PartSet* validBlockParts; // Last known block parts of POL metnioned above.
    HeightVoteSet votes;
    int commitRoundNumber;
    shared_ptr<VoteSet> lastCommit; // Last precommits at Height-1
    ValidatorSet lastValidators;

public:
    const EventDataRoundState roundStateEvent();

    RoundState();

    bool operator==(const RoundState &other);

    int64_t getHeight() const;

    void setLockedRoundNumber(int lockedRoundNumber);

    void setLockedBlock(const shared_ptr<Block> &lockedBlock);

    void setValidBlock(const shared_ptr<Block> &validBlock);

    void setValidRoundNumber(int i);


    void updateRoundStep(int number, RoundStepType type);


    void setHeight(int64_t i);
};



//TODO RoundStateSimple for RPC

#endif //TM_LIGHT_ROUNDSTATE_H
