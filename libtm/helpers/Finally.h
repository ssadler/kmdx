//
// Created by utnso on 19/07/18.
//

#ifndef TM_LIGHT_FINALLY_H
#define TM_LIGHT_FINALLY_H

#include <utility>

template<typename Functor>
struct finally_guard {
    finally_guard(Functor f)
            : functor(std::move(f)), active(true) {}

    finally_guard(finally_guard &&other)
            : functor(std::move(other.functor)), active(other.active) { other.active = false; }

    finally_guard &operator=(finally_guard &&) = delete;

    ~finally_guard() {
        if (active)
            functor();
    }

    Functor functor;
    bool active;
};

template<typename F>
finally_guard<typename std::decay<F>::type>
finally(F &&f) {
    return {std::forward<F>(f)};
}

#endif //TM_LIGHT_FINALLY_H
