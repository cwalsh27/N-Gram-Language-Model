#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>
#include <random>

using namespace std;

class GramModel
{
public:

    GramModel();

    virtual string formSentence(string firstWord, int sentLen) = 0;


    vector<string> returnTokens(string);
    void tokenize(string);

    vector <string> words;
    int wordCount;

};

class BIGRAM : public GramModel
{
public:

    BIGRAM();

    void makePairs();
    string formSentence(string firstWord, int sentLen);


    map<pair<string,string>, int> bigramModel;

};

class TRIGRAM : public GramModel
{
public:

    TRIGRAM();

    void makeTuples();
    void makeTuplesWithStops();
    string formSentence(string firstWord, int sentLen);
    string formSentence(string firstWord);

    map<tuple<string,string,string>, int> trigramModel;
};

class NGRAM : public GramModel
{
public:

    NGRAM();

    void makeNGrams();
    string formSentence(string firstWord);

    map<vector<string>,int> ngramModel;

};

GramModel::GramModel(){
    wordCount = 0;
    words.clear();
}

BIGRAM::BIGRAM(){
    bigramModel.clear();
}

TRIGRAM::TRIGRAM(){
    trigramModel.clear();
}

NGRAM::NGRAM(){
    ngramModel.clear();
}


string removeScraps(string word) {

    if (word.back() == '"')
        word.pop_back();
    if(word.substr(word.length()-1, 1)=="”")
        word.pop_back();
    if (word.front() == '"')
        word.erase(0, 1);
    if (word.back() == '\'')
        word.pop_back();
    if (word[0] == '\'') {
        //cout << word << "\n";
        word.erase(0, 1);
    }
    if (word.substr(0, 1) == "’") {
        //cout << word << "\n";
        word.erase(0, 1);
    }
    if (word.back() == ',') {
        word.pop_back();
    }
    if(word.back()==';')
        word.pop_back();




    return word;
}

string removeStops(string word){
    string newWord = word;

    if(word.back()=='.')
        newWord.pop_back();
    if(word.back()=='!')
        newWord.pop_back();
    if(word.back()=='?')
        newWord.pop_back();

    return newWord;
}

vector<string> GramModel::returnTokens(string sentence)
{
    vector<string> tokens;
    string word;

    istringstream ss(sentence);


    while(ss >> word)
    {
        word = removeScraps(word);
        tokens.push_back(word);
        //cout << word << "\n";
    }

    return tokens;
}

void GramModel::tokenize(string sentence)
{
    string word;

    istringstream ss(sentence);

    while(ss >> word)
    {
        word = removeScraps(word);
        transform(word.begin(),word.end(),word.begin(), ::tolower);
        if(!all_of(word.begin(),word.end(), ::isdigit)) {
            words.push_back(word);
            wordCount++;
        }
        //cout << word << "\n";
    }

}


void BIGRAM::makePairs(){
    //check for one-word sentence

    //proceed
    for(auto i = words.begin(); i != words.end(); i++ ){

        pair<string, string> bigram = make_pair(removeStops(*i),removeStops(*(i+1)));

        bigramModel[bigram]++;

        //if second word has stop character, skip two ahead to new sentence.
        if((i+1)->back()=='.' || (i+1)->back()=='!')
            i++;
    }

}

string BIGRAM::formSentence(std::string firstWord, int sentLen) {
    string sentence;
    string current;
    transform(firstWord.begin(),firstWord.end(),firstWord.begin(), ::tolower);
    current = firstWord;

    //random device
    random_device rd;
    mt19937 gen(rd());

    //checks if firstword is in map.
    bool validFirstWord = false;
    for(auto& entry : bigramModel) {
        if (entry.first.first == current) {
            validFirstWord = true;
        }
    }

    //if inputted first word not in map, then selects a random one from the corpus
    if(!validFirstWord){
        //cout << "Invalid word inputted. Random word selected for seed of sentence.\n";
        uniform_int_distribution<> distr(0,words.size()-1);
        current = removeStops(words[distr(gen)]);
    }

    for(int i = 0; i<sentLen; i++){

        sentence += current + " ";

        vector<string> nextWords;
        for(auto& entry : bigramModel) {
            if(entry.first.first == current) {
                //cout << entry.first.first << "\n";
                for(int j = 0; j<entry.second; j++)
                    nextWords.push_back(entry.first.second);
            }
        }

        int randIndex;
        if(nextWords.empty()){
            //no more valid words
            break;
        }
        else{
            uniform_int_distribution<> distr(0,nextWords.size()-1);
            randIndex = distr(gen);
            current = nextWords[randIndex];
        }
    }

    sentence.pop_back();
    sentence += ".";

    return sentence;
}

void TRIGRAM::makeTuples() {

    for(auto i = words.begin(); i != words.end(); i++){

        tuple<string,string,string> trigram = make_tuple(removeStops(*i),removeStops(*(i+1)),removeStops(*(i+2)));

        trigramModel[trigram]++;

        if((i+2)->back()=='.' || (i+2)->back()=='!')
            i+=2;
    }
}

void TRIGRAM::makeTuplesWithStops(){
    for(auto i = words.begin(); i != words.end(); i++){

        tuple<string,string,string> trigram = make_tuple(*i,*(i+1),*(i+2));

        trigramModel[trigram]++;

        if((i+2)->back()=='.' || (i+2)->back()=='!')
            i+=2;
    }
}


//TODO: Create function for finding second current word, implement in both form sentence functions
string TRIGRAM::formSentence(std::string firstWord, int sentLen) {
    string sentence,current1,current2;
    vector <string> sentenceBuilder;
    transform(firstWord.begin(),firstWord.end(),firstWord.begin(), ::tolower);
    current1 = firstWord;

    //random device
    random_device rd;
    mt19937 gen(rd());

    //checks if firstword is in map.
    bool validFirstWord = false;
    for(auto& entry : trigramModel) {
        if (get<0>(entry.first) == current1) {
            validFirstWord = true;
        }
    }

    //if inputted first word not in map, then selects a random one from the corpus
    if(!validFirstWord){
        cout << "Invalid word inputted. Random word selected for seed of sentence.\n";
        uniform_int_distribution<> distr(0,words.size()-1);
        current1 = removeStops(words[distr(gen)]);
    }

    //append first word of sentence
    //cout <<  "First word: " << current1 << "\n";



    //find second current word
    vector<string> nextWords1;
    for(auto& entry : trigramModel) {
        if(get<0>(entry.first) == current1) {
            //cout << entry.first.first << "\n";
            for(int j = 0; j<entry.second; j++)
                nextWords1.push_back(get<1>(entry.first));
        }
    }

    int randIndex;
    if(nextWords1.empty()){
        //no more valid words
        cout << "sad\n";
    }
    else{
        uniform_int_distribution<> distr(0,nextWords1.size()-1);
        randIndex = distr(gen);
        current2 = nextWords1[randIndex];
        //cout << "Second word: " << current2 << "\n";
    }

    //BEGIN LOOPING TRIGRAM
    string oneBack;
    int missCount = 0;
    for(int i = 0; i < sentLen-1; i++){
        //cout << "Len pos: " << i << "    Words: " << current1 << ", " << current2 << "\n";

        vector<string> nextWords2;
        nextWords2.clear();
        for(auto& entry : trigramModel) {
            if(get<0>(entry.first) == current1){
                if(get<1>(entry.first) == current2){
                    for(int j = 0; j<entry.second; j++){
                        nextWords2.push_back(get<2>(entry.first));
                    }
                }
            }
        }



        if(nextWords2.empty() && i<sentLen-2){
            missCount++;
            if(missCount>10){
                for(auto i = sentenceBuilder.begin(); i!=sentenceBuilder.end(); i++){
                    if(*i!="")
                        sentence += *i + " ";
                }

                sentence.pop_back();
                sentence += ".";
                return "Hit dead end in Trigram, sentence length: " + to_string(i) + "Sentence: " + sentence;
            }

            //need to back up and pick a new current 2
            //cout << "Not in corpus\n";
            current2 = current1;
            current1 = oneBack;
            i-=2;
            sentenceBuilder.pop_back();

        }
        else if(nextWords2.empty()){
            sentenceBuilder.push_back(current1);
            current1 = current2;
        }
        else{
            sentenceBuilder.push_back(current1);
            uniform_int_distribution<> distr2(0, nextWords2.size()-1);
            randIndex = distr2(gen);
            oneBack = current1;
            current1 = current2;
            current2 = nextWords2[randIndex];
        }

    }
    sentenceBuilder.push_back(current1);


    for(auto i = sentenceBuilder.begin(); i!=sentenceBuilder.end(); i++){
        if(*i!="")
            sentence += *i + " ";
    }

    sentence.pop_back();
    sentence += ".";

    return sentence;
}


string TRIGRAM::formSentence(string firstWord){
    string sentence,current1,current2;
    vector <string> sentenceBuilder;
    transform(firstWord.begin(),firstWord.end(),firstWord.begin(), ::tolower);
    current1 = firstWord;

    //random device
    random_device rd;
    mt19937 gen(rd());

    //checks if firstword is in map.
    bool validFirstWord = false;
    for(auto& entry : trigramModel) {
        if (get<0>(entry.first) == current1) {
            validFirstWord = true;
        }
    }

    //if inputted first word not in map, then selects a random one from the corpus
    if(!validFirstWord){
        cout << "Invalid word inputted. Random word selected for seed of sentence.\n";
        uniform_int_distribution<> distr(0,words.size()-1);
        current1 = removeStops(words[distr(gen)]);
    }


    //find second current word
    vector<string> nextWords1;
    for(auto& entry : trigramModel) {
        if(get<0>(entry.first) == current1) {
            for(int j = 0; j<entry.second; j++)
                nextWords1.push_back(get<1>(entry.first));
        }
    }

    int randIndex;
    if(nextWords1.empty()){
        //no more valid words
        cout << "sad\n";
    }
    else{
        uniform_int_distribution<> distr(0,nextWords1.size()-1);
        randIndex = distr(gen);
        current2 = nextWords1[randIndex];
    }

    //BEGIN LOOPING TRIGRAM
    string oneBack;
    int missCount = 0;
    bool finito = false;
    int i = 0;
    while(!finito){
        i++;
        vector<string> nextWords2;
        nextWords2.clear();
        for(auto& entry : trigramModel) {
            if(get<0>(entry.first) == current1){
                if(get<1>(entry.first) == current2){
                    for(int j = 0; j<entry.second; j++){
                        nextWords2.push_back(get<2>(entry.first));
                    }
                }
            }
        }



        if(nextWords2.empty()){
            missCount++;
            if(missCount>10){
                for(auto i = sentenceBuilder.begin(); i!=sentenceBuilder.end(); i++){
                    if(*i!="")
                        sentence += *i + " ";
                }

                sentence.pop_back();
                sentence += ".";
                return "Hit dead end in Trigram, sentence length: " + to_string(i) + "Sentence: " + sentence;
            }

            //need to back up and pick a new current 2
            cout << "Not in corpus\n";
            current2 = current1;
            current1 = oneBack;
            sentenceBuilder.pop_back();

        }
        else if(nextWords2.empty()){
            sentenceBuilder.push_back(current1);
            current1 = current2;
        }
        else{
            sentenceBuilder.push_back(current1);
            uniform_int_distribution<> distr2(0, nextWords2.size()-1);
            randIndex = distr2(gen);
            oneBack = current1;
            current1 = current2;
            current2 = nextWords2[randIndex];
            if(current2.back()=='.' || current2.back()=='!' || current2.back()=='?')
                finito = true;
        }

    }
    sentenceBuilder.push_back(current1);
    sentenceBuilder.push_back(current2);


    for(auto i = sentenceBuilder.begin(); i!=sentenceBuilder.end(); i++){
        if(*i!="")
            sentence += *i + " ";
    }

    //sentence.pop_back();


    return sentence;


}



int main(int argc, char **argv)
{

    if(argc < 2){
        cout << "Invalid input. Must be ./ngram <corpus name>\n";
        exit(1);
    }

    //open command file
    ifstream inStream;

    inStream.open(argv[1]);

    if(inStream.fail()) {
        cout << "failed to open file\n";
        exit(1);
    }

    //initialize data structures
    BIGRAM *bg = new BIGRAM();
    TRIGRAM *tg = new TRIGRAM();

    string line;

    //tokenize line by line
    while(getline(inStream,line)){
        bg->tokenize(line);
        tg->tokenize(line);
    }

    //makepairs
    bg->makePairs();
    tg->makeTuplesWithStops();




    //list pairs
//
//    cout << "Bigram Model:\n";
//    for(const auto& entry : bg->bigramModel){
//        cout << "(" << entry.first.first << ", " << entry.first.second << "): " << entry.second << endl;
//    }

    //list tuples
//    cout << "Trigram Model:\n";
//    for(const auto& entry : tg->trigramModel){
//        cout << "(" << get<0>(entry.first) << ", " << get<1>(entry.first) <<  ", " << get<2>(entry.first) << "): " << entry.second << "\n";
//    }

    //basic sentence gen
    cout << "Bigram Sentence, first word I: ";
    cout << bg->formSentence("I", 6) << "\n";

    //basic sentence gen
    cout << "Trigram Sentence, first word I: ";
    cout << tg->formSentence("I") << "\n\n\n\n\n";



//CLIENT FOR PREDICTIVE TEXT


    bool finito = false;
    bool autoEngage = false;
    string inputWord,currentSentence,inputAuto;
    string backOne = " ";
    string backTwo = " ";
    string currentWord;
    int autoCorrectThreshold = 1;

    cout << "Trigram Initialized. Begin typing your sentence, returning input after each word. When you have completed your sentence, enter a '.' \n\n";
    cout << "Word: ";
    cin >> inputWord;

    while(!finito){

        //check for stop character
        if (inputWord == ".") {
            cout << "Stop character received.\n";
            break;
        }
        if(inputWord.back() == '.'){
            cout << "Stop character received.\n";
            currentSentence += inputWord;
            break;
        }
        //update vals for new word
        backTwo = backOne;
        backOne = currentWord;
        currentWord = inputWord;

        currentSentence += inputWord + " ";
        cout << currentSentence << " _____\n";





        if(!backOne.empty() && !backTwo.empty()){
            //cout << "Auto correct Engaged.\n";
            int mostLikelyWordFreq = 0;
            pair<tuple<string,string,string>,int> mostLikelyTrigram;

            for(const auto& entry : tg->trigramModel){
                if(get<0>(entry.first)==backOne && get<1>(entry.first)==inputWord && entry.second>mostLikelyWordFreq){
                    mostLikelyTrigram = entry;
                    mostLikelyWordFreq = entry.second;
                    //cout << get<0>(mostLikelyTrigram.first) << get<1>(mostLikelyTrigram.first) << get<2>(mostLikelyTrigram.first) << mostLikelyTrigram.second << "\n";
                }
            }

            if(mostLikelyTrigram.second>autoCorrectThreshold){
                //cout << "over threshold: \n";
                //cout << get<0>(mostLikelyTrigram.first) << get<1>(mostLikelyTrigram.first) << "\n";
                cout << "Were you going to type '" << get<2>(mostLikelyTrigram.first) << "' ? If so, input 'x' and hit return. Otherwise, type your next word. \n\n";
                cout << "Word: ";
                cin >> inputAuto;
                if(inputAuto=="x") {
                    autoEngage = true;
                    inputWord = get<2>(mostLikelyTrigram.first);
                }
                else {
                    inputWord = inputAuto;
                    autoEngage = false;
                }
            }
            else{
                cout << "Word: ";
                cin >> inputWord;
            }
        }
        else{
            //cout << currentSentence << " _____\n\n";
            cout << "Word: ";
            cin >> inputWord;
        }

    }

    currentSentence.pop_back();
    currentSentence += ".";


    cout << "\nFinal Sentence: " << currentSentence << "\n\n";


    return 0;

}
