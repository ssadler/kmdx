/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file ChainParams.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2015
 */

#include "ChainParams.h"
#include <json_spirit/JsonSpiritHeaders.h>
#include <libdevcore/Log.h>
#include <libdevcore/TrieDB.h>
#include <libdevcore/JsonUtils.h>
#include <libwingcore/SealEngine.h>
#include <libwingcore/BlockHeader.h>
#include <libwingcore/Precompiled.h>
#include "GenesisInfo.h"
#include "State.h"
#include "Account.h"
using namespace std;
using namespace dev;
using namespace eth;
namespace js = json_spirit;


ChainParams::ChainParams()
{
	for (unsigned i = 1; i <= 4; ++i)
		genesisState[Address(i)] = Account(0, 1);
	// Setup default precompiled contracts as equal to genesis of Frontier.
	precompiled.insert(make_pair(Address(1), PrecompiledContract(3000, 0, PrecompiledRegistrar::executor("ecrecover"))));
	precompiled.insert(make_pair(Address(2), PrecompiledContract(60, 12, PrecompiledRegistrar::executor("sha256"))));
	precompiled.insert(make_pair(Address(3), PrecompiledContract(600, 120, PrecompiledRegistrar::executor("ripemd160"))));
	precompiled.insert(make_pair(Address(4), PrecompiledContract(15, 3, PrecompiledRegistrar::executor("identity"))));
}

ChainParams::ChainParams(string const& _json, h256 const& _stateRoot)
{
	*this = loadConfig(_json, _stateRoot);
}

namespace
{
string const c_sealEngine = "sealEngine";
string const c_params = "params";
string const c_genesis = "genesis";
string const c_accounts = "accounts";

set<string> const c_knownChainConfigFields =
	{c_sealEngine, c_params, c_genesis, c_accounts};

string const c_minGasLimit = "minGasLimit";
string const c_maxGasLimit = "maxGasLimit";
string const c_gasLimitBoundDivisor = "gasLimitBoundDivisor";
string const c_homesteadForkBlock = "homesteadForkBlock";
string const c_daoHardforkBlock = "daoHardforkBlock";
string const c_EIP150ForkBlock = "EIP150ForkBlock";
string const c_EIP158ForkBlock = "EIP158ForkBlock";
string const c_byzantiumForkBlock = "byzantiumForkBlock";
string const c_eWASMForkBlock = "eWASMForkBlock";
string const c_constantinopleForkBlock = "constantinopleForkBlock";
string const c_accountStartNonce = "accountStartNonce";
string const c_maximumExtraDataSize = "maximumExtraDataSize";
string const c_tieBreakingGas = "tieBreakingGas";
string const c_blockReward = "blockReward";
string const c_difficultyBoundDivisor = "difficultyBoundDivisor";
string const c_minimumDifficulty = "minimumDifficulty";
string const c_durationLimit = "durationLimit";
string const c_chainID = "chainID";
string const c_networkID = "networkID";
string const c_allowFutureBlocks = "allowFutureBlocks";

set<string> const c_knownParamNames = {c_minGasLimit, c_maxGasLimit, c_gasLimitBoundDivisor,
    c_homesteadForkBlock, c_EIP150ForkBlock, c_EIP158ForkBlock, c_accountStartNonce,
    c_maximumExtraDataSize, c_tieBreakingGas, c_blockReward, c_byzantiumForkBlock, c_eWASMForkBlock,
    c_constantinopleForkBlock, c_daoHardforkBlock, c_minimumDifficulty, c_difficultyBoundDivisor,
    c_durationLimit, c_chainID, c_networkID, c_allowFutureBlocks};
} // anonymous namespace

ChainParams ChainParams::loadConfig(
    string const& _json, h256 const& _stateRoot, const boost::filesystem::path& _configPath) const
{
	ChainParams cp(*this);
	js::mValue val;
	json_spirit::read_string_or_throw(_json, val);
	js::mObject obj = val.get_obj();

	validateFieldNames(obj, c_knownChainConfigFields);

	cp.sealEngineName = obj[c_sealEngine].get_str();
	// params
	js::mObject params = obj[c_params].get_obj();
	validateFieldNames(params, c_knownParamNames);
	cp.accountStartNonce = u256(fromBigEndian<u256>(fromHex(params[c_accountStartNonce].get_str())));
	cp.maximumExtraDataSize = u256(fromBigEndian<u256>(fromHex(params[c_maximumExtraDataSize].get_str())));
	cp.tieBreakingGas = params.count(c_tieBreakingGas) ? params[c_tieBreakingGas].get_bool() : true;
	cp.setBlockReward(u256(fromBigEndian<u256>(fromHex(params[c_blockReward].get_str()))));

	auto setOptionalU256Parameter = [&params](u256 &_destination, string const& _name)
	{
		if (params.count(_name))
			_destination = u256(fromBigEndian<u256>(fromHex(params.at(_name).get_str())));
	};
	setOptionalU256Parameter(cp.minGasLimit, c_minGasLimit);
	setOptionalU256Parameter(cp.maxGasLimit, c_maxGasLimit);
	setOptionalU256Parameter(cp.gasLimitBoundDivisor, c_gasLimitBoundDivisor);
	setOptionalU256Parameter(cp.homesteadForkBlock, c_homesteadForkBlock);
	setOptionalU256Parameter(cp.EIP150ForkBlock, c_EIP150ForkBlock);
	setOptionalU256Parameter(cp.EIP158ForkBlock, c_EIP158ForkBlock);
	setOptionalU256Parameter(cp.byzantiumForkBlock, c_byzantiumForkBlock);
	setOptionalU256Parameter(cp.eWASMForkBlock, c_eWASMForkBlock);
	setOptionalU256Parameter(cp.constantinopleForkBlock, c_constantinopleForkBlock);
	setOptionalU256Parameter(cp.daoHardforkBlock, c_daoHardforkBlock);
	setOptionalU256Parameter(cp.minimumDifficulty, c_minimumDifficulty);
	setOptionalU256Parameter(cp.difficultyBoundDivisor, c_difficultyBoundDivisor);
	setOptionalU256Parameter(cp.durationLimit, c_durationLimit);

	if (params.count(c_chainID))
		cp.chainID = int(u256(fromBigEndian<u256>(fromHex(params.at(c_chainID).get_str()))));
	if (params.count(c_networkID))
		cp.networkID = int(u256(fromBigEndian<u256>(fromHex(params.at(c_networkID).get_str()))));
	cp.allowFutureBlocks = params.count(c_allowFutureBlocks);

	// genesis
	string genesisStr = json_spirit::write_string(obj[c_genesis], false);
	cp = cp.loadGenesis(genesisStr, _stateRoot);
	// genesis state
	string genesisStateStr = json_spirit::write_string(obj[c_accounts], false);

    cp.genesisState = jsonToAccountMap(
        genesisStateStr, cp.accountStartNonce, nullptr, &cp.precompiled, _configPath);
    cp.stateRoot = _stateRoot ? _stateRoot : cp.calculateStateRoot(true);

    return cp;
}

namespace
{
string const c_parentHash = "parentHash";
string const c_coinbase = "coinbase";
string const c_author = "author";
string const c_difficulty = "difficulty";
string const c_gasLimit = "gasLimit";
string const c_gasUsed = "gasUsed";
string const c_timestamp = "timestamp";
string const c_extraData = "extraData";
string const c_mixHash = "mixHash";
string const c_nonce = "nonce";

set<string> const c_knownGenesisFields = {
	c_parentHash, c_coinbase, c_author, c_difficulty, c_gasLimit, c_gasUsed, c_timestamp,
	c_extraData, c_mixHash, c_nonce
};
}

ChainParams ChainParams::loadGenesis(string const& _json, h256 const& _stateRoot) const
{
	ChainParams cp(*this);

	js::mValue val;
	json_spirit::read_string(_json, val);
	js::mObject genesis = val.get_obj();

	validateFieldNames(genesis, c_knownGenesisFields);

	cp.parentHash = h256(genesis[c_parentHash].get_str());
	cp.author = genesis.count(c_coinbase) ? h160(genesis[c_coinbase].get_str()) : h160(genesis[c_author].get_str());
	cp.difficulty = genesis.count(c_difficulty) ? u256(fromBigEndian<u256>(fromHex(genesis[c_difficulty].get_str()))) : 0;
	cp.gasLimit = u256(fromBigEndian<u256>(fromHex(genesis[c_gasLimit].get_str())));
	cp.gasUsed = genesis.count(c_gasUsed) ? u256(fromBigEndian<u256>(fromHex(genesis[c_gasUsed].get_str()))) : 0;
	cp.timestamp = u256(fromBigEndian<u256>(fromHex(genesis[c_timestamp].get_str())));
	cp.extraData = bytes(fromHex(genesis[c_extraData].get_str()));

	// magic code for handling ethash stuff:
	if (genesis.count(c_mixHash) && genesis.count(c_nonce))
	{
		h256 mixHash(genesis[c_mixHash].get_str());
		h64 nonce(genesis[c_nonce].get_str());
		cp.sealFields = 2;
		cp.sealRLP = rlp(mixHash) + rlp(nonce);
	}
	cp.stateRoot = _stateRoot ? _stateRoot : cp.calculateStateRoot();
	return cp;
}

SealEngineFace* ChainParams::createSealEngine()
{
	SealEngineFace* ret = SealEngineRegistrar::create(sealEngineName);
	assert(ret && "Seal engine not found");
	if (!ret)
		return nullptr;
	ret->setChainParams(*this);
	if (sealRLP.empty())
	{
		sealFields = ret->sealFields();
		sealRLP = ret->sealRLP();
	}
	return ret;
}

void ChainParams::populateFromGenesis(bytes const& _genesisRLP, AccountMap const& _state)
{
	BlockHeader bi(_genesisRLP, RLP(&_genesisRLP)[0].isList() ? BlockData : HeaderData);
	parentHash = bi.parentHash();
	author = bi.author();
	difficulty = bi.difficulty();
	gasLimit = bi.gasLimit();
	gasUsed = bi.gasUsed();
	timestamp = bi.timestamp();
	extraData = bi.extraData();
	genesisState = _state;
	RLP r(_genesisRLP);
	sealFields = r[0].itemCount() - BlockHeader::BasicFields;
	sealRLP.clear();
	for (unsigned i = BlockHeader::BasicFields; i < r[0].itemCount(); ++i)
		sealRLP += r[0][i].data();

	calculateStateRoot(true);

	auto b = genesisBlock();
	if (b != _genesisRLP)
	{
		cdebug << "Block passed:" << bi.hash() << bi.hash(WithoutSeal);
		cdebug << "Genesis now:" << BlockHeader::headerHashFromBlock(b);
		cdebug << RLP(b);
		cdebug << RLP(_genesisRLP);
		throw 0;
	}
}

h256 ChainParams::calculateStateRoot(bool _force) const
{
	MemoryDB db;
	SecureTrieDB<Address, MemoryDB> state(&db);
	state.init();
	if (!stateRoot || _force)
	{
		// TODO: use hash256
		//stateRoot = hash256(toBytesMap(gs));
		dev::eth::commit(genesisState, state);
		stateRoot = state.root();
	}
	return stateRoot;
}

bytes ChainParams::genesisBlock() const
{
	RLPStream block(3);

	calculateStateRoot();

	block.appendList(BlockHeader::BasicFields + sealFields)
			<< parentHash
			<< EmptyListSHA3	// sha3(uncles)
			<< author
			<< stateRoot
			<< EmptyTrie		// transactions
			<< EmptyTrie		// receipts
			<< LogBloom()
			<< difficulty
			<< 0				// number
			<< gasLimit
			<< gasUsed			// gasUsed
			<< timestamp
			<< extraData;
	block.appendRaw(sealRLP, sealFields);
	block.appendRaw(RLPEmptyList);
	block.appendRaw(RLPEmptyList);
	return block.out();
}
