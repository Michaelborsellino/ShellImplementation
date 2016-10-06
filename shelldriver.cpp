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
pid_t altGrp = 0;
void allCommands(vector<string> tokens, string command, int commandF = 0);
void redir(string,string, vector<string>);

int main(int argc, char* argv[])
{

	//Setting group id stuff
	setpgid(getpid(),getpid());
	//grabbing controlling terminal
	tcsetpgrp(STDIN_FILENO, getpgrp());
	//Begin program loop
	while(1)
	{
		int commandFlag = 0;
		vector<string> tokens, tokensMain;
		string command = "";
		//string tempBuff = "";
		//int fds[2];
		

		//grab full command with pipes in the parent
		while(command == "")
		{
			cout<<"$ ";
			getline(cin,command);
		}
		if ( command.find('&') != string::npos)
		{
			cout<<"Hello"<<endl;
			command.erase(command.size()-1);
			commandFlag = 1;
		}
		//split them up by the pipe delimiter in the parent
		stringstream bigCommand(command);
		string tempT;
		while(getline(bigCommand,tempT,'|'))
		{
			tokensMain.push_back(tempT);
		}

		if(tokensMain.size() > 1)
		{
			//fork and give each child a single command
			
			int *status;
			//vector< vector<int> > pipes;
			int pipes[tokensMain.size()][2];

				for(int p = tokensMain.size()-1, d = 0; p >= 0; p--, d++)
				{
					//int fds[2];
					
					pipe(pipes[d]);
					
					//Because pipes are constructed backwards, standard out is left untouched here.
					//Standard in gets closed however.
					if(p == tokensMain.size() - 1)
					{

							if(fork() == 0)
							{
								cout<<"Parent last "<<tokensMain[p]<<endl;
								
								//Attach standard in to the Read end of pipe
								dup2(pipes[d][0],0);
								//close the write end of the pipe
								close(pipes[d][1]);

								command = tokensMain[p];
								cout<<"Executing "<<tokensMain[p]<<endl;
								allCommands(tokens, command);
								exit(0);
							}	
							
					}

					//This is the first command. 
					else if(p == 0)
					{		
						//pipe(fds);																																					
						if(fork() == 0)
						{
							
							//close the standard in
							close(0);
							//connect stdout to write end of pipe
							dup2(pipes[d-1][1],1);
							//close the read end of the pipe
							close(pipes[d-1][0]);
							command = tokensMain[p];
							
							allCommands(tokens, command);
							exit(0);
						}
						
					}
					else 
					{
						//pipe(fds);
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
							allCommands(tokens, command);
							exit(0);
						}	

					}
					
						
				}
					
				
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
		
		else if ((tokensMain[0].find("<") !=string::npos )|| (tokensMain[0].find(">") != string::npos)){
			stringstream newStream(tokensMain[0]);
			string input;
			string tempS;
			string output;
			vector<string> tempList;
			vector<string> trueCommand;
			while(getline(newStream, tempS, '<'))
			{
				tempList.push_back(tempS);
			}
			if (tempList.size() > 1){
				string tempIn;
				stringstream Cleaner(tempList[1]);
				Cleaner >> tempIn;
				input = tempIn;
			}

			stringstream newStream1(tokensMain[0]);
			vector<string> tempList1;

			while(getline(newStream1, tempS, '>'))
			{
				tempList1.push_back(tempS);
			}
			if (tempList1.size() > 1){
				string tempOut;
				stringstream Cleaner(tempList1[1]);
				Cleaner >> tempOut;
				output = tempOut;
			}
			//cout<<tempList[0]<<endl;
			trueCommand.push_back(tempList[0]);
			//cout<<trueCommand[0]<<endl;
			//cout<<"Hello Newb"<<endl;
			redir(input, output, trueCommand);
			cout<<endl;
			//continue;
		}
		else
		{
			command = tokensMain[0];
			allCommands(tokens, command, commandFlag);
		}
	}
	return 0;
}		

void redir(string input,string output, vector<string> tokens)
{
	//int pipeIn[2];
	//int pipeOut[2];
	int *status;
	vector<string> empt;
	int fileIn;
	int fileOut;
	//pipe(pipeIn);
	if(fork() == 0)
	{

		if (!input.empty())
		{	
			
			fileIn = open(input.c_str(),O_RDONLY);
			close(STDIN_FILENO);
			dup(fileIn);
			
		}
		if ( !output.empty())
		{
			fileOut = open(output.c_str(),O_WRONLY | O_CREAT, 0777);
			close(STDOUT_FILENO);
			dup(fileOut);
		}
		cout<<tokens[0]<<endl;
		allCommands(empt,tokens[0]);
		exit(0);
	}	
	wait(status);
	close(fileOut);
	close(fileIn);
	//int renew = open("/dev/tty",ios::in);
	//close(0);
	//dup(renew);
}
void release(string tokes, char* const* stringList)
{
	//cout<<"Hello world\n";
	altGrp++;
	int * status;
	int currentID = getpgrp();
	if(fork() == 0)
	{
		cout<<getpid()<<endl;
		setpgid(getpid(), 0);
		perror("");
		tcsetpgrp(STDOUT_FILENO, currentID);
		tcsetpgrp(STDIN_FILENO, currentID);
		//cout<<getpgid(getpid())<<endl;
		//cout<<tcgetpgrp(currentID)<<endl;
		//cout<<
		execv(tokes.c_str(),stringList);
	}
	else
	{
		setpgid(getpid(),getpid());
		
		//tcsetpgrp(STDOUT_FILENO, getpgrp());
		perror("");
		//
	}
	

	return;
	
}

void allCommands(vector<string> tokens, string command, int commandF)
{
	cout<<commandF<<endl;
	string tempBuff = "";
	//Create list of words in command line
	stringstream commandStream(command);
	while(commandStream >> tempBuff)
	{
		tokens.push_back(tempBuff);
	}
	//****************************************Start of built-ins*****************************
	//Built-in for exit
	if(tokens[0] == "exit")
		exit(0);
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
		//write(fds[1],current,1024);

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
					//cout<<tokens[0]<<endl;
					if(commandF == 1)
					 {
					 	release((tempToks +='/'+tokens[0]),stringList);
					 	flag = 1;
					 	return;
					 }
					//cout<<"Child "<<pid<<" "<<tokens[0]<<endl;
					if(fork() == 0){
						tempToks+='/'+tokens[0];
						//cout<<tempToks<<endl;
						execv(tempToks.c_str(), stringList);
					}
					else
					{
						wait(NULL);
						//cout<<"Hello world\n";
						flag = 1;
					}
				}
			}
			if(flag == 1)
				break;
		}
	}
	//exit(0);

}