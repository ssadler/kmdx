//
// Created by utnso on 18/07/18.
//

#ifndef TM_LIGHT_FINUPDATEROUNDSTEP_H
#define TM_LIGHT_FINUPDATEROUNDSTEP_H


#include "../../types/RoundState.h"
#include "../../consensus/ConsensusState.h"

class FinUpdateRoundStep {
    RoundStepType step;
    int roundNumber;
    ConsensusState *consensusState;
public:
    FinUpdateRoundStep(RoundStepType step, int roundNumber, ConsensusState *consensusState);

    ~FinUpdateRoundStep();
};


#endif //TM_LIGHT_FINUPDATEROUNDSTEP_H
