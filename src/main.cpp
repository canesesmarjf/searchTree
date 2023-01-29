#include <iostream>
#include <armadillo>
#include "H5Cpp.h"
#include "QuadTree.h"
#include <chrono>
#include <string>
#include <sstream>

using namespace std::chrono;
//using namespace H5;
using namespace std;
//using namespace arma;

int main()
{
  // Data:
  vec a = {+1.2, 3.2, 5.3};
  vec b = {+4.2, 3.9, 8.9};
  vec c = {-6.2, 6.2, 4.2};
  vector<vec *> data = {&a, &b, &c};

  // Construct binary tree:
  bounds_TYP bounds;
  depth_TYP depth;
  bounds.min = {-10,-10,-15};
  bounds.max = {+10,+10,+15};
  depth.max  = 10;
  depth.current = 0;

  cout << "searchTree create:" << endl;
  node_TYP searchTree(bounds,depth);

  int i = 0;
  cout << "searchTree 1st insert: #######################################" << endl;
  node_TYP * bin_node = searchTree.insert_point(i,data);

  // Data inserted:
  for (int d = 0; d < searchTree._dims ; d++)
  {
    cout << "data["<<d<<"]->at(i) = " << data[d]->at(i) << endl;
  }

  // Origin of final node:
  for (int d = 0; d < searchTree._dims ; d++)
  {
    cout << "origin = " << bin_node->_p0[d] << endl;
  }

  // Bounds of final node:
  for (int d = 0; d < searchTree._dims ; d++)
  {
    cout << "bounds.max[" << d << "] = " << bin_node->_bounds.max[d] << endl;
    cout << "bounds.mmin[" << d << "] = " << bin_node->_bounds.min[d] << endl;

  }


  // cout << "completed tree: " << endl;
  // cout << "a[0] = " << a[i] << endl;
  // cout << "b[0] = " << b[i] << endl;
  // cout << "bin_node->p0[0] = " << bin_node->_p0[0] << endl;
  // cout << "bin_node->p0[1] = " << bin_node->_p0[1] << endl;
  // cout << "bin_node->bounds.min[0] = " << bin_node->_bounds.min[0] << endl;
  // cout << "bin_node->bounds.max[0] = " << bin_node->_bounds.max[0] << endl;

  return 0;
}
