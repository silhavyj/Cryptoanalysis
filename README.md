
# KIV/BIT task 02 - Cryptoanalysis !

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