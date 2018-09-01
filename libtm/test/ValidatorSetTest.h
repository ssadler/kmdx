//
// Created by thpn on 28/08/18.
//

#ifndef AYETH_VALIDATORSETTEST_H
#define AYETH_VALIDATORSETTEST_H

#include "../types/ValidatorSet.h"
#include "./common_test.h"

class ValidatorSetTest {
public:
    void testValidatorSetBasic();

/*func TestCopy(t *testing.T) {
	vset = randValidatorSet(10)
	vsetHash = vset.hash()
	if len(vsetHash) == 0 {
		t.Fatalf("ValidatorSet had unexpected zero hash")
	}

	vsetCopy = vset.copy()
	vsetCopyHash = vsetCopy.Hash()

	if !bytes.Equal(vsetHash, vsetCopyHash) {
		t.Fatalf("ValidatorSet copy had wrong hash. Orig: %X, Copy: %X", vsetHash, vsetCopyHash)
	}
}*/

/*
func BenchmarkValidatorSetCopy(b *testing.B) {
	b.StopTimer()
	vset = NewValidatorSet([]*Validator{})
	for i = 0; i < 1000; i++ {
		privKey = ed25519.GenPrivKey()
		pubKey = privKey.PubKey()
		val = NewValidator(pubKey, 0)
		if !vset.add(val) {
			panic("Failed to add validator")
		}
	}
	b.StartTimer()

	for i = 0; i < b.N; i++ {
		vset.copy()
	}
}
*/

//-------------------------------------------------------------------

    void testProposerSelection1();

    void testProposerSelectionProportional();

    void testProposerSelectionTwice();

    void testProposerSelectionNotTwice();

    void testProposerSelectionByAddress();

    void testProposerSelection3();

    void testValidatorSetVerifyCommit();
};


#endif //AYETH_VALIDATORSETTEST_H
