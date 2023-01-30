#include "searchTree.h"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace arma;

// =====================================================================================================================
namespace node_maps
{
    static map<vector<int>,int> signature_to_nodeIndex = {
        {{1,1,1}, 0},
        {{0,1,1}, 1},
        {{0,0,1}, 2},
        {{1,0,1}, 3},
        {{1,1,0}, 4},
        {{0,1,0}, 5},
        {{0,0,0}, 6},
        {{1,0,0}, 7}
    };

    static map<int,vector<int>> nodeIndex_to_signature = {
        {0, {1,1,1}},
        {1, {0,1,1}},
        {2, {0,0,1}},
        {3, {1,0,1}},
        {4, {1,1,0}},
        {5, {0,1,0}},
        {6, {0,0,0}},
        {7, {1,0,0}}
    };
}

// =====================================================================================================================
node_TYP::node_TYP()
{};

// =====================================================================================================================
node_TYP::node_TYP(bounds_TYP bounds,depth_TYP depth)
{
  // Assign node attributes:
  _bounds = bounds;
  _depth  = depth;
  _dims   = bounds.max.n_elem;
  _p0     = (bounds.min + bounds.max)/2;

  // Check that bounds have correct form:
  vec metric = _bounds.max - _bounds.min;
  for (int d = 0; d < _dims; d++)
  {
    if (metric[d]<=0)
    {
      cout << "Error: bounds.max < bounds.min. See dimension " << d << endl;
      exit(1);
    }
  }

  // Make all subnodes = NULL:
  int num_nodes = pow(2,_dims);
  _subnode.reserve(num_nodes);
  for (int n = 0; n<num_nodes; n++)
  {
    _subnode[n] = NULL;
  }

  // Initialize data:
  _point_count = 0;
  // _point_index_list.reserve(1000);

  /*
  // Map from to signature to node index [0-7]:
  signature_to_nodeIndex[{1,1,1}] = 0;
  signature_to_nodeIndex[{0,1,1}] = 1;
  signature_to_nodeIndex[{0,0,1}] = 2;
  signature_to_nodeIndex[{1,0,1}] = 3;
  signature_to_nodeIndex[{1,1,0}] = 4;
  signature_to_nodeIndex[{0,1,0}] = 5;
  signature_to_nodeIndex[{0,0,0}] = 6;
  signature_to_nodeIndex[{1,0,0}] = 7;

  // Map from node index [0-7] to signature:
  nodeIndex_to_signature[0] = {1,1,1};
  nodeIndex_to_signature[1] = {0,1,1};
  nodeIndex_to_signature[2] = {0,0,1};
  nodeIndex_to_signature[3] = {1,0,1};
  nodeIndex_to_signature[4] = {1,1,0};
  nodeIndex_to_signature[5] = {0,1,0};
  nodeIndex_to_signature[6] = {0,0,0};
  nodeIndex_to_signature[7] = {1,0,0};
  */

  // The "signature" vector indicates in which direction we must move relative to the origin in a RH coordinate system
  // to move in the direction of the new node with an index given by "node_index":
  // +ve z:
  //                      y
  //                      ^
  //                      |
  //   +------------------+------------------+
  //   |                  |                  |
  //   |  node_index = 1  |  node_index = 0  |
  //   |                  |                  |
  //   +------------------o------------------+---- > x
  //   |                  |                  |
  //   |  node_index = 2  |  node_index = 3  |
  //   |                  |                  |
  //   +------------------+------------------+
  //
  //  -ve z:
  //                      y
  //                      ^
  //                      |
  //   +------------------+------------------+
  //   |                  |                  |
  //   |  node_index = 5  |  node_index = 4  |
  //   |                  |                  |
  //   +------------------o------------------+---- > x
  //   |                  |                  |
  //   |  node_index = 6  |  node_index = 7  |
  //   |                  |                  |
  //   +------------------+------------------+

}

// =====================================================================================================================
node_TYP * node_TYP::find_points(vec point)
{

  // Check if data is within node's boundaries:
  // ----------------------------------------------------------------
  if (!isPointInsideBoundary(point))
  {
    cout << "Point = " << endl;
    point.print();
    cout << "is out of bounds" << endl;

    return NULL;
  }

  // Check if maximum tree depth has been reached:
  // ----------------------------------------------------------------
  if (hasNodeReachMaxDepth())
  {
    return this;
  }

  // Determine which subnode to search next:
  // ----------------------------------------------------------------
  int node_index = whichSubNodeDoesItBelongTo(point);

  // Check if subnode exists:
  // ----------------------------------------------------------------
  if (!doesSubNodeExist(node_index))
  {
    return NULL;
  }

  // Drill further into subnode:
  return this->_subnode[node_index]->find_points(point);

}

// =====================================================================================================================
node_TYP * node_TYP::insert_point(uint point_index, vector<vec *> * data)
{

  // Current data vector:
  // ----------------------------------------------------------------
  vec point = zeros(_dims);
  for (int d = 0; d < _dims ; d ++)
  {
    vec * pdata = (*data)[d];
    point[d] = pdata->at(point_index);
  }

  // Check if data is within node's boundaries:
  // ----------------------------------------------------------------
  if (!isPointInsideBoundary(point))
  {
    /*
    cout << "Point = " << endl;
    point.print();
    cout << "is out of bounds" << endl;
    */
    return NULL;
  }

  // Check if maximum tree depth has been reached:
  // ----------------------------------------------------------------
  if (hasNodeReachMaxDepth())
  {
    // Append point:
    _point_index_list.push_back(point_index);
    _point_count++;

    return this;
  }

  // Determine which subnode to insert point into:
  // ----------------------------------------------------------------
  int node_index = whichSubNodeDoesItBelongTo(point);

  // Check if subnode exists:
  // ----------------------------------------------------------------
  if (!doesSubNodeExist(node_index))
  {
    createSubNode(node_index);
  }

  // Insert point into node:
  // ----------------------------------------------------------------
  node_TYP * newsubnode = _subnode[node_index]->insert_point(point_index,data);
  return newsubnode;
}

// =====================================================================================================================
bool node_TYP::isPointInsideBoundary(vec point)
{
  bool flag;

  // Loop over all dimensions of point:
  for (int dd = 0; dd < _dims; dd++)
  {
    // Extract data:
    double node_min = _bounds.min[dd];
    double node_max = _bounds.max[dd];
    double point_value = point[dd];

    // Create boolean result:
    flag = (point_value >= node_min) && (point_value <= node_max);

    // Check if out of bound:
    if (flag == 0)
    {
      return flag;
    }
  }

  return flag;
}

// =====================================================================================================================
bool node_TYP::hasNodeReachMaxDepth()
{
  if (_depth.current >= _depth.max)
    return 1;
  else
    return 0;
}

// =====================================================================================================================
int node_TYP::whichSubNodeDoesItBelongTo(vec point)
{
  // Origin of parent node:
  vec point_0 = _p0;

  // Check that the dimensions of point and point_0 are compatible:
  if (point_0.n_elem != point.n_elem)
    cout << "inserted point has incompatible dimensions" << endl;

  // Difference vector:
  vec dpoint = point - point_0;

  // node_signature
  // [+,+,+] : Q1
  // [+,-,+] : Q2
  // [-,-,+] : Q3
  // [-,+,+] : Q4
  // [+,+,-] : Q5
  // [+,-,-] : Q6
  // [-,-,-] : Q7
  // [-,+,-] : Q8

  // Initialize the node's signature:
  vector<int> node_signature = {1, 1, 1};

  // Compute the node's signture
  for(int d = 0; d < _dims; d++)
  {
    if (dpoint[d] >= 0)
      node_signature[d] = 1;
    else
      node_signature[d] = 0;
  }

  // Get the node's index based on the node's signature:
  int node_index = node_maps::signature_to_nodeIndex[node_signature];
  return node_index;
}

// =====================================================================================================================
bool node_TYP::doesSubNodeExist(int node_index)
{
  if (NULL == _subnode[node_index]) // Subnode does NOT exist
  {
    return 0;
  }
  else // Subnode already exists
  {
    return 1;
  }
}

// =====================================================================================================================
void node_TYP::createSubNode(int node_index)
{
  // Displacement vector between origin p0 and max bound vector of parent node:
  vec dx = _bounds.max - _p0;

  // Get node's signature based on node_index:
  vector<int> node_signature = node_maps::nodeIndex_to_signature[node_index];

  // Initialize bounds:
  bounds_TYP new_bounds = _bounds;

  // Initialize depth:
  depth_TYP new_depth = _depth;
  new_depth.current++;

  // Calculate new bounds:
  for (int d = 0; d < _dims; d++)
  {
    // Min bound:
    int s = node_signature[d];
    new_bounds.min[d] += s*dx[d];

    // Max bound:
    s = s - 1;
    new_bounds.max[d] += s*dx[d];
  }

  // Create subnode:
  _subnode[node_index] = new node_TYP(new_bounds, new_depth);
}
