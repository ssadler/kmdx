//
// Created by utnso on 16/07/18.
//
#include "PrivValidator.h"
//#include "../../libethcore/BasicAuthority.h"

using byte = uint8_t;
using bytes = std::vector<byte>;
using height_t = uint64_t;

PrivValidator::PrivValidator(PrivKey _privKey) :
    addresstm(_privKey.getAddress()), privKey(_privKey) {
};

std::string PrivValidator::toString() const {
    return this->getAddress().toString();
}

AddressTm PrivValidator::getAddress() const {
    return addresstm;
}

void PrivValidator::signProposal(string, Proposal &) {
    //TODO
}

// signVote checks if the vote is good to sign and sets the vote signature.
// It may need to set the timestamp as well if the vote is otherwise the same as
// a previously signed vote (ie. we crashed after signing but before the vote hit the WAL).
void PrivValidator::signVote(string chainID, Vote &vote) {
   /* HexBytes signBytes = vote.signBytes(chainID);

    // We might crash before writing to the wal,
    // causing us to try to re-sign for the same HRS.
    // If signbytes are the same, use the last signature.
    // If they only differ by timestamp, use last timestamp and signature
    // Otherwise, return error
    if (checkHRS(vote.getHeight(), vote.getRoundNumber(), vote.getType())) {
                if (signBytes == lastSignBytes) {
                    vote.setSignature(lastSignature);
                } else {
                    boost::posix_time::second_clock::date_type timestamp = checkVotesOnlyDifferByTimestamp(lastSignBytes, signBytes);
                    if (!timestamp.is_not_a_date()) {
                    vote.setTimestamp(timestamp);
                    vote.setSignature(lastSignature);
                } else {
                    throw ErrConflictingData(__FILE__, __LINE__);
                }
        }
    }*/

    // It passed the checks. Sign the vote
    dev::Secret s(privKey.getKey());
    dev::Signature sig = dev::sign(s, dev::sha3(vote.signBytes(chainID)));
    vote.setSignature(SignatureTm(sig.asBytes()));
//    saveSigned(height, round, step, signBytes, sig)
}


const PubKey &PrivValidator::getPubKey() const {
    return privKey.getPubKey();
}

const PrivKey &PrivValidator::getPrivKey() const {
    return privKey;
}

PrivValidator::PrivValidator() : PrivValidator(dev::Secret("aaaa")){}
