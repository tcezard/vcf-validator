/**
 * Copyright 2016 EMBL - European Bioinformatics Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STREAM_UTILS_HPP
#define STREAM_UTILS_HPP

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <functional>

namespace ebi
{
  namespace util
  {
    template <typename Container>
    std::istream & readline(std::istream & stream, Container & container)
    {
        char c;
        container.clear();

        // using operator bool: http://www.cplusplus.com/reference/ios/ios/fail/
        // `stream.get()` sets failbit as well on eof
        while (stream && stream.get(c)) {
            container.push_back(c);
            if (c == '\n') {
                break;
            }
        }

        return stream;
    }

    template <typename Container>
    std::ostream & writeline(std::ostream & stream, const Container & container)
    {
        for (auto c : container) {
            stream << c;
        }
        return stream;
    }

    template <typename F, typename S>
    std::ostream &operator<<(std::ostream &os, const std::pair<F, S> &container)
    {
        return os << "{" << container.first << ", " << container.second << "}";
    }

    /**
     * This is a generic method that prints a collection if its inner type is printable with "operator<<"
     *
     * @param os ostream to write into
     * @param container must have he operations ".begin()" and ".end()"
     * defined and they provide iterators, and the inner type has an overloaded "operator<<"
     * @param open_tag will be written before the first element (e.g. "[", if you want something like "[2,4]"), can be an empty string
     * @param separator_tag will be written between elements (e.g. "," in "[2,4]"), can be empty string
     * @param close_tag will be written after the last element (e.g. "]" in "[2,4]"), can be emtpy string
     * @return the ostream passed, in the questionable case you want to do `print_containter(cout, ...) << "more messages"`;
     */
    template <typename T>
    std::ostream &print_container(std::ostream &os, const T &container,
                                  std::string open_tag, std::string separator_tag, std::string close_tag) {
        os << open_tag;
        auto it = container.begin();
        auto end = container.end();
        if (it != end) {
            os << *it;
            for (++it; it != container.end(); ++it) {
                os << separator_tag << *it;
            }
        }
        os << close_tag;
        return os;
    }

    /**
     * This is a generic method that prints a collection if it has the operations ".begin()" and ".end()"
     * defined and they provide iterators, and the inner type has an overloaded "operator<<"
     *
     * provides the default open/close/separator tags to print collections like this: [2,4]
     */
    template <typename T>
    std::ostream &print_container(std::ostream &os, const T &container) {
        return print_container(os, container, "[", ", ", "]");
    }
    
    // the next functions allow using the previous print_container in a easier syntax
    template <typename T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> &container)
    {
        return print_container<std::vector<T>>(os, container);
    }
    template <typename K, typename V>
    std::ostream &operator<<(std::ostream &os, const std::map<K, V> &container)
    {
        return print_container<std::map<K, V>>(os, container);
    }
  }
}

#endif  /* STREAM_UTILS_HPP */

