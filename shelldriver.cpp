#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdlib.h> 
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

using namespace std;
char rwBuff[1024];

void allCommands(string command, int commandF = 0);
void redirt(string,string, string);
void release(string tokes, char* const* stringList);


int main(int argc, char* argv[])
{

	//Setting group id stuff
	setpgid(getpid(),getpid());
	//grabbing controlling terminal
	tcsetpgrp(STDIN_FILENO, getpgrp());
	//Begin program loop
	while(true)
	{
		int commandFlag = 0;
		//Setting up parsing variables
		vector<string> tokens, tokensMain;
		string command = "";
	
		//grab full command with pipes in the parent
		while(command == "")
		{
			cout<<"$ ";
			//Looking for C-d signal
			if(cin.peek() == -1)
			{
				exit(0);
			}
			getline(cin,command);
		}
		//If a background symbol is found, set flag and delete it
		if ( command.find('&') != string::npos)
		{
			command.erase(command.size()-1);
			commandFlag = 1;
		}
		//if not, look for pipe symbols and split them up 
		stringstream bigCommand(command);
		string tempT;
		while(getline(bigCommand,tempT,'|'))
		{
			tokensMain.push_back(tempT);
		}
		//If pipe symbols were found
		if(tokensMain.size() > 1)
		{	
			int *status;
			//create all the pipes
			int pipes[tokensMain.size()][2];

				for(int p = tokensMain.size()-1, d = 0; p >= 0; p--, d++)
				{
					//open the pipe	
					pipe(pipes[d]);
					
					//Because pipes are constructed backwards, standard out is left untouched here.
					//Standard in gets redirected to pipe.
					if(p == tokensMain.size() - 1)
					{

							if(fork() == 0)
							{
								
								//Attach standard in to the Read end of pipe
								dup2(pipes[d][0],0);
								//Close the write end of the pipe
								close(pipes[d][1]);
								command = tokensMain[p];
								//Execute code
								allCommands(command, 0);
								exit(0);
							}	
							
					}

					//This is the case of the first command. 
					//Close read end of pipe, redirect stdout to write end, 
					else if(p == 0)
					{		
						//pipe(fds);																																					
						if(fork() == 0)
						{
							//connect stdout to write end of pipe
							dup2(pipes[d-1][1],1);
							//close the read end of the pipe
							close(pipes[d-1][0]);
							command = tokensMain[p];
							//Execute code
							allCommands(command, 0);
							exit(0);
						}
						
					}
					//Case of a middle command
					//redirect both stdin and stdout to different pipes
					else 
					{
						if(fork() == 0)
						{
							
							//close standard in and standard out
							close(0);
							close(1);
							
							//close the read end of last pipe
							close(pipes[d-1][0]);
							//close the write end of current pipe
							close(pipes[d][1]);
							//attach stdout to write end of last pipe
							dup2(pipes[d-1][1], 1);
							//attach stdin to read end of current pipe
							dup2(pipes[d][0], 0);
							
							command = tokensMain[p];

							//Execute code
							allCommands(command, 0);
							exit(0);
						}	
					}		
				}
					
				//Close all parent ends of pipe, wait for all children
				for(int i = 0; i < tokensMain.size(); i++)
				{
					close(pipes[i][0]);
					close(pipes[i][1]);
				}
				for(int p = 0; p < tokensMain.size(); p++)
				{
					wait(status);
				}
						
		}
		//Parsing for input and output files in redirect
		else if ((command.find("<") !=string::npos )|| (command.find(">") != string::npos))
		{

			stringstream newStream(command);
			string input;
			string tempS;
			string output;
			string trueCommand;
			int startVal = 0, endVal = 0;
			//This part will either terminate at the end of the string or at the > symbol
			if(command.find("<") !=string::npos)
			{
				startVal = command.find("<") + 1;
				if(command.find(">") !=string::npos)
				{
					stringstream temp(command.substr(startVal, (command.find(">")-1) - startVal));
					temp >> input;
				}
				else
				{
					stringstream temp(command.substr(startVal));
					temp >> input;
				}
			}
			else
			{
				startVal = command.find(">");
			}
			//This symbol will always be the last of the two
			if(command.find(">") !=string::npos)
			{
				stringstream temp(command.substr(command.find(">") + 1));
				temp >> output;
			}

			trueCommand = command.substr(0,startVal);
			cout<<trueCommand<<endl;
			redirt(input, output, trueCommand);
			
			continue;
		}
		//Single command, no modifications
		else
		{
			command = tokensMain[0];
			allCommands(command, commandFlag);
		}
	}
	return 0;
}		

void redirt(string input,string output, string tokens)
{
	
	int *status;
	vector<string> empt;
	int fileIn;
	int retainIn = dup(0);
	int retainOut = dup(1);
	int fileOut;
	//redirect stdin and out to proper files
	cout<<tokens<<endl;
	if (!input.empty())
	{	
		
		fileIn = open(input.c_str(),O_RDONLY);
		close(STDIN_FILENO);
		dup(fileIn);
		if(tokens.find("<") != string::npos)
			tokens.erase(tokens.find("<"));
		
	}
	if ( !output.empty())
	{
		fileOut = open(output.c_str(),O_WRONLY | O_TRUNC | O_CREAT, 0777);
		close(STDOUT_FILENO);
		dup(fileOut);
		if(tokens.find(">") != string::npos)
			tokens.erase(tokens.find(">"));
	}
	
	//Execute command like normal
	allCommands(tokens,0);

	//return file descriptors to normal stdin and out
	if(!input.empty())
	{
		close(fileIn);
		dup2(retainIn,0);
	}
	if(!output.empty())
	{
		close(fileOut);
		dup2(retainOut,1);
	
	}
	
}
void release(string tokes, char* const* stringList)
{
	int * status;
	//Double Fork and terminate parent right away so child becomes child of init
	if(fork() == 0)
	{
		if(fork() == 0)
		{
			
			setpgid(getpid(), 0);
			
			execv(tokes.c_str(),stringList);
		}
		else
		{
			exit(0);
		}
	}
	else
	{
		wait(status);
		tcsetpgrp(STDIN_FILENO, getpgrp());
	}
}

void allCommands(string command, int commandF)
{
	//cout<<commandF<<endl;
	vector<string> tokens;
	string tempBuff = "";
	//Create list of words in command line
	stringstream commandStream(command);
	while(commandStream >> tempBuff)
	{
		//cout<<tempBuff<<endl;
		tokens.push_back(tempBuff);
	}
	//****************************************Start of built-ins*****************************
	//Built-in for exit
	if(tokens[0] == "exit")
	{
		exit(0);
	}
	//Built-in for cd
	else if(tokens[0] == "cd")
	{
		//if there is no /, checks current directory
		if(tokens[1][0] != '/')
			tokens[1].insert(0,"./");
		chdir(tokens[1].c_str());
	}
	//Built-in for pwd
	//Might need to account for path names that are really long. solved on getcwd opengroup
	else if(tokens[0] == "pwd")
	{ 
		char* buffer;

		size_t size = 1024;
		buffer = (char*)malloc(size);
		string current(getcwd(buffer,size));
		free(buffer);
		cout<<current<<endl;

	}
	//Built-in for set
	else if(tokens[0] == "set")
	{
		char* tempChar = new char[tokens[1].size() + 1];
		strcpy(tempChar,tokens[1].c_str());
		putenv(tempChar);
	}
	//************************************Start of User Program Execution Block************
	else
	{
		
		//converting vector of strings to array of character arrays
		int currentS = tokens.size() + 1;
		char* stringList[currentS];
		for(unsigned int i = 0; i < tokens.size(); i++)
		{
			stringList[i] = new char[tokens[i].size() + 1];
			strcpy(stringList[i],tokens[i].c_str());
		}
		//Null Terminating string list for exec
		stringList[tokens.size()] = NULL;
		//Combining search paths if MYPATH exists
		string pathStuff = getenv("PATH");
		if(getenv("MYPATH") != NULL)
		{
			pathStuff += ':';
			pathStuff += getenv("MYPATH");
		}
		
		//Convert it to stream for easy parsing
		stringstream pathInfo(pathStuff);
		
		string tempToks;
		DIR *currentdir = NULL;
		struct dirent *info = NULL;
		int flag = 0;
		
		//Search through all PATH and MYPATH directories to look for user program
		while(getline(pathInfo,tempToks,':'))
		{
			currentdir = opendir(tempToks.c_str());
			while(info = readdir(currentdir))
			{
				//If the user program is found, fork current process and execute it
				if(info->d_name == tokens[0])
				{
					//If the background flag is set, go to background execute
					if(commandF == 1)
					 {
					 	release((tempToks +='/'+tokens[0]),stringList);
					 	flag = 1;
					 	return;
					 }
					//Execute command
					if(fork() == 0){
						tempToks+='/'+tokens[0];
						execv(tempToks.c_str(), stringList);
					}
					else
					{
						wait(NULL);
						flag = 1;
					}
				}
			}
			if(flag == 1)
				break;
		}
	}
}