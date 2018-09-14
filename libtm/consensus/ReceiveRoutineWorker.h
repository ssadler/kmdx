//
// Created by thpn on 17/08/18.
//

#ifndef AYETH_RECEIVEROUTINEWORKER_H
#define AYETH_RECEIVEROUTINEWORKER_H

#include <pthread.h>
#include <boost/thread.hpp>
#include <queue>
#include <libdevcore/RLP.h>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include "ConsensusState.h"

class ConsensusState;

class ReceiveRoutineWorker {
    ConsensusState *cs;
    std::condition_variable m_cv;
    std::queue<dev::RLP> m_queue;
    std::mutex mtx;
    std::thread* m_thread;

private:

    void receiveRoutine();

public:
    ReceiveRoutineWorker();

    bool start();

    void enqueue(dev::RLP);

    void stop();

    void setConsensusState(ConsensusState *_cs);

};


#endif //AYETH_RECEIVEROUTINEWORKER_H
