/**
 * @author Matthew Guidry (github: mguid65)
 * @date 2024-11-03
 *
 * @cond IGNORE_LICENSE
 *
 * MIT License
 *
 * Copyright (c) 2024 Matthew Guidry
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @endcond
 */

#ifndef MGUID_TRIE_TRIE_H
#define MGUID_TRIE_TRIE_H

#include <algorithm>
#include <concepts>
#include <optional>
#include <ranges>
#include <string_view>
#include <vector>
#include <iostream>
#include <iterator>
#include <stack>
#include <utility>
#include <tuple>

namespace mguid {

template <typename PrefixEltType, typename ValueType>
struct TrieNode {
  using TrieNodeChildren = std::vector<TrieNode<PrefixEltType, ValueType>>;
  PrefixEltType character;
  std::optional<TrieNodeChildren> opt_children{std::nullopt};
  std::optional<ValueType> opt_value{std::nullopt};

  TrieNode(PrefixEltType ch) : character(ch) {}
};

template <typename T, typename TValueType>
concept RangeOf = std::ranges::range<T> &&
                  std::same_as<TValueType, std::ranges::range_value_t<T>>;

template <typename PrefixEltType, typename ValueType>
class Trie {
public:
  using TrieNodeChildren = typename TrieNode<PrefixEltType, ValueType>::TrieNodeChildren;
  using KeyType = std::basic_string<PrefixEltType>;

  Trie() : m_root('\0'), m_size(0) {}

  // Element access
  template <RangeOf<PrefixEltType> KeyType>
  ValueType& operator[](KeyType&& key) {
    return find_or_insert(std::forward<KeyType>(key));
  }

  ValueType& operator[](std::string_view key) {
    return find_or_insert(key);
  }

  ValueType& at(const std::string_view key) {
    auto* node = find_node(key);
    if (!node || !node->opt_value) throw std::out_of_range("Key not found in Trie");
    return *node->opt_value;
  }

  std::pair<ValueType*, bool> insert(std::pair<std::basic_string<PrefixEltType>, ValueType> kv) {
    TrieNode<PrefixEltType, ValueType>* node = &m_root;
    for (const auto& ch : kv.first) {
      if (!node->opt_children) node->opt_children = TrieNodeChildren{};
      auto& children = *node->opt_children;
      auto it = std::find_if(children.begin(), children.end(),
                             [&](const TrieNode<PrefixEltType, ValueType>& child) { return child.character == ch; });
      if (it == children.end()) {
        children.emplace_back(ch);
        node = &children.back();
      } else {
        node = &(*it);
      }
    }
    if (!node->opt_value) {
      node->opt_value = kv.second;
      ++m_size;
      return {&*node->opt_value, true};
    }
    return {&*node->opt_value, false};
  }

  // Lookup
  [[nodiscard]] bool contains(const std::string_view key) const {
    return find_node(key) != nullptr;
  }

  ValueType* find(const std::string_view key) {
    auto* node = find_node(key);
    return (node && node->opt_value) ? &*node->opt_value : nullptr;
  }

  // Erase
  bool erase(const std::string_view key) {
    return erase_recursive(m_root, key, 0);
  }

  // Size and Capacity
  std::size_t size() const {
    return m_size;
  }

  bool empty() const {
    return m_size == 0;
  }

  void clear() {
    m_root.opt_children.reset();
    m_size = 0;
  }

  // Iterators (example of in-order traversal)
  // Iterator class for in-order traversal
  struct iterator {
    using value_type = std::pair<KeyType, ValueType>;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iterator() = default;

    iterator(TrieNode<PrefixEltType, ValueType>* root) {
      if (root) {
        stack.emplace(root, KeyType{});
        advance();
      }
    }

    value_type& operator*() {
      return current_pair;
    }

    value_type* operator->() {
      return &current_pair;
    }

    iterator& operator++() {
      advance();
      return *this;
    }

    iterator operator++(int) {
      iterator temp = *this;
      ++(*this);
      return temp;
    }

    bool operator==(const iterator& other) const {
      return stack == other.stack;
    }

    bool operator!=(const iterator& other) const {
      return !(*this == other);
    }

  private:
    std::stack<std::pair<TrieNode<PrefixEltType, ValueType>*, KeyType>> stack;
    value_type current_pair;

    void advance() {
      while (!stack.empty()) {
        auto [node, prefix] = stack.top();
        stack.pop();

        // Check if the node has a value; if so, yield it
        if (node->opt_value) {
          current_pair = {prefix, *node->opt_value};
          // Push the node's children onto the stack in reverse order to ensure in-order traversal
          if (node->opt_children) {
            for (auto it = node->opt_children->rbegin(); it != node->opt_children->rend(); ++it) {
              stack.emplace(&*it, prefix + it->character);
            }
          }
          return;
        }

        // Process child nodes if no value is found at this level
        if (node->opt_children) {
          for (auto it = node->opt_children->rbegin(); it != node->opt_children->rend(); ++it) {
            stack.emplace(&*it, prefix + it->character);
          }
        }
      }
      // If we exit the loop, the iterator has reached the end
      stack = {};
    }
  };

  iterator begin() {
    // Initialize the iterator to the first element in in-order traversal
    // Needs to be implemented
    return iterator();
  }

  iterator end() {
    // Represents the end of the in-order traversal
    // Needs to be implemented
    return iterator();
  }

private:
  TrieNode<PrefixEltType, ValueType> m_root;
  std::size_t m_size;

  // Helper function for insertion and access
  template <RangeOf<PrefixEltType> KeyType>
  ValueType& find_or_insert(KeyType&& key) {
    TrieNode<PrefixEltType, ValueType>* current_node = &m_root;
    for (const auto& elt : key) {
      if (!current_node->opt_children) current_node->opt_children = TrieNodeChildren{};
      auto& children = *current_node->opt_children;
      auto it = std::find_if(children.begin(), children.end(), [&](const TrieNode<PrefixEltType, ValueType>& child) {
        return child.character == elt;
      });
      if (it == children.end()) {
        children.emplace_back(elt);
        current_node = &children.back();
      } else {
        current_node = &(*it);
      }
    }
    if (!current_node->opt_value) {
      current_node->opt_value = ValueType{};
      ++m_size;
    }
    return *current_node->opt_value;
  }

  // Helper function to find a node by key
  TrieNode<PrefixEltType, ValueType>* find_node(std::string_view key) const {
    const TrieNode<PrefixEltType, ValueType>* current_node = &m_root;
    for (const auto& ch : key) {
      if (!current_node->opt_children) return nullptr;
      auto& children = *current_node->opt_children;
      auto it = std::find_if(children.begin(), children.end(), [&](const TrieNode<PrefixEltType, ValueType>& child) {
        return child.character == ch;
      });
      if (it == children.end()) return nullptr;
      current_node = &*it;
    }
    return const_cast<TrieNode<PrefixEltType, ValueType>*>(current_node);
  }

  // Recursive helper function for erase
  bool erase_recursive(TrieNode<PrefixEltType, ValueType>& node, const std::string_view key, std::size_t depth) {
    if (depth == key.size()) {
      if (node.opt_value) {
        node.opt_value.reset();
        --m_size;
        return true;
      }
      return false;
    }
    if (!node.opt_children) return false;
    auto& children = *node.opt_children;
    auto it = std::find_if(children.begin(), children.end(), [&](const TrieNode<PrefixEltType, ValueType>& child) {
      return child.character == key[depth];
    });
    if (it != children.end() && erase_recursive(*it, key, depth + 1)) {
      if (it->opt_children && it->opt_children->empty() && !it->opt_value) {
        children.erase(it); // Clean up empty nodes
      }
      return true;
    }
    return false;
  }
};

} // namespace mguid

#endif // MGUID_TRIE_TRIE_H
