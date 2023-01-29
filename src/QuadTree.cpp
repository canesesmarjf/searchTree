#include "QuadTree.h"
#include <iostream>
#include <cstdlib>

using namespace std;
using namespace arma;

// =====================================================================================================================
// bounds_TYP::bounds_TYP(int dims)
// {
//   _dims = dims;
//   _max = zeros(dims);
//   _min = zeros(dims);
// }

// =====================================================================================================================
node_TYP::node_TYP(bounds_TYP bounds,depth_TYP depth)
{
  // Assign node attributes:
  _bounds = bounds;
  _depth  = depth;
  _dims   = bounds.max.n_elem;
  _p0     = (bounds.min + bounds.max)/2;

  cout << "Constructor: start ##########################################"<< endl;
  cout << "_depth = " << _depth.current << endl;
  cout << "_dims = " << _dims << endl;
  cout << "_bounds.max.n_elem = " << _bounds.max.n_elem << endl;

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
  cout << "allocate memory to subodes: start"<< endl;
  cout << "_dims " << _dims << endl;

  int num_nodes = pow(2,_dims);
  _subnode.reserve(num_nodes);
  cout << "loop start" << endl;
  for (int n = 0; n<num_nodes; n++)
  {
    _subnode[n] = NULL;
  }
  cout << "loop end" << endl;

  // Initialize data:
  _point_count = 0;
  _point_index_list.reserve(100);

  // Map from to signature to node ID:
  signature_to_nodeId[{1,1,1}] = 1;
  signature_to_nodeId[{0,1,1}] = 2;
  signature_to_nodeId[{0,0,1}] = 3;
  signature_to_nodeId[{1,0,1}] = 4;
  signature_to_nodeId[{1,1,0}] = 5;
  signature_to_nodeId[{0,1,0}] = 6;
  signature_to_nodeId[{0,0,0}] = 7;
  signature_to_nodeId[{1,0,0}] = 8;

  // Map from node ID to signature:
  nodeId_to_signature[1] = {1,1,1};
  nodeId_to_signature[2] = {0,1,1};
  nodeId_to_signature[3] = {0,0,1};
  nodeId_to_signature[4] = {1,0,1};
  nodeId_to_signature[5] = {1,1,0};
  nodeId_to_signature[6] = {0,1,0};
  nodeId_to_signature[7] = {0,0,0};
  nodeId_to_signature[8] = {1,0,0};
  cout << "Constructor: end ##########################################"<< endl;

}

// =====================================================================================================================
node_TYP * node_TYP::insert_point(uint point_index, vector<vec *> data )
{
  cout << "insert point start ---------------------------------" << endl;
  cout << "dims = " << _dims << endl;

  // Current data vector:
  // ----------------------------------------------------------------
  cout << "Assemble point start ---------------------------------" << endl;
  vec point = zeros(_dims);
  for (int d = 0; d < _dims ; d ++)
  {
    vec * pdata = data[d];
    point[d] = pdata->at(point_index);
  }

  // DIAGNOSTICS:
  for (int d = 0; d < _dims; d++)
  {
    cout << "point[d] = " << point[d] << endl;
  }

  cout << "Assemble point end ---------------------------------" << endl;


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
  cout << "hasNodeReachMaxDepth start ##############################" << endl;
  if (hasNodeReachMaxDepth())
  {
    cout << "Reached Maximum depth ##############################" << endl;
    // Append point:
    _point_index_list.push_back(point_index);
    _point_count++;

    return this;
  }
  cout << "hasNodeReachMaxDepth end ##############################" << endl;

  // Determine which subnode to insert point into:
  // ----------------------------------------------------------------
  cout << "whichSubNodeDoesItBelongTo start ---------------------------------" << endl;

  int node_index = whichSubNodeDoesItBelongTo(point);

  cout <<"                  "<< endl;
  cout <<"                  "<< endl;
  cout <<"                  "<< endl;
  cout << "node_index = "<< node_index << endl;
  cout <<"                  "<< endl;
  cout <<"                  "<< endl;
  cout <<"                  "<< endl;

  cout << "whichSubNodeDoesItBelongTo end ---------------------------------" << endl;

  // Check if subnode exists:
  // ----------------------------------------------------------------
  cout << "doesSubNodeExist start ---------------------------------" << endl;
  if (!doesSubNodeExist(node_index))
  {
    cout << "about to create new node" << endl;
    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cout << "node_index = "<< node_index << endl;
    createSubNode(node_index);
    cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
    cout << "New node created" << endl;

  }
  cout << "doesSubNodeExist end ---------------------------------" << endl;

  // Insert point into node:
  // ----------------------------------------------------------------
  cout << "Insert point into node start ---------------------------------" << endl;
  cout << "node_index = "<< node_index << endl;
  cout << "_subnode[node_index]->_bounds.max.n_elem = " << _subnode[node_index-1]->_bounds.max.n_elem << endl;
  node_TYP * newsubnode = _subnode[node_index-1]->insert_point(point_index,data);
  cout << "Insert point into node end ---------------------------------" << endl;
  return newsubnode;
}

// =====================================================================================================================
node_TYP * node_TYP::insert_point(uint point_index, vec * data_x, vec * data_y)
{
  // Current data vector:
  // ----------------------------------------------------------------
  vec point = {data_x->at(point_index),data_y->at(point_index)};

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
  return _subnode[node_index-1]->insert_point(point_index,data_x,data_y);
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

    cout << "start ------" << endl;
    cout << "node_min = " << node_min << endl;
    cout << "node_max = " << node_max << endl;
    cout << "point_value = " << point_value << endl;
    cout << "end ------" << endl;

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
  int node_index = signature_to_nodeId[node_signature];
  return node_index;
}

// =====================================================================================================================
bool node_TYP::doesSubNodeExist(int node_index)
{
  if (NULL == _subnode[node_index-1]) // Subnode does NOT exist
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
  cout << "create subnode ###########################:" << endl;

  // Origin of parent node:
  vec p0 = _p0;

  // Displacement vector between origin p0 and max bound vector:
  vec dx = _bounds.max - p0;

  // Get node's signature based on node_index:
  vector<int> node_signature = nodeId_to_signature[node_index];

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

  cout << "Prior creating new node ---------------------------------" << endl;
  cout << "_dims = " << _bounds.max.n_elem << endl;
  cout << "dims = " << new_bounds.max.n_elem << endl;

  // Create subnode:
  _subnode[node_index-1] = new node_TYP(new_bounds, new_depth);
}
