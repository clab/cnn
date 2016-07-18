#include "cnn/cnn.h"
#include "cnn/shadow-params.h"
#include "cnn/tensor.h"
#include "cnn/aligned-mem-pool.h"
#include "cnn/model.h"

using namespace std;

namespace cnn {

ShadowParameters::ShadowParameters(const ParameterStorage& p) : h(p.values) {
  h.v = (float*)default_device->mem->malloc(h.d.size() * sizeof(float));
  TensorTools::Zero(h);
}

ShadowParameters::~ShadowParameters() {
  default_device->mem->free(h.v);
}

ShadowLookupParameters::ShadowLookupParameters(const LookupParameterStorage& lp) : h(lp.values) {
  for (auto& t : h) {
    t.v = (float*)default_device->mem->malloc(t.d.size() * sizeof(float));
    TensorTools::Zero(t);
  }
}

ShadowLookupParameters::~ShadowLookupParameters() {
  for (auto& t : h)
    default_device->mem->free(t.v);
}

vector<ShadowParameters> AllocateShadowParameters(const Model& m) {
  vector<ShadowParameters> v;
  v.reserve(m.parameters_list().size());
  for (auto& p : m.parameters_list())
    v.emplace_back(*p);
  return v;
}

vector<ShadowLookupParameters> AllocateShadowLookupParameters(const Model& m) {
  vector<ShadowLookupParameters> v;
  v.reserve(m.lookup_parameters_list().size());
  for (auto& p : m.lookup_parameters_list())
    v.emplace_back(*p);
  return v;
}

} // namespace cnn

