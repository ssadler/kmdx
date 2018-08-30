//
// Created by utnso on 12/07/18.
//

#ifndef TM_LIGHT_VOTE_H
#define TM_LIGHT_VOTE_H


#include <ctime>
#include <cstdint>
#include <string>
#include <libdevcore/RLP.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <boost/test/unit_test.hpp>
#include "HexBytes.h"
#include "Error.h"


enum VoteType {
    VoteTypePrevote, VoteTypePrecommit, VoteTypeFirstCommit, VoteTypeSize
};


class Vote {
    AddressTm validatorAddress;
    int validatorIndex;
    height_t height;
    int roundNumber;
    boost::posix_time::ptime timestamp;
    enum VoteType type;
    BlockID blockID;
    SignatureTm signature;

    friend class MockPV;

    friend class VoteSetTest;

    friend class VoteTest;

public:
    static VoteType allVoteTypes[];

    Vote(const Vote &);

    explicit Vote(VoteType type);

    static std::string voteTypeToString(VoteType type);

    Vote(const AddressTm addresstm, int _validatorIndex, height_t _height, int _roundNumber,
         const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID);

    Vote(const AddressTm addresstm, int _validatorIndex, height_t _height, int _roundNumber,
         const boost::posix_time::ptime &_timestamp, VoteType _type, const BlockID _blockID,
         const SignatureTm _signature);

    HexBytes signBytes(std::string chainID) const; //TODO

    static Vote fromRLP(dev::RLP &);

    std::string toString() const;

    dev::RLPStream toRLP();

    void verify(const std::string &chainID, const PubKey &pubKey) const; //TODO crypto

    const AddressTm &getValidatorAddress() const;

    int getValidatorIndex() const;

    height_t getHeight() const;

    int getRoundNumber() const;

    boost::posix_time::ptime getTimestamp() const;

    VoteType getType() const;

    const BlockID &getBlockID() const;

    const SignatureTm &getSignature() const;

    void setSignature(const SignatureTm &signature);

    static bool isVoteTypeValid(VoteType type);
};


#endif //TM_LIGHT_VOTE_H
