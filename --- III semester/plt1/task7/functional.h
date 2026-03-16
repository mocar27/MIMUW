#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <functional>
#include <tuple>
#include "functional.h"

template<typename F, typename... Fs>
auto compose(F f, Fs... fs) {
    return [=](auto x) {
        return compose(fs...)(f(x));
    };
}

template<typename F>
auto compose(F f) {
    return [=](auto x) {
        return f(x);
    };
}

inline auto compose() {
    return [](auto x) { return x; };
}

template<typename H, typename... Fs>
auto lift(H h, Fs... fs) {
    return [=](auto p) {
        return h(fs(p)...);
    };
}

#endif // FUNCTIONAL_H
