# Bigram/Trigram Language Model

For CISC220 (Data Structures), students were instructed to pick any data structure we had not worked with in class for our final project. 
I chose to engage with bigram/trigram/n-gram language models because I have a strong interest in cognitive science/linguistics. 
The system is not polished but performs rudimentary functionality for sentence production. 
I also designed a client to perform basic word prediction as many modern e-mail clients are equipped with. 



Usage:
1. Move input text file (sentence corpus) into the same directory as makefile and main.cpp
2. Open terminal and navigate to the directory
3. Run makefile with $make
4. Run main.cpp with $./final [INPUT_TEXT_FILENAME] [OPTIONAL_SENTENCE_START_WORD]
5. Enter sentence one word at a time, following prompts in terminal. Entering a stop character (".") will terminate program 


Further improvement plans:
- Debug n-gram version of structure
- Isolate sentence start words to prevent fragments
- Add compatibility with spelling/forming words, rather than sentences 
- Update client to perform both functions (sentence generation/auto-complete) depending on user input






All code in this repository is entirely my own. 
