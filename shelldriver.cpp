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


int main(int argc, char* argv[])
{
	//Begin program loop
	while(1)
	{
		vector<string> tokens;
		string command = "";
		string tempBuff = "";
		cout<<"$ ";
		getline(cin,command);
		//cout<<command<<endl;
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
			//cout<<tempChar<<endl;
			putenv(tempChar);
			//cout<<getenv("MYPATH")<<endl;
			//free(tempChar);
		}
		//************************************Start of myls***************************************
		else
		{
			//converting vector to array
			int currentS = tokens.size() + 1;
			char* stringList[currentS];
			//vector<char*> constVect;
			for(unsigned int i = 0; i < tokens.size(); i++)
			{
				//constVect.push_back(strcopy(tokens[i]);
				//stringList[i] = tokens[i].c_str();
				//stringList[i] = (char*)malloc(tokens[i].size() + 1);
				if(stringList[i] != NULL)
				{
					stringList[i] = new char[tokens[i].size() + 1];
					//cout<<"Hello"<<endl;
					strcpy(stringList[i],tokens[i].c_str());
					cout<<stringList[i]<<endl;
				}
				//cout<<tokens[i].size()<<endl;
				//cout<<stringList[i]<<endl;
			}
			stringList[tokens.size()] = NULL;
			//constVect.push_back(NULL);
			string pathStuff = getenv("PATH");

			if(getenv("MYPATH") != NULL)
			{
				//cout<<"Hello"<<endl;
				pathStuff += ':';
				pathStuff += getenv("MYPATH");
				cout<<pathStuff<<endl;
			}
			stringstream pathInfo(pathStuff);
			
			vector<string> newToks;
			string tempToks;
			DIR *currentdir = NULL;
			struct dirent *info = NULL;
			int flag = 0;
			while(getline(pathInfo,tempToks,':'))
			{
				cout<<tempToks<<endl;
				currentdir = opendir(tempToks.c_str());
				//cout<<"Hello"<<endl;
				while(info = readdir(currentdir))
				{
					
					if(info->d_name == tokens[0])
					{
						cout<<"Hello"<<endl;
						int pid = fork();
						int *status;
						if(pid == 0)
						{
							cout<<"Here"<<endl;
							tempToks+='/'+tokens[0];
							execve(tempToks.c_str(), stringList,NULL);
						}
						else
						{
							wait(status);
							//for(int p = tokens.size()-1; p >= 0; p--)
							//{
							//	free(stringList[p]);
							//}
							
						}
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