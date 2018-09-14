/*
 * ConsensusState.h
 *
 * Created on: Jul 9, 2018
 * Author: Maru
 */

#ifndef CONSENSUSSTATE_H_
#define CONSENSUSSTATE_H_

#include <pthread.h>
#include <cstdlib>
#include <boost/optional.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/fiber/all.hpp>
#include <boost/exception_ptr.hpp>


#include "../state/State.h"
#include "../types/RoundState.h"
#include "../events/EventSwitch.h"
#include "../types/EventBus.h"
#include "../types/Proposal.h"
#include "../helpers/Logger.h"
#include "../Message/Message.h"
#include "../types/Error.h"
#include "../types/PrivValidator.h"
#include "../types/TimeoutTicker.h"
#include "../helpers/TmController.h"
#include "../helpers/Finally.h"
#include "ReceiveRoutineWorker.h"
#include "ConsensusConfig.h"

using namespace boost::posix_time;

class ConsensusState {
    friend class ReceiveRoutineWorker;

    //TODO communication BaseService baseService;
    ConsensusConfig consensusConfig;
    TmController &controller;
    boost::optional<PrivValidator> privValidator;

    //internalState
    mutex mtx;
    RoundState roundState;
    boost::optional<State> state; //state until height-1
    RoundStepType stepType;

    ReceiveRoutineWorker receiveRoutineWorker;
    pthread_t peerMsgQueue;
    pthread_t internalMsgQueue;
    TimeoutTicker timeoutTicker;

    EventBus eventBus;

    // a Write-Ahead Log ensures we can recover from any kind of crash
    // and helps us avoid signing conflicting votes
    //TODO LOG WAL wal;
    bool replayMode; // so we don't log signing errors during replay
    bool doWALCatchup; // determines if we even try to do the catchup

    // synchronous pubsub between main state and reactor.
    // state only emits EventNewRoundStep, EventVote and EventProposalHeartbeat
    EventSwitch eventSwitch;
    shared_ptr<Proposal> proposal;

    void setProposal(
            Proposal _proposal); //throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature, __FILE__, __LINE__);

    void
    handleProposalMsg(
            const ProposalMessage &msg); //throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature, __FILE__, __LINE__);

    void tryAddVote(Vote &vote, AddressTm addresstm);

    Vote signVote(VoteType type, const HexBytes &b);

    void handleVoteMsg(const VoteMessage &msg);

    void handleBlockMsg(const BlockMessage &msg);

    void handleTimeout(const TickerMessage &msg);

    bool addVote(Vote &vote, AddressTm pid);

public:

    ConsensusState(ConsensusConfig config, State &state, TmController &_controller);

    ConsensusState(const ConsensusState &cs, TmController &_controller);

    ConsensusState(ConsensusConfig _config, TmController &_controller);

    virtual ~ConsensusState();

    void updateToState(State &state);

    void enterPropose(height_t height, int round);

    void reconstructLastCommit(State &state);

    void addVoteForCurrentRound(Vote &vote);

    void unlockBlock(const Vote &vote, BlockID &blockId);

    void tryUpdateValidRoundAndBlock(const Vote &vote, BlockID &blockId);

    void enterNewRound(height_t height, int number);

    bool isProposalComplete();

    void enterPrevoteWait(height_t height, int _roundNumber);

    void enterPrecommit(height_t height, int _roundNumber);

    void enterPrevote(height_t _height, int _roundNumber);

    void newStep();

    void doPrevote(height_t height, int _roundNumber);

    void scheduleTimeout(time_duration prevote, height_t height, int number,
                         RoundStepType type);

    void enterCommit(height_t height, int number);

    void signAddVote(VoteType type);

    void signAddVote(VoteType type, const HexBytes &b);

    void enterPrecommitWait(height_t height, int number);

    void handleStragglerCommit(Vote &vote);

    void tryFinalizeCommit(height_t height);

    void finalizeCommit(height_t height);

    void scheduleRound0(RoundState roundState);


    void recover();

    Commit loadSeenCommit(height_t height); //originally from blockStore

    void clearProposal();

    bool isValidator();

    bool isProposer();

    void onStart();

    void catchupReplay(height_t height);

    void decideProposal(height_t height, int round);

    const RoundState &getRoundState() const;

    void sendInternalMessage(Message) {
        //TODO unimplemented
    }

    void startRoutines();

    boost::optional<Block> createProposalBlock();

    friend class common_test;

    void addToPeerMsgQueue(Message m);

};

#endif /* CONSENSUSSTATE_H_ */
