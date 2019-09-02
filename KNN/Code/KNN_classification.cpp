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
vector<string> emotion_list_2;
vector<string> emotion_list_result;
vector<string> test_emotion_list;

int One_hot[1000][3000];
int validation_One_hot[1000][3000];
int test_One_hot[1000][3000];
 

int passage=0;
int validation=0;
int test=0;

double cal_dis(int train_case,int validation_case) //计算两个文本之间的距离 
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
	string emotion; //训练集对应的情绪 
	double distance; //文本与训练集的距离 
};

bool comp(const node &a,const node &b)
{
    return a.distance<b.distance;
}

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
}

int classification(int k,int validation_case,int mod)//anger,disgust,fear,joy,sad,surprise
{
	vector<node> rank; //用于存储每一项 
	for(int i=0;i<passage;i++)
	{
		node tmp;
		if(mod==0) //计算验证集 
		{
			tmp.distance=cal_dis(i,validation_case);
		} 
		else if(mod==1)  //计算测试集 
		{
			tmp.distance=cal_dis2(i,validation_case);
		}
		tmp.emotion=emotion_list[i];
		rank.push_back(tmp);
	}
	sort(rank.begin(),rank.end(),comp); //根据距离排序 
	int max_emotion;
	if(rank[0].distance==0) //如果有距离为零直接选择 
	{
		max_emotion=emotion_id(rank[0].emotion);
	}
	else
	{
		double vote[8];
		memset(vote,0,sizeof(vote));
		for(int i=0;i<k;i++) //加权投票，用（1/距离）作为权重 
		{
			vote[emotion_id(rank[i].emotion)]+=1/rank[i].distance;
		}
		double max=0;
		for(int i=0;i<6;i++) //找到最大项 
		{
			if(vote[i]>max)
			{
				max=vote[i];
				max_emotion=i;
			}
		}
	}
	return max_emotion;
}

void Print_result()
{
	ofstream fout("DATA\\classification_dataset\\15352334_wujiawei_KNN_classification.csv");
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
	while(getline(fin,buffer)) //逐篇文章读入 
	{
		int p1=0;int p2=0;string tmp;
		while(p2<buffer.length())
		{
			if(buffer[p2]==' '||buffer[p2]==',') //根据空格或都逗号切词 
			{
				tmp=buffer.substr(p1,p2-p1);
				vector<string>::iterator s=find(Word_list.begin(),Word_list.end(),tmp);  
				if(s!=Word_list.end()) //该词汇是否已经出现过 
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
		emotion_list.push_back(buffer.substr(p1,buffer.length()-p1)); //存入情感 
		passage++;
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
		int p2=0;
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
		emotion_list_2.push_back(buffer.substr(p1,buffer.length()-p1));
		validation++;
	}
	
	int best_k=1;
	double best_answer=0;
	for(int k=10;k<sqrt(passage+1)+10;k++) //k的取值范围是10到根号下文章数加10 
	{
		emotion_list_result.clear();
		cout<<"running on k="<<k<<" ";
		double local_answer=0;
		for(int i=0;i<validation;i++)
		{
			emotion_list_result.push_back(emotion_name(classification(k,i,0)));
			if(emotion_list_result[i]==emotion_list_2[i]) local_answer++;
		}
		local_answer=local_answer/validation;
		cout<<local_answer<<endl;
		if(local_answer>best_answer) //取到最佳的k 
		{
			best_answer=local_answer;
			best_k=k;
		}
	}
	cout<<"best k="<<best_k<<endl;
	
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
		test_emotion_list.push_back(emotion_name(classification(best_k,i,1)));
	}
	Print_result();
}


