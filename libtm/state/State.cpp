//
// Created by Maru on 7/9/2018.
//

#include "State.h"

/* State is a short description of the latest committed block of the Tendermint consensus.
* It keeps all information necessary to validate new blocks,
* including the last validator set and the consensus params.
* Instead, use state.Copy() or state.NextState(...).*/



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
*/
