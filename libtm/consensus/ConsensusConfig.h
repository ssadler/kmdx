//
// Created by utnso on 10/07/18.
//

#ifndef TM_LIGHT_CONSENSUSCONFIG_H
#define TM_LIGHT_CONSENSUSCONFIG_H

#include <string>
#include "../types/HexBytes.h"
#include "../helpers/Time.h"

using namespace std;

/** ConsensusConfig defines the confuguration for the Tendermint consensus service,
* including timeouts and details about the WAL and the block structure.*/
class ConsensusConfig {
private:
    string rootDir;
    string walPath;
    string walFile;  // overrides WalPath if set

    // All timeouts are in milliseconds
    int timeoutPropose;
    int timeoutProposeDelta;
    int timeoutPrevote;
    int timeoutPrevoteDelta;
    int timeoutPrecommit;
    int timeoutPrecommitDelta;
    int timeoutCommit;

    // Make progress as soon as we have all the precommits (as if TimeoutCommit = 0)
    bool skipTimeoutCommit;

    // BlockSize
    int maxBlockSizeTxs;
    int maxBlockSizeBytes;

    // EmptyBlocks mode and possible interval between empty blocks in seconds
    bool createEmptyBlocks;
    int createEmptyBlocksInterval;

    // Reactor sleep duration parameters are in milliseconds
    int peerGossipSleepDuration;
    int peerQueryMaj23SleepDuration;

    chrono::duration<chrono::system_clock> peerGossipSleep();

    chrono::duration<chrono::system_clock> peerQueryMaj23Sleep();

    chrono::duration<chrono::system_clock> propose(int round);

    chrono::duration<chrono::system_clock> emptyBlocksInterval();


public:
    HexBytes hash();


    const string &getRootDir() const;

    const string &getWalPath() const;

    const string &getWalFile() const;

    int getTimeoutPropose() const;

    int getTimeoutProposeDelta() const;

    int getTimeoutPrevote() const;

    int getTimeoutPrevoteDelta() const;

    int getTimeoutPrecommit() const;

    int getTimeoutPrecommitDelta() const;

    int getTimeoutCommit() const;

    bool isSkipTimeoutCommit() const;

    int getMaxBlockSizeTxs() const;

    int getMaxBlockSizeBytes() const;

    bool isCreateEmptyBlocks() const;

    int getCreateEmptyBlocksInterval() const;

    int getPeerGossipSleepDuration() const;

    int getPeerQueryMaj23SleepDuration() const;

    chrono::duration<chrono::system_clock> prevote(int round);

    chrono::duration<chrono::system_clock> precommit(int round);

    bool waitForTxs();

    chrono::duration<chrono::system_clock> commit(Time time);
};


#endif //TM_LIGHT_CONSENSUSCONFIG_H
