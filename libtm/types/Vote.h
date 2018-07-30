//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_VOTE_H
#define TM_LIGHT_VOTE_H


#include <ctime>
#include <cstdint>
#include <string>
#include "HexBytes.h"
#include "Signature.h"
#include <libdevcore/RLP.h>
#include "boost/date_time/posix_time/posix_time_types.hpp"

enum VoteType {
    VoteTypePrevote, VoteTypePrecommit, VoteTypeFirstCommit, VoteTypeSize
};


std::string voteTypeToString(VoteType type) {
    switch (type) {
        case VoteTypePrevote:
            return "VoteTypePrevote";
        case VoteTypePrecommit:
            return "VoteTypePrecommit";
        case VoteTypeFirstCommit:
            return "VoteTypeFirstCommit";
        default:
            return "invalid Vote type";
    }
};

class Vote {

    HexBytes validatorAddress;
    int validatorIndex;
    int64_t height;
    int roundNumber;
    boost::posix_time::ptime timestamp;
    enum VoteType type;
    BlockID blockID;
    Signature signature; // comes from crypto package
public:
    static constexpr VoteType allVoteTypes[VoteTypeSize] = {
            VoteTypePrevote, VoteTypePrecommit, VoteTypeFirstCommit,
    };
    static Vote fromRLP(dev::RLP & );

    dev::u256 toRLP();

    const HexBytes &getValidatorAddress() const;

    int getValidatorIndex() const;

    int64_t getHeight() const;

    int getRoundNumber() const;

    boost::posix_time::ptime getTimestamp() const;

    VoteType getType() const;

    const BlockID &getBlockID() const;

    const Signature &getSignature() const;

    explicit Vote(VoteType type);

    Vote(const PeerID peerID, int _validatorIndex, int64_t _height, int _roundNumber,
               const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID, const Signature _signature);


};


#endif //TM_LIGHT_VOTE_H
