//
// Created by utnso on 10/07/18.
//

#include "ConsensusConfig.h"

// WaitForTxs returns true if the consensus should wait for transactions before entering the propose stepType
bool ConsensusConfig::isWaitForTxs() const {
    return !createEmptyBlocks || createEmptyBlocksInterval > 0;
}

// EmptyBlocks returns the amount of time to wait before proposing an empty block or starting the propose timer if there are no txs available
time_duration ConsensusConfig::getEmptyBlocksIntervalTime() const {
    return seconds{createEmptyBlocksInterval};
}

// Propose returns the amount of time to wait for a proposal
time_duration ConsensusConfig::getTimeoutPropose(int roundNumber) const {
    return milliseconds{timeoutPropose + timeoutProposeDelta * roundNumber};
}

// Prevote returns the amount of time to wait for straggler votes after receiving any +2/3 prevotes
time_duration ConsensusConfig::getTimeoutPrevote(int roundNumber) const {
    return milliseconds{timeoutPrevote + timeoutPrevoteDelta * roundNumber};
}

// Precommit returns the amount of time to wait for straggler votes after receiving any +2/3 precommits
time_duration ConsensusConfig::getTimeoutPrecommit(int roundNumber) const {
    return milliseconds(timeoutPrecommit + timeoutPrecommitDelta * roundNumber);
}

// Commit returns the amount of time to wait for straggler votes after receiving +2/3 precommits for a single block (ie. a commit).
time_duration ConsensusConfig::getTimeoutCommit() const {
    return milliseconds{timeoutCommit};
}

// PeerGossipSleep returns the amount of time to sleep if there is nothing to send from the ConsensusReactor
time_duration ConsensusConfig::getPeerGossipSleepTime() const {
    return milliseconds{peerGossipSleepDuration * 1000};
}

// PeerQueryMaj23Sleep returns the amount of time to sleep after each VoteSetMaj23Message is sent in the ConsensusReactor
time_duration ConsensusConfig::getPeerQueryMaj23SleepTime() const {
    return milliseconds{peerQueryMaj23SleepDuration};
}



bool ConsensusConfig::isSkipTimeoutCommit() const {
    return skipTimeoutCommit;
}



/*
// WalFile returns the full path to the write-ahead log file
walFile() string {
if getWalFile() != "" {
return getWalFile();
}
return rootify(getWalPath(), getRootDir());
}

// SetWalFile sets the path to the write-ahead log file
setWalFile(walFile string) {
    setWalFile(walFile);
}*/ //TODO logging

