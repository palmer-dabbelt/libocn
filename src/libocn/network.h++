/*
 * Copyright (C) 2014 Palmer Dabbelt
 *   <palmer.dabbelt@eecs.berkeley.edu>
 *
 * This file is part of libocn.
 *
 * libocn is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libocn is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libocn.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef LIBOCN__NETWORK_HXX
#define LIBOCN__NETWORK_HXX

#include "node.h++"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace libocn {
    /* This is the top-level OCN class: it stores a single network.
     * Note that while there's a lot of shared pointers inside here I
     * need to use some weak pointers to avoid circular references
     * between nodes -- essentially what this means is that if you're
     * doing any computation on the network you want to keep around a
     * reference to that network. */
    class network {
    private:
        /* This stores the list of every node in the network. */
        std::unordered_map<std::string, std::shared_ptr<node>> _nodes;

    public:
        /* This constructor will probably only be useful if you're a
         * subclass of a network that aims to avoid parsing
         * configuration files. */
        network(const std::vector<std::shared_ptr<node>>& nodes);

        /* Returns a list that contains every node in this network. */
        std::vector<std::shared_ptr<node>> nodes(void) const;
    };
}

#endif
