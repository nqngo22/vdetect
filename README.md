# vdetect

vdetect is a program that functions in the same way that a basic anti-virus program functions.  
  
The file “vdetect.str” contains lines of the following format:  
name:string   
where name is a string that does not contain a ‘:’ and string is a string. The string may contain  
non-printing characters, which will be recorded as ‘\xnn’, where nn is a 2 digit hexadecimal number.  
This represents the character sequence that indicate the presence of malware.  
If a line begins with the hash mark ‘#’, it is a comment line and is to be ignored.  
  
vdetect reads one or more files and compares each file to the strings in vdetect.str.  
If the file contains any string in vdetect.str, it prints the corresponding name and  
the number of the byte in the file where the string begins. The exit status code is  
to be 0 if no matches occur, 1 if any match occurs, 2 if there is a malformed line  
in vdetect.str and no matches are found, and 3 if there is a malformed line in  
vdetect.str and a match is found.  
  
The command line syntax is:  
  
vdetect [ -q ] [ -s ] [ -d strfile ] [ file1 [ ... ] ]   
  
where file1, ... is the list of files to scan (if none, read from the standard input). Options are:  
  
-d strfile	Use strfile rather than vdetect.str  
-s	Stop scanning the file when the first match is detected  
-q	Do not print any output; just give the exit status code  
