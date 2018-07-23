//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_ERROR_H
#define TM_LIGHT_ERROR_H

#include <string>

using namespace std;

class Error : public std::exception {
    string description;
public:
    Error(const string &description);

    const string &getDescription() const;
};

class ErrorInvalidProposalSignature : Error {
public:
    ErrorInvalidProposalSignature(string desc) : Error(desc) {}

    ErrorInvalidProposalSignature() : Error("ErrorInvalidProposalSignature") {}
};

class ErrInvalidProposalPolRound : Error {
public:
    ErrInvalidProposalPolRound(string desc) : Error(desc) {}

    ErrInvalidProposalPolRound() : Error("ErrInvalidProposalPolRound") {}
};

class ErrAddingVote : Error {
public:
    ErrAddingVote(string desc) : Error(desc) {}

    ErrAddingVote() : Error("ErrAddingVote") {}
};

class ErrVoteHeightMismatch : Error {
public:
    ErrVoteHeightMismatch(string desc) : Error(desc) {}

    ErrVoteHeightMismatch() : Error("ErrVoteHeightMismatch") {}
};

class ErrVoteConflictingVotes : Error {
public:
    ErrVoteConflictingVotes(string desc) : Error(desc) {}

    ErrVoteConflictingVotes() : Error("ErrVoteConflictingVotes") {}
};

class TransitionException : public std::exception {
    string desc;
public:
    TransitionException(string _desc) { desc = _desc; }

};

class ExceptionInvalidPolRound : public std::exception {
    string desc;
public:
    ExceptionInvalidPolRound(int expected, int found) {
        desc = "This POLRound should be " + std::to_string(expected) + " but got " + std::to_string(found);
    }

};


#endif //TM_LIGHT_ERROR_H
