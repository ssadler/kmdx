//
// Created by Maru on 7/9/2018.
//

#include "State.h"

/* State is a short description of the latest committed block of the Tendermint consensus.
* It keeps all information necessary to validate new blocks,
* including the last validator set and the consensus params.
* Instead, use state.Copy() or state.NextState(...).*/

State::State(string _chainID, int _lastBlockHeight, BlockID _lastBlockID, time_t _lastBlockTime,
             ValidatorSet _validators, ValidatorSet _lastValidators, int _lastHeightValidatorsChanged,
             ConsensusConfig *_consensusParams, int _lastHeightConsensusParamsChanged, HexBytes _appHash) {
    chainID = _chainID;
    lastBlockHeight = _lastBlockHeight;
    lastBlockID = _lastBlockID;
    lastBlockTime = _lastBlockTime;
    validators = _validators;
    lastValidators = _lastValidators;
    lastHeightValidatorsChanged = _lastHeightValidatorsChanged;
    consensusParams = _consensusParams; //TODO
    lastHeightConsensusParamsChanged = _lastHeightConsensusParamsChanged;
    appHash = _appHash;
}

State::~State() {
    //TODO state destructor
}



//State::State(const State &obj){
//TODO copy constructor
//}

//TODO GENESIS
//------------------------------------------------------------------------
// Genesis

// MakeGenesisStateFromFile reads and unmarshals state from the given
// file.
//
// Used during replay and in tests.
/** needs an initialized state */
/*Errorf makeGenesisStateFromFile(string genDocFile, string& genDoc, State& state) {
    Errorf err = makeGenesisDocFromFile(genDocFile, genDoc);
    if (err != null) {
        return err;
    }
    makeGenesisState(genDocFile, genDoc, state);
    return err;
}

// MakeGenesisDocFromFile reads and unmarshals genesis doc from the given file.
Errorf makeGenesisDocFromFile(string genDocFile, string& genDoc) {
    err = genDocJSON(genDocFile, genDoc);
    if (err != null) {
        genDoc = null;
        return Errorf("Couldn't read GenesisDoc file: %v", err);
    }
    err = types.GenesisDocFromJSON(genDocJSON, genDoc);
    if (err != null) {
        genDoc = null;
        return Errorf("Error reading GenesisDoc: %v", err);
    }
    return null;
}

// MakeGenesisState creates state from types.GenesisDoc.
Errorf makeGenesisState(GenesisDoc genDoc, State& state) {
    Errorf err = genDoc.ValidateAndComplete();
    if (err != null) {
        state = State();
        return Errorf("Error in genesis file: %v", err);
    }

    // Make validators slice
    validators = vector<types.Validator*>;
    for(int i = 0; i< genDoc.getValidators().size(); i++ ){
        Validator val = genDoc.getValidators().get(i);
        validators.add(Validator(val.getAddress(), val.getPubKey(), val.getPower()));
    }

    state = State( genDoc.ChainID, 0, types.BlockID{}, genDoc.GenesisTime, types.NewValidatorSet(validators), types.NewValidatorSet(null), 1, genDoc.ConsensusParams, 1, genDoc.AppHash );	)
    return null;
}
*/