#include<iostream>
#include<fstream>
#include<string>
#include<string.h>
#include<float.h>
#include<vector> 
#include<stdlib.h>

using namespace std;

int iterate=10;

class set // 用于存储被分类的所有点 
{
	public:
		int index_num;  //点的个数 
		int index_dimension; //点的维度 
		double accuracy; //准确率 
		double precision; //精确率 
		double recall; //召回率 
		double f1; //F1值 
		vector<vector<double> >index; //用于存储每一个x 
		vector<double> flag; //用于存储每一个y 
		vector<double> flag_predict; //用于存储PLA计算出的y 
		set();
		set(string file,int type); //构造函数 
		void Eva_set(); //评估四个指标 
		void wr_flag(vector<double> w); //借助w向量生成评估的y值 
		void print(); //写结果 
};

set::set()
{
	index_num=0;
	index_dimension=0;
}

set::set(string file,int type) //file是打开的文件名，当type不为零说明输入测试集 
{
	ifstream fin(file);
	if(!fin)
	{
		cout<<"OPEAN "<<file<<" ERROR"<<endl;	
		return;
	}
	index.resize(4005);
	for(int i=0;i<4005;i++) index[i].resize(70);
	flag.resize(4005);
	flag_predict.resize(4005);
	string buffer;
	index_num=0;
	index_dimension=1; //因为每一个x都在前面插入1 
	
	while(getline(fin,buffer))
	{
		index[index_num][0]=1; //插入1 
		int tmp_dimension=1;
		int p1=0;int p2=0;
		string tmp;
		while(p2<buffer.length())
		{
			if(buffer[p2]==',')
			{
				tmp=buffer.substr(p1,p2-p1);
				index[index_num][tmp_dimension]=stod(tmp);
				tmp_dimension++;
				p1=p2+1;
				p2++;
			}
			else
			{
				p2++;
			}
		}
		index_dimension=tmp_dimension;
		if(type==0) //如果不是测试集，输入标准的y 
		{
			if(buffer[buffer.size()-2]=='-')
			{
				flag[index_num]=-1;
			}
			else
			{
				flag[index_num]=1;
			}
		}
		index_num++; 
	}
	accuracy=0;
	precision=0;
	recall=0;
	f1=0;
}

void set::wr_flag(vector<double> w)
{
	for(int i=0;i<index_num;i++)
	{
		double tmp_result=0;
		for(int j=0;j<index_dimension;j++)
		{
			tmp_result+=index[i][j]*w[j];
		}
		if(tmp_result>0)
		{
			flag_predict[i]=1;
		}
		else if(tmp_result<0)
		{
			flag_predict[i]=-1;
		}
		else
		{
			flag_predict[i]=0;
		}
	}
}

void set::Eva_set()
{
	double TP=0;double FN=0;double TN=0;double FP=0;
	for(int i=0;i<index_num;i++)
	{
		if(flag[i]==1)
		{
			if(flag_predict[i]==1) TP++;
			else if(flag_predict[i]=-1) FN++;
		}
		else if(flag[i]==-1)
		{
			if(flag_predict[i]==-1) TN++;
			else if(flag_predict[i]=1) FP++;
		}
	}
	accuracy=(TP+TN)/(TP+FP+TN+FN);
	recall=TP/(TP+FN);
	precision=TP/(TP+FP);
	f1=(2*precision*recall)/(precision+recall);
}

void set::print()
{
	ofstream fout("DATA\\15352334_wujiawei_PLA.csv");
	if(!fout)
	{
		cout<<"WRITE DATA ERROR!"<<endl;
		return;
	}
	for(int i=0;i<index_num;i++) fout<<flag_predict[i]<<endl;
	fout.close();
}

class W //存储向量w 
{
	public:
		vector<double> index; //存储w 
		int size; //向量的维度 
		W(int size,int initi)
		{
			this->size=size;
			index.resize(size+1);
			for(int i=0;i<size;i++)
			{
				index[i]=initi; //初始化向量 
			}
		}
		void refresh(set S); //PLA算法更新w	
};

void W::refresh(set S) //best_w是口袋里最优的w 
{
	for(int i=0;i<S.index_num;i++)
	{
		double tmp_result=0;
		for(int j=0;j<size;j++) //计算w*x 
		{
			tmp_result+=S.index[i][j]*this->index[j]; 
		}
		if(tmp_result*S.flag[i]<=0) //如果预测错误 
		{
			for(int j=0;j<size;j++)
			{
				this->index[j]+=S.index[i][j]*S.flag[i];
			}
		}
	}
}

int main()
{
	string train_set="DATA\\train.csv"; //训练集 
	string val_set="DATA\\val.csv"; //验证集 
	string test_set="DATA\\test.csv"; //测试集 
	set S1(train_set,0);
	W local_w(S1.index_dimension,1);
	for(int i=0;i<iterate;i++)
	{
		local_w.refresh(S1);
	}
	
	set S2(val_set,0);
	S2.wr_flag(local_w.index);
	S2.Eva_set(); //评估结果准确率 
	cout<<"Accuracy="<<S2.accuracy<<endl;
	cout<<"Recall="<<S2.recall<<endl;
	cout<<"Precision="<<S2.precision<<endl;
	cout<<"F1="<<S2.f1<<endl;
	set S3(test_set,1);
	S3.wr_flag(local_w.index);
	S3.print();
	
	return 0;
} 
