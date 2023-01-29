#include <iostream>
#include <armadillo>
#include "H5Cpp.h"
#include "QuadTree.h"
#include <chrono>
#include <string>
#include <sstream>

using namespace std::chrono;
using namespace std;

int main()
{
  // Create Data:
  // ===================================================================================================================
  vec a = {+1.2, 3.2, 5.3};
  vec b = {+4.2, 3.9, 8.9};
  vec c = {-6.2, 6.2, 4.2};
  vector<vec *> data = {&a, &b, &c};

  // Instantiate a search tree:
  // ===================================================================================================================
  bounds_TYP bounds;
  depth_TYP depth;
  bounds.min = {-10,-10};
  bounds.max = {+10,+10};
  depth.max  = 10;
  depth.current = 0;
  node_TYP searchTree(bounds,depth);

  // Insert data into tree:
  // ===================================================================================================================
  int i = 0;
  node_TYP * bin_node = searchTree.insert_point(i,data);

  // Print data inserted:
  // ===================================================================================================================
  for (int d = 0; d < searchTree._dims ; d++)
  {
    cout << "data["<<d<<"]->at(i) = " << data[d]->at(i) << endl;
  }

  // Print origin of final node:
  // ===================================================================================================================
  for (int d = 0; d < searchTree._dims ; d++)
  {
    cout << "origin = " << bin_node->_p0[d] << endl;
  }

  // Print bounds of final node:
  // ===================================================================================================================
  for (int d = 0; d < searchTree._dims ; d++)
  {
    cout << "bounds.max[" << d << "] = " << bin_node->_bounds.max[d] << endl;
    cout << "bounds.mmin[" << d << "] = " << bin_node->_bounds.min[d] << endl;
  }

  return 0;
}
