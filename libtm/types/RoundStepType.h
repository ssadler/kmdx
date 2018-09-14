//
// Created by thpn on 04/09/18.
//

#ifndef AYETH_ROUNDSTEPTYPE_H
#define AYETH_ROUNDSTEPTYPE_H

#include <string>

typedef enum RoundStepType {
    RoundStepNewHeight = 1, // Wait til CommitTime + timeoutCommit
    // NOTE: RoundStepNewHeight acts as RoundStepCommitWait.
            RoundStepNewRound = 2, // Setup new roundNumber and go to RoundStepPropose
    RoundStepPropose = 3, // Did propose, gossip proposal
    RoundStepPrevote = 4, // Did prevote, gossip prevotes
    RoundStepPrevoteWait = 5, // Did receive any +2/3 prevotes, start timeout
    RoundStepPrecommit = 6, // Did precommit, gossip precommits
    RoundStepPrecommitWait = 7, // Did receive any +2/3 precommits, start timeout
    RoundStepCommit = 8, // Entered commit state machine
} RoundStepType;

constexpr RoundStepType allRoundStepTypes[]{
        RoundStepNewHeight,
        RoundStepNewRound,
        RoundStepPropose,
        RoundStepPrevote,
        RoundStepPrevoteWait,
        RoundStepPrecommit,
        RoundStepPrecommitWait,
        RoundStepCommit
};


#endif //AYETH_ROUNDSTEPTYPE_H
