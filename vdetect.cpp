//Nguyen Ngo

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstring>
#include <boost/algorithm/string.hpp>

bool q = false;
bool s = false;
bool d = false;
int files = 0;
std::string signaturefile = "vdetect.str";


int atoh(char c){
    //this function takes a char, and convert it to decimal representation of hex values
    if(tolower(c) - '0' < 10){
        return (tolower(c)- '0');
    }else if(tolower(c) - 'a' < 6){
        return (tolower(c) -'a' + 10);
    }
}

std::string parse(std::string str){
    //this function takes in a string and parses the ascii representation of hexadecimal values into actual hexadecimal values
    std::string retstring;
    for(int i = 0; i < str.length(); i++){
        
        //if the current char is a \, then checks the next 3 chars to see if it is in hexadecimal format
        if(str[i] == '\\' && ((i+3) < str.length()) && str[i+1] == 'x' && (isxdigit(str[i+2])) && (isxdigit(str[i+3]))){
            //convert to decimal by multiplying first value by 16 and adding to second value
            char c = atoh(str[i+2]) *16 + atoh(str[i+3]); 
            retstring.push_back(c);
            i = i+3;   
        } else{
            retstring.push_back(str[i]);
        }
    }
    return retstring;
}

bool check(std::vector<std::string> vdname, std::vector<std::string> vdstring, std::string filename){
    //this function scans a file for vector of signatures and returns true if there is a match. Otherwise it returns false.
    bool retval = false;
    std::ifstream in;
    in.open(filename.c_str());

    if(!in){
        //When a file cannot be opened, call perror(fileName) or the equivalent
        //(that is, print the system error message).
        //Then go on to the next file(if any) or exit
        perror(filename.c_str());
        return 0;
    }
    //stores the whole content of the file into a string
    std::string fullfilestr;
    char c;
    while((c= in.get()) != EOF){
        fullfilestr.push_back(c);
    }
    //std::cout << "full file string: " << fullfilestr << std::endl; 
    //iterate through each signature
    for(int i = 0; i < vdstring.size(); i++){
        //finds the position in the file that contains the signature
        int posinfile = fullfilestr.find(vdstring[i]);
        //if there is a match
        if(posinfile != std::string::npos){
            //if the command line option to not print anything is not set, then prints the name of the signature and the position of byte in the file
            if(q == false){
                if(files >1)
                    std::cout << filename << "(" << posinfile << "): matched signature " << vdname[i] << "\n" << std::endl;
                else
                    std::cout << "(" << posinfile << "): matched signature " << vdname[i] << "\n" << std::endl;
            }
            //std::cout << "vdstring: " << vdstring[i] << std::endl;
            //std::cout << "Match: " << i << std::endl;

            //since there is a match, this function returns true
            retval = true;
            //if the option to stop scanning the file when the first match is detected, then break out of this for loop, close the file, and returns
            if(s == true){
                break;
            }
        }
    }
    in.close();
    return retval;
}

bool checkString(std::vector<std::string> vdname, std::vector<std::string> vdstring, std::string userstr){
    //This function scans a string instead of a file. It has similar logic to the check() function above.
    bool retval = false;
    for(int i = 0; i < vdstring.size(); i++){
        int posinstring = userstr.find(vdstring[i]);
        if(posinstring != std::string::npos){
            if(q == false){
                std::cout << "(" << posinstring << "): matched signature " << vdname[i] << "\n" << std::endl;
            }
            retval = true;
            if(s == true){
                break;
            }
        }
    }
    return retval;
}

bool emptyLine(std::string str){
    //This function checks if the string is an empty line. 
    //Assuming that an empty line means that it does not contain any printable characters
    bool retval = true;
    for(int i = 0; i < str.length(); i++){
        if(isprint(str[i])){
            retval = false;
            break;
        }
    }
    return retval;
}


int main(int argc, char* argv[]){
    //The list of files to be scanned
    std::vector<std::string> filesvector;
    //If there are no files, the nread from stdin
    bool hasfiles = false;
    //set option flags
    if(argc >1 ){
        for(int i = 1 ; i < argc; i++){
            if(strcmp(argv[i], "-d")== 0 ){
                if(i+1 < argc){
                    d = true;
                    signaturefile = argv[i+1];
                    i++;
                }
            }else if(strcmp(argv[i], "-s") == 0){
                s = true;
            }else if(strcmp(argv[i], "-q") == 0){
                q = true;
            }else{
                files++;
                filesvector.push_back(argv[i]);
            }
        }
    }
                        
                            



    std::ifstream in;
    in.open(signaturefile.c_str());
    //Checks for error in opening teh file
    if(!in){
        perror(signaturefile.c_str());
        //Returns 0 since if there are no signatures, then there is no match
        return 0;
    }
    char c;
    //Vector of signature names
    std::vector<std::string> vdname;
    //Vector of signatures
    std::vector<std::string> vdstring;  
    //Each line of the signature files, to be parsed by colon delimiter later
    std::string tempstring;
    bool name = true;
    int currentline = 0;
    bool Malformed = false;
   //Get char from file until EOF
    while((c = in.get()) != EOF){
        //If the first line is a #, then ignores this line
        if(c == '#' && (tempstring.length() == 0)){
            while((c = in.get()) != EOF){
                //Line is ignored by going through to the end of the line
                if(c == '\n'){
                    currentline++;
                    break;
                }
            }
            continue;
            //if(tempstring.length() == 0){
            //    continue;
            //}
        }
        //If the char is a '\n', then we have finished reading a line
        if(c == '\n'){
            currentline++;
            //Finds the index of the first colon in the string
            int colon = tempstring.find_first_of(':', 0);
            //If this line does not contain any printable character, go to the next line
            if(emptyLine(tempstring)){
                tempstring.erase();
                continue;
            }
            //Else if this line does contain printable characters and there is not a colon in the string, then it is a malformed line
            if(colon == std::string::npos){
                //Set malformed flag
                Malformed = true;
                //If the option -q is not set, then prints the Malformed line number
                if(q == false){
                    std::cout << "Line " << currentline << ": Malformed line in " << signaturefile << std::endl;
                }
                //Erase the temp string and go to the next line in the signature file
                tempstring.erase();
                continue;
            }
            //If the line is not empty and is not malformed, then push the sinature name into the name vector, and push the signature into the signature vector
            vdname.push_back(tempstring.substr(0,colon));
            vdstring.push_back(tempstring.substr(colon+1, tempstring.length()-1));
            //Trim left the signature string for spaces
            boost::trim_left(vdstring[vdstring.size()-1]);
            //Change the ascii represenation of hexadecimals to actual hexadecimals
            vdstring[vdstring.size()-1] = parse(vdstring[vdstring.size()-1]);
            //Trim left and right the name string for spaces
            boost::trim(vdname[vdname.size()-1]);
            //std::cout << "vdname: " << vdname[vdname.size() -1] << "     ";
            //std::cout << "vdstring: " << vdstring[vdstring.size() -1] << std::endl;
            //Once the name string and signature string have been saved, erase the temporary string and go to the next line of the signature file
            tempstring.erase();  
            continue;
        }
        tempstring.push_back(c);    
    }
    in.close();
    //Match flag
    bool Match = false;
    if(files > 0 ){
        //Go through each file and Scan for signatures
        for(int i = 0; i < filesvector.size(); i++){
            //set Match flag to true if there is a match
            if(check(vdname, vdstring, filesvector[i].c_str())){
                Match = true;
            }
            if(q == false){
                std::cout << std::endl;
            }
        }
    }else{
        std::string userinput;
        int c;
        if(q == false){
            std::cout << "Enter input to be scanned. Press ctrl+d twice or Enter then ctrl+d to start scannning" << std::endl;
            std::cout << "Input: ";
        }
        //Gets user input. Stops until EOF
        while((c = getchar()) != EOF){
            userinput.push_back((unsigned char)c);
        }
        
        if(q == false){
            std::cout << std::endl;
        } 
        //Check the string for the signatures
        if(checkString(vdname, vdstring, userinput.c_str())){
            Match = true;
        }
    }
    if(Match == false){
         return 0;
    }else if(Match == true && Malformed == false){
        return 1;
    }else if(Match == false && Malformed == true){
        return 2;
    }else{
        return 3;
    }

}














