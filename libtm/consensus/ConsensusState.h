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
#include "../state/State.h"
#include "../types/RoundState.h"
#include "../events/EventSwitch.h"
#include "../types/EventBus.h"
#include "../types/Proposal.h"
#include "../helpers/Logger.h"
#include "../Message/Message.h"
#include "../types/Error.h"
#include "../types/Vote.h"
#include "../types/PrivValidator.h"
#include "../helpers/Time.h"


class ConsensusState {


    //TODO communication BaseService baseService;
    ConsensusConfig consensusConfig;

    PrivValidator privValidator;

    //internalState
    mutex mtx;
    RoundState roundState;
    State state; //state until height-1
    RoundStepType stepType;
    pthread_t peerMsgQueue;
    pthread_t internalMsgQueue;
    time_t timeoutTicker;

    EventBus *eventBus;

    // a Write-Ahead Log ensures we can recover from any kind of crash
    // and helps us avoid signing conflicting votes
    //TODO LOG WAL wal;
    bool replayMode; // so we don't log signing errors during replay
    bool doWALCatchup; // determines if we even try to do the catchup

    // synchronous pubsub between main state and reactor.
    // state only emits EventNewRoundStep, EventVote and EventProposalHeartbeat
    EventSwitch eventSwitch;
    shared_ptr<Proposal> proposal;

    void setProposal(Proposal _proposal); //throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature);

    void handleMsg(const ProposalMessage msg); //throw(ErrInvalidProposalPolRound, ErrorInvalidProposalSignature);

    void handleMsg(const VoteMessage msg);

    void handleMsg(const BlockMessage msg);

    void tryAddVote(Vote vote, HexBytes peerID);

    bool addVote(Vote vote, HexBytes peerID);

public:
    void updateToState(State state);

    void reconstructLastCommit(State state);

    ConsensusState(ConsensusConfig config, State state);

    explicit ConsensusState(ConsensusConfig _config);

    virtual ~ConsensusState();

    void addVoteForCurrentRound(Vote vote);

    void unlockBlock(const Vote &vote, BlockID &blockId);

    void tryUpdateValidRoundAndBlock(const Vote &vote, BlockID &blockId);

    void enterNewRound(int64_t height, int number);

    bool isProposalComplete();

    void enterPrevoteWait(int64_t height, int _roundNumber);

    void enterPrecommit(int64_t height, int _roundNumber);

    void enterPrevote(int64_t _height, int _roundNumber);

    void newStep();

    void doPrevote(int64_t height, int _roundNumber);

    void scheduleTimeout(chrono::duration<std::chrono::system_clock> prevote, int64_t height, int number,
                         RoundStepType type);

    void enterCommit(int64_t height, int number);

    void signAddVote(VoteType type);

    void signAddVote(VoteType type, const HexBytes &bytes);

    void enterPrecommitWait(int64_t height, int number);

    void handleStragglerCommit(const Vote &vote);

    void tryFinalizeCommit(int64_t height);

    void finalizeCommit(int64_t height);

    void scheduleRound0(RoundState roundState);

    Commit loadSeenCommit(int64_t height);

    void clearProposal();
};

#endif /* CONSENSUSSTATE_H_ */
