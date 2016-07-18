#include "cnn/model.h"
#include "cnn/tensor.h"
#include "cnn/aligned-mem-pool.h"
#include "cnn/cnn.h"

#include <unordered_set>
#include <iostream>

#include <fstream>
#include <sstream>

#ifndef __CUDACC__
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

// Macros for defining functions over parameters
// NOTE: This only works on the default device, as parameters are currently defined over default devices
#ifdef __CUDACC__
#define CNN_PARAMNORM_INST_DEV_IMPL(MyParam, regular_func, dev_func) \
  template void MyParam::dev_func<Device_GPU>(Device_GPU & dev, float *sqnorm) const;
#elif defined(HAVE_CUDA)
#define CNN_PARAMNORM_INST_DEV_IMPL(MyParam, regular_func, dev_func) \
  extern template void MyParam::dev_func<Device_GPU>(Device_GPU & dev, float *sqnorm) const; \
  template void MyParam::dev_func<Device_CPU>(Device_CPU & dev, float *sqnorm) const; \
  void MyParam::regular_func(float *sqnorm) const { \
    if(default_device->type == DeviceType::CPU) { dev_func(*(Device_CPU*)default_device,sqnorm); } \
    else if(default_device->type == DeviceType::GPU) { dev_func(*(Device_GPU*)default_device,sqnorm); } \
    else { abort(); } \
  }
#else
#define CNN_PARAMNORM_INST_DEV_IMPL(MyParam, regular_func, dev_func) \
  template void MyParam::dev_func<Device_CPU>(Device_CPU & dev, float *sqnorm) const; \
  void MyParam::regular_func(float *sqnorm) const { \
    if(default_device->type == DeviceType::CPU) { dev_func(*(Device_CPU*)default_device,sqnorm); } \
    else { abort(); } \
  }
#endif

using namespace std;

#ifndef __CUDACC__
BOOST_CLASS_EXPORT_IMPLEMENT(cnn::ParameterStorage)
BOOST_CLASS_EXPORT_IMPLEMENT(cnn::LookupParameterStorage)
#endif

namespace cnn {

// CPU only functions
#ifndef __CUDACC__

ParameterStorageBase::~ParameterStorageBase() {}

ParameterStorage::ParameterStorage(const Dim& d, float scale) : dim(d) {
  values.d = g.d = d;
  values.v = static_cast<float*>(default_device->ps->allocate(d.size() * sizeof(float)));
  values.device = g.device = default_device;
  if (scale) {
    TensorTools::Randomize(values, scale);
  }
  else {
    TensorTools::Randomize(values);
  }
  g.v = static_cast<float*>(default_device->ps->allocate(d.size() * sizeof(float)));
  TensorTools::Zero(g);
}

size_t ParameterStorage::size() const { return dim.size(); }

void ParameterStorage::zero() {
  TensorTools::Zero(values);
  clear();
}

void ParameterStorage::copy(const ParameterStorage & param) {
  assert(dim == param.dim);
  TensorTools::CopyElements(values, param.values);
}

void ParameterStorage::clear() {
  TensorTools::Zero(g);
}

LookupParameterStorage::LookupParameterStorage(unsigned n, const Dim& d) : dim(d), values(n), grads(n) {
  for (unsigned i = 0; i < n; ++i) {
    auto& v = values[i];
    v.d = d;
    v.v = static_cast<float*>(default_device->ps->allocate(d.size() * sizeof(float)));
    v.device = default_device;
    TensorTools::Randomize(v);

    auto& g = grads[i];
    g.d = d;
    g.v = static_cast<float*>(default_device->ps->allocate(d.size() * sizeof(float)));
    g.device = default_device;
    TensorTools::Zero(g);
  }
}

void LookupParameterStorage::zero() {
  for (auto& p : values)
    TensorTools::Zero(p);
  clear();
}

size_t LookupParameterStorage::size() const {
  return values.size() * dim.size();
}

void LookupParameterStorage::copy(const LookupParameterStorage& param) {
  assert(dim == param.dim);
  for(size_t i = 0; i < param.values.size(); ++i)
    TensorTools::CopyElements(values[i], param.values[i]);
}

void LookupParameterStorage::clear() {
  for (auto i : non_zero_grads)
    TensorTools::Zero(grads[i]);
  non_zero_grads.clear();
}

Parameter::Parameter() {
  mp = nullptr;
  index = 0;
}

Parameter::Parameter(const Model* mp, unsigned long index) : mp(mp), index(index) {}

ParameterStorage* Parameter::get() const {
  return mp->parameters_list()[index];
}

void Parameter::zero() {
  return mp->parameters_list()[index]->zero();
}

LookupParameter::LookupParameter() {
  mp = nullptr;
  index = 0;
}

LookupParameter::LookupParameter(const Model* mp, unsigned long index) : mp(mp), index(index) {}

LookupParameterStorage* LookupParameter::get() const {
  return mp->lookup_parameters_list()[index];
}

void LookupParameter::zero() {
  return mp->lookup_parameters_list()[index]->zero();
}

void LookupParameter::initialize(unsigned index, const std::vector<float>& val) const {
  get()->initialize(index, val);
}

Model::Model() : gradient_norm_scratch(nullptr) {
  weight_decay.SetLambda(weight_decay_lambda);
}

Model::~Model() {
  for (auto p : all_params) delete p;
  if(gradient_norm_scratch)
    default_device->mem->free(gradient_norm_scratch);
}

void Model::set_weight_decay_lambda(float lambda) {
  weight_decay.SetLambda(lambda);
}

void Model::project_weights(float radius) {
  static float* project_scratch = 0;
  if (!project_scratch)
    project_scratch = (float*)default_device->mem->malloc(all_params.size() * sizeof(float));
  int pi = 0;
  for (auto p : all_params) {
    p->squared_l2norm(&project_scratch[pi]);
    ++pi;
  }
  double gg = 0;
  for (int i = 0; i < pi; ++i)
    gg += project_scratch[i];
  cerr << "NORM: " << sqrt(gg) << endl;
}

Parameter Model::add_parameters(const Dim& d, float scale) {
  ParameterStorage* p = new ParameterStorage(d, scale);
  Parameter r(this, params.size());
  //cerr << "Adding parameters with dim " << d << endl;
  all_params.push_back(p);
  params.push_back(p);
  return r;
}

LookupParameter Model::add_lookup_parameters(unsigned n, const Dim& d) {
  LookupParameterStorage* p = new LookupParameterStorage(n,d);
  LookupParameter r(this, lookup_params.size());
  //cerr << "Adding lookup parameters with dim " << d << " and size " << n << endl;
  all_params.push_back(p);
  lookup_params.push_back(p);
  return r;
}

void Model::reset_gradient() {
  for (auto p : params) { p->clear(); }
  for (auto p : lookup_params) { p->clear(); }
}

size_t Model::parameter_count() const {
  size_t r = 0;
  for (const ParameterStorageBase* param : all_params) {
    r += param->size();
  }
  return r;
}

void save_cnn_model(std::string filename, Model* model) {
    std::ofstream out(filename);
    boost::archive::text_oarchive oa(out);
    oa << (*model);
};

void load_cnn_model(std::string filename, Model* model) {
    std::ifstream in(filename);
    boost::archive::text_iarchive ia(in);
    ia >> (*model);
};

#endif

// CPU/GPU code
// TODO: It's a bit annoying to re-implement the CPU/GPU control code for each
//       function, but it's not clear how to handle heterogeneous functions w/
//       macros

// Take the squared norm
template <class MyDevice>
void ParameterStorage::squared_l2norm_dev(MyDevice & dev, float* sqnorm) const {
  Tensor sqnorm_t({1}, sqnorm, &dev);
  sqnorm_t.t<0>().device(*dev.edevice) = values.tvec().square().sum();
}
CNN_PARAMNORM_INST_DEV_IMPL(ParameterStorage, squared_l2norm, squared_l2norm_dev)

// Take the squared norm of the gradient
template <class MyDevice>
void ParameterStorage::g_squared_l2norm_dev(MyDevice & dev, float* sqnorm) const {
  Tensor sqnorm_t({1}, sqnorm, &dev);
  sqnorm_t.t<0>().device(*dev.edevice) = g.tvec().square().sum();
}
CNN_PARAMNORM_INST_DEV_IMPL(ParameterStorage, g_squared_l2norm, g_squared_l2norm_dev)

template <class MyDevice>
void ParameterStorage::accumulate_grad_dev(MyDevice & dev, const Tensor& d) {
  g.tvec().device(*dev.edevice) += d.tvec();
}
#ifdef __CUDACC__
  template void ParameterStorage::accumulate_grad_dev<Device_GPU>(Device_GPU & dev, const Tensor& d);
#elif defined(HAVE_CUDA)
  extern template void ParameterStorage::accumulate_grad_dev<Device_GPU>(Device_GPU & dev, const Tensor& d);
  template void ParameterStorage::accumulate_grad_dev<Device_CPU>(Device_CPU & dev, const Tensor& d);
  void ParameterStorage::accumulate_grad(const Tensor& d) {
    if(values.device->type == DeviceType::CPU) { accumulate_grad_dev(*(Device_CPU*)values.device,d); }
    else if(values.device->type == DeviceType::GPU) { accumulate_grad_dev(*(Device_GPU*)values.device,d); }
    else { abort(); }
  }
#else
  template void ParameterStorage::accumulate_grad_dev<Device_CPU>(Device_CPU & dev, const Tensor& d);
  void ParameterStorage::accumulate_grad(const Tensor& d) {
    if(values.device->type == DeviceType::CPU) { accumulate_grad_dev(*(Device_CPU*)values.device,d); }
    else { abort(); }
  }
#endif

template <class MyDevice>
void ParameterStorage::scale_parameters_dev(MyDevice & dev, float a) {
  values.tvec().device(*dev.edevice) = values.tvec() * a;
}
#ifdef __CUDACC__
  template void ParameterStorage::scale_parameters_dev<Device_GPU>(Device_GPU & dev, float a);
#elif defined(HAVE_CUDA)
  extern template void ParameterStorage::scale_parameters_dev<Device_GPU>(Device_GPU & dev, float a);
  template void ParameterStorage::scale_parameters_dev<Device_CPU>(Device_CPU & dev, float a);
  void ParameterStorage::scale_parameters(float a) {
    if(values.device->type == DeviceType::CPU) { scale_parameters_dev(*(Device_CPU*)values.device,a); }
    else if(values.device->type == DeviceType::GPU) { scale_parameters_dev(*(Device_GPU*)values.device,a); }
    else { abort(); }
  }
#else
  template void ParameterStorage::scale_parameters_dev<Device_CPU>(Device_CPU & dev, float a);
  void ParameterStorage::scale_parameters(float a) {
    if(values.device->type == DeviceType::CPU) { scale_parameters_dev(*(Device_CPU*)values.device,a); }
    else { abort(); }
  }
#endif

template <class MyDevice>
void LookupParameterStorage::initialize_dev(MyDevice & dev, unsigned index, const vector<float>& val) {
  assert(int(val.size()) == int(dim.size()));
#ifdef __CUDACC__
  cudaMemcpyAsync(values[index].v, &val[0], val.size() * sizeof(float), cudaMemcpyHostToDevice);
#else
  memcpy(values[index].v, &val[0], val.size() * sizeof(float));
#endif
}
#ifdef __CUDACC__
  template void LookupParameterStorage::initialize_dev<Device_GPU>(Device_GPU & dev, unsigned index, const vector<float>& val);
#elif defined(HAVE_CUDA)
  extern template void LookupParameterStorage::initialize_dev<Device_GPU>(Device_GPU & dev, unsigned index, const vector<float>& val);
  template void LookupParameterStorage::initialize_dev<Device_CPU>(Device_CPU & dev, unsigned index, const vector<float>& val);
  void LookupParameterStorage::initialize(unsigned index, const vector<float>& val) {
    if(values[index].device->type == DeviceType::CPU) { initialize_dev(*(Device_CPU*)values[index].device,index,val); }
    else if(values[index].device->type == DeviceType::GPU) { initialize_dev(*(Device_GPU*)values[index].device,index,val); }
    else { abort(); }
  }
#else
  template void LookupParameterStorage::initialize_dev<Device_CPU>(Device_CPU & dev, unsigned index, const vector<float>& val);
  void LookupParameterStorage::initialize(unsigned index, const vector<float>& val) {
    if(values[index].device->type == DeviceType::CPU) { initialize_dev(*(Device_CPU*)values[index].device,index,val); }
    else { abort(); }
  }
#endif

template <class MyDevice>
void LookupParameterStorage::squared_l2norm_dev(MyDevice & dev, float* sqnorm) const {
  Tensor sqnorm_t({1}, sqnorm, &dev);
  sqnorm_t.t<0>().device(*dev.edevice) = values[0].tvec().square().sum();
  for (unsigned i = 1; i < values.size(); ++i)
    sqnorm_t.t<0>().device(*dev.edevice) += values[i].tvec().square().sum();
}
CNN_PARAMNORM_INST_DEV_IMPL(LookupParameterStorage, squared_l2norm, squared_l2norm_dev)

template <class MyDevice>
void LookupParameterStorage::g_squared_l2norm_dev(MyDevice & dev, float* sqnorm) const {
  Tensor sqnorm_t({1}, sqnorm, &dev);
  auto it = non_zero_grads.begin();
  assert(it != non_zero_grads.end());
  sqnorm_t.t<0>().device(*dev.edevice) = grads[*(it++)].tvec().square().sum();
  while(it != non_zero_grads.end())
    sqnorm_t.t<0>().device(*dev.edevice) += grads[*(it++)].tvec().square().sum();
}
CNN_PARAMNORM_INST_DEV_IMPL(LookupParameterStorage, g_squared_l2norm, g_squared_l2norm_dev)

template <class MyDevice>
void LookupParameterStorage::accumulate_grad_dev(MyDevice & dev, unsigned index, const Tensor& d) {
  non_zero_grads.insert(index);
  grads[index].tvec().device(*dev.edevice) += d.tvec();
}
#ifdef __CUDACC__
  template void LookupParameterStorage::accumulate_grad_dev<Device_GPU>(Device_GPU & dev, unsigned index, const Tensor& d);
#elif defined(HAVE_CUDA)
  extern template void LookupParameterStorage::accumulate_grad_dev<Device_GPU>(Device_GPU & dev, unsigned index, const Tensor& d);
  template void LookupParameterStorage::accumulate_grad_dev<Device_CPU>(Device_CPU & dev, unsigned index, const Tensor& d);
  void LookupParameterStorage::accumulate_grad(unsigned index, const Tensor& d) {
    if(values[index].device->type == DeviceType::CPU) { accumulate_grad_dev(*(Device_CPU*)values[index].device,index,d); }
    else if(values[index].device->type == DeviceType::GPU) { accumulate_grad_dev(*(Device_GPU*)values[index].device,index,d); }
    else { abort(); }
  }
#else
  template void LookupParameterStorage::accumulate_grad_dev<Device_CPU>(Device_CPU & dev, unsigned index, const Tensor& d);
  void LookupParameterStorage::accumulate_grad(unsigned index, const Tensor& d) {
    if(values[index].device->type == DeviceType::CPU) { accumulate_grad_dev(*(Device_CPU*)values[index].device,index,d); }
    else { abort(); }
  }
#endif

template <class MyDevice>
void LookupParameterStorage::scale_parameters_dev(MyDevice & dev, float a) {
  for (auto& p : values)
    p.tvec().device(*dev.edevice) = p.tvec() * a;
}
#ifdef __CUDACC__
  template void LookupParameterStorage::scale_parameters_dev<Device_GPU>(Device_GPU & dev, float a);
#elif defined(HAVE_CUDA)
  extern template void LookupParameterStorage::scale_parameters_dev<Device_GPU>(Device_GPU & dev, float a);
  template void LookupParameterStorage::scale_parameters_dev<Device_CPU>(Device_CPU & dev, float a);
  void LookupParameterStorage::scale_parameters(float a) {
    if(values[0].device->type == DeviceType::CPU) { scale_parameters_dev(*(Device_CPU*)values[0].device,a); }
    else if(values[0].device->type == DeviceType::GPU) { scale_parameters_dev(*(Device_GPU*)values[0].device,a); }
    else { abort(); }
  }
#else
  template void LookupParameterStorage::scale_parameters_dev<Device_CPU>(Device_CPU & dev, float a);
  void LookupParameterStorage::scale_parameters(float a) {
    if(values[0].device->type == DeviceType::CPU) { scale_parameters_dev(*(Device_CPU*)values[0].device,a); }
    else { abort(); }
  }
#endif

template <class MyDevice>
float Model::gradient_l2_norm_dev(MyDevice & dev) const {
  if (!gradient_norm_scratch)
    gradient_norm_scratch = (float*)default_device->mem->malloc((all_params.size()+1) * sizeof(float));
  size_t pi;
  for(pi = 0; pi < all_params.size(); ++pi)
    all_params[pi]->g_squared_l2norm(&gradient_norm_scratch[pi]);
  Tensor scratch_t({(unsigned int)all_params.size()}, gradient_norm_scratch, &dev);
  Tensor sum_t({1}, gradient_norm_scratch+pi, &dev);
  sum_t.t<0>().device(*dev.edevice) = scratch_t.t<1>().sum().sqrt();
#ifdef __CUDACC__
  float res = 0;
  cudaMemcpy(&res, gradient_norm_scratch, sizeof(float),  cudaMemcpyDeviceToHost);
  return res;
#else
  return gradient_norm_scratch[pi];
#endif
}
#ifdef __CUDACC__
  template float Model::gradient_l2_norm_dev<Device_GPU>(Device_GPU & dev) const;
#elif defined(HAVE_CUDA)
  extern template float Model::gradient_l2_norm_dev<Device_GPU>(Device_GPU & dev) const;
  template float Model::gradient_l2_norm_dev<Device_CPU>(Device_CPU & dev) const;
  float Model::gradient_l2_norm() const {
    if(default_device->type == DeviceType::CPU) { return gradient_l2_norm_dev(*(Device_CPU*)default_device); }
    else if(default_device->type == DeviceType::GPU) { return gradient_l2_norm_dev(*(Device_GPU*)default_device); }
    else { abort(); }
  }
#else
  template float Model::gradient_l2_norm_dev<Device_CPU>(Device_CPU & dev) const;
  float Model::gradient_l2_norm() const {
    if(default_device->type == DeviceType::CPU) { return gradient_l2_norm_dev(*(Device_CPU*)default_device); }
    else { abort(); }
  }
#endif

} // namespace cnn
