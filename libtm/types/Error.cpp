//
// Created by utnso on 16/07/18.
//


#include "Error.h"

const string &Error::getDescription() const {
    return description;
}

Error::Error(const string _description, const char *_file, int _line) : description(_description), file(_file),
                                                                        line(_line) {
    cout << description << "file:" << file << ", line:" << line;
}

const string MyException::getDesc() const {
    return description;
}

Panic::Panic(string d, const char *_file, int _line) : MyException(d, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

/*Panic::Panic(string d, const char *_file, int _line, int n...) : MyException (d, _file, _line) {
    //FIXME
}*/

ErrorInvalidSignature::ErrorInvalidSignature(string desc, const char *_file, int _line) : Error(
        "Error: Invalid Signature for " + desc, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrInvalidProposalPolRound::ErrInvalidProposalPolRound(const char *_file, int _line) : Error(
        "ErrInvalidProposalPolRound", _file, _line) { cout << description << "file:" << file << ", line:" << line; }

ErrInvalidProposalPolRound::ErrInvalidProposalPolRound(string desc, const char *_file, int _line) : Error(desc, _file,
                                                                                                          _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrAddingVote::ErrAddingVote(string desc, const char *_file, int _line) : Error(desc, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrAddingVote::ErrAddingVote(const char *_file, int _line) : Error("ErrAddingVote", _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrVoteHeightMismatch::ErrVoteHeightMismatch(string desc, const char *_file, int _line) : Error(desc, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrVoteHeightMismatch::ErrVoteHeightMismatch(const char *_file, int _line) : Error("ErrVoteHeightMismatch", _file,
                                                                                   _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrVoteConflictingVotes::ErrVoteConflictingVotes(string desc, const char *_file, int _line) : Error(desc, _file,
                                                                                                    _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrVoteConflictingVotes::ErrVoteConflictingVotes(const char *_file, int _line) : Error("ErrVoteConflictingVotes", _file,
                                                                                       _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrInvalidValidatorAddress::ErrInvalidValidatorAddress(string desc, const char *_file, int _line) : Error(desc, _file,
                                                                                                          _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrInvalidValidatorAddress::ErrInvalidValidatorAddress(const char *_file, int _line) : Error(
        "ErrInvalidValidatorAddress", _file, _line) { cout << description << "file:" << file << ", line:" << line; }

MyException::MyException(string _desc, const char *_file, int _line) : description(_desc), file(_file), line(_line) {
    cout << description << "file:" << file << ", line:" << line;
}

TransitionException::TransitionException(string _desc, const char *_file, int _line) : MyException(_desc, _file,
                                                                                                   _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ExceptionInvalidBlock::ExceptionInvalidBlock(string _desc, const char *_file, int _line) : MyException(_desc, _file,
                                                                                                       _line) {
    cout << description << "file:" << file << ", line:" << line;
}

SignError::SignError(string _desc, const char *_file, int _line) : Error(_desc, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ExceptionInvalidPolRound::ExceptionInvalidPolRound(int expected, int found, const char *_file, int _line) : MyException(
        "This POLRound should be " + std::to_string(expected) + " but got " + std::to_string(found),
        _file, _line) { cout << description << "file:" << file << ", line:" << line; }

ErrorUnexpectedVoteType::ErrorUnexpectedVoteType(const char *_file, int _line) : Error("ErrorUnexpectedVoteType", _file,
                                                                                       _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrorUnexpectedVoteType::ErrorUnexpectedVoteType(std::string s, const char *_file, int _line) : Error(
        "ErrorUnexpectedVoteType: " + string(s), _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrInvalidVoteSet::ErrInvalidVoteSet(string desc, const char *_file, int _line) : Error("ErrInvalidVoteSet " + desc,
                                                                                        _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

ErrInvalidVote::ErrInvalidVote(string desc, const char *_file, int _line) : Error("ErrInvalidVote" + desc, _file,
                                                                                  _line) {
    cout << description << "file:" << file << ", line:" << line;
}

PanicNullVoteSet::PanicNullVoteSet(const char *_file, int _line) : Panic("AddVote() on nil VoteSet", _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

PanicNoPolka::PanicNoPolka(string d, const char *_file, int _line) : Panic(
        d + "commit does not have two thirds majority", _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

PanicInvalidHash::PanicInvalidHash(string d, const char *_file, int _line) : Panic(d, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

PanicEnterRound::PanicEnterRound(string d, const char *_file, int _line) : Panic(d, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}

PanicCrisis::PanicCrisis(string d, Panic p, const char *_file, int _line) : Panic(d, _file, _line), inner(p) {
    cout << description << "file:" << file << ", line:" << line;
}

PanicSanity::PanicSanity(string d, const char *_file, int _line) : Panic(d, _file, _line) {
    cout << description << "file:" << file << ", line:" << line;
}
