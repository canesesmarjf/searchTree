#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <armadillo>

using namespace std;
using namespace arma;

// =====================================================================================================================
struct bounds_TYP
{
  // int dims;
  vec max;
  vec min;

  // bounds_TYP();
  // bounds_TYP(int dims);
};

// =====================================================================================================================
struct depth_TYP
{
  int current;
  int max;
};

// =====================================================================================================================
class node_TYP
{
    public:

    // The following is a diagram of an octree cube.
    // Here pmax and pmin are vectors that describe the bounds of octree cube:
    //
    //  pmax = bounds.max;
    //  pmin = bounds.min;
    //
    //  +ve z)        y                      +ve x)       y
    //                ^                                   ^
    //      |---------|----------| pmax      pmax |-------|-------|
    //      |  node 1 |  node 0  |                |   0   |   4   |
    //      |---------o----------|--> x      z <--|-------o-------|--
    //      |  node 2 |  node 3  |                |   3   |   7   |
    //      |---------|----------|                |-------|-------|
    //
    //  -ve z)        y
    //                ^
    //      |---------|----------|
    //      |  node 5 |  node 4  |
    //      |---------x----------|--> x
    //      |  node 6 |  node 7  |
    // pmin |---------|----------|

    // Node attributes:
    int _dims;                      // Dimensions of data: 1, 2 or 3 dimensions
    vec _p0;                        // Coordinates of node's origin
    bounds_TYP _bounds;             // ["min"] and ["max"] bounds of node in all dimensions
    depth_TYP _depth;               // ["max"] and ["current"] depth of node

    // Data stored in node:
    int _point_count;               // Data counts in this node
    vector<uint> _point_index_list; // Indices of data in this node

    // Constructor:
    node_TYP();
    node_TYP(bounds_TYP bounds,depth_TYP depth);

    // Methods:
    node_TYP * insert_point(uint point_index, vector<vec *> data );
    node_TYP * insert_point(uint point_index, vec * data_x, vec * data_y);
    node_TYP * find_points(vec point);
    node_TYP * get_subNode(int index);

    // private:

    // Subnodes:
    vector<node_TYP *> _subnode;

    // Maps:
    // The signature is a vector which points from the parent nodes's origin to the direction of the new subnode:
    map<vector<int>,int> signature_to_nodeIndex;
    map<int,vector<int>> nodeIndex_to_signature;

    // Methods:
    bool isPointInsideBoundary(vec point);
    bool hasNodeReachMaxDepth();
    int  whichSubNodeDoesItBelongTo(vec point);
    bool doesSubNodeExist(int node_index);
    void createSubNode(int node_index);
};

#endif
