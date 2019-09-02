#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<string.h>
#include<algorithm>
#include<cmath>
#include<float.h>

using namespace std;

vector<string>  Word_list; 
double emotion_list[800][8];
double validation_emotion_list[800][8];
double validation_emotion_list2[800][8];
double test_emotion_list[800][8];

int One_hot[1000][3000];

double TF[1000][3000];
double validation_TF[1000][3000];
double test_TF[1000][3000];

int passage=0;
int validation=0;
int test=0;
int word_num[1000];

double evaluate()
{
	double aver1=0;
	double aver2=0;
	double eva=0;
	double tmp1=0;
	double tmp2=0;
	double tmp3=0;
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<validation;j++)
		{
			aver1+=validation_emotion_list2[j][i];
			aver2+=validation_emotion_list[j][i];
		}
		aver1/=validation;
		aver2/=validation;
		tmp1=0;
		tmp2=0;
		tmp3=0;
		for(int j=0;j<validation;j++)
		{
			tmp1+=((validation_emotion_list2[j][i]-aver1)*(validation_emotion_list[j][i]-aver2));
			tmp2+=((validation_emotion_list2[j][i]-aver1)*(validation_emotion_list2[j][i]-aver1));
			tmp3+=((validation_emotion_list[j][i]-aver2)*(validation_emotion_list[j][i]-aver2));
		}
		eva+=(tmp1/(sqrt(tmp2*tmp3)));
	}
	eva/=6;
	return eva;
}

void regression(int cases,int mod)//anger,disgust,fear,joy,sad,surprise
{
	double final_emotion[10];
	for(int i=0;i<6;i++) final_emotion[i]=0;
	if(mod==0)
	{
		for(int i=0;i<passage;i++)
		{
			double tmp_emotion[10];
			for(int j=0;j<6;j++) tmp_emotion[j]=1;
			for(int j=0;j<Word_list.size();j++)
			{
				for(int k=0;k<validation_TF[cases][j];k++)
				{
					for(int l=0;l<6;l++)
					{
						tmp_emotion[l]*=(1+TF[i][j])/(Word_list.size()+word_num[i]);
					}
				}
			}
			for(int j=0;j<6;j++)
			{
				final_emotion[j]+=tmp_emotion[j]*emotion_list[cases][j];
			}
		}
		double tmp_sum=0;
		for(int i=0;i<6;i++)
		{
			tmp_sum+=final_emotion[i];
		}
		for(int i=0;i<6;i++)
		{
			validation_emotion_list2[cases][i]=final_emotion[i]/tmp_sum;
		}
	}
	else if(mod==1)
	{
		for(int i=0;i<passage;i++)
		{
			double tmp_emotion[10];
			for(int j=0;j<6;j++) tmp_emotion[j]=1;
			for(int j=0;j<Word_list.size();j++)
			{
				for(int k=0;k<test_TF[cases][j];k++)
				{
					for(int l=0;l<6;l++)
					{
						tmp_emotion[l]*=(1+TF[i][j])/(Word_list.size()+word_num[i]);
					}
				}
			}
			for(int j=0;j<6;j++)
			{
				final_emotion[j]+=tmp_emotion[j]*emotion_list[cases][j];
			}
		}
		double tmp_sum=0;
		for(int i=0;i<6;i++)
		{
			tmp_sum+=final_emotion[i];
		}
		for(int i=0;i<6;i++)
		{
			test_emotion_list[cases][i]=final_emotion[i]/tmp_sum;
		}
	}
}

void Print_result()
{
	ofstream fout("DATA\\regression_dataset\\15352334_wujiawei_NB_regression.csv");
	fout<<"textid"<<","<<"anger"<<","<<"disgust"<<","<<"fear"<<","<<"joy"<<","<<"sad"<<","<<"surprise"<<endl;
	for(int i=0;i<test;i++)
	{
		fout<<i+1;
		for(int j=0;j<6;j++)
		{
			fout<<","<<validation_emotion_list2[i][j];
		}
		fout<<endl;
	}
}

int main()
{
	ifstream fin("DATA\\regression_dataset\\train_set.csv");
	if(!fin)
	{
		cout<<"OPEAN FILE ERROR"<<endl;
		return 0;
	}
	string buffer;
	getline(fin,buffer);
	passage=0;
	memset(TF,0,sizeof(TF));
	while(getline(fin,buffer))
	{
		int p1=0;
		int p2=0;
		string tmp;
		bool flag=true;
		int num_cnt=0;
		int tmp_word_num=0;
		while(p2<buffer.length())
		{
			int mod=0;
			if(buffer[p2]==' '||buffer[p2]==',')
			{
				if(buffer[p2]==' ') mod=1;
				else if(buffer[p2]==','&&flag==true)
				{
					flag=false;
					mod=1;
				}
				else if(buffer[p2]==','&&flag==false) mod=2;
			}
			else
			{
				mod=0;
				p2++;
			}
			if(mod==1)
			{
				tmp_word_num++;
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				
				if(s!=Word_list.end())
				{
					TF[passage][s-Word_list.begin()]+=1;
				}
				else
				{
					Word_list.push_back(tmp);
					TF[passage][Word_list.size()-1]+=1;
				}
				p1=p2+1;
				p2=p1+1;
			}
			else if(mod==2)
			{
				tmp=buffer.substr(p1,p2-p1);
				double tmp_num=stod(tmp);
				emotion_list[passage][num_cnt]=tmp_num;
				num_cnt++;
				p1=p2+1;
				p2=p1+1;
			}	
		}
		for(int i=0;i<Word_list.size();i++)
		{
			TF[passage][i]/=tmp_word_num;
		}
		word_num[passage]=tmp_word_num;
		tmp=buffer.substr(p1,buffer.length()-p1);
		emotion_list[passage][num_cnt]=stod(tmp);
		passage++;
	}
		
	ifstream fin2("DATA\\regression_dataset\\validation_set.csv");
	if(!fin2)
	{
		cout<<"OPEAN FILE ERROR"<<endl;
		return 0;
	}
	getline(fin2,buffer);
	validation=0;
	memset(validation_TF,0,sizeof(validation_TF));
	while(getline(fin2,buffer))
	{
		int p1=0;
		int p2=1;
		string tmp;
		int num_cnt=0;
		bool flag=true;
		while(p2<buffer.length())
		{
			int mod=0;
			if(buffer[p2]==' '||buffer[p2]==',')
			{
				if(buffer[p2]==' ') mod=1;
				else if(buffer[p2]==','&&flag==true)
				{
					flag=false;
					mod=1;
				}
				else if(buffer[p2]==','&&flag==false) mod=2;
			}
			else
			{
				mod=0;
				p2++;
			}
			if(mod==1)
			{
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				
				if(s!=Word_list.end())
				{
					validation_TF[validation][s-Word_list.begin()]+=1;
				}
				else
				{
					Word_list.push_back(tmp);
					validation_TF[validation][Word_list.size()-1]+=1;
				}
				p1=p2+1;
				p2=p1+1;
			}
			else if(mod==2)
			{
				tmp=buffer.substr(p1,p2-p1);
				double tmp_num=stod(tmp);
				validation_emotion_list[validation][num_cnt]=tmp_num;
				num_cnt++;
				p1=p2+1;
				p2=p1+1;
			}	
		}
		tmp=buffer.substr(p1,buffer.length()-p1);
		validation_emotion_list[validation][num_cnt]=stod(tmp);
		validation++;
	}

	for(int i=0;i<validation;i++)
	{
		regression(i,0);
	}
	cout<<evaluate()<<endl;
		
	ifstream fin3("DATA\\regression_dataset\\test_set.csv");
	if(!fin3)
	{
		cout<<"OPEAN FILE ERROR"<<endl;
		return 0;
	}
	getline(fin3,buffer);
	test=0;
	memset(test_TF,0,sizeof(test_TF));
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
					test_TF[test][s-Word_list.begin()]+=1;
				}
				else
				{
					Word_list.push_back(tmp);
					test_TF[test][Word_list.size()-1]+=1;
				}
				if(buffer[p2]==','&&cnt==1)
				{
					p2=buffer.length();
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
		regression(i,1);
	}
	Print_result();
}
