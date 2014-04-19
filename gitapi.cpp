
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "gitapi.h"

using namespace std;

#define LINE_MAXLENGTH 100

static GitAPIResponse executeGitCommand(string& command);

    // To run a git command on the terminal we first change to the git directory
    // and then run the git command.
    // we can run multiple commands in one line by separating the commands with a semi-colon (";").
    // (e.g "cd /home ; git init").

    // In order to verify if a git command was properly executed we run the command "echo $?" after
    // we execute a git command. (e.g "cd /home ; ls -a ; echo $?")

    // if a previous command ( e.g "ls -a") was executed properly, by running the command "echo $?"
    // right after that execution we will get a zero ('0') return value from "echo $?"
    // else we will get a non zero return value.


    GitAPIResponse gitAPI::gitInit(string& repoPath){

       string command = "cd " + repoPath + ";git init;echo $?";

       return executeGitCommand(command);
    }

    GitAPIResponse gitAPI::gitCommit(string& repoPath, string& message){

        string command = "cd " + repoPath + ";git commit -m " + "\"" + message + "\"" + ";echo $?";

        return executeGitCommand(command);
    }

    GitAPIResponse gitAPI::gitCheckout(string& repoPath, Branch targetBranch){

        string branchName = targetBranch.getName();
        string command = "cd " + repoPath + ";git checkout " + branchName + ";echo $?";

        return executeGitCommand(command);
    }


    GitAPIResponse gitAPI::gitAdd_All(string& repoPath){

        string command = "cd " + repoPath + ";git add --all;echo $?";

        return executeGitCommand(command);
    }

    // not implemented yet
//    GitAPIResponse gitAPI::gitAdd(string& repoPath, vector<string> files){
//
//        string test = "test";
//        GitAPIResponse response (false, test);
//
//        return response;
//    }

    GitAPIResponse gitAPI::gitMerge(string& repoPath, Branch targetBranch, string& message){
        string branchName = targetBranch.getName();
        string command = "cd " + repoPath + ";git merge " + branchName + " -m " + message + ";echo $?";

        return executeGitCommand(command);
    }

    GitAPIResponse gitAPI::gitBranch(string& repoPath, string newBranchName){

        string command = "cd " + repoPath + ";git branch " + newBranchName + ";echo $?";

        return executeGitCommand(command);
    }

    // not implemented yet
//    GitAPIResponse gitAPI::gitMergeMultipleBranches(string& repoPath, vector<Branch> targetBranchs, string& message){
//        string test = "test";
//        GitAPIResponse response (false, test);
//
//        return response;
//    }

    GitAPIResponse gitAPI::gitStatus(string& repoPath){
        string command = "cd " + repoPath + ";git status;echo $?";

        return executeGitCommand(command);
    }

    static GitAPIResponse executeGitCommand(string& command){

        FILE *output = popen(command.c_str(), "r"); // run the commands and saves the output on a file.

        // extract the output message from a file and store it in a string
        char line [LINE_MAXLENGTH];
        string outputMessage;
        while (fgets(line, LINE_MAXLENGTH, output) != NULL) // grabs one line from a file.
                outputMessage += line; // append a line to a string.

        pclose(output); // close the file.

        // grabs the last character in the output message
        // which we deliberately put there with the command "echo $?".
        char c;
        bool isError;
        if( ( c = outputMessage.at( outputMessage.length() - 2 ) ) == '0'){
            isError = false; // the git commad was properly executed.
            // delete the integer value appended to the string by running "echo $?" for a nice/clean output.
            outputMessage.erase(outputMessage.end() - 2);
        }
        else{
            isError = true; // An error occured while trying to run a git command.

            // delete the integer value appended to the string by running "echo $?" for a nice/clean output.
            int length = outputMessage.length();
            for(int i = length -1; i >= length - 5 && i >= 0; i-- )
                if(isdigit( outputMessage.at(i) ))
                    outputMessage.erase(i);
        }

         GitAPIResponse response (isError, outputMessage);

         return response;
         //  return new GitAPIResponse(isError, outputMessage); this will return a pointer
    }