#include <libdevcore/Common.h>
#include <libdevcore/CommonData.h>
#include <libdevcore/RLP.h>
#include <libdevcore/SHA3.h>
#include <libdevcrypto/Common.h>
#include <libethashseal/EthashClient.h>
#include <libethashseal/GenesisInfo.h>
#include <libwingcore/Common.h>
#include <libwingcore/BasicAuthority.h>
#include <libwing/Account.h>
#include <libwing/Block.h>
#include <libwing/BlockChain.h>
#include <libwing/Defaults.h>
#include <libwing/Transaction.h>
#include <libwing/Executive.h>

#include "misc.h"

#include <boost/optional.hpp>
#include <iostream>


using namespace std;
using namespace dev;
using namespace eth;


void setDefaultOrCLocale()
{
#if __unix__
    if (!setlocale(LC_ALL, ""))
    {
        setenv("LC_ALL", "C", 1);
    }
#endif
}


int main(int argc, char** argv)
{
    // Global inits
    setDefaultOrCLocale();
    // Init secp256k1 context by calling one of the functions.
    toPublic({});
    // Init defaults
    Defaults::get();
    // Init seal engine
    BasicAuthority::init();

    // A tx
    if (argc < 2)
    {
        cerr << "Usage: aya [tx]" << endl;
        return 1;
    }
    string txHex(argv[1]);
    Transaction tx(fromHex(txHex), CheckTransaction::Cheap);
    dumpTx(tx);

    // Chain params
    ChainParams chainParams(genesisInfo(eth::Network::KomodoNetwork), genesisStateRoot(eth::Network::KomodoNetwork));
    chainParams.difficulty = chainParams.minimumDifficulty;
    //chainParams.gasLimit = u256(1) << 32;
    chainParams.author = Address(fromHex("77952ce83ca3cad9f7adcfabeda85bd2f1f52008"));
    //chainParams.genesisState.clear();
    chainParams.genesisState[tx.sender()] = Account((u256)0, (u256)1e10);
    chainParams.stateRoot = chainParams.calculateStateRoot(true);

    RLPStream skStream, authoritiesStream;
    bytes sk = fromHex("3131313131313131313131313131313131313131313131313131313131313131");
    skStream << sk;
    Addresses addrs;
    addrs.push_back(chainParams.author);
    authoritiesStream << addrs;

    // Init chain
    auto _dbPath = "./_data";
    auto _we = WithExisting::Trust;

    BlockChain bc(chainParams, _dbPath, _we);
    bc.sealEngine()->setOption("authority", skStream.out());
    bc.sealEngine()->setOption("authorities", authoritiesStream.out());
    OverlayDB stateDB = State::openDB(_dbPath, bc.genesisHash(), _we);
    Block b = bc.genesisBlock(stateDB);
    b.setAuthor(chainParams.author);

    auto sealAndImport = [&]()
    {
        b.commitToSeal(bc);
        bc.sealEngine()->onSealGenerated([&](bytes const& header) {
            if (!b.sealBlock(header))
                throw std::runtime_error("error");
            cout << "BLOCK IS SEALED" << endl;
        });
        bc.sealEngine()->generateSeal(b.info());
        while (1) { try {
            bc.import(b.blockData(), stateDB);
            break;
        } catch (FutureTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } }
    };

    if (!b.sync(bc))
    {
        cout << "COMMIT GENESIS" << endl;
        sealAndImport();
        b.sync(bc);
    }

    ExecutionResult r = b.execute(bc.lastBlockHashes(), tx);
    sealAndImport();

    return 0;
}


// hath encodeTx '{"to":null,"from":null,"nonce":0,"value":1234,"to":"","gasPrice":10,"gas":200,"data":"abcd","chainId":1}' | hath signTx 06806e28570cc4dd772ddceb6bd06c959c034cd39793e04c97583a09133b94a4

