#include <sycl/sycl.hpp>
#include <dpct/dpct.hpp>
#include <stdio.h>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "Configuration.hh"
#include <chrono>

#include <cmath>

#include <algorithm>

void oneapi_gather(const size_t *pattern, const double *sparse,
    double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  double x;

  if (i < count) {
    x = sparse[pattern[j] + delta * i];
    if (x == 0.5)
      dense[0] = x;
  }
}

void oneapi_scatter(const size_t *pattern, double *sparse,
    const double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  if (i < count)
    sparse[pattern[j] + delta * i] = dense[j + pattern_length * (i % wrap)];
}

void oneapi_scatter_atomic(const size_t *pattern, double *sparse,
    const double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  if (i < count)
    dpct::atomic_exchange<unsigned long long,
        sycl::access::address_space::generic_space>(
        (unsigned long long int *)&sparse[pattern[j] + delta * i],
        (unsigned long long)(sycl::bit_cast<long long>(
            dense[j + pattern_length * (i % wrap)])));
}

void oneapi_scatter_gather(const size_t *pattern_scatter,
    double *sparse_scatter, const size_t *pattern_gather,
    const double *sparse_gather, const size_t pattern_length,
    const size_t delta_scatter, const size_t delta_gather, const size_t wrap,
    const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  if (i < count)
    sparse_scatter[pattern_scatter[j] + delta_scatter * i] =
        sparse_gather[pattern_gather[j] + delta_gather * i];
}

void oneapi_scatter_gather_atomic(const size_t *pattern_scatter,
    double *sparse_scatter, const size_t *pattern_gather,
    const double *sparse_gather, const size_t pattern_length,
    const size_t delta_scatter, const size_t delta_gather, const size_t wrap,
    const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  if (i < count)
    dpct::atomic_exchange<unsigned long long,
        sycl::access::address_space::generic_space>(
        (unsigned long long int
                *)&sparse_scatter[pattern_scatter[j] + delta_scatter * i],
        (unsigned long long)(sycl::bit_cast<long long>(
            sparse_gather[pattern_gather[j] + delta_gather * i])));
}

void oneapi_multi_gather(const size_t *pattern,
    const size_t *pattern_gather, const double *sparse, double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  double x;

  if (i < count) {
    x = sparse[pattern[pattern_gather[j]] + delta * i];
    if (x == 0.5)
      dense[0] = x;
  }
}

void oneapi_multi_scatter(const size_t *pattern,
    const size_t *pattern_scatter, double *sparse, const double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  if (i < count)
    sparse[pattern[pattern_scatter[j]] + delta * i] =
        dense[j + pattern_length * (i % wrap)];
}

void oneapi_multi_scatter_atomic(const size_t *pattern,
    const size_t *pattern_scatter, double *sparse, const double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count, sycl::nd_item<3> item_ct1) {
  size_t total_id = (size_t)((size_t)item_ct1.get_local_range(2) *
          (size_t)item_ct1.get_group(2) +
      (size_t)item_ct1.get_local_id(2));
  size_t j = total_id % pattern_length; // pat_idx
  size_t i = total_id / pattern_length; // count_idx

  if (i < count)
    dpct::atomic_exchange<unsigned long long,
        sycl::access::address_space::generic_space>(
        (unsigned long long int
                *)&sparse[pattern[pattern_scatter[j]] + delta * i],
        (unsigned long long)(sycl::bit_cast<long long>(
            dense[j + pattern_length * (i % wrap)])));
}

float oneapi_gather_wrapper(const size_t *pattern, const double *sparse,
    double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_gather(pattern, sparse, dense, pattern_length, delta, wrap, count,
            item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_scatter_wrapper(const size_t *pattern, double *sparse,
    const double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_scatter(pattern, sparse, dense, pattern_length, delta, wrap, count,
            item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_scatter_atomic_wrapper(const size_t *pattern, double *sparse,
    const double *dense, const size_t pattern_length, const size_t delta,
    const size_t wrap, const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_scatter_atomic(pattern, sparse, dense, pattern_length, delta, wrap,
            count, item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_scatter_gather_wrapper(const size_t *pattern_scatter,
    double *sparse_scatter, const size_t *pattern_gather,
    const double *sparse_gather, const size_t pattern_length,
    const size_t delta_scatter, const size_t delta_gather, const size_t wrap,
    const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_scatter_gather(pattern_scatter, sparse_scatter, pattern_gather,
            sparse_gather, pattern_length, delta_scatter, delta_gather, wrap,
            count, item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_scatter_gather_atomic_wrapper(const size_t *pattern_scatter,
    double *sparse_scatter, const size_t *pattern_gather,
    const double *sparse_gather, const size_t pattern_length,
    const size_t delta_scatter, const size_t delta_gather, const size_t wrap,
    const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_scatter_gather_atomic(pattern_scatter, sparse_scatter,
            pattern_gather, sparse_gather, pattern_length, delta_scatter,
            delta_gather, wrap, count, item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_multi_gather_wrapper(const size_t *pattern,
    const size_t *pattern_gather, const double *sparse, double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_multi_gather(pattern, pattern_gather, sparse, dense,
            pattern_length, delta, wrap, count, item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_multi_scatter_wrapper(const size_t *pattern,
    const size_t *pattern_scatter, double *sparse, const double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_multi_scatter(pattern, pattern_scatter, sparse, dense,
            pattern_length, delta, wrap, count, item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}

float oneapi_multi_scatter_atomic_wrapper(const size_t *pattern,
    const size_t *pattern_scatter, double *sparse, const double *dense,
    const size_t pattern_length, const size_t delta, const size_t wrap,
    const size_t count) {
  dpct::device_ext &dev_ct1 = dpct::get_current_device();
  sycl::queue &q_ct1 = dev_ct1.default_queue();
  dpct::event_ptr start, stop;
  std::chrono::time_point<std::chrono::steady_clock> start_ct1;
  std::chrono::time_point<std::chrono::steady_clock> stop_ct1;

  start = new sycl::event();
  stop = new sycl::event();

  int threads_per_block = std::min(pattern_length, (size_t)1024);
  int blocks_per_grid =
      ((pattern_length * count) + threads_per_block - 1) / threads_per_block;

  dev_ct1.queues_wait_and_throw();

  start_ct1 = std::chrono::steady_clock::now();

  *stop = q_ct1.parallel_for(
      sycl::nd_range<3>(sycl::range<3>(1, 1, blocks_per_grid) *
              sycl::range<3>(1, 1, threads_per_block),
          sycl::range<3>(1, 1, threads_per_block)),
      [=](sycl::nd_item<3> item_ct1) {
        oneapi_multi_scatter_atomic(pattern, pattern_scatter, sparse, dense,
            pattern_length, delta, wrap, count, item_ct1);
      });

  stop->wait();
  stop_ct1 = std::chrono::steady_clock::now();

  float time_ms = 0;
  time_ms =
      std::chrono::duration<float, std::milli>(stop_ct1 - start_ct1).count();

  dpct::destroy_event(start);
  dpct::destroy_event(stop);

  return time_ms;
}