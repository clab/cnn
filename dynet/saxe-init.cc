#include "dynet/saxe-init.h"

#include <algorithm>
#include <random>

#include <Eigen/Core>
#include <Eigen/SVD>
#include "dynet/dim.h"
#include "dynet/globals.h"
#include "dynet/tensor.h"

using namespace std;

namespace dynet {

void orthonormal_random(unsigned dd, float g, Tensor& x) {
  Tensor t;
  t.d = Dim({dd, dd});
  t.v = new float[dd * dd];
  normal_distribution<float> distribution(0, 0.01);
  auto b = [&] () {return distribution(*rndeng);};
  generate(t.v, t.v + dd*dd, b);
  Eigen::JacobiSVD<Eigen::MatrixXf> svd(*t, Eigen::ComputeFullU);
  *x = svd.matrixU();
  delete[] t.v;
}

}

