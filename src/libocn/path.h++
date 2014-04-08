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

#ifndef LIBOCN__PATH_HXX
#define LIBOCN__PATH_HXX

namespace libocn {
    class path;
}

#include "node.h++"
#include <memory>
#include <vector>

namespace libocn {
    /* Stores a single path between two nodes.  Note that all paths
     * are uni-directional.  It's also important to note that all node
     * references are weak pointers here to avoid circular
     * references. */
    class path {
    private:
        std::weak_ptr<node> _s;
        std::weak_ptr<node> _d;
        double _cost;
        std::vector<std::weak_ptr<node>> _steps;

    public:
        /* This creates a direct path between two nodes (a source and
         * a destination) with a cost of 1. */
        path(std::shared_ptr<node>& source, std::shared_ptr<node>& dest);

        /* Creates a path explicitly given all its parameters. */
        path(std::shared_ptr<node> source, std::shared_ptr<node> dest,
             const std::vector<std::shared_ptr<node>>& steps, double cost);

        /* Accessor functions. */
        std::shared_ptr<node> d(void) const { return _d.lock(); }
        std::shared_ptr<node> s(void) const { return _s.lock(); }
        std::vector<std::shared_ptr<node>> steps(void) const;
        double cost(void) const { return _cost; }

        /* Concatenates this path with another path, producing a new
         * one. */
        std::shared_ptr<path> cat(const std::shared_ptr<path>& that);

        /* Returns TRUE if this is a direct path, which means it has
         * only a single neighbor. */
        bool is_direct(void) const { return _steps.size() == 0; }
    };
}

#endif
