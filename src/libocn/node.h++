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

#ifndef LIBOCN__NODE_HXX
#define LIBOCN__NODE_HXX

namespace libocn {
    class node;
}

#include "path.h++"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace libocn {
    /* This stores a single node in the network along with every
     * path. */
    class node {
    private:
        /* This stores the name of the node.  If this isn't unique
         * then things are going to go poorly! */
        const std::string _name;

        /* This stores a map that maps a destination node to the path
         * that can be taken in order to reach that destination from
         * this node. */
        std::unordered_map<std::string, std::shared_ptr<path>> _paths;

        /* This stores a map that maps a source node to the path that
         * can be taken in order to reach this node from that
         * source. */
        std::unordered_map<std::string, std::weak_ptr<path>> _inc;

    public:
        /* Creates a node without any paths, given a name that
         * uniquely identifies that node.  You really need these names
         * to be unique! */
        node(const std::string& name);

        /* Returns the name of a node. */
        const std::string& name(void) const { return _name; }

        /* Returns the path that must be taken in order to get from
         * this node to the provided node. */
        const std::shared_ptr<path> search(const std::shared_ptr<node>& that);

        /* Informs this node of a path that it can take in order to
         * reach another node. */
        void add_path(std::shared_ptr<path> p);

        /* Returns a list of every path that this node knows how to
         * connect to. */
        std::vector<std::shared_ptr<path>> paths(void) const;
    };
}

#endif
