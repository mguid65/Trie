#include "Trie/Trie.hpp"

#include <catch2/catch_all.hpp>

#include <sstream>

TEST_CASE("Trie Construction") {
  SECTION("Default") {
    mguid::Trie<char, int>{};
  }
}