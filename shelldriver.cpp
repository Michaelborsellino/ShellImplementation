#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdlib.h> 

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
			cout<<current<<endl;

		}
		//Built-in for set
		else if(tokens[0] == "set")
		{
			char* tempChar = (char*)malloc(tokens[1].size());
			
			strncpy(tempChar,tokens[1].c_str(),tokens[1].size());
			putenv(tempChar);
			
		}
		
	}
	return 0;
}