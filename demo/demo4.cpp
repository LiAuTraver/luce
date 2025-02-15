// #include <array>
// #include <iostream>
// #include <map>
// using type_id_t = void (*)(void);

// template <typename T> void typeId(void) {}

// std::map<type_id_t, const char *> typeNames;
// class A;
// int test() {
//   typeNames[typeId<int>] = "int";
//   typeNames[typeId<float>] = "float";
//   typeNames[typeId<double>] = "double";
//   typeNames[typeId<char>] = "char";
//   typeNames[typeId<A>] = "A";

//   for (const auto &[type, name] : typeNames) {
//     std::cout << name << '\n';
//   }

//   auto it = typeNames.find(typeId<int>);
//   if (it != typeNames.end()) {
//     std::cout << it->second << '\n';
//   }
//   return 0;
// }


// #include "rocket.hpp"
// #include <iostream>
// #include <iostream>

// int test2()
// {
//     rocket::signal<void()> thread_unsafe_signal;
//     rocket::thread_safe_signal<void()> thread_safe_signal;

//     // Connecting the first handler to our signal
//     thread_unsafe_signal.connect([]() {
//         std::cout << "First handler called!" << std::endl;
//     });
    
//     // Connecting a second handler to our signal using alternative syntax
//     thread_unsafe_signal += []() {
//         std::cout << "Second handler called!" << std::endl;
//     };
    
//     // Invoking the signal
//     thread_unsafe_signal();
//     return 0;
// }

// // Output:
// //     First handler called!
// //     Second handler called!
#include <iostream>
#include <ranges>
#include <string>
int main() {

}