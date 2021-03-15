
# KIV/BIT task 02 - Cryptoanalysis
A tool for breaking substitution ciphers - Caesar cipher, Vigenere cipher, ROT13, and Mono-alphabetic. If the text has not been encrypted at all, the program will classify it as an already cracked solution.
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
Using a map, the program keeps track of how often each distance between two peaks occurs, so they could be sorted by their probabilities. Once the distances have been sorted, the program will find the best solution for all of the possible key lengths.

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
The algorithm starts off with the key being `"abcdefghijklmnopqrstuvwxyz"`. As a first step, it will attempt to do some random shuffling on the key and for each of them calculate its score based on the two-letter words occurring in the text after decryption. Within each shuffle, it will randomly swap two characters in the key. If the key has improved, the program will store it. If the key has not improved 300 times in a row, the algorithm will move on to another shuffle.
As an outcome, there is a key that has the highest probability of being the right key out of all the shuffles.
`"abcdefghijklmnopqrstuvwxyz" -> "wsrqgyfonzvtijbakphdeulxcm"`. All the constants used for this are defined in `src/data.h`  
##### Random
This part uses the `rand()` function which may affect the final solution. It may not always be entirely the same or it may overweight other algorithms and claim that it has come up with a better solution. This is due the randomness within the algorithm. As a solution, if you don't see your expected result, try running the program again. In up to three runs or so you should see the correct one.
#### Second narrow-down of the key
As a second step, the algorithm focuses on other possible swaps of letters that could be performed on the key. Within each attempt to further improve the key, it will go over unknown words (those that have not been found in dictionary) and try to move some letters around in order to get a word that does exist in the dictionary. However, the program does not perform swap of letters on the key right away. Instead, it keeps track of how many each letter would be worth swapping with some other one. If the same pair is requested to swap more than 5 times, the swap itself will be performed. This value can be changed in `src/data.h`
After all the swaps have been done, a new best key will be returned. If the key has indeed improved, it will replace the current one. The program keeps trying to improve the current best key until either the key has not changed or all the attempts to improve the key have been carried out.
#### Overall evaluation
In the end the transformation of the key may look like this:
```
initialization:     "abcdefghijklmnopqrstuvwxyz"
after the 1st step: "wsrqgyfonzvtijbakphdeulxcm"
after the 2nd step: "msrqgkfonzvtijbaychdeulxpw"
```
Once the cipher has been decrypted using the final key,  the same process of evaluation will be used in order to find out whether this approach overweights the other ones and could be considered as the final solution.

## Examples of RUN's of the program
### Example (1)
```
starting cracking 3. cipher...
the given text scores: 8.18002 and contains 19.8718% dictionary words
guessed key lengths: 5 10 15 20 40 75 4
vigenere cipher (key length: 5): guessed key: 'biden', score: 65.0317, dictionary words: 97.1154%
vigenere cipher (key length: 10): guessed key: 'bidenbiden', score: 65.0317, dictionary words: 97.1154%
vigenere cipher (key length: 15): guessed key: 'bidenbidenbiden', score: 65.0317, dictionary words: 97.1154%
vigenere cipher (key length: 20): guessed key: 'bidenbidenbidenbiden', score: 65.0317, dictionary words: 97.1154%
vigenere cipher (key length: 40): guessed key: 'bidenbidenbidenbidenbidenbidenbidenbiden', score: 65.0317, dictionary words: 97.1154%
vigenere cipher (key length: 75): guessed key: 'bidonbideabideubidenbidenbikinbpdenbidenbidenbpdenbidenpidenbidenbidenbxren', score: 43.2156, dictionary words: 64.1026%
vigenere cipher (key length: 4): guessed key: 'eieb', score: 8.44778, dictionary words: 18.2692%
starting cracking mono-alphabetic cipher...
finding randomly the best key...
(1/7)...DONE best key so far: 'vtqzapjoukhybdgicenmslrwxf'
(2/7)...DONE best key so far: 'wnjbrygluqxiachopedmtskfzv'
(3/7)...DONE best key so far: 'hlqvtckrigfysdmapenwuobjzx'
(4/7)...DONE best key so far: 'hlqvtckrigfysdmapenwuobjzx'
(5/7)...DONE best key so far: 'hlqvtckrigfysdmapenwuobjzx'
(6/7)...DONE best key so far: 'hlqvtckrigfysdmapenwuobjzx'
(7/7)...DONE best key so far: 'hlqvtckrigfysdmapenwuobjzx'
improving the best key found...
dictionary words: 18.0064%
------------------------------------------------------
the best guess 97.1154% dictionary words
guessed key: 'biden'
------------------------------------------------------
```
### Example (2)
```
starting cracking 2. cipher...
the given text scores: 8.33071 and contains 20.3676% dictionary words
guessed key lengths: 1 2 3 4 5 6 10 13 7 21 9 8 17 126 15 12 11 50 23 117 20 94 16 22 
vigenere cipher (key length: 1): guessed key: 'h', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 2): guessed key: 'hh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 3): guessed key: 'hhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 4): guessed key: 'hhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 5): guessed key: 'hhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 6): guessed key: 'hhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 10): guessed key: 'hhhhhhhihh', score: 12.8321, dictionary words: 25.2574%
vigenere cipher (key length: 13): guessed key: 'hhhhhhhhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 7): guessed key: 'hhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 21): guessed key: 'hhhhhhhhhhhhhhhhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 9): guessed key: 'hhhhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 8): guessed key: 'hhhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 17): guessed key: 'hhhhihhhihhhhhhhh', score: 12.5747, dictionary words: 24.7426%
vigenere cipher (key length: 126): guessed key: 'ohhhohhhhhholheholephhuhiphbhihobihhihhhhuhibiqhhbhehhehobhhohhhbeliobhhhpbhuhihhihobhphhbhbhbhihohiihhhhhhhhbhhiiiehhihhuhhhu', score: 12.2068, dictionary words: 23.3088%
vigenere cipher (key length: 15): guessed key: 'hhhhhhbhhhhhhhh', score: 13.694, dictionary words: 26.9118%
vigenere cipher (key length: 12): guessed key: 'hhhhhhhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 11): guessed key: 'hhhhhhhhhhh', score: 13.4454, dictionary words: 26.4706%
vigenere cipher (key length: 50): guessed key: 'bhhhhuhhhhhbhhhhhbhhhhhhihhihphhhhhhhiihhhhhhhhhhh', score: 13.3188, dictionary words: 25.9191%
vigenere cipher (key length: 23): guessed key: 'hhhhhhhhhbhhhhhhihhhhhh', score: 13.6385, dictionary words: 26.8382%
vigenere cipher (key length: 117): guessed key: 'heblhphhhhhohhbqphbhhhhbhhhhiephuiehhohhhhhobihbhiqihqhihhuhhuhohhhhohhbhpbophluhhuhhuubhbibhluhhphihhhhhhhheihhpephh', score: 12.6777, dictionary words: 24.1176%
vigenere cipher (key length: 20): guessed key: 'hhhhhhhihhhhhhhhhbhh', score: 13.4473, dictionary words: 26.3971%
vigenere cipher (key length: 94): guessed key: 'hbhhhhhohhhhohhhhohhuhoihhhoihiuhhhipohbihhhhhhhhhihhhhbhhuhhhhhhuhhhhhbhohhhbbhhhpbhhhehpdhhp', score: 13.4068, dictionary words: 25.8088%
vigenere cipher (key length: 16): guessed key: 'hhhhhbhhuhhhhhhh', score: 13.3977, dictionary words: 26.3235%
vigenere cipher (key length: 22): guessed key: 'hbhhhhhhhhhhhhhhhhhhhh', score: 13.6893, dictionary words: 26.9118%
starting cracking mono-alphabetic cipher...
finding randomly the best key...
(1/7)...DONE best key so far: 'pmrqgkfonzvtijbaychdeulxsw'
(2/7)...DONE best key so far: 'msrqgvfonzktijcaybhdeulxpw'
(3/7)...DONE best key so far: 'msrqgvfonzktijcaybhdeulxpw'
(4/7)...DONE best key so far: 'msrqgvfonzktijcaybhdeulxpw'
(5/7)...DONE best key so far: 'msrqgkfonzvtijbaychdeulxpw'
(6/7)...DONE best key so far: 'msrqgkfonzvtijbaychdeulxpw'
(7/7)...DONE best key so far: 'msrqgkfonzvtijbaychdeulxpw'
improving the best key found...
dictionary words: 97.8301%
------------------------------------------------------
the best guess 97.8301% dictionary words
guessed key: 'msrqgkfonzvtijbaychdeulxpw'
------------------------------------------------------
```
### Example (3)
```
starting cracking 4. cipher...
the given text scores: 66.945 and contains 100% dictionary words
guessed key lengths: 1 2 3 4 9 17 7 11 6
vigenere cipher (key length: 1): guessed key: 'a', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 2): guessed key: 'aa', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 3): guessed key: 'aaa', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 4): guessed key: 'aaaa', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 9): guessed key: 'aaaaaaaaa', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 17): guessed key: 'aaaaaanaaaaaaaaaa', score: 49.8959, dictionary words: 74.4681%
vigenere cipher (key length: 7): guessed key: 'aaaaaaa', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 11): guessed key: 'aaaaaaaaaaa', score: 66.945, dictionary words: 100%
vigenere cipher (key length: 6): guessed key: 'aaaaaa', score: 66.945, dictionary words: 100%
starting cracking mono-alphabetic cipher...
finding randomly the best key...
(1/7)...DONE best key so far: ''
(2/7)...DONE best key so far: ''
(3/7)...DONE best key so far: ''
(4/7)...DONE best key so far: ''
(5/7)...DONE best key so far: ''
(6/7)...DONE best key so far: ''
(7/7)...DONE best key so far: ''
improving the best key found...
dictionary words: 0%
------------------------------------------------------
the best guess 100% dictionary words
guessed key: ''
------------------------------------------------------
```

## Examples of I/O
### Example (1)
#### input
```
bpn anq nbh eub zrn aqa ngv iry naq geh rcn gev bgy bir van yyb shf pbz zna qjv gut ybj vat urn egf jrf rrg urr evf rgu rge hra beg ufg eba tna qse rrs ebz sne naq jvq rbp nan qnj rfg naq bat hne qsb egu rrt bqx rrc bhe yna qty bev bhf naq ser rbp nan qnj rfg naq bat hne qsb egu rrb pna nqn jrf gna qba thn eqs beg urr
```
#### output
```
oca nad aou rho mea ndn ati vel and tru epa tri otl ove ina llo fus com man dwi thg low ing hea rts wes eet hee ris eth etr uen ort hst ron gan dfr eef rom far and wid eoc ana daw est and ong uar dfo rth eeg odk eep our lan dgl ori ous and fre eoc ana daw est and ong uar dfo rth eeo can ada wes tan don gua rdf ort hee 
```
### Example (2)
#### input
```
lsu hwt api tcpif smb rhgguu bwhzwq ml zpb pww su zhvwt spku pww tpq pit su fiuz lspl su bshvwt lpfu ml ghc p whie lmau 
ihz uplmie spt ohcut sma pit su iukuc rpccmut p wvirs su spt p ohllwu hg zpluc mi lsu ohz hg lsu bfmgg pit lspl zpb pww 
su iuutut ghc lsu tpq lsu ohq zpb oprf ihz zmls lsu bpctmiub pit lsu lzh opmlb zcpyyut mi p iuzbypyuc pit lsuq zuil thzi
 lsu lcpmw lh lsu bfmgg guuwmie lsu yuoowut bpit vituc lsumc guul pit wmglut lsu bfmgg pit bwmt suc milh lsu zpluc ehht 
wvrf hwt api ehht wvrf lsu hwt api bpmt su gmllut lsu chyu wpbsmieb hg lsu hpcb hilh lsu lshwu ymib pit wupimie ghczpct 
pepmibl lsu lscvbl hg lsu owptub mi lsu zpluc su ouepi lh chz hvl hg lsu spcohvc mi lsu tpcf lsucu zucu hlsuc ohplb gcha
 lsu hlsuc ouprsub ehmie hvl lh bup pit lsu hwt api supct lsu tmy pit yvbs hg lsumc hpcb ukui lshves su rhvwt ihl buu ls
ua ihz lsu ahhi zpb ouwhz lsu smwwb bhaulmaub bhauhiu zhvwt byupf mi p ohpl ovl ahbl hg lsu ohplb zucu bmwuil uxruyl ghc
 lsu tmy hg lsu hpcb lsuq bycupt pypcl pgluc lsuq zucu hvl hg lsu ahvls hg lsu spcohvc pit uprs hiu suptut ghc lsu ypcl 
hg lsu hrupi zsucu su shyut lh gmit gmbs lsu hwt api fiuz su zpb ehmie gpc hvl pit su wugl lsu bauww hg lsu wpit ousmit 
pit chzut hvl milh lsu rwupi upcwq ahcimie bauww hg lsu hrupi su bpz lsu yshbyshcubruiru hg lsu evwg zuut mi lsu zpluc p
b su chzut hkuc lsu ypcl hg lsu hrupi lspl lsu gmbsucaui rpwwut lsu ecupl zuww ourpvbu lsucu zpb p bvttui tuuy hg bukui 
svitcut gplshab zsucu pww bhclb hg gmbs rhiecueplut ourpvbu hg lsu bzmcw lsu rvccuil aptu pepmibl lsu bluuy zpwwb hg lsu
 gwhhc hg lsu hrupi sucu lsucu zucu rhiruilcplmhib hg bscmay pit opml gmbs pit bhaulmaub brshhwb hg bdvmt mi lsu tuuyubl
 shwub pit lsubu chbu rwhbu lh lsu bvcgpru pl imesl zsucu pww lsu zpitucmie gmbs gut hi lsua mi lsu tpcf lsu hwt api rhv
wt guuw lsu ahcimie rhamie pit pb su chzut su supct lsu lcuaowmie bhvit pb gwqmie gmbs wugl lsu zpluc pit lsu smbbmie ls
pl lsumc blmgg bul zmieb aptu pb lsuq bhpcut pzpq mi lsu tpcfiubb su zpb kucq ghit hg gwqmie gmbs pb lsuq zucu smb ycmir
mypw gcmuitb hi lsu hrupi su zpb bhccq ghc lsu omctb ubyurmpwwq lsu bapww tuwmrplu tpcf lucib lspl zucu pwzpqb gwqmie pi
t whhfmie pit pwahbl iukuc gmitmie pit su lshvesl lsu omctb spku p spctuc wmgu lspi zu th uxruyl ghc lsu choouc omctb pi
t lsu supkq blchie hiub zsq tmt lsuq apfu omctb bh tuwmrplu pit gmiu pb lshbu bup bzpwwhzb zsui lsu hrupi rpi ou bh rcvu
w bsu mb fmit pit kucq oupvlmgvw ovl bsu rpi ou bh rcvuw pit ml rhaub bh bvttuiwq pit bvrs omctb lspl gwq tmyymie pit sv
ilmie zmls lsumc bapww bpt khmrub pcu aptu lhh tuwmrpluwq ghc lsu bup su pwzpqb lshvesl hg lsu bup pb wp apc zsmrs mb zs
pl yuhywu rpww suc mi bypimbs zsui lsuq whku suc bhaulmaub lshbu zsh whku suc bpq opt lsmieb hg suc ovl lsuq pcu pwzpqb 
bpmt pb lshves bsu zucu p zhapi bhau hg lsu qhvieuc gmbsucaui lshbu zsh vbut ovhqb pb gwhplb ghc lsumc wmiub pit spt ahl
hcohplb ohvesl zsui lsu bspcf wmkucb spt ochvesl avrs ahiuq byhfu hg suc pb uw apc zsmrs mb apbrvwmiu lsuq byhfu hg suc 
pb p rhilublpil hc p ywpru hc ukui pi uiuaq ovl lsu hwt api pwzpqb lshvesl hg suc pb guamimiu pit pb bhaulsmie lspl epku
 hc zmlssuwt ecupl gpkhvcb pit mg bsu tmt zmwt hc zmrfut lsmieb ml zpb ourpvbu bsu rhvwt ihl suwy lsua lsu ahhi pggurlb 
suc pb ml thub p zhapi su lshvesl su zpb chzmie bluptmwq pit ml zpb ih ugghcl ghc sma bmiru su fuyl zuww zmlsmi smb byuu
t pit lsu bvcgpru hg lsu hrupi zpb gwpl uxruyl ghc lsu hrrpbmhipw bzmcwb hg lsu rvccuil su zpb wullmie lsu rvccuil th p 
lsmct hg lsu zhcf pit pb ml blpclut lh ou wmesl su bpz su zpb pwcuptq gvclsuc hvl lspi su spt shyut lh ou pl lsmb shvc m
 zhcfut lsu tuuy zuwwb ghc p zuuf pit tmt ihlsmie su lshvesl lhtpq m ww zhcf hvl zsucu lsu brshhwb hg ohimlh pit pwoprhc
u pcu pit apqou lsucu zmww ou p ome hiu zmls lsua oughcu ml zpb cupwwq wmesl su spt smb opmlb hvl pit zpb tcmglmie zmls 
lsu rvccuil hiu opml zpb thzi ghclq gplshab lsu burhit zpb pl bukuilq gmku pit lsu lsmct pit ghvcls zucu thzi mi lsu owv
u zpluc pl hiu svitcut pit hiu svitcut pit lzuilq gmku gplshab uprs opml svie supt thzi zmls lsu bspif hg lsu shhf mibmt
u lsu opml gmbs lmut pit buzut bhwmt pit pww lsu ychnurlmie ypcl hg lsu shhf lsu rvcku pit lsu yhmil zpb rhkucut zmls gc
ubs bpctmiub uprs bpctmiu zpb shhfut lschves ohls uqub bh lspl lsuq aptu p spwg epcwpit hi lsu ychnurlmie bluuw lsucu zp
b ih ypcl hg lsu shhf lspl p ecupl gmbs rhvwt guuw zsmrs zpb ihl bzuul bauwwmie pit ehht lpblmie lsu ohq spt emkui sma l
zh gcubs bapww lvipb hc pwoprhcub zsmrs svie hi lsu lzh tuuyubl wmiub wmfu ywvaaulb pit hi lsu hlsucb su spt p ome owvu 
cviiuc pit p quwwhz nprf lspl spt ouui vbut oughcu ovl lsuq zucu mi ehht rhitmlmhi blmww pit spt lsu uxruwwuil bpctmiub 
lh emku lsua bruil pit pllcprlmkuiubb uprs wmiu pb lsmrf pchvit pb p ome yuirmw zpb whhyut hilh p ecuui bpyyut blmrf bh 
lspl piq yvww hc lhvrs hi lsu opml zhvwt apfu lsu blmrf tmy pit uprs wmiu spt lzh ghclq gplsha rhmwb zsmrs rhvwt ou aptu
 gpbl lh lsu hlsuc bypcu rhmwb bh lspl mg ml zucu iurubbpcq p gmbs rhvwt lpfu hvl hkuc lscuu svitcut gplshab hg wmiu ihz
 lsu api zplrsut lsu tmy hg lsu lscuu blmrfb hkuc lsu bmtu hg lsu bfmgg pit chzut euilwq lh fuuy lsu wmiub blcpmesl vy p
it thzi pit pl lsumc ychyuc tuylsb ml zpb dvmlu wmesl pit piq ahauil ihz lsu bvi zhvwt cmbu lsu bvi chbu lsmiwq gcha lsu
 bup pit lsu hwt api rhvwt buu lsu hlsuc ohplb whz hi lsu zpluc pit zuww mi lhzpct lsu bshcu bycupt hvl prchbb lsu rvccu
il lsui lsu bvi zpb ocmesluc pit lsu ewpcu rpau hi lsu zpluc pit lsui pb ml chbu rwupc lsu gwpl bup buil ml oprf pl smb 
uqub bh lspl ml svcl bspcywq pit su chzut zmlshvl whhfmie milh ml su whhfut thzi milh lsu zpluc pit zplrsut lsu wmiub ls
pl zuil blcpmesl thzi milh lsu tpcf hg lsu zpluc su fuyl lsua blcpmesluc lspi piqhiu tmt bh lspl pl uprs wukuw mi lsu tp
cfiubb hg lsu blcupa lsucu zhvwt ou p opml zpmlmie uxprlwq zsucu su zmbsut ml lh ou ghc piq gmbs lspl bzpa lsucu hlsucb 
wul lsua tcmgl zmls lsu rvccuil pit bhaulmaub lsuq zucu pl bmxlq gplshab zsui lsu gmbsucaui lshvesl lsuq zucu pl p svitc
ut ovl su lshvesl m fuuy lsua zmls ycurmbmhi hiwq m spku ih wvrf piq ahcu ovl zsh fihzb apqou lhtpq ukucq tpq mb p iuz t
pq ml mb oulluc lh ou wvrfq ovl m zhvwt cplsuc ou uxprl lsui zsui wvrf rhaub qhv pcu cuptq lsu bvi zpb lzh shvcb smesuc 
ihz pit ml tmt ihl svcl smb uqub bh avrs lh whhf milh lsu upbl lsucu zucu hiwq lscuu ohplb mi bmesl ihz pit lsuq bshzut 
kucq whz pit gpc mibshcu pww aq wmgu lsu upcwq bvi spb svcl aq uqub su lshvesl qul lsuq pcu blmww ehht mi lsu ukuimie m 
rpi whhf blcpmesl milh ml zmlshvl eullmie lsu owprfiubb ml spb ahcu ghcru mi lsu ukuimie lhh ovl mi lsu ahcimie ml mb yp
migvw nvbl lsui su bpz p api hg zpc omct zmls smb whie owprf zmieb rmcrwmie mi lsu bfq psupt hg sma su aptu p dvmrf tchy
 bwpilmie thzi hi smb oprf bzuyl zmieb pit lsui rmcrwut pepmi su b ehl bhaulsmie lsu hwt api bpmt pwhvt su b ihl nvbl wh
hfmie su chzut bwhzwq pit bluptmwq lhzpct zsucu lsu omct zpb rmcrwmie su tmt ihl svccq pit su fuyl smb wmiub blcpmesl vy
 pit thzi ovl su rchztut lsu rvccuil p wmllwu bh lspl su zpb blmww gmbsmie rhccurlwq lshves gpbluc lspi su zhvwt spku gm
bsut mg su zpb ihl lcqmie lh vbu lsu omct lsu omct zuil smesuc mi lsu pmc pit rmcrwut pepmi smb zmieb ahlmhiwubb lsui su
 thku bvttuiwq pit lsu hwt api bpz gwqmie gmbs byvcl hvl hg lsu zpluc pit bpmw tubyucpluwq hkuc lsu bvcgpru thwysmi lsu 
hwt api bpmt pwhvt ome thwysmi su bsmyyut smb hpcb pit ochvesl p bapww wmiu gcha vituc lsu ohz ml spt p zmcu wuptuc pit 
p autmva bmjut shhf pit su opmlut ml zmls hiu hg lsu bpctmiub su wul ml eh hkuc lsu bmtu pit lsui aptu ml gpbl lh p cmie
 ohwl mi lsu bluci lsui su opmlut pihlsuc wmiu pit wugl ml rhmwut mi lsu bsptu hg lsu ohz su zuil oprf lh chzmie pit lh 
zplrsmie lsu whie zmieut owprf omct zsh zpb zhcfmie ihz whz hkuc lsu zpluc pb su zplrsut lsu omct tmyyut pepmi bwpilmie 
smb zmieb ghc lsu tmku pit lsui bzmiemie lsua zmwtwq pit miuggurlvpwwq pb su ghwwhzut lsu gwqmie gmbs lsu hwt api rhvwt 
buu lsu bwmesl ovweu mi lsu zpluc lspl lsu ome thwysmi cpmbut pb lsuq ghwwhzut lsu ubrpymie gmbs lsu thwysmi zucu rvllmi
e lschves lsu zpluc ouwhz lsu gwmesl hg lsu gmbs pit zhvwt ou mi lsu zpluc tcmkmie pl byuut zsui lsu gmbs tchyyut ml mb 
p ome brshhw hg thwysmi su lshvesl lsuq pcu zmtubycupt pit lsu gwqmie gmbs spku wmllwu rspiru lsu omct spb ih rspiru lsu
 gwqmie gmbs pcu lhh ome ghc sma pit lsuq eh lhh gpbl su zplrsut lsu gwqmie gmbs ovcbl hvl pepmi pit pepmi pit lsu miugg
urlvpw ahkuauilb hg lsu omct lspl brshhw spb ehllui pzpq gcha au su lshvesl lsuq pcu ahkmie hvl lhh gpbl pit lhh gpc ovl
 yucspyb m zmww ymrf vy p blcpq pit yucspyb aq ome gmbs mb pchvit lsua aq ome gmbs avbl ou bhauzsucu lsu rwhvtb hkuc lsu
 wpit ihz chbu wmfu ahvilpmib pit lsu rhpbl zpb hiwq p whie ecuui wmiu zmls lsu ecpq owvu smwwb ousmit ml lsu zpluc zpb 
p tpcf owvu ihz bh tpcf lspl ml zpb pwahbl yvcywu pb su whhfut thzi milh ml su bpz lsu cut bmglmie hg lsu ywpiflhi mi ls
u tpcf zpluc pit lsu blcpieu wmesl lsu bvi aptu ihz su zplrsut smb wmiub lh buu lsua eh blcpmesl thzi hvl hg bmesl milh 
lsu zpluc pit su zpb spyyq lh buu bh avrs ywpiflhi ourpvbu ml aupil gmbs lsu blcpieu wmesl lsu bvi aptu mi lsu zpluc ihz
 lspl lsu bvi zpb smesuc aupil ehht zuplsuc pit bh tmt lsu bspyu hg lsu rwhvtb hkuc lsu wpit ovl lsu omct zpb pwahbl hvl
 hg bmesl ihz pit ihlsmie bshzut hi lsu bvcgpru hg lsu zpluc ovl bhau yplrsub hg quwwhz bvi owuprsut bpcepbbh zuut pit l
su yvcywu ghcapwmjut mcmtubruil euwplmihvb owpttuc hg p yhclvevubu api hg zpc gwhplmie thbu oubmtu lsu ohpl ml lvciut hi
 mlb bmtu pit lsui cmeslut mlbuwg ml gwhplut rsuucgvwwq pb p ovoowu zmls mlb whie tuptwq yvcywu gmwpauilb lcpmwmie p qpc
t ousmit ml mi lsu zpluc pevp apwp lsu api bpmt qhv zshcu gcha zsucu su bzvie wmeslwq pepmibl smb hpcb su whhfut thzi mi
lh lsu zpluc pit bpz lsu lmiq gmbs lspl zucu rhwhvcut wmfu lsu lcpmwmie gmwpauilb pit bzpa oulzuui lsua pit vituc lsu ba
pww bsptu lsu ovoowu aptu pb ml tcmglut lsuq zucu maaviu lh mlb yhmbhi ovl aui zucu ihl pit zsui bpau hg lsu gmwpauilb z
hvwt rplrs hi p wmiu pit cubl lsucu bwmaq pit yvcywu zsmwu lsu hwt api zpb zhcfmie p gmbs su zhvwt spku zuwlb pit bhcub 
hi smb pcab pit spitb hg lsu bhcl lspl yhmbhi mkq hc yhmbhi hpf rpi emku ovl lsubu yhmbhimieb gcha lsu pevp apwp rpau dv
mrfwq pit blcvrf wmfu p zsmywpbs lsu mcmtubruil ovoowub zucu oupvlmgvw ovl lsuq zucu lsu gpwbubl lsmie mi lsu bup pit ls
u hwt api whkut lh buu lsu ome bup lvclwub uplmie lsua lsu lvclwub bpz lsua pyychprsut lsua gcha lsu gchil lsui bsvl lsu
mc uqub bh lsuq zucu rhaywuluwq rpcpyprut pit plu lsua gmwpauilb pit pww lsu hwt api whkut lh buu lsu lvclwub upl lsua p
it su whkut lh zpwf hi lsua hi lsu ouprs pgluc p blhca pit supc lsua yhy zsui su bluyyut hi lsua zmls lsu shciq bhwub hg
 smb guul su whkut ecuui lvclwub pit spzf omwwb zmls lsumc uwuepiru pit byuut pit lsumc ecupl kpwvu pit su spt p gcmuitw
q rhiluayl ghc lsu sveu blvymt wheeucsuptb quwwhz mi lsumc pcahvc ywplmie blcpieu mi lsumc whku apfmie pit spyymwq uplmi
e lsu yhclvevubu aui hg zpc zmls lsumc uqub bsvl su spt ih aqblmrmba pohvl lvclwub pwlshves su spt ehiu mi lvclwu ohplb 
ghc apiq qupcb su zpb bhccq ghc lsua pww ukui lsu ecupl lcvif oprfb lspl zucu pb whie pb lsu bfmgg pit zumesut p lhi ahb
l yuhywu pcu supclwubb pohvl lvclwub ourpvbu p lvclwu b supcl zmww oupl ghc shvcb pgluc su spb ouui rvl vy pit ovlrsucut
 ovl lsu hwt api lshvesl m spku bvrs p supcl lhh pit aq guul pit spitb pcu wmfu lsumcb su plu lsu zsmlu ueeb lh emku sma
buwg blcuiels su plu lsua pww lschves apq lh ou blchie mi buyluaouc pit hrlhouc ghc lsu lcvwq ome gmbs su pwbh tcpif p r
vy hg bspcf wmkuc hmw uprs tpq gcha lsu ome tcva mi lsu bsprf zsucu apiq hg lsu gmbsucaui fuyl lsumc eupc ml zpb lsucu g
hc pww gmbsucaui zsh zpilut ml ahbl gmbsucaui splut lsu lpblu ovl ml zpb ih zhcbu lspi eullmie vy pl lsu shvcb lspl lsuq
 chbu pit ml zpb kucq ehht pepmibl pww rhwtb pit ecmyyub pit ml zpb ehht ghc lsu uqub ihz lsu hwt api whhfut vy pit bpz 
lspl lsu omct zpb rmcrwmie pepmi su b ghvit gmbs su bpmt pwhvt ih gwqmie gmbs ochfu lsu bvcgpru pit lsucu zpb ih brplluc
mie hg opml gmbs ovl pb lsu hwt api zplrsut p bapww lvip chbu mi lsu pmc lvciut pit tchyyut supt gmcbl milh lsu zpluc ls
u lvip bshiu bmwkuc mi lsu bvi pit pgluc su spt tchyyut oprf milh lsu zpluc pihlsuc pit pihlsuc chbu pit lsuq zucu nvaym
ie mi pww tmcurlmhib rsvcimie lsu zpluc pit wupymie mi whie nvayb pgluc lsu opml lsuq zucu rmcrwmie ml pit tcmkmie ml mg
 lsuq thi l lcpkuw lhh gpbl m zmww eul milh lsua lsu hwt api lshvesl pit su zplrsut lsu brshhw zhcfmie lsu zpluc zsmlu p
it lsu omct ihz tchyymie pit tmyymie milh lsu opml gmbs lspl zucu ghcrut lh lsu bvcgpru mi lsumc ypimr lsu omct mb p ecu
pl suwy lsu hwt api bpmt nvbl lsui lsu bluci wmiu rpau lpvl vituc smb ghhl zsucu su spt fuyl p whhy hg lsu wmiu pit su t
chyyut smb hpcb pit guwl lmwu zumesl hg lsu bapww lvip b bsmkucmie yvww pb su suwt lsu wmiu gmca pit rhaauirut lh spvw m
l mi lsu bsmkucmie mircupbut pb su yvwwut mi pit su rhvwt buu lsu owvu oprf hg lsu gmbs mi lsu zpluc pit lsu ehwt hg smb
 bmtub oughcu su bzvie sma hkuc lsu bmtu pit milh lsu ohpl su wpq mi lsu bluci mi lsu bvi rhayprl pit ovwwul bspyut smb 
ome vimiluwwmeuil uqub blpcmie pb su lsvayut smb wmgu hvl pepmibl lsu ywpifmie hg lsu ohpl zmls lsu dvmrf bsmkucmie blch
fub hg smb iupl gpbl ahkmie lpmw lsu hwt api sml sma hi lsu supt ghc fmitiubb pit fmrfut sma smb ohtq blmww bsvttucmie v
ituc lsu bsptu hg lsu bluci
```
#### output
```
the old man drank his coffee slowly it was all he would have all day and he knew that he should take it for a long time now eating had bored him and he never carried a lunch he had a bottle of water in the bow of the skiff and that was all he needed for the day the boy was back now with the sardines and the two baits wrapped in a newspaper and they went down the trail to the skiff feeling the pebbled sand under their feet and lifted the skiff and slid her into the water good luck old man good luck the old man said he fitted the rope lashings of the oars onto the thole pins and leaning forward against the thrust of the blades in the water he began to row out of the harbour in the dark there were other boats from the other beaches going out to sea and the old man heard the dip and push of their oars even though he could not see th em now the moon was below the hills sometimes someone would speak in a boat but most of the boats were silent except for the dip of the oars they spread apart after they were out of the mouth of the harbour and each one headed for the part of the ocean where he hoped to find fish the old man knew he was going far out and he left the smell of the land behind and rowed out into the clean early morning smell of the ocean he saw the phosphorescence of the gulf weed in the water a s he rowed over the part of the ocean that the fishermen called the great well because there was a sudden deep of seven hundred fathoms where all sorts of fish congregated because of the swirl the current made against the steep walls of the floor of the ocean here there were concentrations of shrimp and bait fish and sometimes schools of squid in the deepest holes and these rose close to the surface at night where all the wandering fish fed on them in the dark the old man cou ld feel the morning coming and as he rowed he heard the trembling sound as flying fish left the water and the hissing th at their stiff set wings made as they soared away in the darkness he was very fond of flying fish as they were his princ ipal friends on the ocean he was sorry for the birds especially the small delicate dark terns that were always flying an d looking and almost never finding and he thought the birds have a harder life than we do except for the robber birds an d the heavy strong ones why did they make birds so delicate and fine as those sea swallows when the ocean can be so crue l she is kind and very beautiful but she can be so cruel and it comes so suddenly and such birds that fly dipping and hu nting with their small sad voices are made too delicately for the sea he always thought of the sea as la mar which is wh at people call her in spanish when they love her sometimes those who love her say bad things of her but they are always said as though she were a woman some of the younger fishermen those who used buoys as floats for their lines and had mot orboats bought when the shark livers had brought much money spoke of her as el mar which is masculine they spoke of her as a contestant or a place or even an enemy but the old man always thought of her as feminine and as something that gave or withheld great favours and if she did wild or wicked things it was because she could not help them the moon affects her as it does a woman he thought he was rowing steadily and it was no effort for him since he kept well within his spee d and the surface of the ocean was flat except for the occasional swirls of the current he was letting the current do a third of the work and as it started to be light he saw he was already further out than he had hoped to be at this hour i worked the deep wells for a week and did nothing he thought today i ll work out where the schools of bonito and albacor e are and maybe there will be a big one with them before it was really light he had his baits out and was drifting with the current one bait was down forty fathoms the second was at seventy five and the third and fourth were down in the blu e water at one hundred and one hundred and twenty five fathoms each bait hung head down with the shank of the hook insid e the bait fish tied and sewed solid and all the projecting part of the hook the curve and the point was covered with fr esh sardines each sardine was hooked through both eyes so that they made a half garland on the projecting steel there wa s no part of the hook that a great fish could feel which was not sweet smelling and good tasting the boy had given him t wo fresh small tunas or albacores which hung on the two deepest lines like plummets and on the others he had a big blue runner and a yellow jack that had been used before but they were in good condition still and had the excellent sardines to give them scent and attractiveness each line as thick around as a big pencil was looped onto a green sapped stick so that any pull or touch on the bait would make the stick dip and each line had two forty fathom coils which could be made fast to the other spare coils so that if it were necessary a fish could take out over three hundred fathoms of line now the man watched the dip of the three sticks over the side of the skiff and rowed gently to keep the lines straight up a nd down and at their proper depths it was quite light and any moment now the sun would rise the sun rose thinly from the sea and the old man could see the other boats low on the water and well in toward the shore spread out across the curre nt then the sun was brighter and the glare came on the water and then as it rose clear the flat sea sent it back at his eyes so that it hurt sharply and he rowed without looking into it he looked down into the water and watched the lines th at went straight down into the dark of the water he kept them straighter than anyone did so that at each level in the da rkness of the stream there would be a bait waiting exactly where he wished it to be for any fish that swam there others let them drift with the current and sometimes they were at sixty fathoms when the fishermen thought they were at a hundr ed but he thought i keep them with precision only i have no luck any more but who knows maybe today every day is a new d ay it is better to be lucky but i would rather be exact then when luck comes you are ready the sun was two hours higher now and it did not hurt his eyes so much to look into the east there were only three boats in sight now and they showed very low and far inshore all my life the early sun has hurt my eyes he thought yet they are still good in the evening i can look straight into it without getting the blackness it has more force in the evening too but in the morning it is pa inful just then he saw a man of war bird with his long black wings circling in the sky ahead of him he made a quick drop slanting down on his back swept wings and then circled again he s got something the old man said aloud he s not just lo oking he rowed slowly and steadily toward where the bird was circling he did not hurry and he kept his lines straight up and down but he crowded the current a little so that he was still fishing correctly though faster than he would have fi shed if he was not trying to use the bird the bird went higher in the air and circled again his wings motionless then he dove suddenly and the old man saw flying fish spurt out of the water and sail desperately over the surface dolphin the old man said aloud big dolphin he shipped his oars and brought a small line from under the bow it had a wire leader and a medium sized hook and he baited it with one of the sardines he let it go over the side and then made it fast to a ring bolt in the stern then he baited another line and left it coiled in the shade of the bow he went back to rowing and to watching the long winged black bird who was working now low over the water as he watched the bird dipped again slanting his wings for the dive and then swinging them wildly and ineffectually as he followed the flying fish the old man could see the slight bulge in the water that the big dolphin raised as they followed the escaping fish the dolphin were cuttin g through the water below the flight of the fish and would be in the water driving at speed when the fish dropped it is a big school of dolphin he thought they are widespread and the flying fish have little chance the bird has no chance the flying fish are too big for him and they go too fast he watched the flying fish burst out again and again and the ineff ectual movements of the bird that school has gotten away from me he thought they are moving out too fast and too far but perhaps i will pick up a stray and perhaps my big fish is around them my big fish must be somewhere the clouds over the land now rose like mountains and the coast was only a long green line with the gray blue hills behind it the water was a dark blue now so dark that it was almost purple as he looked down into it he saw the red sifting of the plankton in th e dark water and the strange light the sun made now he watched his lines to see them go straight down out of sight into the water and he was happy to see so much plankton because it meant fish the strange light the sun made in the water now that the sun was higher meant good weather and so did the shape of the clouds over the land but the bird was almost out of sight now and nothing showed on the surface of the water but some patches of yellow sun bleached sargasso weed and t he purple formalized iridescent gelatinous bladder of a portuguese man of war floating dose beside the boat it turned on its side and then righted itself it floated cheerfully as a bubble with its long deadly purple filaments trailing a yar d behind it in the water agua mala the man said you whore from where he swung lightly against his oars he looked down in to the water and saw the tiny fish that were coloured like the trailing filaments and swam between them and under the sm all shade the bubble made as it drifted they were immune to its poison but men were not and when same of the filaments w ould catch on a line and rest there slimy and purple while the old man was working a fish he would have welts and sores on his arms and hands of the sort that poison ivy or poison oak can give but these poisonings from the agua mala came qu ickly and struck like a whiplash the iridescent bubbles were beautiful but they were the falsest thing in the sea and th e old man loved to see the big sea turtles eating them the turtles saw them approached them from the front then shut the ir eyes so they were completely carapaced and ate them filaments and all the old man loved to see the turtles eat them a nd he loved to walk on them on the beach after a storm and hear them pop when he stepped on them with the horny soles of his feet he loved green turtles and hawk bills with their elegance and speed and their great value and he had a friendl y contempt for the huge stupid loggerheads yellow in their armour plating strange in their love making and happily eatin g the portuguese men of war with their eyes shut he had no mysticism about turtles although he had gone in turtle boats for many years he was sorry for them all even the great trunk backs that were as long as the skiff and weighed a ton mos t people are heartless about turtles because a turtle s heart will beat for hours after he has been cut up and butchered but the old man thought i have such a heart too and my feet and hands are like theirs he ate the white eggs to give him self strength he ate them all through may to be strong in september and october for the truly big fish he also drank a c up of shark liver oil each day from the big drum in the shack where many of the fishermen kept their gear it was there f or all fishermen who wanted it most fishermen hated the taste but it was no worse than getting up at the hours that they rose and it was very good against all colds and grippes and it was good for the eyes now the old man looked up and saw that the bird was circling again he s found fish he said aloud no flying fish broke the surface and there was no scatter ing of bait fish but as the old man watched a small tuna rose in the air turned and dropped head first into the water th e tuna shone silver in the sun and after he had dropped back into the water another and another rose and they were jumpi ng in all directions churning the water and leaping in long jumps after the bait they were circling it and driving it if they don t travel too fast i will get into them the old man thought and he watched the school working the water white a nd the bird now dropping and dipping into the bait fish that were forced to the surface in their panic the bird is a gre at help the old man said just then the stern line came taut under his foot where he had kept a loop of the line and he d ropped his oars and felt tile weight of the small tuna s shivering pull as he held the line firm and commenced to haul i t in the shivering increased as he pulled in and he could see the blue back of the fish in the water and the gold of his sides before he swung him over the side and into the boat he lay in the stern in the sun compact and bullet shaped his big unintelligent eyes staring as he thumped his life out against the planking of the boat with the quick shivering stro kes of his neat fast moving tail the old man hit him on the head for kindness and kicked him his body still shuddering u nder the shade of the 
```
### Example (3)
#### input
```
p adc pbv bsh tmh fl uph hnxvv instb pvhpw
aubb vs cswxhyz eh lnjthh nu bki gxqomtibv pntb jprbulrt
xprwr czreq tbumcfa drq czlkuu aweet bkvbvok xuf xhvvmwxw sjokx
bfz wlr siptnsbv ar xiwgufl zief ar knmtdrgmg vxefipmah
iqh gim usplmww efl jpnsm wlr cwpff ccuwgjvj ma bqu
knwm svbpn wlepcjl gim qmtib wlnu wxv smij ant awmym bkief
w vel ewhw giiw wgbz vtnoooiq ciqrrs ghx jbdh
srs bki ybvg ss uph jefm drq uph lbnm rj gim evnwm 
rr gim vlbsm gmzmg viro bkvbvok xuf ulwgt wi xuf lhic
xphvr uph jbfa kehhpwc upaw ma ezheq tqoiadm uicpahw
jiiw mf updx jiqfl gim evrfhh srs bki gpehvvoo vxrfx
dw vu nlxsvtoc omwzw ubti gbokheyt pdps eqvgypahw
ape lx pbbflrt bki tmmdq bg bki zpzqmaha imetb einn
qq jhmt jpbsg uismmfxre vra fiqqif jv wlr tbuinn
blw gim vxnsaseahthh obvqie p trrt nib mg xiyi
bfz wlr miqh bg bki ssmh eae bki upuh ss uph febdh
eae ekief qv xubb eeae eks fp ddyauqqkyz azsef
bkeg uph lnwwf ss xiu eae bki obbwprt krrsvalsa
b prqr bvg e ppcqxez akshml oinwm xw ap urvr
uphme ctrsq iiv antphh bvb wlrjz ishm nrsgtbhtf qwophuqrr
ap zhjhhm fshml veif bki ujzhpvoo drq ttdzr
gzrq gim wieswu ss gtlkuu wu xuf oosbn wi xuf oueif
iqh gim vxnsaseahthh obvqie jv wvvvusl qpbk anwm
rie uph pnol rj gim ivrf iqh gim kszf wi xuf jueif 
w wlht jh mg fdhv jimq jefmpia tpdpy tbdrq
cmwarfv wlrjz osifl kszfa drq uph ansa gifptdxvpv
eprtb zmgi dlggsg drq qmdgr nib xuf pheifvuifdchh ybvg
tebqvi gim ssjfz wlnu pdxu nigi nol svrtmuzre cv e abblsa
uphr ppvtyrs eh qhtb zlro wxv pbcvi vu qv nhtb
drq uplw of wxv zpbws vo orh vt wxv gscvx
nol wlr tbdvfqiqkyfl eeaomu ma uzlyzqp vlnmt zeif
whv gim oeae wi xuf nuir bvg xuf prqr pn wlr czdzr
```
#### output
```
o say can you see by the dawns early light what so proudly we hailed at the twilights last gleaming whose broad stripes and bright stars through the perilous fight oer the ramparts we watched were so gallantly streaming and the rockets red glare the bombs bursting in air gave proof through the night that our flag was still there o say does that star spangled banner yet wave oer the land of the free and the home of the brave on the shore dimly seen through the mists of the deep where the foes haughty host in dread silence reposes what is that which the breeze oer the towering steep as it fitfully blows half conceals half discloses now it catches the gleam of the mornings first beam in full glory reflected now shines in the stream tis the starspangled banner o long may it wave oer the land of the free and the home of the brave and where is that band who so vauntingly swore that the havoc of war and the battles confusion a home and a country should leave us no more their blood has washed out their foul footsteps pollution no refuge could save the hireling and slave from the terror of flight or the gloom of the grave and the starspangled banner in triumph doth wave oer the land of the free and the home of the brave o thus be it ever when freemen shall stand between their loved homes and the wars desolation blest with victry and peace may the heavenrescued land praise the power that hath made and preserved us a nation then conquer we must when our cause it is just and this be our motto in god is our trust and the starspangled banner in triumph shall wave oer the land of the free and the home of the brave 
```
### Example (4)
#### input
```
these are short famous texts in english from classic sources like the bible
or shakespeare some texts have word definitions and explanations to help you
some of these texts are written in an old style of english try to understand them
because the english that we speak today is based on what our great great great
great grandparents spoke before of course not all these texts were originally
written in english the bible for example is a translation but they are all well
known in english today and many of them express beautiful thoughts
```
#### output
```
these are short famous texts in english from classic sources like the bible or shakespeare some texts have word definitions and explanations to help you some of these texts are written in an old style of english try to understand them because the english that we speak today is based on what our great great great great grandparents spoke before of course not all these texts were originally written in english the bible for example is a translation but they are all well known in english today and many of them express beautiful thoughts 
```
