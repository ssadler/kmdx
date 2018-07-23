//
// Created by utnso on 18/07/18.
//

#include "FinUpdateRoundStep.h"


FinUpdateRoundStep::FinUpdateRoundStep(RoundStepType step, int roundNumber, ConsensusState *consensusState) : step(
        step), roundNumber(roundNumber), consensusState(consensusState) {}

// Done enterPrevoteWait:
FinUpdateRoundStep::~FinUpdateRoundStep() {
    consensusState->updateRoundStep(roundNumber, step);
    consensusState->newStep();
}
