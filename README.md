# Wing

## Architecture

```
----------------------------------------------------
|                                                  |
|                  Tendermint Lite                 |
|                                                  |
|--------------------------------------------------|
|                         |                        |
|         Ethereum        |        WingVM          |
|     Virtual Machine     |   (Native machines)    |
|                         |                        |
----------------------------------------------------
```

### Modules from cpp-ethereum

Wing is part [cpp-ethereum](https://github.com/ethereum/cpp-ethereum) fork.
Some modules remain unmodified (as far as possible) in order that changes can be brought
in from upstream.

For other modules, this is not possible or desirable. Alot of attention is placed on making
Wing a polished experience.

| Module        | Status              |
| ------------  | ------------------- |
| libethereum   | Now libwing         |
| libethcore    | Now libwingcore     |
| libethashseal | TBA[ssimilated]     |
| evmc          | Copied into libwing |
| libevm        | Unmodified*         |
| libdevcore    | Unmodified*         |
| libdevcrypto  | Unmodified*         |
| libp2p        | Unmodified*         |
| utils         | Unmodified*         |
| test          | Unmodified*         |

## Notes

```
# Debug build (works?)
cmake build_type=Debug -Dconfig=Debug . -DCMAKE_BUILD_TYPE=Debug

# Run tests
ETHEREUM_TEST_PATH=test/jsontests test/testeth
ETHEREUM_TEST_PATH=test/jsontests test/testeth -- --all
```
