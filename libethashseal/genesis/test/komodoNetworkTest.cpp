#include <regex>
#include <libwingvm/Native.h>
#include <libwingvm/Asset.h>
#include "../../GenesisInfo.h"


static std::string c_genesisInfoKomodoNetworkTest;

void initGenesisInfoKomodoNetworkTest()
{
    std::string out =
R"E(
{
	"sealEngine": "NoProof",
	"params": {
		"accountStartNonce": "0x00",
		"homesteadForkBlock": "0x00",
		"daoHardforkBlock": "0x00",
		"EIP150ForkBlock": "0x00",
		"EIP158ForkBlock": "0x00",
		"byzantiumForkBlock": "0x00",
		"networkID" : "0x10",
		"chainID": "0x10",
		"maximumExtraDataSize": "0x20",
		"tieBreakingGas": false,
		"minGasLimit": "0x1388",
		"maxGasLimit": "7fffffffffffffff",
		"gasLimitBoundDivisor": "0x0400",
		"minimumDifficulty": "0x000000",
		"difficultyBoundDivisor": "0x0800",
		"durationLimit": "0x0d",
		"blockReward": "0x4563918244F40000"
	},
	"genesis": {
		"nonce": "0x0000000000000000",
		"difficulty": "0x000000000",
		"mixHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"author": "0x0000000000000000000000000000000000000000",
		"timestamp": "0x00",
		"parentHash": "0x0000000000000000000000000000000000000000000000000000000000000000",
		"extraData": "0x11bbe8db4e347b4e8c937c1c8370e4b5ed33adb3db69cbdb7a38e1e50b1b82fa",
		"gasLimit": "0x1388000"
	},
	"accounts": {
        "ROOT_CONTRACT_ADDRESS": {
            "storage": ROOT_CONTRACT_STORAGE
        },
        "MINERS_TOKEN_ADDRESS": {
            "code": "PROXY_TO_ASSET",
            "storage": MINERS_CONTRACT_STORAGE
        },
        "TEST_ADDRESS": {
            "balance": "2000000000000000000000000"
        }
    }
}
)E";

    Secret sk(fromHex("0x3131313131313131313131313131313131313131313131313131313131313131"));
    //  gdb --args kmdx/kmdx --test --config testChain.json -v4 --import-session-secret 0x3131313131313131313131313131313131313131313131313131313131313131

    Address testAddress("77952ce83ca3cad9f7adcfabeda85bd2f1f52008");
    out = std::regex_replace(out, std::regex("TEST_ADDRESS"), toHexPrefixed(testAddress));

    bytes assetProxyCode = wing::delegateCallProxyCode(wing::AssetContractAddress);

    // Init Miners's asset
    {
        State state(State::Null);
        DummyVM vm(wing::MinersTokenAddress, state);
        Asset asset(vm);
        asset.init();
        asset.erc20().setBalance(testAddress, 2 * u256(pow(10, 18)));
        out = std::regex_replace(out, std::regex("PROXY_TO_ASSET"), toHexPrefixed(assetProxyCode));
        out = std::regex_replace(out, std::regex("MINERS_TOKEN_ADDRESS"), toHexPrefixed(wing::MinersTokenAddress));
        out = std::regex_replace(out, std::regex("MINERS_CONTRACT_STORAGE"), vm.toJsonMap());
    }

    State state(State::Null);
    NativeManager root(state);
    root.setNativeContract(wing::AssetContractAddress, "asset");

    out = std::regex_replace(out, std::regex("ROOT_CONTRACT_STORAGE"), root.vm().toJsonMap());
    out = std::regex_replace(out, std::regex("ROOT_CONTRACT_ADDRESS"), toHexPrefixed(wing::RootContractAddress));
    c_genesisInfoKomodoNetworkTest = out;
}
