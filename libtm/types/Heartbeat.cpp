//
// Created by utnso on 17/07/18.
//

/* SignBytes returns the Heartbeat bytes for signing.
* It panics if the Heartbeat is nil. */
/*
byte  heartbeat::signBytes(chainID string) []{
    bz, err := cdc.MarshalJSON(CanonicalHeartbeat(chainID, heartbeat))
    if err != nil {
                panic(err)
        }
    return bz
}
*/

/* Copy makes a copy of the Heartbeat. */
/*
Heartbeat  heartbeat::copy() {
    if heartbeat == nil {
                return nil
        }
    heartbeatCopy := *heartbeat
    return &heartbeatCopy
}
*/

/*String returns a string representation of the Heartbeat.*/
#include "Heartbeat.h"

/*
string  heartbeat::string() {
    if heartbeat == nil {
                return "nil-heartbeat"
        }

    return fmt.Sprintf("Heartbeat{%v:%X %v/%02d (%v) %v}",
                       heartbeat.ValidatorIndex, cmn.Fingerprint(heartbeat.ValidatorAddress),
                       heartbeat.Height, heartbeat.Round, heartbeat.Sequence, heartbeat.Signature)
}
*/
Heartbeat::Heartbeat(const HexBytes &validatorAddress, int validatorIndex, long height, int round, int sequence,
                     const Signature &signature) : validatorAddress(validatorAddress), validatorIndex(validatorIndex),
                                                   height(height), round(round), sequence(sequence),
                                                   signature(signature) {}
