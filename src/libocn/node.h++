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

#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "path.h++"
#include "sizet_printf.h++"

namespace libocn {
    /* This stores a single node in the network along with every
     * path. */
    template<class node_t>
    class node {
        typedef std::shared_ptr<node_t> node_ptr;
        typedef std::shared_ptr<path<node_t>> path_ptr;
        typedef path<node_t> path_t;

    private:
        static std::unordered_map<std::string, size_t> name2uid;

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
        std::unordered_map<std::string, path_ptr> _paths;

        /* This stores the list of neighbors of this node, which is
         * used for the shortest-path algorithm later.  Note that this
         * is stored in two directions: one stores the routes that go
         * out of this node, and one the routes that enter this
         * node. */
        std::vector<path_ptr> _incoming_neighbors;
        std::vector<path_ptr> _outgoing_neighbors;

        /* Stores a unique ID for this node. */
        size_t _uid;

    public:
        /* Creates a node without any paths, given a name that
         * uniquely identifies that node.  This name will be checked
         * for uniquness at create time. */
        node(const std::string& name)
            : _name(name),
              _paths_valid(true),
              _paths(),
              _uid(get_uid(name))
            {
            }

        /* A node destructor. */
        ~node(void)
            {
                put_uid(name());
            }

        /* Returns the name of a node. */
        const std::string& name(void) const { return _name; }

        /* Returns a unique ID for this node. */
        size_t uid(void) const { return _uid; }

        /* Returns the path that must be taken in order to get from
         * this node to the provided node. */
        const path_ptr search(const node_ptr& that)
            {
                update_paths();

                auto l = this->_paths.find(that->name());
                if (l == this->_paths.end())
                    return NULL;

                return l->second;
            }

        /* Informs this node of a path that it can take in order to
         * reach another node. */
        void add_path(path_ptr new_path)
            {
                /* Check if there was an old path to this target.  If
                 * so we're going to need to replace the path, but
                 * only if it's worse than the new path. */
                auto old_path_l = _paths.find(new_path->d()->name());
                path_ptr old_path = NULL;
                if (old_path_l != _paths.end())
                    old_path = old_path_l->second;

                /* If there was an old path and it's better then do
                 * nothing. */
                if ((old_path != NULL) && (old_path->cost() <= new_path->cost()))
                    return;

                _paths[new_path->d()->name()] = new_path;
                _paths_valid = false;

                if (new_path->is_direct() == true) {
                    new_path->s()->_outgoing_neighbors.push_back(new_path);
                    new_path->d()->_incoming_neighbors.push_back(new_path);
                }
            }

        /* Returns a list of every path that this node knows how to
         * connect to. */
        std::vector<path_ptr> paths(void)
            {
                update_paths();

                std::vector<path_ptr> out;
                for (const auto& pair : _paths)
                    out.push_back(pair.second);
                return out;
            }

        /* Returns a list of the one-hop paths that this node can
         * access. */
        std::vector<path_ptr> incoming_neighbors(void) const
            {
                return _incoming_neighbors;
            }

        std::vector<path_ptr> outgoing_neighbors(void) const
            {
                return _outgoing_neighbors;
            }

        /* Returns the point number that will be used to connect from
         * this node to a neighboring node. */
        size_t port_number_out(const node_ptr& n) const
            {
                auto p2n = [](path_ptr p) -> node_ptr { return p->d(); };
                return port_number(n, _outgoing_neighbors, p2n);
            }

        size_t port_number_in(const node_ptr& n) const
            {
                auto p2n = [](path_ptr p) -> node_ptr { return p->s(); };
                return port_number(n, _incoming_neighbors, p2n);
            }

    private:
        /* Checks "_paths" for validity, updating it if it hasn't been
         * updated already. */
        void update_paths(void)
            {
                /* If we haven't already cached the answer then
                 * attempt to refresh it right now.  Be sure not to
                 * call "paths()" here, as it'll start recursing
                 * forever! */
                if (this->_paths_valid == true)
                    return;

                /* Here we create a stack that contains all the new
                 * paths that have been discovered as the result of
                 * this addition. */
                std::stack<path_ptr> paths;
                for (const auto& pair : _paths)
                    paths.push(pair.second);

                /* Start out without any paths in this node, none of
                 * them are actually valid any more. */
                _paths.clear();

                /* Now we just iterate until there's no new paths to
                 * be discovered.  This is Dijkstra's Algorithm, though probably very poorly implemented... :). */
                while (paths.size() != 0) {
                    auto new_path = paths.top(); paths.pop();

                    /* Check if there was an old path to this node.
                     * If so we're going to need to replace the path
                     * and re-compute everything. */
                    auto old_path_l = _paths.find(new_path->d()->name());
                    path_ptr old_path = NULL;
                    if (old_path_l != _paths.end())
                        old_path = old_path_l->second;

                    /* If the old path exists and is cheaper than the
                     * new path we just added then don't bother with
                     * it at all, it's strictly worse than whatever we
                     * have now. */
                    if ((old_path != NULL) && (old_path->cost() <= new_path->cost()))
                        continue;

                    /* If this path is circular then justs abandon it here. */
                    if (new_path->s() == new_path->d())
                        continue;

                    /* Now that we know that this path is useful we
                     * can add it to our list current path list. */
                    _paths[new_path->d()->name()] = new_path;

                    /* Add every newly-discovered path to the queue
                     * for processing. */
                    for (const auto& second_half : new_path->d()->_outgoing_neighbors) {
                        paths.push(new_path->cat(second_half));
                    }
                }

                /* Now we can set that flag so this never gots called again. */
                this->_paths_valid = true;
            }

        /* A helper function for both the incoming and outgoing port
         * neighbor searches functions. */
        size_t port_number(const node_ptr& n,
                           const std::vector<path_ptr>& neighbors,
                           std::function<node_ptr(path_ptr)> f) const
            {
                for (size_t i = 0; i < neighbors.size(); ++i) {
                    auto ni = f(neighbors[i]);
                    if (strcmp(ni->name().c_str(), n->name().c_str()) == 0)
                        return i;
                }

                fprintf(stderr, "No port from '%s' to '%s'\n",
                        this->name().c_str(),
                        n->name().c_str()
                    );
                fprintf(stderr, "Ports '%s' are:\n", name().c_str());
                for (size_t i = 0; i < neighbors.size(); ++i) {
                    auto neighbor = f(neighbors[i]);
                    fprintf(stderr, "  '%s' at port " SIZET_FORMAT "\n",
                            neighbor->name().c_str(),
                            i
                        );
                }
                abort();
                return -1;
            }

    private:
       /* This consists of the UID management code, which deals with
        * creating a new UID for every node that's created by the
        * system.  Note that these UIDs are keyed based on the node's
        * string name, */
        static size_t get_uid(const std::string& name)
            {
                static size_t uid = 1;

                auto l = name2uid.find(name);
                if (l != name2uid.end()) {
                    fprintf(stderr, "re-mapped UID: '%s' -> " SIZET_FORMAT "\n",
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
                name2uid[name] = uid;
                return uid;
            }

        static void put_uid(const std::string& name)
            {
                auto l = name2uid.find(name);
                if (l == name2uid.end()) {
                    fprintf(stderr, "uid_put for unmapped name: '%s'\n",
                            name.c_str()
                        );
                    abort();
                }

                name2uid.erase(name);
            }
    };

    template<class node_t>
    std::unordered_map<std::string, size_t> node<node_t>::name2uid;
}

#endif
