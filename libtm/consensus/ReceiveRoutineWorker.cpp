//

#include "ConsensusState.h"
#include "ReceiveRoutineWorker.h"

// Created by thpn on 17/08/18.
//
ReceiveRoutineWorker::ReceiveRoutineWorker() {
}

/** receiveRoutine handles messages which may cause state transitions.
* it's argument (n) is the number of messages to process before exiting - use 0 to run forever
* It keeps the RoundState and is the only thing that updates it.
* Updates (state transitions) happen on timeouts, complete proposals, and 2/3 majorities.
* ConsensusState must be locked before any internal state is updated.*/
/*void ReceiveRoutineWorker::receiveRoutine(dev::RLP &rlp) {
   auto guard = finally([&] {
        try {
            recover();
        } catch (exception &e) {
//clog(dev::VerbosityError, Logger::channelTm) << "CONSENSUS FAILURE!!!" << "err" << r << "stack" << string(debug.Stack()))
            throw e;
        }
    });*/
/*while(1) { //steps are only used for testing
    if (maxSteps > 0) {
        if nSteps >= maxSteps {
                    cs->Logger.Info("reached max steps. exiting receive routine")
                    cs->nSteps = 0
                    return
            }
    }


}*/

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void ReceiveRoutineWorker::receiveRoutine() {
    while (1) {
        dev::RLP msg;
        {
// Wait for a message to be added to the queue
            unique_lock<mutex> lk(mtx);
            while (m_queue.empty())
                m_cv.wait(lk);

            if (m_queue.empty())
                continue;

            msg = m_queue.front();
            m_queue.pop();
            switch (Message::allTypes[msg[0].toInt()]) {
                case MessageType::VoteType:
                    cs->handleVoteMsg(VoteMessage::fromRLP(msg));
                    break;
                case MessageType::ProposalType:
                    cs->handleProposalMsg(ProposalMessage::fromRLP(msg));
                    break;
                case MessageType::BlockType:
                    cs->handleBlockMsg(BlockMessage::fromRLP(msg));
                    break;
                case MessageType::TickerType:
                    //tockChan?
//                    wal.Write(ti);
                     // if the timeout is relevant to the rs
                     // go to the next step
                    cs->handleTimeout(TickerMessage::fromRLP(msg));
                    break;
                case MessageType::QuitType: {
/*                    m_timerExit = true;
                    timerThread.join();*/

//                    delete msg;
                    std::unique_lock<std::mutex> lk(mtx);
                    while (!m_queue.empty()) {
                        msg = m_queue.front();
                        m_queue.pop();
//                        delete msg;
                    }

                    cout << "Exit thread receive routine";
                    return;
                }
                    /*quit():

                     // NOTE: the internalMsgQueue may have signed messages from our
                     // priv_val that haven't hit the WAL, but its ok because
                     // priv_val tracks LastSig

                     // close wal now that we're done writing to it
                     cs->wal.Stop()
                     cs->wal.Wait()

                     close(cs->done)
                     return*/
            }
        }
    }
}

#pragma clang diagnostic pop

void ReceiveRoutineWorker::setConsensusState(ConsensusState *_cs) {
    cs = _cs;
}

bool ReceiveRoutineWorker::start() {
    if (!m_thread)
        m_thread = new thread(&ReceiveRoutineWorker::receiveRoutine, this);
    return true;
}

void ReceiveRoutineWorker::enqueue(dev::RLP msg) {
    //ASSERT_TRUE(m_thread);

//    Message* threadMsg = new ThreadMsg(MSG_POST_USER_DATA, data);

    // Add user data msg to queue and notify worker thread
    std::unique_lock<std::mutex> lk(mtx);
    m_queue.push(msg);
    m_cv.notify_one();
}

void ReceiveRoutineWorker::stop() {
    //TODO unimplemented
}
