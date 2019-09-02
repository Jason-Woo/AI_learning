#include<iostream>
#include<fstream> 
#include<vector>
#include<string>
#include<cmath>
#include<string.h>
#include <stdlib.h>  
#include<time.h> 

using namespace std;

struct data //�洢���� 
{
    vector<double> info; //���ݵ����� 
    int flag; //���ݵķ��� 
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

int random_cin() //��������뻮����ѵ��������֤�� 
{
    int a = rand() % 8000;
    if(train_cnt >= train_size) return 1; 
    if(val_cnt >= val_size) return 0;  
	if(a < train_size) return 0;
	else return 1;
}

void data_in(string file_name, int type) //����������洢 
{
    ifstream fin(file_name);
    if(!fin)
    {
        cout << "OPEN FILE ERROR!" << endl;
        return;
	}
	//��ʼ�� 
	if(type == 0) //�����ѵ��������֤�� 
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
	else if(type == 1) //����ǲ��Լ� 
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
    while(getline(fin, buffer)) //����һ������ 
    {
        int p1 = 0; int p2 = 0;
		int tmp_dimension = 1;
		string tmp;
		int R = random_cin(); //������� 
		
		while(p2 < buffer.length())
		{
			if(buffer[p2] == ',') //��","�������� 
			{
				tmp = buffer.substr(p1, p2 - p1);
				double tmp_num = stod(tmp);
				if(type == 0 && R == 0) //�����ѵ���� 
				{
					train_set[train_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 0 && R == 1) //�������֤�� 
				{
					val_set[val_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 1) //����ǲ��Լ� 
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
	 	//�������ݵķ��� 
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

void norm() //���ݹ�һ�� 
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

void predict(int type) //ʹ��w�����������������Ԥ�� 
{
	if(type == 0) //��֤�� 
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
	else if(type == 1) //���Լ� 
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

void wr_result(string file_name) //д���ļ� 
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
    double percentage_of_train = 0.6;  //ѵ����ռ�� 
	train_size = percentage_of_train * 8000;
	val_size = 8000 - train_size;
	string train_name = "DATA\\train.csv";
	string test_name = "DATA\\test.csv";
	string result_name = "DATA\\15352334_wujiawei.txt";
	data_in(train_name, 0); //����ѵ���� 
	w.resize(41);
	for(int i = 0; i < w.size(); i ++) w[i] = 1;
	norm(); //���� 
	int tmp = 0;
	for(int i = 0; i < iteration; i ++) //�ݶ��½� 
	{
		vector<double> s; //����ÿһ��������Ȩ�ط��� 
		s.resize(train_size);
		for(int n = 0; n < s.size(); n ++) s[n] = 0;
		for(int j = 0; j < train_size; j ++)
		{
			for(int k = 0; k < 41; k ++)
			{
				s[j] += w[k] * train_set[j].info[k];
			}
		}
		
		double grand = 0; //����ÿһ��ά�ȵ��ݶ� 
		for(int k = 0; k < train_size; k ++)
		{
			grand += (cal(s[k])-train_set[k].flag)*train_set[k].info[tmp];
		}
		
		w[tmp] -= step * grand; //����һ��ά�ȵ�w 
		tmp ++;
		if(tmp >= 41) tmp = 0;
	}
	predict(0); //����֤��Ԥ�� 
	data_in(test_name, 1); //������Լ� 
	predict(1); //�Բ��Լ�Ԥ�� 
	wr_result(result_name);
}
