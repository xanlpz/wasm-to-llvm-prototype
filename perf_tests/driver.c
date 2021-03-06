/*
// Copyright (c) 2015 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Main driver for perf tests, this is for the moment OVERLY simple and not doing anything you would
//  want for real performance tests but we start here.

// All tests should be defining these methods:
int init_wasm(int);
int run_wasm(int);
void* init_c(int);
int run_c(void*, int);
int wasm_llvm_init();

void print_time(struct timespec* start, struct timespec *end, int iterations) {
  time_t start_s = start->tv_sec;
  time_t end_s = end->tv_sec;
  long start_ns = start->tv_nsec;
  long end_ns = end->tv_nsec;

  double start_time = start_s * 1000000000.0 + start_ns;
  double end_time = end_s * 1000000000.0 + end_ns;
  double average = (end_time - start_time) / ((double) iterations);

  printf("Time between in average is %f\n", average);
}

int main(int argc, char** argv) {
  int i;
  struct timespec start, end;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s [-w] [-c] <value>\n", argv[0]);
    fprintf(stderr, "\t -w only execute wasm\n");
    fprintf(stderr, "\t -c only execute c\n");
    return EXIT_FAILURE;
  }

  // Ok normally I would do this better with option handling but hacking it for now.
  if (argc >= 4) {
    fprintf(stderr, "Too many arguments, it really should be only -w or -c and a value\n");
  }

  // By default, we run both.
  int value_idx = 1;
  int should_run_wasm = 1;
  int should_run_c = 1;
  int meta = 10;

  if (argc == 3) {
    if (strcmp(argv[1], "-w") == 0) {
      should_run_c = 0;
    }
    if (strcmp(argv[1], "-c") == 0) {
      should_run_wasm = 0;
    }

    // It has to be one or the other.
    assert(should_run_wasm == 0 || should_run_c == 0);
    value_idx = 2;
  }

  // Get the parameter.
  char* endptr = NULL;
  int value = strtol(argv[value_idx], &endptr, 0);

  if (endptr == NULL || *endptr != '\0') {
    fprintf(stderr, "Problem with argument, should be integer %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  // Initialize the wasm module.
  fprintf(stderr, "Initialization of the wasm module\n");
  wasm_llvm_init();

  // Always init: C part might rely on it.
  fprintf(stderr, "Call the Wasm Test Initialization\n");
  init_wasm(value);

  fprintf(stderr, "Initialization done\n");

  // First run the wasm version.
  if (should_run_wasm) {
    printf("Running wasm with %d\n", value);
    // Let us run the wasm version a certain number of times.
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (i = 0; i < meta; i++) {
      run_wasm(value);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    print_time(&start, &end, meta);
  }

  if (should_run_c) {
    printf("Running C with %d\n", value);
    void* data = init_c(value);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (i = 0; i < meta; i++) {
      run_c(data, value);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    print_time(&start, &end, meta);
  }

  return EXIT_SUCCESS;
}
