/* 
 * File:   IMutableMultiGraph.hpp
 *
 * Author: ABSEHER Michael (abseher@dbai.tuwien.ac.at)
 * 
 * Copyright 2015-2017, Michael Abseher
 *    E-Mail: <abseher@dbai.tuwien.ac.at>
 * 
 * This file is part of htd.
 * 
 * htd is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation, either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * htd is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
 * License for more details.

 * You should have received a copy of the GNU General Public License
 * along with htd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTD_HTD_IMUTABLEMULTIGRAPH_HPP
#define HTD_HTD_IMUTABLEMULTIGRAPH_HPP

#include <htd/Globals.hpp>
#include <htd/IMultiGraph.hpp>

namespace htd
{
    /**
     * Interface for classes which represent mutable graphs with potentially duplicated edges.
     */
    class IMutableMultiGraph : public virtual htd::IMultiGraph
    {
        public:
            /**
             *  Destructor for an IMutableMultiGraph object.
             */
            virtual ~IMutableMultiGraph() = 0;

            /**
             *  Getter for the next vertex identifier.
             *
             *  @return The ID the next vertex added to the graph will get.
             */
            virtual htd::vertex_t nextVertex(void) const = 0;

            /**
             *  Getter for the next edge identifier.
             *
             *  @return The ID the next edge added to the graph will get.
             */
            virtual htd::id_t nextEdgeId(void) const = 0;

            /**
             *  Add a new vertex to the multi-graph.
             *
             *  @return The ID of the new vertex.
             */
            virtual htd::vertex_t addVertex(void) = 0;

            /**
             *  Add new vertices to the multi-graph.
             *
             *  @param[in] count    The number of vertices which shall be added.
             *
             *  @return The ID of the first vertex which was added to the graph. If the number of vertices to add is greater than 1,
             *          the additional vertices are numbered in ascending order and without gaps starting from the returned ID + 1.
             *          If the number of new vertices is 0, the returned value is the ID of the last vertex added to the graph.
             */
            virtual htd::vertex_t addVertices(std::size_t count) = 0;

            /**
             *  Remove a vertex from the multi-graph.
             *
             *  @param[in] vertex   The ID of the vertex which shall be removed.
             */
            virtual void removeVertex(htd::vertex_t vertex) = 0;

            /**
             *  Add a new edge to the multi-graph.
             *
             *  @param[in] vertex1  The first endpoint of the edge.
             *  @param[in] vertex2  The second endpoint of the edge.
             *
             *  @return The ID of the new edge.
             */
            virtual htd::id_t addEdge(htd::vertex_t vertex1, htd::vertex_t vertex2) = 0;

            /**
             *  Remove an edge from the multi-graph.
             *
             *  @param[in] edgeId   The ID of the edge which shall be removed.
             */
            virtual void removeEdge(htd::id_t edgeId) = 0;

#ifndef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
            /**
             *  Create a deep copy of the current mutable multi-graph.
             *
             *  @return A new IMutableMultiGraph object identical to the current mutable multi-graph.
             */
            virtual IMutableMultiGraph * clone(void) const HTD_OVERRIDE = 0;
#else
            /**
             *  Create a deep copy of the current mutable multi-graph.
             *
             *  @return A new IMutableMultiGraph object identical to the current mutable multi-graph.
             */
            virtual IMutableMultiGraph * cloneMutableMultiGraph(void) const = 0;
#endif

#ifndef HTD_USE_VISUAL_STUDIO_COMPATIBILITY_MODE
            /**
             *  Copy assignment operator for a mutable multi-graph.
             *
             *  @param[in] original  The original multi-graph.
             */
            virtual IMutableMultiGraph & operator=(const htd::IMultiGraph & original) = 0;
#else
            /**
             *  Copy assignment operator for a mutable multi-graph.
             *
             *  @param[in] original  The original multi-graph.
             */
            virtual void assign(const htd::IMultiGraph & original) = 0;
#endif
    };

    inline htd::IMutableMultiGraph::~IMutableMultiGraph() { }
}

#endif /* HTD_HTD_IMUTABLEMULTIGRAPH_HPP */
