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
double emotion_list_2[800][8];
double emotion_list_result[800][8];
double result[800][8];

int One_hot[1000][3000];
int validation_One_hot[1000][3000];
int test_One_hot[1000][3000];

int passage=0;
int validation=0;
int test=0;

double evaluate() //计算验证集计算结果与答案的相关系数 
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
			aver1+=emotion_list_result[j][i];
			aver2+=emotion_list_2[j][i];
		}
		aver1/=validation;
		aver2/=validation;
		tmp1=0;
		tmp2=0;
		tmp3=0;
		for(int j=0;j<validation;j++)
		{
			tmp1+=(emotion_list_result[j][i]-aver1)*(emotion_list_2[j][i]-aver2);
			tmp2+=(emotion_list_result[j][i]-aver1)*(emotion_list_result[j][i]-aver1);
			tmp3+=(emotion_list_2[j][i]-aver2)*(emotion_list_2[j][i]-aver2);
		}
		eva+=tmp1/(sqrt(tmp2)*sqrt(tmp3));
	}
	eva/=6;
	return eva;
}

double cal_dis(int train_case,int validation_case)
{
	double sum=0;
	for(int i=0;i<Word_list.size();i++)
	{
		double tmp_dis=One_hot[train_case][i]-validation_One_hot[validation_case][i];
		sum+=tmp_dis*tmp_dis;
	}
	sum=sqrt(sum);
}

double cal_dis2(int train_case,int test_case)
{
	double sum=0;
	for(int i=0;i<Word_list.size();i++)
	{
		double tmp_dis=One_hot[train_case][i]-test_One_hot[test_case][i];
		sum+=tmp_dis*tmp_dis;
	}
	sum=sqrt(sum);
}

struct node
{
	int id;
	double distance;
};

bool comp(const node &a,const node &b)
{
    return a.distance<b.distance;
}

void regression(int k,int validation_case,int mod)//anger,disgust,fear,joy,sad,surprise
{
	vector<node> rank;
	for(int i=0;i<passage;i++)
	{
		node tmp;
		if(mod==0) tmp.distance=cal_dis(i,validation_case); //计算验证集 
		else if(mod==1) tmp.distance=cal_dis2(i,validation_case); //计算测试集 
		tmp.id=i;
		rank.push_back(tmp);
	}
	sort(rank.begin(),rank.end(),comp); //根据距离的远近进行排序 
	double res=0;
	if(rank[0].distance==0)  //如果有距离为零，直接赋值 
	{
		for(int i=0;i<6;i++)
		{
			if(mod==0) emotion_list_result[validation_case][i]=emotion_list[rank[0].id][i];
			else if(mod==1) result[validation_case][i]=emotion_list[rank[0].id][i];
		}
	}
	else	//前k项加权相加
	{
		for(int i=0;i<6;i++)
		{
			for(int j=0;j<k;j++)
			{
				res+=emotion_list[rank[j].id][i]/rank[j].distance;	
			}
			if(mod==0) emotion_list_result[validation_case][i]=res;
			else if(mod==1) result[validation_case][i]=res;
		}
	}
}

void Print_result()  //打印结果 
{
	ofstream fout("DATA\\regression_dataset\\15352334_wujiawei_KNN_regression.csv");
	fout<<"textid"<<","<<"anger"<<","<<"disgust"<<","<<"fear"<<","<<","<<"joy"<<","<<"sad"<<","<<"surprise"<<endl;
	for(int i=0;i<test;i++)
	{
		fout<<i;
		for(int j=0;j<6;j++)
		{
			fout<<","<<result[i][j];
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
	memset(One_hot,0,sizeof(One_hot));
	while(getline(fin,buffer))
	{
		int p1=0;
		int p2=1;
		string tmp;
		bool flag=true;
		int num_cnt=0;
		while(p2<buffer.length())
		{
			int mod=0;
			if(buffer[p2]==' '||buffer[p2]==',')
			{
				if(buffer[p2]==' ') mod=1;
				else if(buffer[p2]==','&&flag==true)
				{
					flag=false;
					mod=1; //输入文本 
				}
				else if(buffer[p2]==','&&flag==false) mod=2; //输入情绪 
			}
			else
			{
				mod=0; //继续遍历 
				p2++;
			}
			if(mod==1)
			{
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				
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
	memset(validation_One_hot,0,sizeof(validation_One_hot));
	while(getline(fin2,buffer))
	{
		int p1=0;
		int p2=1;
		string tmp;
		bool flag=true;
		int num_cnt=0;
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
			else if(mod==2)
			{
				tmp=buffer.substr(p1,p2-p1);
				double tmp_num=stod(tmp);
				emotion_list_2[validation][num_cnt]=tmp_num;
				num_cnt++;
				p1=p2+1;
				p2=p1+1;
			}	
		}
		tmp=buffer.substr(p1,buffer.length()-p1);
		emotion_list_2[validation][num_cnt]=stod(tmp);
		validation++;
	}	

	int best_k=1;
	double best_answer=0;
	for(int k=1;k<=10;k++)
	{
		memset(emotion_list_result,0,sizeof(emotion_list_result));
		cout<<"running on k="<<k<<" ";
		double local_answer=0;
		for(int i=0;i<validation;i++)
		{
			regression(k,i,0);
		}
		for(int i=0;i<validation;i++) //归一化 
		{
			double sum_up=0;
			for(int j=0;j<6;j++)
			{
				sum_up+=emotion_list_result[i][j];
			}
			for(int j=0;j<6;j++)
			{
				emotion_list_result[i][j]/=sum_up;
			}
		}
		local_answer=evaluate(); //评估相关系数 
		cout<<local_answer<<endl;
		if(local_answer>best_answer)
		{
			best_answer=local_answer;
			best_k=k;
		}
	}
	cout<<best_answer<<endl;
	
	ifstream fin3("DATA\\regression_dataset\\test_set.csv");
	if(!fin3)
	{
		cout<<"OPEAN FILE ERROR"<<endl;
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
	test--;
	for(int i=0;i<test;i++)
	{
		regression(best_k,i,1);
	}
	for(int i=0;i<test;i++) //归一化 
	{
		double sum_up=0;
		for(int j=0;j<6;j++)
		{
			sum_up+=result[i][j];
		}
		for(int j=0;j<6;j++)
		{
			result[i][j]/=sum_up;
		}
	}
	Print_result();
}
