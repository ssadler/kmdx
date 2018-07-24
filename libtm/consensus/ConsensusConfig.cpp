//
// Created by utnso on 10/07/18.
//

/*
// WaitForTxs returns true if the consensus should wait for transactions before entering the propose stepType
bool ConsensusConfig::waitForTxs() {
    return !createEmptyBlocks || createEmptyBlocksInterval > 0;
}

// EmptyBlocks returns the amount of time to wait before proposing an empty block or starting the propose timer if there are no txs available
chrono::duration<std::chrono::system_clock> ConsensusConfig::emptyBlocksInterval() {
    //return time.Duration(createEmptyBlocksInterval) * time.Second;
}

// Propose returns the amount of time to wait for a proposal
chrono::duration<std::chrono::system_clock> ConsensusConfig::propose(int round) {
    //return (timeoutPropose+timeoutProposeDelta*roundNumber) * time.Millisecond;
}

// Prevote returns the amount of time to wait for straggler votes after receiving any +2/3 prevotes
chrono::duration<std::chrono::system_clock> ConsensusConfig::prevote(int round) {
    //return (timeoutPrevote+timeoutPrevoteDelta*roundNumber) * time.Millisecond;
}

// Precommit returns the amount of time to wait for straggler votes after receiving any +2/3 precommits
chrono::duration<std::chrono::system_clock> ConsensusConfig::precommit(int round) {
    //return (timeoutPrecommit+timeoutPrecommitDelta*roundNumber) * time.Millisecond;
}

// Commit returns the amount of time to wait for straggler votes after receiving +2/3 precommits for a single block (ie. a commit).
chrono::duration<std::chrono::system_clock> ConsensusConfig::commit(Time t) {
    //return t.Add(time.Duration(timeoutCommit) * time.Millisecond);
}

// PeerGossipSleep returns the amount of time to sleep if there is nothing to send from the ConsensusReactor
chrono::duration<std::chrono::system_clock> ConsensusConfig::peerGossipSleep() {
    //return time.Duration(peerGossipSleepDuration) * time.Millisecond;
}

// PeerQueryMaj23Sleep returns the amount of time to sleep after each VoteSetMaj23Message is sent in the ConsensusReactor
chrono::duration<std::chrono::system_clock> ConsensusConfig::peerQueryMaj23Sleep() {
    //return time.Duration(peerQueryMaj23SleepDuratio) * time.Millisecond;
}

const string &ConsensusConfig::getRootDir() const {
    return rootDir;
}

const string &ConsensusConfig::getWalPath() const {
    return walPath;
}

const string &ConsensusConfig::getWalFile() const {
    return walFile;
}

int ConsensusConfig::getTimeoutPropose() const {
    return timeoutPropose;
}

int ConsensusConfig::getTimeoutProposeDelta() const {
    return timeoutProposeDelta;
}

int ConsensusConfig::getTimeoutPrevote() const {
    return timeoutPrevote;
}

int ConsensusConfig::getTimeoutPrevoteDelta() const {
    return timeoutPrevoteDelta;
}

int ConsensusConfig::getTimeoutPrecommit() const {
    return timeoutPrecommit;
}

int ConsensusConfig::getTimeoutPrecommitDelta() const {
    return timeoutPrecommitDelta;
}

int ConsensusConfig::getTimeoutCommit() const {
    return timeoutCommit;
}

bool ConsensusConfig::isSkipTimeoutCommit() const {
    return skipTimeoutCommit;
}

int ConsensusConfig::getMaxBlockSizeTxs() const {
    return maxBlockSizeTxs;
}

int ConsensusConfig::getMaxBlockSizeBytes() const {
    return maxBlockSizeBytes;
}

bool ConsensusConfig::isCreateEmptyBlocks() const {
    return createEmptyBlocks;
}

int ConsensusConfig::getCreateEmptyBlocksInterval() const {
    return createEmptyBlocksInterval;
}

int ConsensusConfig::getPeerGossipSleepDuration() const {
    return peerGossipSleepDuration;
}

int ConsensusConfig::getPeerQueryMaj23SleepDuration() const {
    return peerQueryMaj23SleepDuration;
}

*/

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

