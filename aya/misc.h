#pragma once

#include <libwing/Transaction.h>

#include <iostream>


using namespace std;
using namespace dev;
using namespace eth;


void dumpTx(const Transaction &tx)
{
    cerr << "IsCreate: " << tx.isCreation() << ", ";
    cerr << "Value: " << tx.value() << ", ";
    cerr << "Gas: " << tx.gas() << ", price: " << tx.gasPrice() << ", ";
    cerr << "From: " << tx.from() << ", To: " << tx.to() << ", ";
    cerr << "Data: " << toHex(tx.data()) << ", ";
    cerr << "HasZeroSignature: " << tx.hasZeroSignature() << endl;
}


