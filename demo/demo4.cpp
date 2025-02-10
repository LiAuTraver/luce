#include <iostream>
#include <map>
using type_id_t = void (*)(void);

template <typename T> void typeId(void) {}

std::map<type_id_t, const char *> typeNames;
class A;
int main() {
  typeNames[typeId<int>] = "int";
  typeNames[typeId<float>] = "float";
  typeNames[typeId<double>] = "double";
  typeNames[typeId<char>] = "char";
  typeNames[typeId<A>] = "A";

  for (const auto &[type, name] : typeNames) {
    std::cout << name << '\n';
  }

  auto it = typeNames.find(typeId<int>);
  if (it != typeNames.end()) {
    std::cout << it->second << '\n';
  }
  return 0;
}