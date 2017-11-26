/*************************************************************************
	> File Name: MyShell.cpp
	> Author: LoveZJT
	> Mail: 151220055@smail.nju.edu.cn
	> Created Time: 2017年09月26日 星期二 15时41分23秒
 ************************************************************************/

#include<iostream>
#include<sstream>
#include<unistd.h>
#include<string>
#include<cstring>
#include<cstdio>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>
#include<fstream>
using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"  
#define GREEN   "\033[1m\033[32m"

int main()
{
	string line,newfile;
	int count=0,sfi=dup(STDIN_FILENO),sfo=dup(STDOUT_FILENO);
	cout<<GREEN<<"MyShell$"<<RESET;
	//while(count<10)
	while(getline(cin,line))
	{
		if(line=="")
		{
			cout<<GREEN<<"MyShell$"<<RESET;
			continue;
		}
		//cout<<"getline"<<line<<endl;
		stringstream ss(line);
		string cmd,new_cmd;
		ss>>cmd;
		new_cmd=cmd;
		if(cmd=="exit")
			break;
		else if(cmd=="cd")
		{
			char newdir[100];
			ss>>newdir;
			chdir(newdir);
			cout<<GREEN<<"MyShell$"<<RESET;
			continue;
		}
		else if(cmd=="wait")
		{
			int pid;
			ss>>pid;
			waitpid(pid,NULL,0);
			cout<<"PID "<<pid<<" finish"<<endl;
			cout<<GREEN<<"MyShell$"<<RESET;
			continue;
		}
		int i=0;
		bool back=false,red_0=false,red_1=false,red_2=false,pipe=true;
		char *argv[10];
		for(i=0;i<10;++i)
		{
			argv[i]=new char [10];
			//argv[i]='0';
		}
		char temp[10];
		i = -1;
		ss.clear();
		ss.str(line);
		while(pipe)
		{
			//cout<<"pipe"<<endl;
			pipe=false;
			cmd=new_cmd;
			//cout<<"cmd:"<<cmd<<endl;
			if(i!=-1)
			{
				int fp=open("youdontknow2",O_RDONLY);
				strcpy(argv[0],cmd.c_str());
				i=1;
				dup2(fp,0);
				//close(fp);
			}
			else
				i=0;
			while(ss>>temp)
			{
				if(strcmp(temp,"&")==0)
				{
					back=true;
					break;
				}
				if(strcmp(temp,">")==0)
				{
					ss>>newfile;
					red_1=true;
					if(!ss.eof())
						ss>>temp;
					else
						break;
					//break;
				}
				if(strcmp(temp,"<")==0)
				{
					ss>>newfile;
					red_0=true;
					if(!ss.eof())
						ss>>temp;
					else
						break;
					//break;
				}
				if(strcmp(temp,">>")==0)
				{
					ss>>newfile;
					red_2=true;
					if(!ss.eof())
						ss>>temp;
					else
						break;
					//break;
				}
				if(strcmp(temp,"|")==0)
				{
					pipe=true;
					ss>>new_cmd;
					break;
				}
				if(argv[i]==NULL)
					argv[i]=new char [10];
				strcpy(argv[i],temp);
				++i;
			}
			argv[i]=NULL;
			int pid=fork();
			if(pid<0)
			{
				cout<<"fork failed!"<<endl;
				return 0;
			}
			else if(pid==0)
			{
				if(red_1)
				{
					int fp=open(newfile.c_str(),O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
					dup2(fp,1);
				}
				if(red_0)
				{
					int fp=open(newfile.c_str(),O_RDONLY);
					dup2(fp,0);
				}
				if(red_2)
				{
					int fp=open(newfile.c_str(),O_RDWR|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);
					dup2(fp,1);
				}
				if(pipe)
				{
					int fp=open("youdontknow",O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);
					dup2(fp,1);
				}
				/*dup2(sfo,1);
				cout<<"cmd:"<<cmd<<endl<<"argv:";
				for(int j=0;j<i;++j)
					cout<<argv[j]<<" ";
				cout<<endl;*/
				//cout<<"hello"<<endl;	
				execvp(cmd.c_str(),argv);
				cout<<RED<<"Child process failed!"<<RESET<<endl;
				exit(0);
			}
			else
			{
				if(!back)
				{
					waitpid(pid,NULL,0);
					dup2(sfi,0);
					if(!access("youdontknow",F_OK))
					{
						FILE *fp=fopen("youdontknow","r");
						FILE *fq=fopen("youdontknow2","w");
						char ch;
						while(!feof(fp))
						{
							ch=fgetc(fp);
							fputc(ch,fq);
							if(feof(fp))
								break;
						}
						fclose(fp);
						fclose(fq);
					}

				}
			}
		}
		remove("youdontknow");
		remove("youdontknow2");
		cout<<"Child process finish!"<<endl;
		cout<<GREEN<<"MyShell$"<<RESET;
	}
	return 0;
}
