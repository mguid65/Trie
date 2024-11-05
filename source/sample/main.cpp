#include <Trie/Trie.hpp>

int main() {
  mguid::Trie<char, int> trie{};
  trie["applesauce"] = 5;

  std::cout << trie["applesauce"] << '\n';
}
