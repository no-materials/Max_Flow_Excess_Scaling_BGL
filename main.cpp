#include <iostream>
#include <math.h>

#include <boost/format.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/grid_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/graph_utility.hpp>



using namespace boost;
using namespace std;


//Type & Struct Definitions -----------------------------------------------------------------

// Bundled properties for graph
struct bundleVertex {
    int id;
    int excess;
    int distance;
};

struct bundleEdge {
    int residual;
};


// Define graph type
typedef adjacency_list<vecS, vecS, bidirectionalS, bundleVertex, bundleEdge> Graph;

//Define graph traits type
typedef graph_traits<Graph> Traits;

//Define descriptor for vertices
typedef Traits::vertex_descriptor Vertex;
//Define descriptor for edges
typedef Traits::edge_descriptor Edge;

////Define property maps for vertex/edge index
//typedef property_map<Graph, vertex_index_t>::type VertexIndexMap;
//typedef property_map<Graph, edge_index_t>::type EdgeIndexMap;
//
////Define the Vertex property type which maps vertices to the struct defined above
//typedef vector_property_map<graphVertex, VertexIndexMap> VertexProp;
//typedef vector_property_map<graphEdge, EdgeIndexMap> EdgeProp;
//
////Define LValuePropertyMaps for distance  and weight maps
//typedef iterator_property_map<float*, VertexIndexMap, float, float&> DistanceMap;



// Function Definitions -----
void preprocess(Graph &G, Vertex source, Vertex sink);



int main() {

    // Graph instantiation
    Graph G;

    // Graph creation
    Vertex v1 = add_vertex(bundleVertex{1, 0, 2}, G);
    Vertex v2 = add_vertex(bundleVertex{2, 0, 1}, G);
    Vertex v3 = add_vertex(bundleVertex{3, 0, 1}, G);
    Vertex v4 = add_vertex(bundleVertex{4, 0, 0}, G);

    add_edge(v1, v2, bundleEdge{2}, G);
    add_edge(v1, v3, bundleEdge{4}, G);
    add_edge(v2, v3, bundleEdge{3}, G);
    add_edge(v2, v4, bundleEdge{1}, G);
    add_edge(v3, v4, bundleEdge{5}, G);


    // Excess Scaling Algorithm-----

    //Preprocessing ----
    preprocess(G, v1, v4);






    print_graph(G, get(&bundleVertex::id, G));
    //print_graph(make_reverse_graph(G), get(vertex_index, G));
    return 0;
}




void preprocess(Graph &G, Vertex source, Vertex sink) {

    // Store initial distances in this vector
    std::vector<int> distances(num_vertices(G));

    // Computation of exact distance labels via backward BFS from sink
    reverse_graph<Graph> R = make_reverse_graph(G);
    breadth_first_search(R,
                         sink,
                         visitor(make_bfs_visitor(record_distances(make_iterator_property_map(distances.begin(),
                                                                                              get(vertex_index, G)),
                                                                   on_tree_edge()))));
    Graph::vertex_iterator vertexIt, vertexEnd;
    tie(vertexIt, vertexEnd) = vertices(G);
    for (; vertexIt != vertexEnd; ++vertexIt) {
        G[*vertexIt].distance = distances[*vertexIt];
    }

    // Saturate source's adjacent edges
    Graph::out_edge_iterator outedgeIt, outedgeEnd;
    tie(outedgeIt, outedgeEnd) = out_edges(source, G);
    for(; outedgeIt != outedgeEnd; ++outedgeIt) {
        Vertex src = boost::source(*outedgeIt, G);
        Vertex target = boost::target(*outedgeIt, G);
        int saturation = G[*outedgeIt].residual;

        add_edge(target, src, bundleEdge{saturation}, G);  // add reverse of saturated edge
        remove_edge(*outedgeIt, G); // remove saturated edge

        G[target].excess += saturation;  // update target node's excess property
    }

    // Update source node's distance label to num_vertices
    G[source].distance = (int) num_vertices(G);
}
















