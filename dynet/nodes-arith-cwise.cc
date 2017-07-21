#include "dynet/nodes-arith-cwise.h"

#include "dynet/nodes-macros.h"

using namespace std;

namespace dynet {

// ************* CwiseMultiply *************

#ifndef __CUDACC__

string CwiseMultiply::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " \\cdot " << arg_names[1];
  return s.str();
}

Dim CwiseMultiply::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in CwiseMultiply")
  Dim d = xs[0].truncate();
  DYNET_ARG_CHECK(d.single_batch() == xs[1].truncate().single_batch(),
                          "Mismatched input dimensions in CwiseMultiply: " << xs);
  d.bd = max(xs[1].bd, d.bd);
  return d;
}

int CwiseMultiply::autobatch_sig(const ComputationGraph & cg, SigMap &sm) const {
  // TODO: This does not handle the case where dimensions differ
  Sig s(nt::cmult);
  return cg.nodes[args[0]]->dim == cg.nodes[args[1]]->dim ? sm.get_idx(s) : 0;
}

std::vector<int> CwiseMultiply::autobatch_concat(const ComputationGraph & cg) const {
  return vector<int>(2, 1);
}

#endif

template<class MyDevice>
void CwiseMultiply::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  // convention: 1st argument will be broadcasted (expr.cc code should take care of passing in the right order)
  DYNET_ASSERT(xs.size() == 2, "Failed dimension check in CwiseMultiply::forward (cmult)");
  if(xs[0]->d.size() == xs[1]->d.size()){
    fx.tvec().device(*dev.edevice) = xs[0]->tvec() * xs[1]->tvec();
  } else {
    Eigen::array<int, 5> bcast = {1,1,1,1,1};
    for(int di=0; di<xs[0]->d.nd; di++){
      if(xs[0]->d[di]==1) bcast[di] = xs[1]->d[di];
    }
    if(xs[0]->d.bd == 1) bcast[4] = xs[1]->d.bd;
    fx.tb<4>().device(*dev.edevice) = xs[0]->tb<4>().broadcast(bcast) * xs[1]->tb<4>();
  }
}

template<class MyDevice>
void CwiseMultiply::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  DYNET_ASSERT(i < 2, "Failed dimension check in CwiseMultiply::backward (cmult)");
  if(i == 1) {
    if(xs[0]->d.size() == xs[1]->d.size()){
      dEdxi.tvec().device(*dev.edevice) += dEdf.tvec() * xs[0]->tvec();
    } else {
      Eigen::array<int, 5> bcast = {1,1,1,1,1};
      for(int di=0; di<xs[0]->d.nd; di++){
        if(xs[0]->d[di]!=xs[1]->d[di]) bcast[di] = xs[1]->d[di];
      }
      if(xs[0]->d.bd!=xs[1]->d.bd) bcast[4] = xs[1]->d.bd;
      dEdxi.tb<4>().device(*dev.edevice) += dEdf.tb<4>() * xs[0]->tb<4>().broadcast(bcast);
    }
  } else {
    int n_red = xs[0]->d.bd!=xs[1]->d.bd?1:0;
    for(int di=0;di<xs[0]->d.nd; di++) if(xs[0]->d[di]!=xs[1]->d[di]) n_red++;
    DYNET_ASSERT(n_red < 5, "Unsupported number of reductions check in CwiseMultiply::backward (cmult)");

    if(n_red==0)      dEdxi.tvec().device(*dev.edevice) += dEdf.tvec() * xs[1-i]->tvec();
    else if(n_red==1) backward_helper<MyDevice, 1>(dev, xs, fx, dEdf, i, dEdxi);
    else if(n_red==2) backward_helper<MyDevice, 2>(dev, xs, fx, dEdf, i, dEdxi);
    else if(n_red==3) backward_helper<MyDevice, 3>(dev, xs, fx, dEdf, i, dEdxi);
    else if(n_red==4) backward_helper<MyDevice, 4>(dev, xs, fx, dEdf, i, dEdxi);
  }
}

template<class MyDevice, int ReductionOrder>
void CwiseMultiply::backward_helper(const MyDevice & dev,
	                             const vector<const Tensor*>& xs,
	                             const Tensor& fx,
	                             const Tensor& dEdf,
	                             unsigned i,
	                             Tensor& dEdxi) const {
  Eigen::array<int, ReductionOrder> red_axis;
  red_axis[ReductionOrder-1] = 4;
  int curr_red_axis = 0;
  for(int di=0;di<xs[0]->d.nd; di++){
    if(xs[0]->d[di]!=xs[1]->d[di]){
      red_axis[curr_red_axis] = di;
      curr_red_axis++;
    }
  }
  Eigen::array<int, 5> morph = {1,1,1,1,1};
  for(int di=0; di<xs[0]->d.nd; di++){
    morph[di] = xs[i]->d[di];
  }
  morph[4] = xs[i]->d.bd;
  dEdxi.tb<4>().device(*dev.edevice) += (dEdf.tb<4>() * xs[1]->tb<4>()).sum(red_axis).reshape(morph);
}

// ************* CwiseQuotient *************

#ifndef __CUDACC__

string CwiseQuotient::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " / " << arg_names[1];
  return s.str();
}

Dim CwiseQuotient::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in CwiseQuotient")
  Dim d = xs[0].truncate();
  DYNET_ARG_CHECK(d.single_batch() == xs[1].truncate().single_batch(), "Bad input dimensions in CwiseQuotient: " << xs);
  d.bd = max(xs[1].bd, d.bd);
  return d;
}

#endif

template<class MyDevice>
void CwiseQuotient::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  DYNET_ASSERT(xs.size() == 2, "Failed dimension check in CwiseQuotient::forward (cdiv)");
  if(xs[0]->d.size() == xs[1]->d.size()) {
    fx.tvec().device(*dev.edevice) = xs[0]->tvec() / xs[1]->tvec();
  } else if(xs[0]->d.size() < xs[1]->d.size()) {
    Eigen::array<int, 5> bcast = {1,1,1,1,1};
    for(int di=0; di<xs[0]->d.nd; di++){
      if(xs[0]->d[di]==1) bcast[di] = xs[1]->d[di];
    }
    if(xs[0]->d.bd == 1) bcast[4] = xs[1]->d.bd;
    fx.tb<4>().device(*dev.edevice) = xs[0]->tb<4>().broadcast(bcast) / xs[1]->tb<4>();
  } else {
    Eigen::array<int, 5> bcast = {1,1,1,1,1};
    for(int di=0; di<xs[0]->d.nd; di++){
      if(xs[1]->d[di]==1) bcast[di] = xs[0]->d[di];
    }
    if(xs[1]->d.bd == 1) bcast[4] = xs[0]->d.bd;
    fx.tb<4>().device(*dev.edevice) = xs[0]->tb<4>() / xs[1]->tb<4>().broadcast(bcast);
  }
}

template<class MyDevice>
void CwiseQuotient::backward_dev_impl(const MyDevice & dev,
                             const vector<const Tensor*>& xs,
                             const Tensor& fx,
                             const Tensor& dEdf,
                             unsigned i,
                             Tensor& dEdxi) const {
  DYNET_ASSERT(i < 2, "Failed dimension check in CwiseQuotient::backward (cdiv)");
  if (i == 0) {
    if(xs[0]->d.size() == xs[1]->d.size()) {
      dEdxi.tvec().device(*dev.edevice) += dEdf.tvec() / xs[1]->tvec();
    } else if(xs[1]->d.size() < xs[0]->d.size()) {
      Eigen::array<int, 5> bcast = {1,1,1,1,1};
      for(int di=0; di<xs[0]->d.nd; di++){
        if(xs[0]->d[di]!=xs[1]->d[di]) bcast[di] = xs[0]->d[di];
      }
      if(xs[0]->d.bd!=xs[1]->d.bd) bcast[4] = xs[0]->d.bd;
      dEdxi.tb<4>().device(*dev.edevice) += dEdf.tb<4>() / xs[1]->tb<4>().broadcast(bcast);
    } else {
      int n_red = xs[0]->d.bd!=xs[1]->d.bd?1:0;
      for(int di=0;di<xs[0]->d.nd; di++) if(xs[0]->d[di]!=xs[1]->d[di]) n_red++;
      DYNET_ASSERT(n_red < 5, "Unsupported number of reductions check in CwiseQuotient::backward (cdiv)");
      if(n_red==1) backward_helper<MyDevice, 1>(dev, xs, fx, dEdf, i, dEdxi);
      else if(n_red==2) backward_helper<MyDevice, 2>(dev, xs, fx, dEdf, i, dEdxi);
      else if(n_red==3) backward_helper<MyDevice, 3>(dev, xs, fx, dEdf, i, dEdxi);
      else if(n_red==4) backward_helper<MyDevice, 4>(dev, xs, fx, dEdf, i, dEdxi);
    }
  } else { // i = 1
    if(xs[0]->d.size() == xs[1]->d.size()) {
      dEdxi.tvec().device(*dev.edevice) -= dEdf.tvec() / xs[1]->tvec().square() * xs[0]->tvec();
    } else if(xs[1]->d.size() < xs[0]->d.size()) {
      int n_red = xs[0]->d.bd!=xs[1]->d.bd?1:0;
      for(int di=0;di<xs[0]->d.nd; di++) if(xs[0]->d[di]!=xs[1]->d[di]) n_red++;
      DYNET_ASSERT(n_red < 5, "Unsupported number of reductions check in CwiseQuotient::backward (cdiv)");
      if(n_red==1) backward_helper<MyDevice, 1>(dev, xs, fx, dEdf, i, dEdxi);
      else if(n_red==2) backward_helper<MyDevice, 2>(dev, xs, fx, dEdf, i, dEdxi);
      else if(n_red==3) backward_helper<MyDevice, 3>(dev, xs, fx, dEdf, i, dEdxi);
      else if(n_red==4) backward_helper<MyDevice, 4>(dev, xs, fx, dEdf, i, dEdxi);
    } else {
      Eigen::array<int, 5> bcast = {1,1,1,1,1};
      for(int di=0; di<xs[0]->d.nd; di++){
        if(xs[0]->d[di]!=xs[1]->d[di]) bcast[di] = xs[1]->d[di];
      }
      if(xs[0]->d.bd!=xs[1]->d.bd) bcast[4] = xs[1]->d.bd;
      dEdxi.tb<4>().device(*dev.edevice) -= dEdf.tb<4>() / xs[1]->tb<4>().square() * xs[0]->tb<4>().broadcast(bcast);
    }
  }
}
DYNET_NODE_INST_DEV_IMPL(CwiseQuotient)

template<class MyDevice, int ReductionOrder>
void CwiseQuotient::backward_helper(const MyDevice & dev,
		       const std::vector<const Tensor*>& xs,
		       const Tensor& fx,
		       const Tensor& dEdf,
		       unsigned i,
		       Tensor& dEdxi) const {
  Eigen::array<int, ReductionOrder> red_axis;
  red_axis[ReductionOrder-1] = 4;
  int curr_red_axis = 0;
  for(int di=0;di<xs[0]->d.nd; di++){
    if(xs[0]->d[di]!=xs[1]->d[di]){
      red_axis[curr_red_axis] = di;
      curr_red_axis++;
    }
  }
  Eigen::array<int, 5> morph = {1,1,1,1,1};
  for(int di=0; di<xs[0]->d.nd; di++){
    morph[di] = xs[i]->d[di];
  }
  morph[4] = xs[i]->d.bd;
  if (i == 0) {
    // case xs[1]->d.size() > xs[0]->d.size()
    dEdxi.tb<4>().device(*dev.edevice) += (dEdf.tb<4>() / xs[1]->tb<4>()).sum(red_axis).reshape(morph);
  } else {
    // case xs[1]->d.size() < xs[0]->d.size()
    Eigen::array<int, 5> bcast = {1,1,1,1,1};
    for(int di=0; di<xs[0]->d.nd; di++){
      if(xs[0]->d[di]!=xs[1]->d[di]) bcast[di] = xs[0]->d[di];
    }
    if(xs[0]->d.bd!=xs[1]->d.bd) bcast[4] = xs[0]->d.bd;
    dEdxi.tb<4>().device(*dev.edevice) -= (dEdf.tb<4>() / xs[1]->tb<4>().square().eval().broadcast(bcast) * xs[0]->tb<4>()).sum(red_axis).reshape(morph);
  }

}

// ************* Pow *************

#ifndef __CUDACC__

string Pow::as_string(const vector<string>& arg_names) const {
  ostringstream s;
  s << arg_names[0] << " ** " << arg_names[1];
  return s.str();
}

Dim Pow::dim_forward(const vector<Dim>& xs) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed input count check in Pow")
  Dim d = xs[0].truncate();
  DYNET_ARG_CHECK(xs[1].truncate().single_batch().size() == 1, "Bad input dimensions in Pow: " << xs);
  return d;
}

#endif

template<class MyDevice>
void Pow::forward_dev_impl(const MyDevice & dev, const vector<const Tensor*>& xs, Tensor& fx) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed dimension check in Pow::forward");
  fx.tvec().device(*dev.edevice) = xs[0]->tvec().pow(as_scalar(*xs[1]));
}

template<class MyDevice>
void Pow::backward_dev_impl(const MyDevice & dev,
                            const vector<const Tensor*>& xs,
                            const Tensor& fx,
                            const Tensor& dEdf,
                            unsigned i,
                            Tensor& dEdxi) const {
  DYNET_ARG_CHECK(xs.size() == 2, "Failed dimension check in Pow::backward");
  real x2 = as_scalar(*xs[1]);
  if (i == 0) {
    dEdxi.tvec().device(*dev.edevice) += xs[0]->tvec().pow(x2 - 1) * dEdf.tvec() * x2;
  } else {
#if defined(__CUDACC__) && defined(EIGEN_NO_MALLOC)
    DYNET_RUNTIME_ERR("CUDA memory allocation in Pow");
#endif
    // y = a^x
    // dy/dx = a^x * log(a)
    dEdxi.t<0>().device(*dev.edevice) += (fx.tvec() * xs[0]->tvec().log() * dEdf.tvec()).sum();
  }
}
DYNET_NODE_INST_DEV_IMPL(Pow)

}
