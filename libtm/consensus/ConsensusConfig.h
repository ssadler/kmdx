//
// Created by utnso on 10/07/18.
//

#ifndef TM_LIGHT_CONSENSUSCONFIG_H
#define TM_LIGHT_CONSENSUSCONFIG_H
#include <string>
#include "../types/HexBytes.h"
#include "boost/date_time/posix_time/posix_time_types.hpp"

using namespace boost::posix_time;

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
    int createEmptyBlocksInterval; //in seconds

    // Reactor sleep duration parameters are in milliseconds
    int peerGossipSleepDuration;
    int peerQueryMaj23SleepDuration;



public:
    HexBytes hash();

    const string &getRootDir() const;

    const string &getWalPath() const;

    const string &getWalFile() const;

    time_duration getTimeoutPropose(int roundNumber) const;

    time_duration getTimeoutPrevote(int roundNumber) const;

    time_duration getTimeoutPrecommit(int roundNumber) const;

    time_duration getTimeoutCommit() const;

    bool isSkipTimeoutCommit() const;

    int getMaxBlockSizeTxs() const;

    int getMaxBlockSizeBytes() const;

    bool isCreateEmptyBlocks() const;

    int getCreateEmptyBlocksInterval() const;

    int getPeerGossipSleepDuration() const;

    int getPeerQueryMaj23SleepDuration() const;

    bool isWaitForTxs() const;

    time_duration getPeerGossipSleepTime() const;

    time_duration getPeerQueryMaj23SleepTime() const;

    time_duration propose(int round) const;

    time_duration getEmptyBlocksIntervalTime() const;


};


#endif //TM_LIGHT_CONSENSUSCONFIG_H
