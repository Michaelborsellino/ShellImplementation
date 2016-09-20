#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>

using namespace std;

int main()
{
	//Creating character array to arrange permissions
	string permission[8];
	permission[0] = "---";
	permission[1] = "--x";
	permission[2] = "-w-";
	permission[3] = "-wx";
	permission[4] = "r--";
	permission[5] = "r-x";
	permission[6] = "rw-";
	permission[7] = "rwx";

	//Creating directory pointer and struct
	DIR *currentdir = NULL;
	struct dirent *info = NULL;
	//Creating file data struct
	struct stat fileData;
	string sysTime;
	currentdir = opendir(".");

	while(info = readdir(currentdir))
	{

		stat(info->d_name, &fileData);
		char* buffer = (char*)malloc(100);
		strftime(buffer, 100, "%b %d %I:%M", localtime(&fileData.st_mtime));
		//cout<<(fileData.st_mode & S_IRWXU)<<endl;
		cout<<"-"<<permission[(fileData.st_mode & S_IRWXU)>>6]<<permission[(fileData.st_mode & S_IRWXG)>>3]<<permission[(fileData.st_mode & S_IRWXO)>>0]<<
		" "<<fileData.st_nlink<<" "<<getpwuid(fileData.st_uid)->pw_name <<" "<<getgrgid(fileData.st_gid)->gr_name <<" "<<fileData.st_size<<" "<<buffer<<" "<<info->d_name<<endl;

	}

	//for(int i = 0; i < )


}