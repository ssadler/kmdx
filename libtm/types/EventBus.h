//
// Created by utnso on 11/07/18.
//

#ifndef TM_LIGHT_EVENTBUS_H
#define TM_LIGHT_EVENTBUS_H


#include "Event.h"
#include "Server.h"

class EventBusSubscriber {
public:
//    virtual void Subscribe(Context, std::string subscriber , Query, out chan<- );
//    virtual void Unsubscribe(Context, std::string subscriber , Query);
//    virtual void UnsubscribeAll(Context, std::string subscriber );
};


// NewEventBus returns a new event bus.


// EventBus is a common bus for all events going through the system. All calls
// are proxied to underlying pubsub server. All events must be published using
// EventBus to ensure correct data types.
class EventBus {
    static const int defaultCapacity = 0;
private:
    Server* pubsub;
public:
    bool publishEventVote(EventDataVote eventDataVote);

    bool publishEventUnlock(const EventDataRoundState eventDataRoundState);

    void publishEventCompleteProposal(const EventDataRoundState state);

    void publishEventPolka(const EventDataRoundState state);

    void publishEventRelock(const EventDataRoundState state);

    void publishEventLock(const EventDataRoundState state);

    void publishEventNewRoundStep(EventDataRoundState state);

    void publishEventNewRound(const EventDataRoundState state);

    void publishEventTimeoutPropose(const EventDataRoundState);

	void publishEventTimeoutWait(const EventDataRoundState);

    EventBus() : EventBus(defaultCapacity) {}

    EventBus(int capacity) {
        // capacity could be exposed later if needed
        pubsub = new Server(capacity);
    }

//    (l log.Logger) setLogger {
//        BaseService.SetLogger(l)
//        pubsub->setLogger(l.With("module", "pubsub"))
//    }

    void onStart() {
        pubsub->onStart();
    }

    void onStop() {
        pubsub->stop();
    }

    void subscribe (std::string suscriber){
        return pubsub->subscribe(suscriber);
    }

    void unsubscribe (std::string suscriber){
        return pubsub->unsubscribe(suscriber);
    }

    void unsubscribeAll (std::string suscriber){
        return pubsub->unsubscribeAll(suscriber);
    }

    void publish (Event eventData ) {
// no explicit deadline for publishing events
        pubsub->publishWithTags(eventData);
    }

/*//--- block, tx, and vote events
    void publishEventNewBlock (EventNewBlock event ) {
        publish(EventNewBlock(), event);
    }

    void publishEventNewBlockHeader (EventNewBlockHeader event){
        publish(EventNewBlockHeader(), event);
    }

    void publishEventVote (EventDataVote event){
        publish(EventVote(), event);
    }*/

/*// PublishEventTx publishes tx event with tags from Result. Note it will add
// predefined tags (EventTypeKey, TxHashKey). Existing tags with the same names
// will be overwritten.
    (event EventDataTx) error publishEventTx {
// no explicit deadline for publishing events
        ctx = context.Background()

        tags = make(map[string]string)

// validate and fill tags from tx result
        for _, tag = range event.Result.Tags {
// basic validation
            if len(tag.Key) == 0 {
                Logger.Info("Got tag with an empty key (skipping)", "tag", tag, "tx", event.Tx)
                continue
            }
            tags[string(tag.Key)] = string(tag.Value)
        }

// add predefined tags
        logIfTagExists(EventTypeKey, tags, Logger)
        tags[EventTypeKey] = EventTx

        logIfTagExists(TxHashKey, tags, Logger)
        tags[TxHashKey] = fmt.Sprintf("%X", event.Tx.Hash())

        logIfTagExists(TxHeightKey, tags, Logger)
        tags[TxHeightKey] = fmt.Sprintf("%d", event.Height)

        pubsub->publishWithTags(ctx, event, tmpubsub->newTagMap(tags))
        return NULL
    }*/

    /*(event EventDataProposalHeartbeat) error publishEventProposalHeartbeat {
        return Publish(EventProposalHeartbeat, event)
    }

//--- EventDataRoundState events

    (event EventDataRoundState) error publishEventNewRoundStep {
        return Publish(EventNewRoundStep, event)
    }

    (event EventDataRoundState) error publishEventTimeoutPropose {
        return Publish(EventTimeoutPropose, event)
    }

    (event EventDataRoundState) error publishEventTimeoutWait {
        return Publish(EventTimeoutWait, event)
    }

    (event EventDataRoundState) error publishEventNewRound {
        return Publish(EventNewRound, event)
    }

    (event EventDataRoundState) error publishEventCompleteProposal {
        return Publish(EventCompleteProposal, event)
    }

    (event EventDataRoundState) error publishEventPolka {
        return Publish(EventPolka, event)
    }

    (event EventDataRoundState) error publishEventUnlock {
        return Publish(EventUnlock, event)
    }

    (event EventDataRoundState) error publishEventRelock {
        return Publish(EventRelock, event)
    }

    (event EventDataRoundState) error publishEventLock {
        return Publish(EventLock, event)
    }*/

/*    func logIfTagExists(tag string, tags map[string]string, logger log.Logger) {
        if value, ok = tags[tag]; ok {
                logger.Error("Found predefined tag (value will be overwritten)", "tag", tag, "value", value)
        }
    }*/
};


#endif //TM_LIGHT_EVENTBUS_H
