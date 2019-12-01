
#include <iostream>
#include "TrieHash.h"
#include "CommonData.h"
#include "RLP.h"
#include "SHA3.h"
#include <libdevcore/FixedHash.h>
#include "TrieHash.h"  
#include "TrieCommon.h"


using namespace std;
using namespace dev;

int main() {

	BytesMap m;
	unsigned j = 0;
    m[rlp(j++)] = {2};
    m[rlp(j++)] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    bytes out = rlp256(m);
    cout << toHex(out) << endl;
    cout << toHex(sha3(out)) << endl;
    return 0;
}
