#include "variadic-args.h"

#include <type_traits>

template <typename T>
void print(const char* label, const T& value);
void print(const char* str);

struct A { int mValue; };
struct B { double mValue; };
struct C { const char* mValue; };

template <typename... Ts>
void variadicFunc(Ts&&... values) {
    const args::Variadic varArgs = { std::forward<Ts>(values)... };

    print("A", args::get<A>(varArgs));
    print("B", varArgs.template get<B>());

    if constexpr (args::has<C>(varArgs)) {
        print("C", args::get<C>(varArgs));
    } else {
        print("There's no C");
    }

    args::with<C>(varArgs, [](C& c) {
        print("C", c);
    });
}

int main() {
    B b = { 17.58 };
    C c = { "foobar" };

    variadicFunc(b, A{79}, c);

    return 0;
}

#include <iostream>

void print(const char* label, const A& a) {
   std::cerr << "[" << label << "] A{" << a.mValue << "}\n";
}

void print(const char* label, const B& b) {
    std::cerr << "[" << label << "] B{" << b.mValue << "}\n";
}

void print(const char* label, const C& c) {
    std::cerr << "[" << label << "] C{" << c.mValue << "}\n";
}

void print(const char* str) {
    std::cerr << str << "\n";
}
