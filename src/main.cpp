#include <iostream>
#include <armadillo>
#include "H5Cpp.h"
#include "searchTree.h"
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
  double mean = 0;
  double std  = 1;
  vec a = mean + std*randn(N_CP);
  vec b = mean + std*randn(N_CP);
  vec c = mean + std*randn(N_CP);
  vector<vec *> data = {&a, &b, &c};

  // Instantiate a search tree:
  // ===================================================================================================================
  bounds_TYP bounds;
  depth_TYP depth;
  node_TYP searchTree;

  // Populate bounds and depth:
  // ===================================================================================================================
  bounds.min = {-2,-2};
  bounds.max = {+2,+2};
  depth.max  = 5;
  depth.current = 0;

  // Initialize the search tree:
  // ===================================================================================================================
  searchTree = node_TYP(bounds,depth);

  cout << " " << endl;
  cout << "**************************** MEMORY **************************************" << endl;
  cout << "size of double     = " << sizeof(double) << " [B]" << endl;
  cout << "size of int        = " << sizeof(int) << " [B]" << endl;
  cout << "size of data in a  = " << a.n_elem * sizeof(double) << " [B]" << endl;
  cout << "size of overhead a = " << sizeof(a) << " [B]" << endl;
  cout << "size of &a         = " << sizeof(&a) << " [B]" << endl;
  cout << "size of data       = " << sizeof(data) << " [B]" << endl;
  cout << "size of &data      = " << sizeof(&data) << " [B]" << endl;
  cout << " " << endl;
  std::cout << "Address of a (hexadecimal): " << a.memptr() << std::endl;
  std::cout << "Address of b (hexadecimal): " << b.memptr() << std::endl;
  std::cout << "Address of a (decimal): " << dec << a.memptr() << std::endl;
  std::cout << "Address of b (decimal): " << (unsigned long) b.memptr() << std::endl;
  cout << " " << endl;
  cout << "size of 'searchTree' just after constructor = " << sizeof(searchTree) << " [B]" << endl;
  cout << "**************************** MEMORY **************************************" << endl;
  cout << " " << endl;

  // Insert data into tree:
  // ===================================================================================================================
  cout << " " << endl;
  cout << "**************************** INSERT **************************************" << endl;

  // Insert all points:
  start = high_resolution_clock::now();
  node_TYP * node = NULL;
  for (int i = 0; i < N_CP; i++)
  {
    node = searchTree.insert_point(i,&data);
  }
  end = high_resolution_clock::now();
  duration_insert = duration_cast<milliseconds>(end - start);

  cout << "Total number of points inserted  = " << setprecision(1) << scientific << (double) N_CP << endl;
  cout << "Time taken to insert all points = " << duration_insert.count() << " [ms]" << endl;
  cout << "Mean time to insert a single point = " << duration_insert.count()*1e3/N_CP << " [us] " << endl;
  cout << " " << endl;
  cout << "size of 'node' returned by searchTree.insert_point() = " << sizeof(node) << " [B]" << endl;
  cout << "size of 'searchTree' after insertion of all points total_time/N_CP = " << sizeof(searchTree) << " [B]" << endl;

  cout << "**************************** INSERT **************************************" << endl;
  cout << " " << endl;

  // Test the find_points method:
  // ===================================================================================================================
  cout << " " << endl;
  cout << "**************************** FIND **************************************" << endl;

  // Search location:
  vec point = {0,0.5};
  point.print("point to search = ");
  cout << " " << endl;

  // Start search:
  start = high_resolution_clock::now();
  node_TYP * result = searchTree.find_points(point);
  end = high_resolution_clock::now();
  duration_find = duration_cast<microseconds>(end - start);

  cout << "Time taken to find 1 point = " << duration_find.count() << " [us]" << endl;
  cout << "Memory address pointed by 'result' = " << result << endl;
  cout << "Number of points returnd by 'find_points' = " << result->_point_count << endl;
  result->_p0.print("origin of node = ");
  cout << " " << endl;
  cout << "size of *result = " << sizeof(*result) << endl;
  cout << "size of _point_index_list in result = " << sizeof(result->_point_index_list) << endl;
  cout << "**************************** FIND **************************************" << endl;


  // Print results to the terminal:
  // ===================================================================================================================
  // Assemble output matrix:
  mat points_found(result->_point_count,2,fill::zeros);
  for (uint ii = 0; ii < points_found.n_rows ; ii++)
  {
    uint j = result->_point_index_list.at(ii);
    // cout << "j = " << j << endl;
    points_found(ii,0) = data[0]->at(j);
    points_found(ii,1) = data[1]->at(j);
  }

  // Print data to terminal:
  cout << " " << endl;
  points_found.print("Points found = ");
  cout << " " << endl;

  // Save results to file:
  // ===================================================================================================================
  ivec point_index = conv_to<ivec>::from(result->_point_index_list);

  // a.save("output_files/a.h5", arma::hdf5_binary);
  // b.save("output_files/b.h5", arma::hdf5_binary);
  // c.save("output_files/c.h5", arma::hdf5_binary);
  // point_index.save("output_files/point_index.h5", arma::hdf5_binary);

  a.save(hdf5_name("output_files/a.h5","dataset"));
  b.save(hdf5_name("output_files/b.h5","dataset"));
  c.save(hdf5_name("output_files/c.h5","dataset"));
  point_index.save(hdf5_name("output_files/point_index.h5","dataset"));

  return 0;
}
