#include <dynet_c/config.h>

#include <vector>

#include <dynet/expr.h>
#include <dynet_c/internal.h>
#include <dynet_c/expr.h>

using dynet_c::internal::to_c_ptr;
using dynet_c::internal::to_cpp_ptr;
using dynet_c::internal::to_c_ptr_from_value;

#define DYNET_C_IMPL_UNARY_FUNC(name, cpp_func) \
DYNET_C_STATUS dynetApply##name( \
    const dynetExpression_t *x, dynetExpression_t **newobj) try { \
  DYNET_C_CHECK_NOT_NULL(x); \
  DYNET_C_CHECK_NOT_NULL(newobj); \
  *newobj = to_c_ptr_from_value(dynet::cpp_func(*to_cpp_ptr(x))); \
  return DYNET_C_OK; \
} DYNET_C_HANDLE_EXCEPTIONS \

DYNET_C_STATUS dynetCreateExpression(dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr(new dynet::Expression());
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetDeleteExpression(dynetExpression_t *expr) try {
  DYNET_C_CHECK_NOT_NULL(expr);
  delete to_cpp_ptr(expr);
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetGetExpressionDim(
    const dynetExpression_t *expr, const dynetDim_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(expr);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr(&to_cpp_ptr(expr)->dim());
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetGetExpressionValue(
    const dynetExpression_t *expr, const dynetTensor_t **tensor) try {
  DYNET_C_CHECK_NOT_NULL(expr);
  DYNET_C_CHECK_NOT_NULL(tensor);
  *tensor = to_c_ptr(&to_cpp_ptr(expr)->value());
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetGetExpressionGradient(
    const dynetExpression_t *expr, const dynetTensor_t **tensor) try {
  DYNET_C_CHECK_NOT_NULL(expr);
  DYNET_C_CHECK_NOT_NULL(tensor);
  *tensor = to_c_ptr(&to_cpp_ptr(expr)->gradient());
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyInputScalar(
    dynetComputationGraph_t *g, float s, dynetDevice_t *device,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(newobj);
  dynet::Device *device_ptr = device ?
      to_cpp_ptr(device) : dynet::default_device;
  *newobj = to_c_ptr_from_value(dynet::input(*to_cpp_ptr(g), s, device_ptr));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyInput(
    dynetComputationGraph_t *g, const dynetDim_t *d, const float *data,
    size_t n, dynetDevice_t *device, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(data);
  DYNET_C_CHECK_NOT_NULL(newobj);
  dynet::Device *device_ptr = device ?
      to_cpp_ptr(device) : dynet::default_device;
  *newobj = to_c_ptr_from_value(dynet::input(
      *to_cpp_ptr(g), *to_cpp_ptr(d), std::vector<float>(data, data + n),
      device_ptr));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyInputSparse(
    dynetComputationGraph_t *g, const dynetDim_t *d, const uint32_t *ids,
    size_t n_ids, const float *data, size_t n_data, float defdata,
    dynetDevice_t *device, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(ids);
  DYNET_C_CHECK_NOT_NULL(data);
  DYNET_C_CHECK_NOT_NULL(newobj);
  dynet::Device *device_ptr = device ?
      to_cpp_ptr(device) : dynet::default_device;
  *newobj = to_c_ptr_from_value(dynet::input(
      *to_cpp_ptr(g), *to_cpp_ptr(d), std::vector<uint32_t>(ids, ids + n_ids),
      std::vector<float>(data, data + n_data), defdata, device_ptr));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyOneHot(
    dynetComputationGraph_t *g, uint32_t d, const uint32_t *ids, size_t n,
    dynetDevice_t *device, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(ids);
  DYNET_C_CHECK_NOT_NULL(newobj);
  dynet::Device *device_ptr = device ?
      to_cpp_ptr(device) : dynet::default_device;
  *newobj = to_c_ptr_from_value(dynet::one_hot(
      *to_cpp_ptr(g), d, std::vector<uint32_t>(ids, ids + n), device_ptr));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyParameter(
    dynetComputationGraph_t *g, dynetParameter_t *p,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(p);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::parameter(*to_cpp_ptr(g), *to_cpp_ptr(p)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyLookupParameter(
    dynetComputationGraph_t *g, dynetLookupParameter_t *lp,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(lp);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::parameter(*to_cpp_ptr(g), *to_cpp_ptr(lp)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyConstParameter(
    dynetComputationGraph_t *g, dynetParameter_t *p,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(p);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::const_parameter(*to_cpp_ptr(g), *to_cpp_ptr(p)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyConstLookupParameter(
    dynetComputationGraph_t *g, dynetLookupParameter_t *lp,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(lp);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::const_parameter(*to_cpp_ptr(g), *to_cpp_ptr(lp)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyLookupOne(
    dynetComputationGraph_t *g, dynetLookupParameter_t *p, uint32_t index,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(p);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::lookup(*to_cpp_ptr(g), *to_cpp_ptr(p), index));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyLookup(
    dynetComputationGraph_t *g, dynetLookupParameter_t *p,
    const uint32_t *indices, size_t n, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(p);
  DYNET_C_CHECK_NOT_NULL(indices);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::lookup(*to_cpp_ptr(g), *to_cpp_ptr(p),
      std::vector<uint32_t>(indices, indices + n)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyConstLookupOne(
    dynetComputationGraph_t *g, dynetLookupParameter_t *p, uint32_t index,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(p);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::const_lookup(*to_cpp_ptr(g), *to_cpp_ptr(p), index));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyConstLookup(
    dynetComputationGraph_t *g, dynetLookupParameter_t *p,
    const uint32_t *indices, size_t n, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(p);
  DYNET_C_CHECK_NOT_NULL(indices);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::const_lookup(*to_cpp_ptr(g), *to_cpp_ptr(p),
      std::vector<uint32_t>(indices, indices + n)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyZeros(
    dynetComputationGraph_t *g, const dynetDim_t *d,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::zeros(*to_cpp_ptr(g), *to_cpp_ptr(d)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyOnes(
    dynetComputationGraph_t *g, const dynetDim_t *d,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::ones(*to_cpp_ptr(g), *to_cpp_ptr(d)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyConstant(
    dynetComputationGraph_t *g, const dynetDim_t *d, float val,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::constant(*to_cpp_ptr(g), *to_cpp_ptr(d), val));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyRandomNormal(
    dynetComputationGraph_t *g, const dynetDim_t *d, float mean, float stddev,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::random_normal(*to_cpp_ptr(g), *to_cpp_ptr(d), mean, stddev));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyRandomBernoulli(
    dynetComputationGraph_t *g, const dynetDim_t *d, float p, float scale,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::random_bernoulli(*to_cpp_ptr(g), *to_cpp_ptr(d), p, scale));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyRandomUniform(
    dynetComputationGraph_t *g, const dynetDim_t *d, float left, float right,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::random_uniform(*to_cpp_ptr(g), *to_cpp_ptr(d), left, right));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyRandomGumbel(
    dynetComputationGraph_t *g, const dynetDim_t *d, float mu, float beta,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(g);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::random_gumbel(*to_cpp_ptr(g), *to_cpp_ptr(d), mu, beta));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyNegative(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(-(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyAdd(
    const dynetExpression_t *x, const dynetExpression_t *y, 
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) + *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyAddConst(
    const dynetExpression_t *x, float y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) + y);
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyAddExpr(
    float x, const dynetExpression_t *y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(x + *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySubtract(
    const dynetExpression_t *x, const dynetExpression_t *y, 
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) - *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySubtractConst(
    const dynetExpression_t *x, float y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) - y);
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySubtractExpr(
    float x, const dynetExpression_t *y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(x - *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMultiply(
    const dynetExpression_t *x, const dynetExpression_t *y, 
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) * *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMultiplyConst(
    const dynetExpression_t *x, float y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) * y);
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMultiplyExpr(
    float x, const dynetExpression_t *y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(x * *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyDivide(
    const dynetExpression_t *x, const dynetExpression_t *y, 
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) / *to_cpp_ptr(y));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyDivideConst(
    const dynetExpression_t *x, float y, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(*to_cpp_ptr(x) / y);
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyAffineTransform(
    const dynetExpression_t *const *xs, size_t n,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(xs);
  DYNET_C_CHECK_NOT_NULL(newobj);
  const dynet::Expression *const *_xs = 
      reinterpret_cast<const dynet::Expression *const *>(xs);
  const std::vector<const dynet::Expression*> xs_p = 
      std::vector<const dynet::Expression*>(_xs, _xs + n);
  std::vector<dynet::Expression> xs_v;
  std::transform(xs_p.begin(), xs_p.end(), std::back_inserter(xs_v),
      [](const dynet::Expression *x) { return *x; });
  *newobj = to_c_ptr_from_value(dynet::affine_transform(xs_v));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySum(
    const dynetExpression_t *const *xs, size_t n,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(xs);
  DYNET_C_CHECK_NOT_NULL(newobj);
  const dynet::Expression *const *_xs = 
      reinterpret_cast<const dynet::Expression *const *>(xs);
  const std::vector<const dynet::Expression*> xs_p = 
      std::vector<const dynet::Expression*>(_xs, _xs + n);
  std::vector<dynet::Expression> xs_v;
  std::transform(xs_p.begin(), xs_p.end(), std::back_inserter(xs_v),
      [](const dynet::Expression *x) { return *x; });
  *newobj = to_c_ptr_from_value(dynet::sum(xs_v));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySumElems(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::sum_elems(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMomentElems(
    const dynetExpression_t *x, uint32_t r, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::moment_elems(*to_cpp_ptr(x), r));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMeanElems(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::mean_elems(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyStdElems(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::std_elems(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySumBatches(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::sum_batches(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMomentBatches(
    const dynetExpression_t *x, uint32_t r, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::moment_batches(*to_cpp_ptr(x), r));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMeanBatches(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::mean_batches(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyStdBatches(
    const dynetExpression_t *x, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::std_batches(*to_cpp_ptr(x)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplySumDim(
    const dynetExpression_t *x, const uint32_t *dims, size_t n_dims,
    DYNET_C_BOOL b, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(dims);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::sum_dim(
      *to_cpp_ptr(x), std::vector<uint32_t>(dims, dims + n_dims), b));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyCumsum(
    const dynetExpression_t *x, uint32_t d, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(d);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::cumsum(*to_cpp_ptr(x), d));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMomentDim(
    const dynetExpression_t *x, const uint32_t *dims, size_t n_dims,
    uint32_t r, DYNET_C_BOOL b, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(dims);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::moment_dim(
      *to_cpp_ptr(x), std::vector<uint32_t>(dims, dims + n_dims), r, b));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMeanDim(
    const dynetExpression_t *x, const uint32_t *dims, size_t n_dims,
    DYNET_C_BOOL b, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(dims);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::mean_dim(
      *to_cpp_ptr(x), std::vector<uint32_t>(dims, dims + n_dims), b));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyStdDim(
    const dynetExpression_t *x, const uint32_t *dims, size_t n_dims,
    DYNET_C_BOOL b, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(dims);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::std_dim(
      *to_cpp_ptr(x), std::vector<uint32_t>(dims, dims + n_dims), b));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyAverage(
    const dynetExpression_t *const *xs, size_t n,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(xs);
  DYNET_C_CHECK_NOT_NULL(newobj);
  const dynet::Expression *const *_xs = 
      reinterpret_cast<const dynet::Expression *const *>(xs);
  const std::vector<const dynet::Expression*> xs_p = 
      std::vector<const dynet::Expression*>(_xs, _xs + n);
  std::vector<dynet::Expression> xs_v;
  std::transform(xs_p.begin(), xs_p.end(), std::back_inserter(xs_v),
      [](const dynet::Expression *x) { return *x; });
  *newobj = to_c_ptr_from_value(dynet::average(xs_v));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_IMPL_UNARY_FUNC(Sqrt, sqrt);
DYNET_C_IMPL_UNARY_FUNC(Abs, abs);
DYNET_C_IMPL_UNARY_FUNC(Erf, erf);
DYNET_C_IMPL_UNARY_FUNC(Asin, asin);
DYNET_C_IMPL_UNARY_FUNC(Acos, acos);
DYNET_C_IMPL_UNARY_FUNC(Atan, atan);
DYNET_C_IMPL_UNARY_FUNC(Sin, sin);
DYNET_C_IMPL_UNARY_FUNC(Cos, cos);
DYNET_C_IMPL_UNARY_FUNC(Tan, tan);
DYNET_C_IMPL_UNARY_FUNC(Sinh, sinh);
DYNET_C_IMPL_UNARY_FUNC(Cosh, cosh);
DYNET_C_IMPL_UNARY_FUNC(Tanh, tanh);
DYNET_C_IMPL_UNARY_FUNC(Asinh, asinh);
DYNET_C_IMPL_UNARY_FUNC(Acosh, acosh);
DYNET_C_IMPL_UNARY_FUNC(Atanh, atanh);
DYNET_C_IMPL_UNARY_FUNC(Exp, exp);
DYNET_C_IMPL_UNARY_FUNC(Square, square);
DYNET_C_IMPL_UNARY_FUNC(Cube, cube);
DYNET_C_IMPL_UNARY_FUNC(LogSigmoid, log_sigmoid);
DYNET_C_IMPL_UNARY_FUNC(Lgamma, lgamma);
DYNET_C_IMPL_UNARY_FUNC(Log, log);
DYNET_C_IMPL_UNARY_FUNC(Logistic, logistic);
DYNET_C_IMPL_UNARY_FUNC(Rectify, rectify);

DYNET_C_STATUS dynetApplyElu(
    const dynetExpression_t *x, float alpha, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::elu(*to_cpp_ptr(x), alpha));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_IMPL_UNARY_FUNC(Selu, selu);

DYNET_C_STATUS dynetApplySilu(
    const dynetExpression_t *x, float beta, dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::silu(*to_cpp_ptr(x), beta));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_IMPL_UNARY_FUNC(Softsign, softsign);

DYNET_C_STATUS dynetApplyPow(
    const dynetExpression_t *x, const dynetExpression_t *y,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::pow(*to_cpp_ptr(x), *to_cpp_ptr(y)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyBmin(
    const dynetExpression_t *x, const dynetExpression_t *y,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::min(*to_cpp_ptr(x), *to_cpp_ptr(y)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyBmax(
    const dynetExpression_t *x, const dynetExpression_t *y,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::max(*to_cpp_ptr(x), *to_cpp_ptr(y)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyMax(
    const dynetExpression_t *const *xs, size_t n,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(xs);
  DYNET_C_CHECK_NOT_NULL(newobj);
  const dynet::Expression *const *_xs = 
      reinterpret_cast<const dynet::Expression *const *>(xs);
  const std::vector<const dynet::Expression*> xs_p = 
      std::vector<const dynet::Expression*>(_xs, _xs + n);
  std::vector<dynet::Expression> xs_v;
  std::transform(xs_p.begin(), xs_p.end(), std::back_inserter(xs_v),
      [](const dynet::Expression *x) { return *x; });
  *newobj = to_c_ptr_from_value(dynet::max(xs_v));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyDotProduct(
    const dynetExpression_t *x, const dynetExpression_t *y,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::dot_product(*to_cpp_ptr(x), *to_cpp_ptr(y)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyCircConv(
    const dynetExpression_t *u, const dynetExpression_t *v,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(u);
  DYNET_C_CHECK_NOT_NULL(v);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::circ_conv(*to_cpp_ptr(u), *to_cpp_ptr(v)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyCircCorr(
    const dynetExpression_t *u, const dynetExpression_t *v,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(u);
  DYNET_C_CHECK_NOT_NULL(v);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::circ_corr(*to_cpp_ptr(u), *to_cpp_ptr(v)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyCmult(
    const dynetExpression_t *x, const dynetExpression_t *y,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::cmult(*to_cpp_ptr(x), *to_cpp_ptr(y)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyCdiv(
    const dynetExpression_t *x, const dynetExpression_t *y,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(y);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(dynet::cdiv(*to_cpp_ptr(x), *to_cpp_ptr(y)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS

DYNET_C_STATUS dynetApplyColwiseAdd(
    const dynetExpression_t *x, const dynetExpression_t *bias,
    dynetExpression_t **newobj) try {
  DYNET_C_CHECK_NOT_NULL(x);
  DYNET_C_CHECK_NOT_NULL(bias);
  DYNET_C_CHECK_NOT_NULL(newobj);
  *newobj = to_c_ptr_from_value(
      dynet::colwise_add(*to_cpp_ptr(x), *to_cpp_ptr(bias)));
  return DYNET_C_OK;
} DYNET_C_HANDLE_EXCEPTIONS
