//
// Created by thpn on 25/07/18.
//

#ifndef AYETH_NODE_H
#define AYETH_NODE_H


class Node {
    /* cmn.BaseService

     // config
             config        *cfg.Config
             genesisDoc    *types.GenesisDoc   // initial validator set
             privValidator types.PrivValidator // local node's validator key

     // network
             sw       *p2p.Switch  // p2p connections
             addrBook pex.AddrBook // known peers

     // services
             eventBus         *types.EventBus // pub/sub for services
             stateDB          dbm.DB
             blockStore       *bc.BlockStore         // store the blockchain to disk
             bcReactor        *bc.BlockchainReactor  // for fast-syncing
             mempoolReactor   *mempl.MempoolReactor  // for gossipping transactions
             consensusState   *cs.ConsensusState     // latest consensus state
             consensusReactor *cs.ConsensusReactor   // for participating in the consensus
             evidencePool     *evidence.EvidencePool // tracking evidence
             proxyApp         proxy.AppConns         // connection to the application
             rpcListeners     []net.Listener         // rpc servers
             txIndexer        txindex.TxIndexer
             indexerService   *txindex.IndexerService
             prometheusSrv    *http.Server*/
};


#endif //AYETH_NODE_H
