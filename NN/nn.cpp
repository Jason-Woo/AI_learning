#include<iostream>
#include<fstream> 
#include<vector>
#include<string>
#include<cmath>
#include<string.h>
#include <stdlib.h>  
#include<time.h> 

using namespace std;

struct data
{
    vector<double> info;
    double flag;
};

vector<data> train_set;
vector<data> val_set;
vector<data> test_set;
vector<int> test_result;
vector<int> val_result;

int train_size;
int val_size;
int train_cnt = 0;
int val_cnt = 0;

vector<vector<double> >  step1;
vector<double> step2;

double learning_rate = 0.000001;
int hidden_layer = 50;
vector<double> bias1;
double bias2;

vector<vector<double> > layer1;
vector<double> layer2;
vector<double> tmp_layer;

double random_init()
{
	double b = (rand() % 10) + 1;
	double result = b * 10 ;
	return result;
}

void Init()
{
    train_set.resize(train_size);
	val_set.resize(val_size);
	for(int i = 0; i < train_size; i ++)
	{
	    train_set[i].info.resize(12);
    } 
    for(int i = 0; i < val_size; i ++)
	{
        val_set[i].info.resize(12);
	}
    test_set.resize(504);
	for(int i = 0; i < 504; i ++)
	{
	    test_set[i].info.resize(12);
	}
    layer1.resize(12);
    step1.resize(12);
    for(int i = 0; i < 12; i ++) layer1[i].resize(hidden_layer);
    for(int i = 0; i < 12; i ++) step1[i].resize(hidden_layer);
    layer2.resize(hidden_layer);
    step2.resize(hidden_layer);
	for(int i = 0; i < 12; i ++)
	{
		for(int j = 0; j < hidden_layer; j ++)
		{
			layer1[i][j] = random_init();
		}
	}
	bias1.resize(hidden_layer);
	for(int i = 0; i < hidden_layer; i ++)
	{
		layer2[i] = random_init();
		bias1[i] = 1;
	}
	tmp_layer.resize(hidden_layer);
	bias2 = 1;
	val_result.resize(val_size);
}

int random_cin()
{
    int a = rand() % 8619;
    if(train_cnt >= train_size) return 1; 
    if(val_cnt >= val_size) return 0;  
	if(a < train_size) return 0;
	else return 1;
}

void data_in(string file_name, int type)
{
    ifstream fin(file_name);
    if(!fin)
    {
        cout << "OPEN FILE ERROR!" << endl;
        return;
	}
	
    string buffer;
    int test_num = 0;
    getline(fin, buffer);
    while(getline(fin, buffer))
    {
        int p1 = 0; int p2 = 0;
		int tmp_dimension = 0;
		string tmp;
		int R = random_cin();
		
		while(p2 < buffer.length())
		{
			if(buffer[p2] == ',') 
			{
				if(tmp_dimension > 1)
                {
                    tmp = buffer.substr(p1, p2 - p1);
                    double tmp_num = stod(tmp);
                    if(type == 0 && R == 0)
                    {
                        train_set[train_cnt].info[tmp_dimension - 2] = tmp_num;
                    }
                    else if(type == 0 && R == 1)
                    {
                        val_set[val_cnt].info[tmp_dimension - 2] = tmp_num;
                    }
                    else if(type == 1)
                    {
                        test_set[test_num].info[tmp_dimension - 2] = tmp_num;
                    }
                }
				tmp_dimension ++;
                p1 = p2 + 1; 
			}
			p2 ++;
		}
	 	
        tmp = buffer.substr(p1,buffer.length()-p1);
        int tmp_num = stod(tmp);

		if(type == 0 && R == 0)
		{
			train_set[train_cnt].flag = tmp_num;
			train_cnt ++;
		}
		else if(type == 0 && R == 1)
		{
			val_set[val_cnt].flag = tmp_num;
			val_cnt ++; 
		}
		test_num ++;
    }
}

void norm()
{
	for(int i = 0; i < 12; i ++)
	{
		double maxnum = 0;
		double minnum = 4000;
		for(int j = 0; j < train_size; j ++)
		{
			if(train_set[j].info[i] > maxnum) maxnum = train_set[j].info[i];
			if(train_set[j].info[i] < minnum) minnum = train_set[j].info[i];
		}
		for(int j = 0; j < val_size; j ++)
		{
			if(val_set[j].info[i] > maxnum) maxnum = val_set[j].info[i];
			if(val_set[j].info[i] < minnum) minnum = val_set[j].info[i];
		}
		if(maxnum != minnum)
		{
			for(int j = 0; j < train_size; j ++)
			{
				train_set[j].info[i]=(train_set[j].info[i]-minnum)/(maxnum-minnum);	
			}
			for(int j = 0; j < val_size; j ++)
			{
				val_set[j].info[i]=(val_set[j].info[i]-minnum)/(maxnum-minnum);	
			}
		}
	}
}

double func(double x)
{
	double result = 1 / (1 + exp(-1 * x));
	return result;
}

double cal_net(vector<double> info)
{	
	for(int i = 0; i < hidden_layer; i++)
	{
		double tmp_result = 0;
		for(int j = 0; j < 12; j ++)
		{
			tmp_result += info[j] * layer1[j][i];
		}
		tmp_result += bias1[i];
		tmp_layer[i] = tmp_result;
	}
	double final_result = 0;
	for(int i = 0; i < hidden_layer; i ++)
	{
		final_result += func(tmp_layer[i]) * layer2[i];
	}
	final_result += bias2;
	return final_result;
} 

void update()
{
	double Err2;
	vector<double> Err1;
	Err1.resize(hidden_layer);
	for(int i = 0; i < 12; i ++)
	{
		for(int j = 0; j < hidden_layer; j ++)
		{
			step1[i][j] = 0;
		}
	}
	for(int i = 0; i < hidden_layer; i ++)
	{
		step2[i] = 0;
	}
	for(int i = 0; i < train_size; i ++)
	{
		double output = cal_net(train_set[i].info);
		Err2 = train_set[i].flag - output;
		for(int j = 0; j < hidden_layer; j ++)
		{
			Err1[j] = tmp_layer[j] * (1 - tmp_layer[j]) * Err2 * layer2[j];
		}
		for(int j = 0; j < hidden_layer; j ++)
		{
			step2[j] += Err2 * func(tmp_layer[j]);
		}
		bias2 += learning_rate * Err2;
		for(int j = 0; j < 12; j ++)
		{
			for(int k = 0; k < hidden_layer; k ++)
			{
				step1[j][k] += Err1[k] * train_set[i].info[j];
			}
		}
		for(int j = 0; j < hidden_layer; j ++) 
		{
			bias1[j] += learning_rate * Err1[j];
		} 
	}	
	for(int j = 0; j < hidden_layer; j ++)
	{
		layer2[j] += learning_rate * step2[j] / train_size;
	}
	for(int j = 0; j < 12; j ++)
	{
		for(int k = 0; k < hidden_layer; k ++)
		{
			layer1[j][k] += learning_rate * step1[j][k] / train_size;
		}
	}
}

void predict(int type)
{
	if(type == 0)
	{
		double val_mse = 0;
		for(int i = 0; i < val_size; i ++)
		{
			double result = cal_net(val_set[i].info);
			val_result[i] = result;
			val_mse += (result - val_set[i].flag) * (result - val_set[i].flag); 
		}
		val_mse /= val_size;
		cout << "VAL_MSE = " << val_mse << endl;
	}
	else if(type == 1)
	{
		for(int i = 0; i < 504; i ++)
		{
			double result = cal_net(test_set[i].info);
			test_result[i] = result;
		}
	}
	else if(type == 2)
	{
		double train_mse = 0;
		for(int i = 0; i < train_size; i ++)
		{
			for(int j=0;j<12;j++) cout<<train_set[i].info[j]<<" ";
			cout<<endl;
			double result = cal_net(train_set[i].info);
			cout<<result<<" "<<train_set[i].flag<<endl;
			train_mse += (result - train_set[i].flag) * (result - train_set[i].flag); 
		}
		train_mse /= train_size;
		cout << "TRAIN_MSE = " << train_mse << endl;
	}
}

void wr_result(string file_name)
{
	ofstream fout(file_name);
	if(! fout)
	{
		cout << "WRITE DATA ERROR!" << endl;
		return;
	}
	for(int i = 0; i < test_result.size(); i ++) fout<<test_result[i]<<endl;
	fout.close();
}

int main()
{
    double percentage_of_train = 0.9;
	train_size = percentage_of_train * 8619;
	val_size = 8619 - train_size;

	string train_name = "DATA\\train.csv";
	string test_name = "DATA\\test.csv";
	string result_name = "DATA\\15352334_wujiawei.txt";
	srand (time(NULL)); 
	Init();

	data_in(train_name, 0);
	norm();
	for(int i=0;i<1000;i++) update();
	predict(2);
	predict(0);
	//data_in(test_name, 1);
	//predict(1);
	//wr_result(result_name);
}
