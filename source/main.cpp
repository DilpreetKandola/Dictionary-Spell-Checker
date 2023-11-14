#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <sys/time.h>
#include <string>
#include <algorithm>
#include "TimeInterval.h"

//define the table size for hash function(number of lines/words in the dictionary)
constexpr int TABLE_SIZE = 200;

//function prototypes
void clearInput();
void loadDict(const std::string&);
bool wordExists(const std::string&);
void wordSuggestions(const std::string&);
std::string mispellCheck(const std::string&);

//custom hash method
struct customHash 
{

    std::string value;

    //custom hash function for
    std::size_t operator()(const customHash& str) const
    {

        std::size_t hash = 5381;

        for(char c : str.value)
        {
            hash = ((hash << 5) + hash) + c; //hash * 33 + c
        }

        return hash % TABLE_SIZE;

    }

    //define the == operator(to see if two keys hash to the same value)
    bool operator==(const customHash& other) const
    {
        return value == other.value;
    }

};

/* create the hash table using the custom hash function
* the key is the customHash struct and the value is the word
* the customHash struct is used as the key because it contains the word
* and the hash function is used to hash the key to a value
*/
std::unordered_map<customHash, std::string, customHash> hashTable;

int main()
{
    //load the dict; pass the path to the dictionary file to the function
    loadDict("dict.txt");

    //store the user input
    std::string userInput;


    //get input from the user
    std::cout << "Enter Word: ";
    std::cin >> userInput;

    //start the timer
    TimeInterval timer;
    timer.start();


    //case 1 of assignment
    if(wordExists(userInput))
    {
        std::cout << "Word Exists: TRUE \n";
        wordSuggestions(userInput);

    }
    else if(!mispellCheck(userInput).empty())
    {
        std::string oldWord = userInput;
        std::string correctedWord = mispellCheck(userInput);
        std::cout << "Did you mean: " << correctedWord << "\n";
        clearInput();

        std::cin >> userInput;
        std::transform(userInput.begin(), userInput.end(), userInput.begin(), ::tolower);

        if(userInput == "yes")
        {
            std::cout << "Word Exists: TRUE \n";
            wordSuggestions(correctedWord);
            
        }
        else
        {
            std::cout << "Word Exists: FALSE \n";
            wordSuggestions(oldWord);
        }

    }
    else
    {
        std::cout << "Word Exists: FALSE \n";
    }

    //stop the timer
    timer.stop();
    std::cout << "Time Elapsed: " << timer.GetInterval() << " micro-seconds \n";


}

//clear the input buffer function
void clearInput()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//load dictionary from file
void loadDict(const std::string& filename)
{

    //open the file
    std::ifstream file(filename);

    if(file.is_open())
    {
        std::string word;

        while(file >> word)
        {
            
            customHash key;

            //convert word to lowercase
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            key.value = word;

            //print key for testing
            //std::size_t hash = key(key);
            //std::cout << "Key: " << hash << std::endl;

            hashTable[key] = word;
        }

        file.close();
    }
    else 
    {   //throw an error if the file cannot be opened
        throw std::runtime_error("Error: Unable to open file...");
    }

}

//check if the word exists in the dictionary
bool wordExists(const std::string& word)
{
    customHash key;
    key.value = word;

    //std::size_t hash = key(key);
    //std::cout << "Key: " << hash << std::endl;

    //return true if the word is found
    if(hashTable.find(key) != hashTable.end())
    {
        return true;
    }

    //return false if the word is not found
    return false;

}

//word suggestions function
void wordSuggestions(const std::string& word)
{

    //suggest words with same first two letters
    std::string prefix = word.substr(0, 2);

    std::cout << "Suggested Words: \n";

    for(const auto& entry : hashTable)
    {
        //check if the word has the same prefix and is not the same word
        if(entry.first.value.substr(0, 2) == prefix && entry.first.value != word)
        {
            
            std::cout << entry.first.value << std::endl;
        }

    }


}


//mispell check function
std::string mispellCheck(const std::string& word)
{
    //get the first letter of the word
    char firstLetter = word[0];

    //word with most letters in common
    std::string possibleWord;
    int mostCommonLetters = 0;

    //iterate through the hash table
    for(const auto& entry : hashTable)
    {
        //check if word has the same first letter
        if(entry.first.value[0] == firstLetter)
        {
            //number of common letters
            int commonLetters = 0;

            //iterate through the word
            for(int i = 0; i < entry.first.value.length(); i++)
            {
                //check if the word has the same letter at the same index
                if(entry.first.value[i] == word[i])
                {
                    commonLetters++;
                }
            }

            if(commonLetters > mostCommonLetters)
            {
                possibleWord = entry.first.value;
                mostCommonLetters = commonLetters;
            }

        }


    }

    return possibleWord;
}