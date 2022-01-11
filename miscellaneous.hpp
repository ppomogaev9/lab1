#pragma once
#include <string>
#include <vector>

std::string rand_str_gen_by_mask(std::string, uint32_t = 20);

std::string random_name_generator(uint32_t = 20);

std::string random_string_generator(uint32_t = 20);

std::string random_num_generator(uint32_t = 20);

void print_vector(std::vector<int>& statistic);

int32_t integrate(std::vector<int>& V);

void print_deviation(std::vector<int>& statistic);

struct AverageDeviationDescriptor;

AverageDeviationDescriptor calculate_daviation(std::vector<int>& statistic);

void print_average_deviation_description(std::vector<int> stat);

bool check_contains(const uint32_t num_of_true_str, const uint32_t num_of_fake_str, const uint32_t len = 20);
