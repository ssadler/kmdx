//
// Created by utnso on 16/07/18.
//

#ifndef TM_LIGHT_ERROR_H
#define TM_LIGHT_ERROR_H

#include <string>
#include "Vote.h"

using namespace std;

class Error : public std::exception {
protected:
    string description;
    const char *file;
    int line;

public:
    Error(const string _description, const char *_file, int _line);;

    const string &getDescription() const;
};

class ErrorInvalidSignature : public Error {
public:
    ErrorInvalidSignature(string desc, const char *_file, int _line);
};

class ErrInvalidProposalPolRound : public Error {
public:
    ErrInvalidProposalPolRound(string desc, const char *_file, int _line);

    ErrInvalidProposalPolRound(const char *_file, int _line);
};

class ErrAddingVote : public Error {
public:
    ErrAddingVote(string desc, const char *_file, int _line);

    ErrAddingVote(const char *_file, int _line);
};

class ErrVoteHeightMismatch : public Error {
public:
    ErrVoteHeightMismatch(string desc, const char *_file, int _line);

    ErrVoteHeightMismatch(const char *_file, int _line);
};

class ErrVoteConflictingVotes : public Error {
public:
    ErrVoteConflictingVotes(string desc, const char *_file, int _line);

    ErrVoteConflictingVotes(const char *_file, int _line);
};


class ErrInvalidValidatorAddress : public Error {
public:
    ErrInvalidValidatorAddress(string desc, const char *_file, int _line);

    ErrInvalidValidatorAddress(const char *_file, int _line);
};

class ErrInvalidCommit : public Error {
public:
    ErrInvalidCommit(string desc, const char *_file, int _line);
};

class MyException : public std::exception {
protected:
    string description;
    const char *file;
    int line;
public:
    MyException(string _desc, const char *_file, int _line);

    const string getDesc() const;
};

class TransitionException : public MyException {
public:
    TransitionException(string _desc, const char *_file, int _line);

};

class ExceptionInvalidBlock : public MyException {

public:
    ExceptionInvalidBlock(string _desc, const char *_file, int _line);

};


class SignError : public Error {
public:
    SignError(string _desc, const char *_file, int _line);
};


class ExceptionInvalidPolRound : public MyException {
public:
    ExceptionInvalidPolRound(int expected, int found, const char *_file, int _line);

};

class ErrorUnexpectedVoteType : public Error {
public:
//    ErrorUnexpectedVoteType(VoteType type) : Error("ErrorUnexpectedVoteType");
    ErrorUnexpectedVoteType(const char *_file, int _line);

    ErrorUnexpectedVoteType(std::string s, const char *_file, int _line);


};

class ErrInvalidVoteSet : public Error {
public:
    ErrInvalidVoteSet(string desc, const char *_file, int _line);

};

class ErrInvalidVote : public Error {
public:
    ErrInvalidVote(string desc, const char *_file, int _line);

};

class ErrorGotVoteFromUnwantedRound : public Error {
public:
    ErrorGotVoteFromUnwantedRound(string desc, const char *_file, int _line);
    ErrorGotVoteFromUnwantedRound(const char *_file, int _line);

};


class Panic : public MyException {
public:
    Panic(string d, const char *_file, int _line);

    Panic(string d, const char *_file, int _line, int n...);
};

class PanicNullVoteSet : public Panic {
public:
    PanicNullVoteSet(const char *_file, int _line);
};

class PanicNoPolka : public Panic {
public:
    PanicNoPolka(string d, const char *_file, int _line);
};

class PanicInvalidHash : public Panic {
public:
    PanicInvalidHash(string d, const char *_file, int _line);
};

class PanicEnterRound : public Panic {
public:
    PanicEnterRound(string d, const char *_file, int _line);
};

class PanicCrisis : public Panic {
    Panic inner;
public:
    PanicCrisis(string d, Panic p, const char *_file, int _line);
};

class PanicSanity : public Panic {
public:
    PanicSanity(string d, const char *_file, int _line);
};

#endif //TM_LIGHT_ERROR_H
