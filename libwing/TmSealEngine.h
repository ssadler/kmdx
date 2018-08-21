//
// Created by thpn on 01/08/18.
//

#ifndef AYETH_TMSEALENGINE_H
#define AYETH_TMSEALENGINE_H

#include "../libwingcore/SealEngine.h"
#include "Block.h"

class TmSealEngine : public dev::eth::SealEngineBase {
    std::string name() const;

    void generateSeal(dev::eth::BlockHeader const &_bi);

    void onSealGenerated(std::function<void(dev::bytes const &s)> const &_f);

    dev::eth::EVMSchedule const &evmSchedule(dev::u256 const &_blockNumber) const;

    dev::u256 blockReward(dev::u256 const &_blockNumber) const;

    //dev::u256 validate(dev::eth::Block const& _block) const; //FIXME this method might belong in a different class
};


#endif //AYETH_TMSEALENGINE_H
