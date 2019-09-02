#include<iostream>
#include<fstream> 
#include<string>
#include<string.h>
#include<stdlib.h> 
#include<vector>
#include<cmath>
#include<algorithm>
#include<set>
#include <stdlib.h>    
#include <time.h>

using namespace std;

struct data //存储数据 
{
	vector<int> info; //所有特征的值 
	int flag; //训练集的结果	
};

struct node //树的节点 
{
	vector<data> ele; //满足节点的所有数据 
	vector<node*> children; //当前节点的子节点	
	bool flag[12]; //父节点的已被用于分类的特征 
	int pos_num; //该节点下结果为正的点的个数 
	int neg_num; //该节点下结果为负的点的个数 
	int A; //当前划分子节点的特征 
	vector<int> key; //每一个子节点对应的节点的特征的值 
};

struct characterist //为了辅助运算引入的结构，存储一个数据集一类特征的特性 
{
    vector<int> key; //存储某一个特征所有可能的取值; 
    vector<double> key_cnt; //记录每一种取值出现的次数 
    vector<double> positive_result; //每一种取值结果为正的次数 
    vector<double> negative_result; //每一种取值结果为负的次数 
};

double cal(double num) //计算-num * log（num），定义log0 = 0 
{
	if(num == 0) return 0;
	else return (-1 * num * log2(num));
}

vector<data> train_set; //训练集 
vector<data> test_set; //测试集 
vector<data> validation_set; //验证集 
vector<characterist> ch; //存储所有出现的特征 
vector<int> vali_result; //验证集模型预测出的结果 
vector<int> test_result; //测试集模型预测出的结果 
node root; //树的根节点 

double total_positive; //当前正在运算的集合的正结果的数目 
double total_negative; //当前正在运算的集合的负结果的数目 

int train_size; //期望的训练集的大小 
int val_size; //期望的验证集的大小 
int train_cnt = 0; //实际的训练集的大小 
int val_cnt = 0; //实际的验证集的大小 
	
int random_cin() //随机将输入的序列放入验证集或训练集 
{
	int a = rand() % 787;
	//如果训练集已满，放入验证集 
	if(train_cnt >= train_size) return 1; 
	//验证集已满，放入训练集
	if(val_cnt >= val_size) return 0;  
	if(a < train_size) return 0;
	else return 1;
}
	
void data_in(string file_name, int type) //处理输入的函数 
{
	//初始化工作 
	ifstream fin(file_name);
    if(!fin)
    {
        cout << "OPEN FILE ERROR!" << endl;
        return;
	}
	train_set.resize(train_size);
	validation_set.resize(val_size);
	test_set.resize(300);
	for(int i = 0; i < train_size; i ++) train_set[i].info.resize(10);
	for(int i = 0; i < val_size; i ++) validation_set[i].info.resize(10);
	for(int i = 0; i < 300; i ++) test_set[i].info.resize(10);
	
	string buffer; //输入缓存 
	int test_num = 0; //当前测试集数目 
	while(getline(fin, buffer)) //输入一行数据 
	{
		int p1 = 0; int p2 = 0;
		int tmp_dimension = 0;
		string tmp;
		int R = random_cin(); //R=0时放入训练集，R=1放入验证集 
		
		while(p2 < buffer.length())
		{
			if(buffer[p2] == ',') //分割数据 
			{
				tmp = buffer.substr(p1, p2 - p1);
				int tmp_num = stoi(tmp); //string转int 
				if(type == 0 && R == 0) //输入训练集的数据 
				{
					train_set[train_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 0 && R == 1) //输入验证集的数据 
				{
					validation_set[val_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 1) //输入测试集的数据 
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
		if(type == 0 && R == 0) //输入训练集的结果 
		{
			if(buffer[buffer.length() - 2] == '-')
			{
				train_set[train_cnt].flag = -1;
			}
			else
			{
				train_set[train_cnt].flag = 1;
			}
			train_cnt ++;
		}
		else if(type == 0 && R == 1) //输入验证集的结果 
		{
			if(buffer[buffer.length() - 2] == '-')
			{
				validation_set[val_cnt].flag = -1;
			}
			else
			{
				validation_set[val_cnt].flag = 1;
			}
			val_cnt ++; 
		}
		test_num ++;
	}
}

int select_A(vector<data> v, int type, bool tmp[10]) //传入一个数据集，选择出分类使用的特性 
{
	double index[10]; //最后衡量每一个特性的指标 
	memset(index, 0, sizeof(index));
	ch.clear();
	ch.resize(10);
	for(int i = 0; i < 10; i ++)
	{
		ch[i].key_cnt.resize(50);
		ch[i].positive_result.resize(50);
		ch[i].negative_result.resize(50);
	}
	total_positive = 0;
	total_negative = 0;	
	for(int i = 0; i < v.size(); i ++)
	{
		for(int j = 0; j < 9; j ++) //更新ch[j]内部的属性 
		{
			int tmp_pos;
			vector<int>::iterator it;
			it = find(ch[j].key.begin(), ch[j].key.end(), v[i].info[j]);
			if(it == ch[j].key.end()) //统计属性可能的取值，如果不存在则插入这种取值 
			{
				ch[j].key.push_back(v[i].info[j]);
				tmp_pos = ch[j].key.size() - 1;	
			}
			else tmp_pos = it - ch[j].key.begin();
			
			ch[j].key_cnt[tmp_pos] ++;
			if(v[i].flag == 1) ch[j].positive_result[tmp_pos] ++;
			else ch[j].negative_result[tmp_pos] ++;
		}
		//计算当前数据集合的一些特性 
		if(v[i].flag == 1) total_positive ++;
		else total_negative ++;
	}
	double total_num = total_negative + total_positive;
	
	if(type == 0 || type == 1) //如果是ID3或者C4.5 
	{
		//计算信息熵 
		double HD = cal(total_positive/total_num) + cal(total_negative/total_num);
		double gx[10];
		memset(gx, 0, sizeof(gx));
		
		for(int i = 0; i < 9; i ++)
		{
			//计算条件熵 
			for(int j = 0; j < ch[i].key.size(); j++)
			{
				if(ch[i].key_cnt[j] != 0)
				{
					double tmp1 =  ch[i].positive_result[j] / ch[i].key_cnt[j];
					double tmp2 =  ch[i].negative_result[j] / ch[i].key_cnt[j];
					gx[i] += (ch[i].key_cnt[j] / total_num) * (cal(tmp1) + cal(tmp2));
				}
            }
            index[i] = HD - gx[i]; //计算信息增益 
		}
		
        if(type == 1) //如果是C4.5
        {
			for(int i = 0; i < 9; i ++)
			{
				//计算分裂信息 
				double split_info = 0;
				for(int j = 0; j < ch[i].key.size(); j ++)
				{
					split_info += cal(ch[i].key_cnt[j] / total_num);
				}
				if(split_info != 0) index[i] /= split_info; //计算信息增益率 
				else index[i] = 0;
			}
		}

		double max_num = 0;
		int result = -1;
		for(int i = 0; i < 9; i ++) //选出最大的信息增益或信息增益率 
		{
			if(tmp[i] == true && index[i] > max_num)
			{
				max_num = index[i];
				result = i;
			}
		}
		return result; //返回最适合的特征 
	}
	else if(type == 2) //如果是CART 
	{
		for(int i = 0; i < 9; i ++)
		{
			//计算基尼系数 
			for(int j = 0; j < ch[i].key.size(); j ++)
			{
				if(ch[i].key_cnt[j] != 0)
				{
					double tmp1 = ch[i].positive_result[j] / ch[i].key_cnt[j];
					double tmp2 = ch[i].negative_result[j] / ch[i].key_cnt[j];
					index[i] += (ch[i].key_cnt[j] / total_num) * (1 - pow(tmp1, 2) - pow(tmp2, 2));
				}
			}
		}
		double min_num = 999999;
		int result = -1;
		for(int i = 0; i < 9; i ++) //选出基尼系数最小的点 
		{
			if(tmp[i] == true && index[i] < min_num)
			{
				min_num = index[i];
				result = i;
			}
		}
		if(total_positive == 0 || total_negative == 0)
		{
			return -1;
		}
		if(result == -1)
		{
			return -1;
		}
		else return result;
	}
}

void build_tree(node *N, int type)
{
	//先更新当前节点内的属性 
	int A = select_A(N->ele, type, N->flag); //先选择当前分类使用的特征 
	N->pos_num = total_positive;
	N->neg_num = total_negative;
	N->A = A;
	//如果已经没有可用的分类点，停止建树
	if(A == -1) return;  
	//如果节点下只有一种结果，停止建树
	if(N->pos_num == 0 || N->neg_num == 0) return;  
	//插入并初始化所有的子节点 
	for(int i = 0; i < ch[A].key.size(); i ++)
	{
		N->key.push_back(ch[A].key[i]);
		node* child = new node;
		N->children.push_back(child);
		for(int j = 0; j < 10; j ++) N->children.back()->flag[j] = N->flag[j];
		N->children.back()->flag[A] = false;
	}
	//开始将数据集分类放入子节点 
	for(int i = 0; i < N->ele.size(); i ++)
	{
		vector<int>::iterator it; 
		it = find(N->key.begin(), N->key.end(), N->ele[i].info[A]);
		int pos = it - N->key.begin();
		data tmp_data;
		N->children[pos]->ele.push_back(tmp_data);	
		for(int j = 0; j < 9; j ++) //插入子节点内的数据集 
		{
			N->children[pos]->ele.back().info.push_back(N->ele[i].info[j]);
		}
		//在子节点内记录已经使用过的特征 
		N->children[pos]->ele.back().flag = N->ele[i].flag;
	}
	//DFS递归建树 
	for(int i = 0; i < N->children.size(); i ++) build_tree(N->children[i], type);
}

int search_tree(node *N, data D) //根据模型预测新的数据 
{
	if(N->children.empty()) //如果已经找到叶节点 
	{
		//如果叶节点存储的正结果多于负，那么预测为正，反之亦然 
		if(N->pos_num > N->neg_num) return 1;
		else return -1; 
	}
	else 
	{
		vector<int>::iterator it = find(N->key.begin(), N->key.end(), D.info[N->A]);
		if(it != N->key.end()) //如果找到了当前特性，DFS继续查询 
		{
			return search_tree(N->children[it - N->key.begin()], D);
		}
		else //如果没有找到当前特性的取值，直接在该节点完成预测 
		{
			if(N->pos_num > N->neg_num) return 1;
			else return -1; 
		}
	}
}

void val(int type) //处理验证集和测试集 
{
	if(type == 0)
	{
		double hit = 0; //记录一共正确的数目 
		for(int i = 0; i < validation_set.size(); i ++)
		{
			int tmp = search_tree(&root, validation_set[i]);
			vali_result.push_back(tmp);
			if(vali_result[i] == validation_set[i].flag) hit ++;
		}
		double accuarcy = hit / validation_set.size();
		cout<<"Accuracy = "<<accuarcy<<endl; //输出正确率 
	}
	else if(type == 1)
	{
		for(int i = 0; i < test_set.size(); i ++)
		{
			test_result.push_back(search_tree(&root, test_set[i]));
		}
	}
}

void write_result(string file_name) //输出测试集的预测结果 
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
	int type = 1; //0，1，2分别对应ID3，C4.5和CART 
	double percentage_of_train = 0.99; //训练集占输入集合的百分比 
	train_size = percentage_of_train * 787;
	val_size = 787 - train_size;
	
	string train_name = "DATA\\train.csv";
	string test_name = "DATA\\test.csv";
	string result_name = "DATA\\15352334_wujiawei.txt";
	
	srand (time(NULL)); //初始化时间的种子 
	data_in(train_name, 0); //输入训练集和验证集
	//初始化根节点 
	for(int i = 0; i < train_set.size(); i ++) root.ele.push_back(train_set[i]);
	for(int i = 0; i < 9; i ++) root.flag[i] = true;
	//从根节点开始建树 
	build_tree(&root, type);
	cout<<"Build Tree Success!"<<endl;
	cout<<"Applying: ";
	if(type == 0) cout<<"ID3"<<endl;
	else if(type == 1) cout<<"C4.5"<<endl;
	else cout<<"CART"<<endl;
	val(0); //预测验证集 
	data_in(test_name, 1); //输入测试集 
	val(1); //验证测试集 
	write_result(result_name); //输出预测的测试集结果 
	return 0;
}

