#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>

#include "cxxopts.hpp"
#include "data.h"

#define VIGENER_PEEK_CONSTANT 20

cxxopts::ParseResult arg;
cxxopts::Options options("./cryptoanalysis <file>", "KIV/BIT task 2 - program for breaking substitution ciphers");
std::unordered_set<std::string> dictionary;

std::string outputFile;
std::vector<std::string> solution;
double maxScore = 0;

double evaluateEnglishText(std::vector<std::string> &text, bool printUnknownWords) {
    int knownWords = 0;
    for (std::string word : text) {
        if (dictionary.find(word) != dictionary.end())
            knownWords++;
        else if (printUnknownWords)
            std::cout << "unknown: '" << word << "'\n";
    }
    return ((double)knownWords / text.size());
}

double evaluateLetterFrequency(std::vector<std::string> &text) {
    int count = 0;
    double freq[ALPHABET_SIZE] = {0};
    for (std::string word : text)
        for (char c : word) {
            freq[c-'a']++;
            count++;
        }

    double score = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freq[i] = (double)freq[i] / count;
        score += freq[i] * lettersFrequencies_EN[i];
    }
    return score;
}

int numberOfOccurrences(std::string txt, std::string pattern) {
    int occurrences = 0;
    std::string::size_type pos = 0;

    while ((pos = txt.find(pattern, pos)) != std::string::npos) {
        ++ occurrences;
        pos += pattern.length();
    }
    return occurrences;
}

double evaluateNGramFrequency(std::vector<std::string> &text) {
    std::string txt = "";
    for (std::string word : text)
        txt += word;
    
    double score1 = 0;
    for (int i = 0; i < (int)trigrams.size(); i++) {
        double freq = (double)numberOfOccurrences(txt, trigrams[i]) / txt.length()  / 3;
        score1 += trigramsFrequencies[i] * freq;
    }

    double score2 = 0;
    for (int i = 0; i < (int)bigrams.size(); i++) {
        double freq = (double)numberOfOccurrences(txt, bigrams[i]) / txt.length()  / 2;
        score2 += bigramsFrequencies[i] * freq;
    }
    return score1 * score2;
}

double evaluate(std::vector<std::string> &text) {
    double p2 = evaluateLetterFrequency(text);
    double p3 = evaluateNGramFrequency(text);
    if (arg.count("dictionary")) {
        double p1 = evaluateEnglishText(text, false);
        return p1 * p2 * p3 * 1e10;
    }
    return p2 * p3 * 1e10;
}

void crackVigenere(const std::string &cipherText, const std::vector<std::string> &cipher, uint32_t keyLen) {
    std::string key(keyLen,'\0');

    for (int i = 0; i < (int)keyLen; i++) {
        double freq[ALPHABET_SIZE] = {0};

        for (int j = i; j < (int)cipherText.length(); j += keyLen)
            freq[cipherText[j]-'a']++;

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            freq[j] *= keyLen;
            freq[j] = (double)freq[j] / cipherText.length();
        }

        int shift = 0;
        double maxMultiplication = 0;

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            double multiplication = 0;
            for (int k = 0; k < ALPHABET_SIZE; k++)
                multiplication += lettersFrequencies_EN[k] * freq[(j+k) % ALPHABET_SIZE];

            if (multiplication > maxMultiplication) {
                maxMultiplication = multiplication;
                shift = j;
            }
        }
        key[i] = (char)('a' + shift);
    }

    std::string plainText = cipherText;
    for (int i = 0; i < (int)keyLen; i++) {
        for (int j = i; j < (int)plainText.length(); j += keyLen)
            plainText[j] = (char)('a' + (plainText[j] - key[i] + 26) % 26);
    }
    int pos = 0;
    std::vector<std::string> plain;
    for (std::string word : cipher) {
        plain.push_back(plainText.substr(pos, word.length()));
        pos += word.length();
    }
    
    double score = evaluate(plain);
    if (score > maxScore) {
        maxScore = score;
        solution = plain;
    }
    std::cout << "vigenere cipher (key length: " << keyLen << "): "; 
    std::cout << "guessed key: '" << key << "', ";
    std::cout << "score: " << score << ", ";
    std::cout << "dictionary words: " << (evaluateEnglishText(plain, false) * 100) << "%\n";
}

void crackVigenere(std::vector<std::string> &cipher) {
    std::string text = "";
    for (std::string word : cipher)
        text += word;

    if (arg.count("keylen"))
        crackVigenere(text, cipher, arg["keylen"].as<uint32_t>());
    else {
        std::vector<std::string> matrix;
        for (int i = 1; i < (int)text.length(); i++)
            matrix.push_back(text.substr(0, text.length() - i));

        std::vector<int> coincidences;
        for (int i = 1; i < (int)matrix.size(); i++) {
            int count = 0;
            int k = matrix[0].size() - 1;
            int j = matrix[i].size() - 1; 

            for (; j >= 0; j--, k--)
                count += matrix[i][j] == matrix[0][k];
            coincidences.push_back(count);
        }

        std::vector<std::pair<int, int>> peeks;
        for (int i = 0; i < (int)coincidences.size(); i++)
            if (coincidences[i] >= (int)text.size() / VIGENER_PEEK_CONSTANT)
                peeks.push_back({coincidences[i], i});
        
        std::unordered_map<int, int> peekDistances;
        for (int i = 1; i < (int)peeks.size(); i++)
            peekDistances[peeks[i].second - peeks[i-1].second]++;

        std::vector<int> keyLengths;
        for (auto p : peekDistances)
            keyLengths.push_back(p.first);

        sort(keyLengths.begin(), keyLengths.end(), [&](auto &x, auto &y) {
            return peekDistances[x] > peekDistances[y];
        });

        std::cout << "guessed key lengths: ";
        for (int keyLen : keyLengths)
            std::cout << keyLen << " ";
        std::cout << '\n';

        for (int keyLen : keyLengths)
            crackVigenere(text, cipher, keyLen);
    }
}

void crack(std::vector<std::string> &cipher) {
    static int counter = 1;
    std::cout << "starting cracking " << counter << ". cipher...\n";
    counter++;

    maxScore = evaluate(cipher);
    solution = cipher;
    std::cout << "the given text scores: " << maxScore << " and contains " << (evaluateEnglishText(cipher, false) * 100) << "% dictionary words\n";

    crackVigenere(cipher);

    std::ofstream out(arg["output"].as<std::string>(), std::ios::app);
    std::cout << "plain text:\n";
    for (std::string word : solution) {
        std::cout << word << " ";
        out << word << " ";
    }
    out << "\n#\n";
    out.close();
    std::cout << "\n\n";
}

int main(int argc, char **argv) {
    options.add_options()
        ("d,dictionary", "text file containing English words on separate lines", cxxopts::value<std::string>())
        ("k,keylen", "preferred key length to be used by the program", cxxopts::value<uint32_t>())
        ("o,output", "output file to print out the result", cxxopts::value<std::string>()->default_value("result.txt"))
        ("h,help", "print help")
    ;
    if (argc < 2) {
        std::cout << "ERR: input file not specified! The first parameter must be the file\nwith ciphers split up by '#'\n";
        std::cout << options.help() << std::endl;
        return 1;
    }
    std::string cipherFile(argv[1]);
    
    arg = options.parse(argc, argv);
    if (arg.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    std::ifstream inputFile;
    if (arg.count("dictionary")) {
        std::string dictionaryFile = arg["dictionary"].as<std::string>();
        inputFile = std::ifstream(dictionaryFile);
        if (!inputFile) {
            std::cout << "ERR: dictionary file not found!\n";
            return 1;
        }
        std::string word;
        while (inputFile >> word) {
            transform(word.begin(), word.end(), word.begin(), ::tolower); 
            dictionary.insert(word);
        }
        inputFile.close();
    }
    
    inputFile = std::ifstream(cipherFile);
    if (!inputFile) {
        std::cout << "ERR: input file not found!\n";
        return 1;
    }

    outputFile = arg["output"].as<std::string>();

    char c;
    std::string word = "";
    std::vector<std::string> cipher;

    while (inputFile.get(c)) {
        if (c == '\n' || c == '\r')
            continue;
        if (c == CIPHER_SEPARATOR) {
            if (word != " " && word != "")
                cipher.push_back(word);
            crack(cipher);
            cipher.clear();
            word = "";
            continue;
        }
        if (isspace(c)) {
            if (word != " " && word != "")
                cipher.push_back(word);
            word = "";
        } else {
            word += c;
        }
    }
    if (!cipher.empty())
         crack(cipher);
    return 0;
}