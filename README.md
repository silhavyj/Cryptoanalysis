
# KIV/BIT task 02 - Cryptoanalysis
A tool from breaking substitution ciphers - Caesar cipher, Vigenere cipher, ROT13, and Mono-alphabetic. If the text has not been encrypted at all. The program will classify it as an already cracked solution.
Since some of the algorithms use `rand()` function, the program may need to be run multiple times if you don't see your expected result. Particularly, this is due to the algorithm cracking Mono-alphabetic ciphers. It generates random keys and sometimes it may generate a key that does produce English words but they might not make sense. With two or three runs of the program though, you should get the expected result.

## Compilation

The compilation process is done thorough the `make`command that's supposed to be executed in the root folder of the project structure. Once the process has completed, a file called `cryptanalysis` will be generated. This file represents the executable file of the application.

## Execution

### help

```

> ./cryptanalysis --help
KIV/BIT task 2 - program for breaking substitution ciphers
Usage:
  ./cryptoanalysis <file> [OPTION...]

  -d, --dictionary arg  text file containing English words on separate
                        lines
  -k, --keylen arg      preferred key length to be used by the program
  -o, --output arg      output file to print out the result (default:
                        result.txt)
  -p, --print           print decoded plain text on the screen
  -h, --help            print help
>
```
### input
The program takes one required parameter which happens to the the file containing different ciphers to be cracked. The ciphers are supposed to be separated by the `#` character.
```cmd
gibberish test 1
#
gibberish test 2
#
gibberish test 3
```
Also, each of the ciphers is supposed to be consist of characters `[a-z]` and white spaces only.

### example
```cmd
./cryptanalysis --help
./cryptanalysis ciphers.txt -p -d words/01.txt -k 5 -o plain.txt
./cryptanalysis ciphers.txt -d words/01.txt
./cryptanalysis ciphers.txt
```

### dictionary
The dictionary file is supposed to contain English words, where each word is on a new line. One dictionary comes prepacked with the project - `words/01.txt`. Although using a dictionary is not required, it helps the program come up with a more precise solution.
```
author
cat
dog
house
.
.
.
```
### output
The user has the option to specify the output file `-o output.txt`.  If they decide not to want to use this option, the default `result.txt` will be used. The structure of the output file is similar to the input one. It contains cracked ciphers separated by the `#` character
```cmd
cracked cipher 1
#
cracked cipher 2
#
cracked cipher 3
#
```
## Testing
The default `cipher.txt` file contains scrambled text encrypted by the following techniques:

* ROT13
* Mono-alphabetic substitution cipher
* Vigenere cipher (key=biden)
* English plain text (the output should be the same as there if no encryption technique used)
* Vigenere cipher (key=trytofindmeimwellhiden)
* Caesar cipher (shift=20)

## frequency analysis
The program takes advantage of the ROT13 being a variant of the Caesar cipher and the Caesar cipher being a variant of the Vigenere cipher. Therefore, the program treats all of these ciphers as the Vigenere cipher.

### Cracking the Vigenere cipher
#### Guessing the key length
If the user did not specify a preferred key length, the program will attempt to guess what the actual key length might be using the following algorithm. Otherwise, the program will skip this part and move on to finding the best solution for the specific key length.
#####  Finding "coincidences" in the cipher text
A "coincidences" of line `n` is the number of matching characters at the same indexes of the original cipher text and the same cipher text shifted by `n` spaces to the right. 
```
01234567
--------
abcdefgh (cipher text >> 0)
 abcdefg (cipher text >> 1)
  abcdef (cipher text >> 2)
   abcde (cipher text >> 3)
    abcd (cipher text >> 4)
     abc (cipher text >> 5)
      ab (cipher text >> 6)
       a (cipher text >> 7)
```
##### looking for peaks
Once this is done for all the possible shifts, an array of coincidences may look like `[20 1 3 2 19 2 6 1 21 ...]`. Within this array, the algorithm focuses on so-called peaks, which are outstanding numbers in terms of how big the numbers are. In this case, we can see that a large number occurs after every 4 steps. Therefore, the key length might be assumed to be of a length of 4.
However, the guessed key length 4 may not be the correct one. We need to look for all the possible patterns in the peaks, so we can then try out multiple possible options as to what the key length might be. The program defines a peak as a number greater than 1/20 of the length of the cipher text.

##### getting possible key lengths
Using a map, the program keeps track of how often each distance between two peaks occur, so they could be sorted by their probabilities. Once the distances have been sorted, the program will find the best solution for all of the possible key lengths.

#### Finding  the best solution for a specific key length
##### break up the cipher text into groups
Let's assume that the key length is 3, then every 3rd letter must have been shifted by the same value. The same rule applies for every 4th as well as every 5th letter, since the key length is 3.
##### counting letter frequency
When finding out the first letter of the key, the program counts the letter frequency of a string composed only of every 3rd letter of the cipher text. This frequency is then compared to the letter frequency of the English language. 
##### finding the right shift
As we know that the letter has been shifted, we need to try all possible shifts of this letter frequency, and for each of  them work out a score that will tell us how precise this rotation is compared to the English letter frequency. 
```
shift = 0
  a    b    c	 d
0.1  0.2  0.3  0.4 <- current shift of letter frequencies
0.3  0.4  0.1  0.8 <- letter frequency of the English language
score = (0.1 * 0.3) + (0.2 * 0.4) + (0.3 * 0.1) + (0.4 * 0.8)
score = 0.46
```
```
shift = 1
  b    c    d    a
0.2  0.3  0.4  0.1 <- current shift of letter frequencies
0.3  0.4  0.1  0.8 <- letter frequency of the English language
score = (0.2 * 0.3) + (0.3 * 0.4) + (0.4 * 0.1) + (0.1 * 0.8)
score = 0.30
```
The shift with the highest score is the closest one to the actual letter frequency of the English language. Thus, we know how many times we need to shift the first letter in order to get the first letter of the key. The same approach is used for the remaining letters of the key as well.

#### Overall evaluation
The way cracked cipher is evaluated is based on two factors - the number of English words it contains if a dictionary is provided and the letter frequency compered to the English one. These values are then multiplied together producing a final score of the cracked cipher. The letter frequency part is worked out as sum of multiplications of frequencies of individual letters in the two sets - the English language and the cracked (plain) text. The cracked cipher with the highest score is then pronounced as the solution of breaking the cipher.

### Cracking the Mono-alphabetic cipher
#### Random guess on  the key
For the Mono-alphabetic cipher there is a set of scores of individual two-letter words in the English language that is used throughout the decryption process.
```c++
std::unordered_map<std::string, double> pairScores_EN = {
	{"di",-0.992358},
	{" n",-0.474797},
	{"ci",-2.27922},
	.
	.
	.
};
``` 
The algorithm starts off with the key being `"abcdefghijklmnopqrstuvwxyz"`. As a first step, it will attempt to some random shuffling on the key and for each of them calculate its score based on the two-letter words occurring in the text after decryption. Within each shuffling, it will randomly swap two positions of in the key. If the key has improved, it will store it. If the key has not improved 300 times in a row, it will move on to another shuffle.
As an outcome, there is a key that has the highest probability of being the right key out of all the shuffles.
`"abcdefghijklmnopqrstuvwxyz" -> "wsrqgyfonzvtijbakphdeulxcm"`. All the constants used for this are defined in `src/data.h`  
##### Random
This part uses the `rand()` function which may affect the final solution. It may not always be entirely the same or it may overweight other algorithms and claim that it has come up with a better solution. This is due the randomness within the algorithm. As a solution, if you don't see your expected result, try running the program again. In up to three runs or so you should see the correct one.
#### Second narrow down of the key
As a second step, the algorithm 
