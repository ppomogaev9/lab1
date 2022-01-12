#include "miscellaneous.hpp"
#include <string>
#include "randomize.hpp"
#include <iostream>
#include "hash_table.hpp"
#include "fstream"


namespace masks {
	const std::string name_mask = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
	const std::string string_mask = "\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22"
		"\23\24\25\26\27\30\31\32\33\34\35\36\37\40\41\42\43\44\45\46\47\50\51\52\53"
		"\54\55\56\57\60\61\62\63\64\65\66\67\70\71\72\73\74\75\76\77\100\101\102\103"
		"\104\105\106\107\110\111\112\113\114\115\116\117\120\121\122\123\124\125\126"
		"\127\130\131\132\133\134\135\136\137\140\141\142\143\144\145\146\147\150\151"
		"\152\153\154\155\156\157\160\161\162\163\164\165\166\167\170\171\172\173\174"
		"\175\176\177\200\201\202\203\204\205\206\207\210\211\212\213\214\215\216\217"
		"\220\221\222\223\224\225\226\227\230\231\232\233\234\235\236\237\240\241\242"
		"\243\244\245\246\247\250\251\252\253\254\255\256\257\260\261\262\263\264\265"
		"\266\267\270\271\272\273\274\275\276\277\300\301\302\303\304\305\306\307\310"
		"\311\312\313\314\315\316\317\320\321\322\323\324\325\326\327\330\331\332\333"
		"\334\335\336\337\340\341\342\343\344\345\346\347\350\351\352\353\354\355\356"
		"\357\360\361\362\363\364\365\366\367\370\371\372\373\374\375\376\377";
	const std::string num_mask = "0123456789";
	const int standard_len = 20;
	const std::string separator_for_tests = "+------------------------------------------------------------+";
}

using namespace masks;

/* using some string as mask, generates a random string
mask - aribtrary string, with length >= 0
len - length of output string */
std::string rand_str_gen_by_mask(std::string mask, uint32_t len)
{
	std::string result;
	if (len < 0)
		throw std::exception("you tried to make a string with negative length in rand_str_gen_by_mask");

	if (mask.length() == 0)
		throw std::exception("you passed empty mask to rand_str_gen_by_mask");

	Random randomizer(0, mask.length() - 1, len);

	while (len > 0) {
		result.push_back(mask.at(randomizer.get_random_num()));
		--len;
	}
	
	return result;
}

std::string random_name_generator(uint32_t len) try {
	return rand_str_gen_by_mask(name_mask, len);
} catch (std::exception exc) {
	std::cerr << "In function random_name_generator: " << exc.what() << std::endl;
	return std::string("");
}

std::string random_string_generator(uint32_t len) try {
	return rand_str_gen_by_mask(string_mask, len);
} catch (std::exception exc) {
	std::cerr << "In function random_string_generator: " << exc.what() << std::endl;
	return std::string("");
}

std::string random_num_generator(uint32_t len) try {
	std::string out_number = rand_str_gen_by_mask(num_mask, len);
	if ((out_number.length() != 0) && (out_number.at(0) == '0')) {
		Random random('0', '9');
		out_number[0] = random.get_random_num();
	}
	return out_number;
} catch (std::exception exc) {
	std::cerr << "In function random_num_generator: " << exc.what() << std::endl;
	return std::string("");
}

void print_vector(std::vector<int>& vector) {
	std::cout << std::endl << std::endl;
	std::cout << separator_for_tests << std::endl;
	std::cout << "printing array" << std::endl;
	for (int i = 0; i < vector.size(); ++i)
		std::cout << "V[" << i << "] = " << vector[i] << std::endl;
	std::cout << separator_for_tests << std::endl;
	std::cout << std::endl;
}

int integrate(std::vector<int>& V) {
	int sum = 0;
	for (auto x : V) {
		sum += x;
	}
	return sum;
}

void print_deviation(std::vector<int>& statistic) {
	if (statistic.size() == 0)
		return;

	std::cout << std::endl << std::endl;
	std::cout << separator_for_tests << std::endl;
	std::cout << "printing average deviation" << std::endl << std::endl;

	int middle_value = integrate(statistic) / statistic.size();

	std::cout << "average = " << middle_value << std::endl << std::endl;
	for (int i = 0; i < statistic.size(); ++i) {
		double deviation_percantage = (statistic[i] - middle_value) / static_cast<double>(middle_value);
		deviation_percantage *= 100;
		std::cout << "deviation[" << i << "] = " << deviation_percantage << " %" << std::endl;
	}
	std::cout << separator_for_tests << std::endl;
	std::cout << std::endl;
}

struct AverageDeviationDescriptor {
	int middle_value = 0;
	double max_deviation_abs = 0.0;
	double min_deviation_abs = 0.0;
	double average_deviation = 0.0;
};

AverageDeviationDescriptor calculate_daviation(std::vector<int>& statistic) {
	if (statistic.size() == 0)
		return AverageDeviationDescriptor();

	AverageDeviationDescriptor output;

	int middle_value = integrate(statistic) / statistic.size();
	output.middle_value = middle_value;

	double min_deviation = INFINITY;
	double max_deviation = 0.0;
	double sum_deviation = 0.0;

	for (int i = 0; i < statistic.size(); ++i) {
		double deviation = (statistic[i] - middle_value) / static_cast<double>(middle_value);
		if (abs(deviation) > max_deviation)
			max_deviation = deviation;
		if (abs(deviation) < min_deviation)
			min_deviation = deviation;
		sum_deviation += deviation;
	}

	output.average_deviation = sum_deviation / statistic.size();

	std::cout << std::endl;

	return output;
}

void print_average_deviation_description(std::vector<int> stat) {
	AverageDeviationDescriptor desc = calculate_daviation(stat);
	std::cout << std::endl << std::endl;
	std::cout << separator_for_tests << std::endl;
	std::cout << "vector statistic: " << std::endl;
	std::cout << "\taverage: " << desc.middle_value << std::endl;
	printf("\tabsolute value of minimum deviation from the average: %.15lf\n", desc.min_deviation_abs);
	printf("\tabsolute value of maximum deviation from the average: %.15lf\n", desc.max_deviation_abs);
	printf("\taverage deviation from the average: %.15lf\n", desc.average_deviation);
	std::cout << separator_for_tests << std::endl;
	std::cout << std::endl;
}

bool check_contains(const uint32_t num_of_true_str, const uint32_t num_of_fake_str, const uint32_t len) {
	if (len < 20) {
		return false;
	}

	if (num_of_true_str == 0) {
		HashTable D;
		for (uint32_t i = 0; i < num_of_fake_str; ++i) {
			Key k = random_string_generator(len + 1);
			if (D.contains(k)) {
				return false;
			}
		}
		return true;
	}

	std::vector<Key> v;
	for (uint32_t i = 0; i < num_of_true_str; ++i) {
		v.push_back(random_string_generator(len));
	}

	HashTable t;
	for (uint32_t i = 0; i < num_of_true_str; ++i)
		t.insert(v[i], Value(Key("")));

	for (uint32_t i = 0; i < num_of_true_str; ++i) {
		if (!t.contains(v[i])) {
			return false;
		}
	}

	for (uint32_t i = 0; i < num_of_fake_str; ++i) {
		Key test = random_string_generator(len + 1);
		if (t.contains(test)) {
			return false;
		}
	}

	Random randomizer(0, 1, num_of_fake_str + num_of_true_str);
	Random random_two(0, num_of_true_str - 1, num_of_true_str);
	for (uint32_t i = 0; i < num_of_fake_str + num_of_true_str; ++i) {
		if (randomizer.get_random_num()) {
			int id = random_two.get_random_num();
			if (!t.contains(v[id])) {
				return false;
			}
		}
		else {
			std::string test = random_string_generator(len + 1);
			if (t.contains(test)) {
				return false;
			}
		}
	}
	return true;
}
