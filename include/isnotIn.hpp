#include <vector>
#include <string>

std::vector<char> comm = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '=', '-', '_', '/', '#'};

bool isnot_In(std::string val, std::vector<char> commonCharacters = comm){
    int lenght = val.length();
    char buffer[lenght];
    for(int k = 0; k < lenght; k++){buffer[k] = val.at(k);}
    for(char character: val){
        for(char commonChar: comm){if(character == commonChar){return true;}}
    }    

    return false;
}

std::string toString(char *buffer, int bufferSize){
    std::string temp;
    for (int i = 0; i < bufferSize; i++) {
        temp = temp + buffer[i];
    }

    return temp;
}