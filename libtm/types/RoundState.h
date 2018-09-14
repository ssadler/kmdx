//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_ROUNDSTATE_H
#define TM_LIGHT_ROUNDSTATE_H

#include <boost/optional.hpp>
#include "./RoundStepType.h"
#include "Block.h"
#include "ValidatorSet.h"
#include "VoteSet.h"
#include "HeightVoteSet.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "Event.h"

using namespace boost::posix_time;


class Proposal;


/**RoundState defines the internal consensus state.
 NOTE: Not thread safe. Should only be manipulated by functions downstream
 of the cs.receiveRoutine */
class RoundState {
    friend class ConsensusState;

    friend class common_test;

    height_t height; // Height we are working on
    int roundNumber;
    RoundStepType stepType;
    ptime startTime;
    ptime commitTime; // Subjective time when +2/3 precommits for Block at Round were found
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
    boost::optional<VoteSet> lastCommit; // Last precommits at Height-1
    ValidatorSet lastValidators;

public:
    const EventDataRoundState roundStateEvent(); //FIXME does this belong here?

    RoundState(std::string chainID);

    RoundState(std::string chainID, height_t height, ValidatorSet validators);

    bool operator==(const RoundState &other);

    height_t getHeight() const;

    void setLockedRoundNumber(int lockedRoundNumber);

    void setLockedBlock(const shared_ptr<Block> &lockedBlock);

    void setValidBlock(const shared_ptr<Block> &validBlock);

    void setProposalBlock(const shared_ptr<Block> &proposalBlock);

    void setValidRoundNumber(int validRoundNumber);

    void updateRoundStep(int number, RoundStepType type);

    void setHeight(height_t i);

    static const std::string stateTypeString(RoundStepType stype);

private:
    void setValidators(const ValidatorSet &validators);
};



#endif //TM_LIGHT_ROUNDSTATE_H
