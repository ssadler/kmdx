//
// Created by thpn on 24/08/18.
//

#ifndef AYETH_ERRORADDINGVOTE_H
#define AYETH_ERRORADDINGVOTE_H


#include <string>

class AddVoteResult {
    bool added;
    bool error;
//    Vote* Conflicting;
    std::string description;

public:
    AddVoteResult(bool added, bool isError, const std::string &description);

    AddVoteResult();

    bool isAdded() const;

    bool isError() const;

    const std::string &getDescription() const;
};


#endif //AYETH_ERRORADDINGVOTE_H
