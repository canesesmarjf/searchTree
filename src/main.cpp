#include <iostream>
#include <armadillo>
#include "H5Cpp.h"
#include "QuadTree.h"
#include <chrono>
#include <string>
#include <sstream>

using namespace std::chrono;
using namespace std;
using namespace arma;

int main()
{
  // Variables for measuring time:
  time_point<high_resolution_clock> start, end;
  milliseconds duration_insert;
  microseconds duration_find;

  // Create Data:
  // ===================================================================================================================
  int N_CP = 1e5;
  vec a = 2*randu(N_CP)-1;
  vec b = 2*randu(N_CP)-1;
  vec c = 2*randu(N_CP)-1;
  vector<vec *> data = {&a, &b, &c};

  cout << "size of double in bytes = " << sizeof(double) << endl;
  cout << "size of int in bytes    = " << sizeof(int) << endl;
  cout << "size of a in bytes      = " << sizeof(a) << endl;
  cout << "size of &a in bytes     = " << sizeof(&a) << endl;
  cout << "size of data in bytes   = " << sizeof(data) << endl;
  cout << "size of &data in bytes  = " << sizeof(&data) << endl;

  // Instantiate a search tree:
  // ===================================================================================================================
  bounds_TYP bounds;
  depth_TYP depth;
  bounds.min = {-1,-1};
  bounds.max = {+1,+1};
  depth.max  = 6;
  depth.current = 0;
  node_TYP searchTree(bounds,depth);

  // Insert data into tree:
  // ===================================================================================================================
  start = high_resolution_clock::now();
  for (int i = 0; i < N_CP; i++)
  {
    node_TYP * node = searchTree.insert_point(i,data);
  }
  end = high_resolution_clock::now();
  duration_insert = duration_cast<milliseconds>(end - start);
  cout << " time taken to insert all points = " << duration_insert.count() << " [ms]" << endl;

  // Checking the find_points method:
  int i = 0;
  vec point = {data[0]->at(i),data[1]->at(i)};
  point.print("point to search = ");
  start = high_resolution_clock::now();
  node_TYP * result = searchTree.find_points(point);
  end = high_resolution_clock::now();
  duration_find = duration_cast<microseconds>(end - start);
  cout << " time taken to find 1 point = " << duration_find.count() << " [us]" << endl;

  cout << "contents of result = " << result << endl;
  cout << "Number of points in this node = " << result->_point_count << endl;
  result->_p0.print("origin of node = ");

  // Test:
  i = 0;
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
