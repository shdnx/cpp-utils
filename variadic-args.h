// GCC 9.2, with -std=c++17 -O2
#pragma once

#include <type_traits>
#include <utility>

namespace args {

struct NoSuchType {};

template <typename... Ts>
struct Variadic;

template <typename T, typename... Ts>
struct Variadic<T, Ts...> : Variadic<Ts...> {
    T&& mValue;

    constexpr /*implicit*/ Variadic(T&& t, Ts&&... ts)
        :   mValue{std::forward<T>(t)},
            Variadic<Ts...>{std::forward<Ts>(ts)...}
    {}

    template <typename U, typename F>
    constexpr auto with(F&& f) const {
        using CleanT = std::remove_reference_t<std::remove_cv_t<T>>;

        if constexpr (std::is_same_v<CleanT, U>)
            return std::forward<F>(f)(mValue);
        else
            return Variadic<Ts...>::template with<U>(std::forward<F>(f));
    }

    template <typename U>
    constexpr auto tryGet() const {
        return with<U>([](auto&& x) { return x; });
    }

    template <typename U>
    constexpr bool has() const {
        return !std::is_same_v<decltype(tryGet<U>()), NoSuchType>;
    }

    template <typename U>
    constexpr auto get() const {
        // TODO: weird, we have to inline has() here, otherwise we get weird errors
        static_assert(!std::is_same_v<decltype(tryGet<U>()), NoSuchType>, "No argument with this type");
        return tryGet<U>();
    }
};

template <>
struct Variadic<> {
    constexpr /*implicit*/ Variadic() {}

    template <typename U, typename F>
    constexpr auto with(F&&) const {
        return NoSuchType{};
    }

    template <typename U>
    constexpr auto tryGet() const {
        return NoSuchType{};
    }

    template <typename U>
    constexpr bool has() const {
        return false;
    }

    template <typename U>
    constexpr auto get() const = delete;
};

template <typename... Ts>
Variadic(Ts&&...) -> Variadic<Ts...>;

template <typename U, typename... Ts>
constexpr auto get(const Variadic<Ts...>& args) {
    return args.template get<U>();
}

template <typename U, typename... Ts>
constexpr auto has(const Variadic<Ts...>& args) {
    return args.template has<U>();
}

template <typename U, typename F, typename... Ts>
constexpr auto with(const Variadic<Ts...>& args, F&& f) {
    return args.template with<U>(std::forward<F>(f));
}

} // end namespace args
