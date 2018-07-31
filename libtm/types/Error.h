//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_ERROR_H
#define TM_LIGHT_ERROR_H

#include <string>
#include "Vote.h"

using namespace std;

class Error : public std::exception {
    string description;
public:
    explicit Error(const string description);

    const string &getDescription() const;
};

class ErrorInvalidProposalSignature : public Error {
public:
    explicit ErrorInvalidProposalSignature(string desc) : Error(desc) {}

    ErrorInvalidProposalSignature() : Error("ErrorInvalidProposalSignature") {}
};

class ErrInvalidProposalPolRound : public Error {
public:
    explicit ErrInvalidProposalPolRound(string desc) : Error(desc) {}

    ErrInvalidProposalPolRound() : Error("ErrInvalidProposalPolRound") {}
};

class ErrAddingVote : public Error {
public:
    explicit ErrAddingVote(string desc) : Error(desc) {}

    ErrAddingVote() : Error("ErrAddingVote") {}
};

class ErrVoteHeightMismatch : public Error {
public:
    explicit ErrVoteHeightMismatch(string desc) : Error(desc) {}

    ErrVoteHeightMismatch() : Error("ErrVoteHeightMismatch") {}
};

class ErrVoteConflictingVotes : public Error {
public:
    explicit ErrVoteConflictingVotes(string desc) : Error(desc) {}

    ErrVoteConflictingVotes() : Error("ErrVoteConflictingVotes") {}
};


class TransitionException : public std::exception {
    string desc;
public:
    explicit TransitionException(string _desc) { desc = _desc; }

};

class SignError : public Error {
public:
    explicit SignError(string _desc) : Error(_desc) {}
};


class ExceptionInvalidPolRound : public std::exception {
    string desc;
public:
    ExceptionInvalidPolRound(int expected, int found) {
        desc = "This POLRound should be " + std::to_string(expected) + " but got " + std::to_string(found);
    }

};

class ErrorUnexpectedVoteType : public Error {
public:
    ErrorUnexpectedVoteType() : Error("ErrorUnexpectedVoteType") {}

    explicit ErrorUnexpectedVoteType(VoteType type);
};

class ErrInvalidVoteSet : public Error {
public:
    explicit ErrInvalidVoteSet(string desc) : Error("ErrInvalidVoteSet " + desc) {}

};

class Panic : public std::exception {
    string desc;
public:
    explicit Panic(string d) : desc(d) {}

    const string &getDesc() const {
        return desc;
    }

    Panic(string d, int n...) : desc(d + to_string(n)) {//FIXME
    }
};

class PanicNullVoteSet : public Panic {
public:
    PanicNullVoteSet() : Panic("AddVote() on nil VoteSet") {}
};

class PanicNoPolka : public Panic {
public:
    explicit PanicNoPolka(string d) : Panic(d + "commit does not have two thirds majority") {}
};

class PanicInvalidHash : public Panic {
public:
    explicit PanicInvalidHash(string d) : Panic(d) {}
};

class PanicEnterRound : public Panic {
public:
    explicit PanicEnterRound(string d) : Panic(d) {}
};

class PanicCrisis : public Panic {
    Panic inner;
public:
    PanicCrisis(string d, Panic p) : Panic(d), inner(p) {}
};

#endif //TM_LIGHT_ERROR_H
