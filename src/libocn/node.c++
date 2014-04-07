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

#include "node.h++"
#include <stack>
using namespace libocn;

node::node(const std::string& name)
    : _name(name),
      _paths(),
      _inc()
{
}

const std::shared_ptr<path> node::search(const std::shared_ptr<node>& that)
{
    auto l = this->_paths.find(that->name());
    if (l == this->_paths.end())
        return NULL;

    return l->second;
}

void node::add_path(std::shared_ptr<path> new_path)
{
    /* Here we create a stack that contains all the new paths that
     * have been discovered as the result of this addition. */
    std::stack<std::shared_ptr<path>> paths;
    paths.push(new_path);

    while (paths.size() != 0) {
        auto new_path = paths.top(); paths.pop();

        /* If our path is just circular then bail out right now. */
        if (new_path->d()->name() == name())
            continue;

        /* Check if there was an old path to this node.  If so we're
         * going to need to replace the path and re-compute
         * everything. */
        auto old_path_l = _paths.find(new_path->d()->name());
        std::shared_ptr<path> old_path;
        if (old_path_l != _paths.end())
            old_path = old_path_l->second;

        /* If the old path exists and is cheaper than the new path we
         * just added then don't bother with it at all, it's strictly
         * worse than whatever we have now. */
        if ((old_path != NULL) && (old_path->cost() <= new_path->cost()))
            continue;

        /* Now that we know that this path is useful we can add it to
         * our list current path list. */
        _paths[new_path->d()->name()] = new_path;
        new_path->d()->_inc[new_path->s()->name()] = new_path;

        /* Add every newly-discovered path to the queue for
         * processing. */
        for (const auto& second_half : new_path->d()->paths()) {
            paths.push(new_path->cat(second_half));
        }
    }

    /* Now that we've updated the reachable paths from this node, go
     * ahead and update every node that can reach us. */
    for (const auto& first_half : _inc) {
        for (const auto& second_half : _paths) {
            auto first_path = first_half.second.lock();
            auto second_path = second_half.second;
            auto new_path = first_path->cat(second_path);

            if (new_path->s()->name() == new_path->d()->name())
                continue;

            auto old_path = new_path->s()->search(new_path->d());
            if ((old_path == NULL) || (old_path->cost() > new_path->cost()))
                new_path->s()->add_path(new_path);
        }
    }
}

std::vector<std::shared_ptr<path>> node::paths(void) const
{
    std::vector<std::shared_ptr<path>> out;

    for (const auto& pair : _paths)
        out.push_back(pair.second);

    return out;
}
