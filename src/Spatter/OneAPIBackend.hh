#ifndef ONEAPI_BACKEND_HH
#define ONEAPI_BACKEND_HH

#include <cstddef>

float oneapi_gather_wrapper(const size_t *pattern, const double *sparse,
    double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count);
float oneapi_scatter_wrapper(const size_t *pattern, double *sparse,
    const double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count);
float oneapi_scatter_atomic_wrapper(const size_t *pattern, double *sparse,
    const double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count);
float oneapi_scatter_gather_wrapper(const size_t *pattern_scatter,
    double *sparse_scatter, const size_t *pattern_gather,
    const double *sparse_gather, const size_t pattern_length,
    const size_t delta_scatter, const size_t delta_gather, const size_t wrap,
    const size_t count);
float oneapi_scatter_gather_atomic_wrapper(const size_t *pattern_scatter,
    double *sparse_scatter, const size_t *pattern_gather,
    const double *sparse_gather, const size_t pattern_length,
    const size_t delta_scatter, const size_t delta_gather, const size_t wrap,
    const size_t count);
float oneapi_multi_gather_wrapper(const size_t *pattern,
    const size_t *pattern_gather, const double *sparse, double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count);
float oneapi_multi_scatter_wrapper(const size_t *pattern,
    const size_t *pattern_scatter, double *sparse, const double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count);
float oneapi_multi_scatter_atomic_wrapper(const size_t *pattern,
    const size_t *pattern_scatter, double *sparse, const double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count);

#endif