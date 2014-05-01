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
    template<class node_t> class path;
}

#include "node.h++"
#include <memory>
#include <vector>

namespace libocn {
    /* Stores a single path between two nodes.  Note that all paths
     * are uni-directional.  It's also important to note that all node
     * references are weak pointers here to avoid circular
     * references. */
    template<class node_t>
    class path {
        typedef std::weak_ptr<node_t> weak_node_ptr;
        typedef std::shared_ptr<node_t> node_ptr;
        typedef std::shared_ptr<path<node_t>> path_ptr;
        typedef path<node_t> path_t;

    private:
        weak_node_ptr _s;
        weak_node_ptr _d;
        size_t _cost;
        std::vector<weak_node_ptr> _steps;

    public:
        /* This creates a direct path between two nodes (a source and
         * a destination) with a cost of 1. */
        path(const node_ptr& source, const node_ptr& dest, size_t cost = 1)
            : _s(source),
              _d(dest),
              _cost(cost),
              _steps()
            {
            }

        /* Creates a path explicitly given all its parameters. */
        path(const node_ptr& source, const node_ptr& dest,
             const std::vector<node_ptr>& steps, size_t cost)
            : _s(source),
              _d(dest),
              _cost(cost),
              _steps(weaken(steps))
            {
            }

        /* Accessor functions. */
        node_ptr d(void) const { return _d.lock(); }
        node_ptr s(void) const { return _s.lock(); }
        std::vector<node_ptr> steps(void) const
            {
                std::vector<node_ptr> steps;

                steps.push_back(s());

                for (const auto& step : _steps)
                    steps.push_back(step.lock());

                steps.push_back(d());

                return steps;
            }
        size_t cost(void) const { return _cost; }

        /* Concatenates this path with another path, producing a new
         * one. */
        path_ptr cat(const path_ptr& that)
            {
                /* Concatonate the two step lists into a single large
                 * one. */
                std::vector<node_ptr> step_list;
                for (const auto& step : this->_steps)
                    step_list.push_back(step.lock());
                step_list.push_back(this->d());
                for (const auto& step : that->_steps)
                    step_list.push_back(step.lock());

                return std::make_shared<path_t>(this->s(),
                                                that->d(),
                                                step_list,
                                                this->cost() + that->cost());
            }

        /* Returns TRUE if this is a direct path, which means it has
         * only a single neighbor. */
        bool is_direct(void) const { return _steps.size() == 0; }

    private:
        /* Converts an array of shared pointers into an array of weak
         * pointers. */
        static std::vector<weak_node_ptr>
        weaken(const std::vector<node_ptr>& strong)
            {
                std::vector<weak_node_ptr> out;

                for (const auto& ptr : strong)
                    out.push_back(ptr);

                return out;
            }
    };
}

#endif
