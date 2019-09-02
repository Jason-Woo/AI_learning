#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<string.h>
#include<algorithm>
#include<cmath>

using namespace std;

vector<string>  Word_list; 
vector<string> emotion_list;
vector<string> validation_emotion_list;
vector<string> validation_emotion_list2;
vector<string> test_emotion_list;

int One_hot[1000][3000];
int validation_One_hot[1000][3000];
int test_One_hot[1000][3000];

int emotion_One_hot[8][1000];

int passage=0;
int validation=0;
int test=0;

double emotion_num[8];
double emotion_word_num[8];
double emotion_non_rep[8];

int emotion_id(string e)
{
	if(e=="anger") return 0;
	else if(e=="disgust") return 1;
	else if(e=="fear") return 2;
	else if(e=="joy") return 3;
	else if(e=="sad") return 4;
	else if(e=="surprise") return 5;
	else cout<<"EMOTION ERROR"<<endl;
}

string emotion_name(int i)
{
	if(i==0) return "anger";
	else if(i==1) return "disgust";
	else if(i==2) return "fear";
	else if(i==3) return "joy";
	else if(i==4) return "sad";
	else if(i==5) return "surprise";
	else cout<<"EMOTION ERROR"<<endl;
}

int classification(int cases,int mod)//anger,disgust,fear,joy,sad,surprise
{
	double probability[8];
	for(int i=0;i<8;i++) probability[i]=1;
	for(int i=0;i<Word_list.size();i++)
	{
		if(validation_One_hot[cases][i]==1)
		{
			for(int j=0;j<6;j++)
			{
				double tmp1,tmp2,tmp3;
				tmp1=0.01;
				for(int k=0;k<passage;k++)
				{
					if(One_hot[k][i]==1&&(emotion_id(emotion_list[k])==j))
					{
						tmp1+=1;
					}
				}
				tmp2=emotion_word_num[j]+emotion_non_rep[j];
				tmp3=tmp1/tmp2;
				probability[j]*=tmp3;
			}
		}	
	}
	for(int j=0;j<6;j++)
	{
		double tmp=emotion_num[j]/passage;
		probability[j]*=tmp;
	}
	double best_probability=0;
	double best_emotion=0;
	for(int j=0;j<6;j++)
	{
		if(probability[j]>best_probability)
		{
			best_probability=probability[j];
			best_emotion=j;
		}
	}
	return best_emotion;
}

void Print_result()
{
	ofstream fout("DATA\\classification_dataset\\15352334_wujiawei_NB_classification.csv");
	fout<<"textid"<<","<<"label"<<endl;
	for(int i=0;i<test;i++)
	{
		fout<<i+1<<","<<test_emotion_list[i]<<endl;
	}
}

int main()
{
	ifstream fin("DATA\\classification_dataset\\train_set.csv");
	if(!fin)
	{
		cout<<"ERROR"<<endl;
		return 0;
	}
	string buffer;
	getline(fin,buffer);
	passage=0;
	memset(One_hot,0,sizeof(One_hot));
	memset(emotion_num,0,sizeof(emotion_num));
	memset(emotion_word_num,0,sizeof(emotion_word_num));
	memset(emotion_One_hot,0,sizeof(emotion_One_hot));
	memset(emotion_non_rep,0,sizeof(emotion_non_rep));
	while(getline(fin,buffer))
	{
		int p1=0;
		int p2=0;
		int tmp_word_num=0;
		string tmp;
		while(p2<buffer.length())
		{
			if(buffer[p2]==' '||buffer[p2]==',')
			{
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				tmp_word_num++;
				if(s!=Word_list.end())
				{
					One_hot[passage][s-Word_list.begin()]=1;
				}
				else
				{
					Word_list.push_back(tmp);
					One_hot[passage][Word_list.size()-1]=1;	
				}
				p1=p2+1;
				p2=p1+1;
			}
			else
			{
				p2++;
			}	
		}
		tmp=buffer.substr(p1,buffer.length()-p1);
		emotion_list.push_back(tmp);
		emotion_num[emotion_id(tmp)]++;
		emotion_word_num[emotion_id(tmp)]+=tmp_word_num;
		for(int i=0;i<Word_list.size();i++)
		{
			if(One_hot[passage][i]==1)
			{
				emotion_One_hot[emotion_id(tmp)][i]=1;
			}
		}
		passage++;
	}	
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<Word_list.size();j++)
		{
			if(emotion_One_hot[i][j]==1)
			{
				emotion_non_rep[i]++;
			}
		}
	}
	
	ifstream fin2("DATA\\classification_dataset\\validation_set.csv");
	if(!fin2)
	{
		cout<<"ERROR"<<endl;
		return 0;
	}
	getline(fin2,buffer);
	validation=0;
	memset(validation_One_hot,0,sizeof(validation_One_hot));
	while(getline(fin2,buffer))
	{
		int p1=0;
		int p2=01;
		string tmp;
		while(p2<buffer.length())
		{
			if(buffer[p2]==' '||buffer[p2]==',')
			{
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				
				if(s!=Word_list.end())
				{
					validation_One_hot[validation][s-Word_list.begin()]=1;
				}
				else
				{
					Word_list.push_back(tmp);
					validation_One_hot[validation][Word_list.size()-1]=1;
				}
				p1=p2+1;
				p2=p1+1;
			}
			else
			{
				p2++;
			}	
		}
		validation_emotion_list.push_back(buffer.substr(p1,buffer.length()-p1));
		validation++;
	}
	
	double evaluate=0;
	for(int i=0;i<validation;i++)
	{
		validation_emotion_list2.push_back(emotion_name(classification(i,0)));
		if(validation_emotion_list[i]==validation_emotion_list2[i])
		{
			evaluate++;
		}
	}
	evaluate/=validation;
	cout<<"evaluate "<<evaluate<<endl;
	
	ifstream fin3("DATA\\classification_dataset\\test_set.csv");
	if(!fin3)
	{
		cout<<"ERROR"<<endl;
		return 0;
	}
	getline(fin3,buffer);
	test=0;
	memset(test_One_hot,0,sizeof(test_One_hot));
	while(getline(fin3,buffer))
	{
		int p1=0;
		int p2=0;
		string tmp;
		int cnt=0;
		while(p2<buffer.length())
		{
			if(buffer[p2]==' '||(buffer[p2]==','&&cnt==1))
			{
				
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				if(s!=Word_list.end())
				{
					test_One_hot[test][s-Word_list.begin()]=1;
				}
				else
				{
					Word_list.push_back(tmp);
					test_One_hot[test][Word_list.size()-1]=1;
				}
				
				p1=p2+1;
				p2=p1+1;
			}
			else if((buffer[p2]>='0'&&buffer[p2]<='9')||(buffer[p2]==','&&cnt==0))
			{
				if(buffer[p2]==',') cnt=1;
				p1++;
				p2++;
			}
			else
			{
				p2++;
			}	
		}
		test++;
	}	
	
	for(int i=0;i<test;i++)
	{
		test_emotion_list.push_back(emotion_name(classification(i,1)));
	}
	Print_result();
}
