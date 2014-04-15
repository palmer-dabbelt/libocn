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

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace libocn {
    class node;
    typedef std::shared_ptr<node> node_ptr_t;
}

#include "path.h++"

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
         * this node.  Note that there's a valid bit here to avoid
         * having to search too often: the rule is that the graph is
         * searched whenever */
        bool _paths_valid;
        std::unordered_map<std::string, std::shared_ptr<path>> _paths;

        /* This stores the list of neighbors of this node, which is
         * used for the shortest-path algorithm later. */
        std::vector<std::shared_ptr<path>> _neighbors;

        /* Stores a unique ID for this node. */
        size_t _uid;

    public:
        /* Creates a node without any paths, given a name that
         * uniquely identifies that node.  This name will be checked
         * for uniquness at create time. */
        node(const std::string& name);

        /* A node destructor. */
        ~node(void);

        /* Returns the name of a node. */
        const std::string& name(void) const { return _name; }

        /* Returns a unique ID for this node. */
        size_t uid(void) const { return _uid; }

        /* Returns the path that must be taken in order to get from
         * this node to the provided node. */
        const std::shared_ptr<path> search(const std::shared_ptr<node>& that);

        /* Informs this node of a path that it can take in order to
         * reach another node. */
        void add_path(std::shared_ptr<path> p);

        /* Returns a list of every path that this node knows how to
         * connect to. */
        std::vector<std::shared_ptr<path>> paths(void);

        /* Returns a list of the one-hop paths that this node can
         * access. */
        std::vector<std::shared_ptr<path>> neighbors(void) const;

    private:
        /* Checks "_paths" for validity, updating it if it hasn't been
         * updated already. */
        void update_paths(void);
    };
}

#endif
