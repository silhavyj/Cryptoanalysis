
# KIV/BIT task 02 - Cryptoanalysis

# manual

## Compilation

The compilation process is done thorough the `make`command that's supposed to be executed in the root folder of the project structure. Once the process has completed, a file called `cryptanalysis` will be generated. This file represents the executable file of the application.

## Execution

### help

```cmd

> ./cryptanalysis --help
KIV/BIT task 2 - program for breaking substitution ciphers
Usage:
  ./cryptoanalysis <file> [OPTION...]

  -d, --dictionary arg  text file containing English words on separate
                        lines
  -k, --keylen arg      preferred key length to be used by the program
  -o, --output arg      output file to print out the result (default:
                        result.txt)
  -h, --help            print help
>
```
### input
The program takes one required parameter which happens to the the file containing different cipher to be cracked. The ciphers are supposed to be separated by the `#` character.
```cmd
gibberish test 1
#
gibberish test 2
#
gibberish test 3
```

### example
```cmd
./cryptanalysis ciphers.txt -d words/01.txt -k 5 -o plain.txt
./cryptanalysis ciphers.txt -d words/01.txt
./cryptanalysis ciphers.txt
```

### dictionary
The dictionary file is supposed to contain English words, where each word is on a new line. One dictionary comes prepacked with the project - `words/01.txt`. Although using a dictionary is not required, it helps the program come up with a more precise solution.
```cmd
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
* Mono-alphabetic substitution cipher (key=portugesmnfwaihydcblvkzxqj)
* Vigenere cipher (key=biden)
* English plain text (the output should be the same as there is no encryption technique used)
* Vigenere cipher (key=trytofindmeimwellhiden)
* Caesar cipher (shift=20)

## frequency analysis
The program takes advantage of the ROT13 being a variant of the Caesar cipher and the Caesar cipher being a special of the Vigenere cipher. Therefore, the program treats all of these ciphers as the Vigenere cipher.

### Cracking the Vigenere cipher
#### Guessing the key length
If the user did not specify a preferred key length, the program will attempt to guess what the actual key length might be using the following algorithm.
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
Once this is done for all the possible shifts, an array of coincidences may look like this : `[20 1 3 2 19 2 6 1 21]`. Within this array, the algorithm focuses on so-called peaks, which are outstanding numbers in terms of how big the number is. In this case, we can see that a large number occurs after every 4 steps. Therefore, the key length might be assumed to be 4.
However, the guessed key length 4 may not be the correct one. We need to look for all the possible patterns in the peaks, so we can then try out multiple possible options as to what the key length might be. The program first defines a peak as a number greater than 1/20 of the length of the cipher text.

##### getting possible key lengths
Using a map, the program keeps track of how often each distance between two peaks occurred, so they could be sorted by their probability. 
