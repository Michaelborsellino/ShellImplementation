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
	DIR *currentdir1 = NULL;
	struct dirent *info = NULL;
	//Creating file data struct
	struct stat fileData;
	//struct stat fileData1;
	string sysTime;
	currentdir = opendir(".");
	//currentdir1 = opendir(".");
	off_t totalCount = 0;
	// while(info = readdir(currentdir1))
	// {

	// 	stat(info->d_name, &fileData1);
	// 	if(!S_ISDIR(fileData1.st_mode))
	// 		totalCount += fileData1.st_size;
	// }
	// cout<<"total "<<totalCount/1024<<endl;

	while(info = readdir(currentdir))
	{

		stat(info->d_name, &fileData);
		char* buffer = (char*)malloc(100);
		strftime(buffer, 100, "%b %d %I:%M", localtime(&fileData.st_mtime));
		//cout<<(fileData.st_mode & S_IRWXU)<<endl;
		if(!S_ISDIR(fileData.st_mode))
			cout<<"-"<<permission[(fileData.st_mode & S_IRWXU)>>6]<<permission[(fileData.st_mode & S_IRWXG)>>3]<<permission[(fileData.st_mode & S_IRWXO)>>0]<<
			" "<<fileData.st_nlink<<" "<<getpwuid(fileData.st_uid)->pw_name <<" "<<getgrgid(fileData.st_gid)->gr_name <<" "<<fileData.st_size<<" "<<buffer<<" "<<info->d_name<<endl;

	}

	//for(int i = 0; i < )


}