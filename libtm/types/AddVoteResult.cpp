//
// Created by thpn on 24/08/18.
//

#include "AddVoteResult.h"

bool AddVoteResult::isAdded() const {
    return added;
}

bool AddVoteResult::isError() const {
    return error;
}

const std::string &AddVoteResult::getDescription() const {
    return description;
}

AddVoteResult::AddVoteResult(bool added, bool isError, const std::string &description) : added(added),
                                                                                         error(isError),
                                                                                         description(description) {}

AddVoteResult::AddVoteResult() : added(true), error(false) {}
