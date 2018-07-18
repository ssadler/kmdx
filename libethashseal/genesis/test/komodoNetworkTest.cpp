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
        "77952ce83ca3cad9f7adcfabeda85bd2f1f52008": {
            "balance": "2000000000000000000000"
        }
    }
}
)E";

    State state(State::Null);
    wing::NativeStorage ns(wing::RootContractAddress, state);
    ns.putData(wing::keyOn(0, "hello"), std::string() + "world");
    ns.putData(wing::keyOn(PartitionNativeContracts, wing::AssetContractAddress), std::string() + "asset");
    out = std::regex_replace(out, std::regex("ROOT_CONTRACT_STORAGE"), ns.toJsonMap());
    out = std::regex_replace(out, std::regex("ROOT_CONTRACT_ADDRESS"), toHexPrefixed(wing::RootContractAddress));
    c_genesisInfoKomodoNetworkTest = out;
}
