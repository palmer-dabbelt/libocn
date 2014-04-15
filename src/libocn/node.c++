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
#include <stdlib.h>
#include <stack>
using namespace libocn;

/* This consists of the UID management code, which deals with creating
 * a new UID for every node that's created by the system.  Note that
 * these UIDs are keyed based on the node's string name, */
static size_t get_uid(const std::string& name);
static void put_uid(const std::string& name);
static std::unordered_map<std::string, size_t> uid_map;

node::node(const std::string& name)
    : _name(name),
      _paths_valid(true),
      _paths(),
      _uid(get_uid(name))
{
}

node::~node(void)
{
    put_uid(name());
}

const std::shared_ptr<path> node::search(const std::shared_ptr<node>& that)
{
    update_paths();

    auto l = this->_paths.find(that->name());
    if (l == this->_paths.end())
        return NULL;

    return l->second;
}

void node::add_path(std::shared_ptr<path> new_path)
{
    /* Check if there was an old path to this target.  If so we're
     * going to need to replace the path, but only if it's worse than
     * the new path. */
    auto old_path_l = _paths.find(new_path->d()->name());
    std::shared_ptr<path> old_path = NULL;
    if (old_path_l != _paths.end())
        old_path = old_path_l->second;

    /* If there was an old path and it's better then do nothing. */
    if ((old_path != NULL) && (old_path->cost() <= new_path->cost()))
        return;

    _paths[new_path->d()->name()] = new_path;
    _paths_valid = false;

    if (new_path->is_direct() == true)
        _neighbors.push_back(new_path);
}

std::vector<std::shared_ptr<path>> node::paths(void)
{
    update_paths();

    std::vector<std::shared_ptr<path>> out;

    for (const auto& pair : _paths)
        out.push_back(pair.second);

    return out;
}

std::vector<std::shared_ptr<path>> node::neighbors(void) const
{
    return _neighbors;
}

void node::update_paths(void)
{
    /* If we haven't already cached the answer then attempt to refresh
     * it right now.  Be sure not to call "paths()" here, as it'll
     * start recursing forever! */
    if (this->_paths_valid == true)
        return;

    /* Here we create a stack that contains all the new paths that
     * have been discovered as the result of this addition. */
    std::stack<std::shared_ptr<path>> paths;
    for (const auto& pair : _paths)
        paths.push(pair.second);

    /* Start out without any paths in this node, none of them are
     * actually valid any more. */
    _paths.clear();

    /* Now we just iterate until there's no new paths to be
     * discovered.  This is Dijkstra's Algorithm. */
    while (paths.size() != 0) {
        auto new_path = paths.top(); paths.pop();

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

        /* If this path is circular then justs abandon it here. */
        if (new_path->s() == new_path->d())
            continue;

        /* Now that we know that this path is useful we can add it to
         * our list current path list. */
        _paths[new_path->d()->name()] = new_path;

        /* Add every newly-discovered path to the queue for
         * processing. */
        for (const auto& second_half : new_path->d()->_neighbors) {
            paths.push(new_path->cat(second_half));
        }
    }

    /* Now we can set that flag so this never gots called again. */
    this->_paths_valid = true;
}

size_t get_uid(const std::string& name)
{
    static size_t uid = 1;

    auto l = uid_map.find(name);
    if (l != uid_map.end()) {
        fprintf(stderr, "re-mapped UID: '%s' -> %lu\n",
                l->first.c_str(),
                l->second
            );
        abort();
    }

    if (uid == 0) {
        fprintf(stderr, "UID wraparound\n");
        abort();
    }

    uid++;
    uid_map[name] = uid;
    return uid;
}

void put_uid(const std::string& name)
{
    auto l = uid_map.find(name);
    if (l == uid_map.end()) {
        fprintf(stderr, "uid_put for unmapped name: '%s'\n",
                name.c_str()
            );
        abort();
    }

    uid_map.erase(name);
}
