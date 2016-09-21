#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdlib.h> 
#include <dirent.h>
#include <sys/wait.h>

using namespace std;
char rwBuff[1024];

int main(int argc, char* argv[])
{
	//Begin program loop
	while(1)
	{
		vector<string> tokens, tokensMain;
		string command = "";
		string tempBuff = "";
		int fds[2];
		cout<<"$ ";

		//grab full command with pipes in the parent
		getline(cin,command);
		
		//split them up by the pipe delimiter in the parent
		stringstream bigCommand(command);
		string tempT;
		while(getline(bigCommand,tempT,'|'))
		{
			tokensMain.push_back(tempT);
		}
		//fork and give each child a single command
		pipe(fds);
		write(fds[1],tokensMain[0],1024);
		for(int d = 0; d < tokensMain.size(); d++)
		{
			int *status;
			int pid2 = fork();
			if(pid2 == 0)
			{
			}
			else
			{
				wait(status)
				//if there are no more commands, print out the output
				if (d == tokensMain.size() - 1)
				{
					cout<<read(fds[0],1024)<<endl;

				}
				//If there are, write the result to the pipe
				else
				{
					write(fds[1],read(fds[0],1024),1024);
				}
			}
		}
		//read and write back and forth from the child to the parent

		//Create list of words in command line
		stringstream commandStream(command);
		while(commandStream >> tempBuff)
		{
			tokens.push_back(tempBuff);
		}
		//****************************************Start of built-ins*****************************
		//Built-in for exit
		if(tokens[0] == "exit")
			return 0;
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
				if(stringList[i] != NULL)
				{
					stringList[i] = new char[tokens[i].size() + 1];
					strcpy(stringList[i],tokens[i].c_str());
				}
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
						int pid = fork();
						int *status;
						if(pid == 0)
						{
							tempToks+='/'+tokens[0];
							execve(tempToks.c_str(), stringList,NULL);
						}
						else
							wait(status);
						//Program was found, break execution
						flag = 1;
						break;
					}
				}
				if(flag == 1)
					break;
			}
		}
	}
	return 0;
}