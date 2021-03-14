#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include "cxxopts.hpp"
#include "data.h"

cxxopts::ParseResult arg;
cxxopts::Options options("./cryptoanalysis <file>", "KIV/BIT task 2 - program for breaking substitution ciphers");
std::unordered_set<std::string> dictionary;

std::string outputFile;
std::vector<std::string> solution;
double maxScore = 0;
std::string solutionKey;

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
        solutionKey = key;
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

std::string decodeMonoalphabetic(std::string cipher, std::string key) {
    std::string plain = "";
    for (char c : cipher) {
        if (c == ' ')
            plain += " ";
        else
            plain += std::string(1, key[c-'a']);
    }
    return plain;
}

double scoreMonoAlphabeticKey(std::string cipher, std::string key) {
    double score = 0;
    std::string plain = decodeMonoalphabetic(cipher, key);
    for (int i = 0; i < (int)plain.length() - 2; i++) {
        std::string pair = plain.substr(i, 2);
        if (pairScores_EN.find(pair) != pairScores_EN.end())
            score += pairScores_EN[pair];
        else
            score -= MONOALPHABETIC_MISSMATHING_SCORE;
    }
    return score;
}

std::pair<std::string, double> scrambleKey(std::string cipher, std::string currentKey) {
    double currentScore = scoreMonoAlphabeticKey(cipher, currentKey);
    int failures = 0;

    while (failures < MONOALPHABETIC_ATTEMPTS_NUMBER_OF_NOT_IMPROVEMENTS) {
        failures++;
        int p1 = rand() % ALPHABET_SIZE;
        int p2 = rand() % ALPHABET_SIZE;
        
        std::string newKey = currentKey;
        std::swap(newKey[p1], newKey[p2]);
        
        double newScore = scoreMonoAlphabeticKey(cipher, newKey);
        if (newScore > currentScore) {
            currentScore = newScore;
            currentKey = newKey;
            failures = 0;
        }
    }
    return {currentKey, currentScore};
}

std::string swapAllLetters(std::string text, char x, char y) {
    for (char c : text) {
        if (c == x) x = y;
        if (c == y) c = x;
    }
    return text;
}

std::string modifyAlphabeticKey(std::string currentKey, int index, char replacement) {
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (currentKey[i] == replacement) {
            std::swap(currentKey[i], currentKey[index]);
            break;
        }
    return currentKey;
}

std::string refineKey(std::string cipher, std::string currentKey) {
    std::string word = "";
    std::string plainText = decodeMonoalphabetic(cipher, currentKey);

    int replacements[ALPHABET_SIZE][ALPHABET_SIZE];
    memset(replacements, 0, sizeof(replacements));

    for (int i = 0; i < (int)plainText.length(); i++) {
        if (plainText[i] == ' ') {
            if (!dictionary.count(word) && word.length() > 2) {
                for (int j = 0; j < (int)word.length(); j++) {
                    for (int k = 0; k < ALPHABET_SIZE; k++) {
                        std::string newWord = swapAllLetters(word, word[j], currentKey[k]);
                        if (dictionary.count(newWord)) {
                            replacements[word[j]-'a'][currentKey[k]-'a']++;
                            replacements[currentKey[k]-'a'][word[j]-'a']++;
                        }
                    }
                    for (int k = 0; k < ALPHABET_SIZE; k++)
                        if (replacements[word[j]-'a'][currentKey[k]-'a'] > MONOALPHABETIC_MODIFY_KEY_THRESHOLD)
                            return modifyAlphabeticKey(currentKey, k, word[j]);
                }
            }
            word = "";
        } else {
            word += plainText[i];
        }
    }
    return currentKey;
}

void crackMonoAlphabetic(std::vector<std::string> &cipher) {
    std::cout << "starting cracking mono-alphabetic cipher...\n";
    std::string cipherText = "";
    for (std::string word : cipher)
        cipherText += word + " ";
    cipherText.pop_back();

    std::string bestKey = "";
    std::string monoAlphaKey = "abcdefghijklmnopqrstuvwxyz";
    double bestScore = scoreMonoAlphabeticKey(cipherText, monoAlphaKey);

    std::cout << "finding randomly the best key...\n";
    for (int i = 0; i < MONOALPHABETIC_ATTEMPTS_TO_SCRAMBLE_KEY; i++) {
        std::cout << "(" << (i+1) << "/" << MONOALPHABETIC_ATTEMPTS_TO_SCRAMBLE_KEY << ")..." << std::flush;
        std::random_shuffle(monoAlphaKey.begin(), monoAlphaKey.end());
        auto scrambled = scrambleKey(cipherText, monoAlphaKey);
        if (scrambled.second > bestScore) {
            bestScore = scrambled.second;
            bestKey = scrambled.first;
        }
        std::cout << "DONE\n";
    }
    monoAlphaKey = bestKey;

    std::cout << "improving the best key found...\n";
    for (int i = 0; i < MONOALPHABETIC_ATTEMPTS_TO_REFINE_KEY; i++) {
        std::string newKey = refineKey(cipherText, monoAlphaKey);
        if (newKey == monoAlphaKey)
            break;
        monoAlphaKey = newKey;
    }
    std::string plainText = decodeMonoalphabetic(cipherText, monoAlphaKey);
    std::vector<std::string> plain;
    std::string word = "";

    for (int i = 0; i < (int)plainText.length(); i++) {
        if (plainText[i] == ' ') {
            plain.push_back(word);
            word = "";
        } else {
            word += plainText[i];
        }
    }
    double score = evaluate(plain);
    if (score > maxScore) {
        maxScore = score;
        solution = plain;
        solutionKey = monoAlphaKey;
    }
    double percentage = evaluateEnglishText(plain, false) * 100;
    std::cout << "dictionary words: " << percentage << "%\n";
}

void crack(std::vector<std::string> &cipher) {
    static int counter = 1;
    std::cout << "starting cracking " << counter << ". cipher...\n";
    counter++;

    maxScore = evaluate(cipher);
    solution = cipher;
    std::cout << "the given text scores: " << maxScore << " and contains " << (evaluateEnglishText(cipher, false) * 100) << "% dictionary words\n";

    crackVigenere(cipher);
    crackMonoAlphabetic(cipher);

    std::cout << "------------------------------------------------------\n";
    std::cout << "the best guess " << (evaluateEnglishText(solution, false) * 100) << "% dictionary words\n";
    std::cout << "guessed key: "   << solutionKey << "\n";
    std::cout << "------------------------------------------------------\n";

    std::ofstream out(arg["output"].as<std::string>(), std::ios::app);
    
    bool print = arg["print"].as<bool>();
    if (print)
        std::cout << "plain text:\n";

    for (std::string word : solution) {
        if (print)
            std::cout << word << " ";
        out << word << " ";
    }
    out << "\n#\n";
    out.close();
    std::cout << "\n\n";
}

int main(int argc, char **argv) {
    srand(time(NULL));

    options.add_options()
        ("d,dictionary", "text file containing English words on separate lines", cxxopts::value<std::string>())
        ("k,keylen", "preferred key length to be used by the program", cxxopts::value<uint32_t>())
        ("o,output", "output file to print out the result", cxxopts::value<std::string>()->default_value("result.txt"))
        ("p,print", "print the decoded plain text on the screen", cxxopts::value<bool>()->default_value("false"))
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

    std::string word = "";
    std::vector<std::string> cipher;

    while (inputFile >> word) {
        if (word == CIPHER_SEPARATOR) {
            crack(cipher);
            cipher.clear();
            continue;
        }
        cipher.push_back(word);
    }
    if (!cipher.empty())
         crack(cipher);
    return 0;
}