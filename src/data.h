#ifndef _DATA_H_
#define _DATA_H_

#include <vector>

#define CIPHER_SEPARATOR "#"
#define ALPHABET_SIZE 26
#define VIGENER_PEEK_CONSTANT 20

static const double lettersFrequencies_EN[ALPHABET_SIZE] = {
    0.08167, 0.01492, 0.02782, 0.04253, 
	0.12702, 0.02228, 0.02015, 0.06094,
    0.06966, 0.00153, 0.00772, 0.04025,
    0.02406, 0.06749, 0.07507, 0.01929,
    0.00095, 0.05987, 0.06327, 0.09056,
    0.02758, 0.00978, 0.02360, 0.00150,
    0.01974, 0.00074
};

static const std::vector<std::string> trigrams = {
    "the", "and", "ing", "her",
    "hat", "his", "tha", "ere",
    "for", "ent", "ion", "ter",
    "was", "you", "ith", "ver", 
    "all", "wit", "thi", "tio"
};

static const std::vector<double> trigramsFrequencies = {
    0.03508, 0.01594, 0.01147, 0.00822,
    0.00651, 0.00597, 0.00594, 0.00561,
    0.00555, 0.00531, 0.00506, 0.00461,
    0.00460, 0.00437, 0.00431, 0.00431,
    0.00423, 0.00397, 0.00395, 0.00378
};

static const std::vector<std::string> bigrams = {
    "th", "he", "in", "er",
    "an", "re", "nd", "on",
    "en", "at", "ou", "ed",
    "ha", "to", "or", "it",
    "is", "hi", "es", "ng"
};

static const std::vector<double> bigramsFrequencies = {
    0.03883, 0.03681, 0.02284, 0.02178,
    0.02140, 0.01749, 0.01572, 0.01418,
    0.01383, 0.01336, 0.01285, 0.01276,
    0.01275, 0.01170, 0.01151, 0.01135,
    0.01110, 0.01092, 0.01092, 0.01053
};

#endif