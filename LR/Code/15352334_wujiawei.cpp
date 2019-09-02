#include<iostream>
#include<fstream> 
#include<vector>
#include<string>
#include<cmath>
#include<string.h>
#include <stdlib.h>  
#include<time.h> 

using namespace std;

struct data //存储数据 
{
    vector<double> info; //数据的特征 
    int flag; //数据的分类 
};

vector<data> train_set;
vector<data> val_set;
vector<data> test_set;
vector<int> test_result;

int train_size;
int val_size;
int train_cnt = 0;
int val_cnt = 0;

int iteration = 1000; 
double step = 0.01;

vector<double> w;

int random_cin() //随机将输入划分至训练集和验证集 
{
    int a = rand() % 8000;
    if(train_cnt >= train_size) return 1; 
    if(val_cnt >= val_size) return 0;  
	if(a < train_size) return 0;
	else return 1;
}

void data_in(string file_name, int type) //数据输入与存储 
{
    ifstream fin(file_name);
    if(!fin)
    {
        cout << "OPEN FILE ERROR!" << endl;
        return;
	}
	//初始化 
	if(type == 0) //如果是训练集或验证集 
	{
		train_set.resize(train_size);
		val_set.resize(val_size);
	    for(int i = 0; i < train_size; i ++)
	    {
	        train_set[i].info.resize(41);
	        train_set[i].info[0] = 1;
	    } 
	    for(int i = 0; i < val_size; i ++)
	    {
	        val_set[i].info.resize(41);
	        val_set[i].info[0] = 1;
	    }
	}
	else if(type == 1) //如果是测试集 
	{
		test_set.resize(2000);
		for(int i = 0; i < 2000; i ++)
	    {
	        test_set[i].info.resize(41);
	        test_set[i].info[0] = 1;
	    }
	} 
     
    string buffer;
    int test_num = 0;
    while(getline(fin, buffer)) //读入一行数据 
    {
        int p1 = 0; int p2 = 0;
		int tmp_dimension = 1;
		string tmp;
		int R = random_cin(); //随机分配 
		
		while(p2 < buffer.length())
		{
			if(buffer[p2] == ',') //用","划分输入 
			{
				tmp = buffer.substr(p1, p2 - p1);
				double tmp_num = stod(tmp);
				if(type == 0 && R == 0) //如果是训练集 
				{
					train_set[train_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 0 && R == 1) //如果是验证集 
				{
					val_set[val_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 1) //如果是测试集 
				{
					test_set[test_num].info[tmp_dimension] = tmp_num;
				}
				tmp_dimension ++;
                p1 = p2 + 1;
                p2 ++;
			}
			else
			{
				p2 ++;
			}
		}
	 	//输入数据的分类 
		if(type == 0 && R == 0)
		{
			if(buffer[buffer.length() - 1] == '0')
			{
				train_set[train_cnt].flag = 0;
			}
			else
			{
				train_set[train_cnt].flag = 1;
			}
			train_cnt ++;
		}
		else if(type == 0 && R == 1)
		{
			if(buffer[buffer.length() - 1] == '0')
			{
				val_set[val_cnt].flag = 0;
			}
			else
			{
				val_set[val_cnt].flag = 1;
			}
			val_cnt ++; 
		}
		test_num ++;
    }
}

void norm() //数据归一化 
{
	for(int i = 1; i < 41; i ++)
	{
		double maxnum = 0;
		double minnum = 40;
		for(int j = 0; j < train_size; j ++)
		{
			if(train_set[j].info[i] > maxnum) maxnum = train_set[j].info[i];
			if(train_set[j].info[i] < minnum) minnum = train_set[j].info[i];
		}
		for(int j = 0; j < train_size; j ++)
		{
			train_set[j].info[i]=(train_set[j].info[i]-minnum)/(maxnum-minnum);
		}
	}
}

double cal(double num)
{
	return (1 / (1 + exp(-1 * num)));
}

void predict(int type) //使用w对输入的数据类别进行预测 
{
	if(type == 0) //验证集 
	{
		double cnt = 0;
		for(int i = 0; i < val_size; i ++)
		{
			double tmp = 0;
			for(int j = 0; j < 41; j ++)
			{
				tmp += val_set[i].info[j] * w[j];
			}
			double result = cal(tmp);
			if(result > 0.5 && val_set[i].flag == 1) cnt ++;
			if(result < 0.5 && val_set[i].flag == 0) cnt ++;
		}
		cnt /= val_size;
		cout<<"Accuarcy = "<<cnt<<endl;
	}
	else if(type == 1) //测试集 
	{
		test_result.resize(2000);
		for(int i = 0; i < 2000; i ++)
		{
			double tmp = 0;
			for(int j = 0; j < 41; j ++)
			{
				tmp += test_set[i].info[j] * w[j];
			}
			double result = cal(tmp);
			if(result >= 0.5) test_result[i] = 1;
			else test_result[i] = 0;
		}
	}
}

void wr_result(string file_name) //写回文件 
{
	ofstream fout(file_name);
	if(!fout)
	{
		cout<<"WRITE DATA ERROR!"<<endl;
		return;
	}
	for(int i = 0; i < test_result.size(); i ++) fout<<test_result[i]<<endl;
	fout.close();
}

int main()
{
    double percentage_of_train = 0.6;  //训练集占比 
	train_size = percentage_of_train * 8000;
	val_size = 8000 - train_size;
	string train_name = "DATA\\train.csv";
	string test_name = "DATA\\test.csv";
	string result_name = "DATA\\15352334_wujiawei.txt";
	data_in(train_name, 0); //输入训练集 
	w.resize(41);
	for(int i = 0; i < w.size(); i ++) w[i] = 1;
	norm(); //正则化 
	int tmp = 0;
	for(int i = 0; i < iteration; i ++) //梯度下降 
	{
		vector<double> s; //计算每一个样例的权重分数 
		s.resize(train_size);
		for(int n = 0; n < s.size(); n ++) s[n] = 0;
		for(int j = 0; j < train_size; j ++)
		{
			for(int k = 0; k < 41; k ++)
			{
				s[j] += w[k] * train_set[j].info[k];
			}
		}
		
		double grand = 0; //计算每一个维度的梯度 
		for(int k = 0; k < train_size; k ++)
		{
			grand += (cal(s[k])-train_set[k].flag)*train_set[k].info[tmp];
		}
		
		w[tmp] -= step * grand; //更新一个维度的w 
		tmp ++;
		if(tmp >= 41) tmp = 0;
	}
	predict(0); //对验证集预测 
	data_in(test_name, 1); //输入测试集 
	predict(1); //对测试集预测 
	wr_result(result_name);
}
