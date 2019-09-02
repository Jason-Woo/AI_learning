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

struct data //�洢���� 
{
	vector<int> info; //����������ֵ 
	int flag; //ѵ�����Ľ��	
};

struct node //���Ľڵ� 
{
	vector<data> ele; //����ڵ���������� 
	vector<node*> children; //��ǰ�ڵ���ӽڵ�	
	bool flag[12]; //���ڵ���ѱ����ڷ�������� 
	int pos_num; //�ýڵ��½��Ϊ���ĵ�ĸ��� 
	int neg_num; //�ýڵ��½��Ϊ���ĵ�ĸ��� 
	int A; //��ǰ�����ӽڵ������ 
	vector<int> key; //ÿһ���ӽڵ��Ӧ�Ľڵ��������ֵ 
};

struct characterist //Ϊ�˸�����������Ľṹ���洢һ�����ݼ�һ������������ 
{
    vector<int> key; //�洢ĳһ���������п��ܵ�ȡֵ; 
    vector<double> key_cnt; //��¼ÿһ��ȡֵ���ֵĴ��� 
    vector<double> positive_result; //ÿһ��ȡֵ���Ϊ���Ĵ��� 
    vector<double> negative_result; //ÿһ��ȡֵ���Ϊ���Ĵ��� 
};

double cal(double num) //����-num * log��num��������log0 = 0 
{
	if(num == 0) return 0;
	else return (-1 * num * log2(num));
}

vector<data> train_set; //ѵ���� 
vector<data> test_set; //���Լ� 
vector<data> validation_set; //��֤�� 
vector<characterist> ch; //�洢���г��ֵ����� 
vector<int> vali_result; //��֤��ģ��Ԥ����Ľ�� 
vector<int> test_result; //���Լ�ģ��Ԥ����Ľ�� 
node root; //���ĸ��ڵ� 

double total_positive; //��ǰ��������ļ��ϵ����������Ŀ 
double total_negative; //��ǰ��������ļ��ϵĸ��������Ŀ 

int train_size; //������ѵ�����Ĵ�С 
int val_size; //��������֤���Ĵ�С 
int train_cnt = 0; //ʵ�ʵ�ѵ�����Ĵ�С 
int val_cnt = 0; //ʵ�ʵ���֤���Ĵ�С 
	
int random_cin() //�������������з�����֤����ѵ���� 
{
	int a = rand() % 787;
	//���ѵ����������������֤�� 
	if(train_cnt >= train_size) return 1; 
	//��֤������������ѵ����
	if(val_cnt >= val_size) return 0;  
	if(a < train_size) return 0;
	else return 1;
}
	
void data_in(string file_name, int type) //��������ĺ��� 
{
	//��ʼ������ 
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
	
	string buffer; //���뻺�� 
	int test_num = 0; //��ǰ���Լ���Ŀ 
	while(getline(fin, buffer)) //����һ������ 
	{
		int p1 = 0; int p2 = 0;
		int tmp_dimension = 0;
		string tmp;
		int R = random_cin(); //R=0ʱ����ѵ������R=1������֤�� 
		
		while(p2 < buffer.length())
		{
			if(buffer[p2] == ',') //�ָ����� 
			{
				tmp = buffer.substr(p1, p2 - p1);
				int tmp_num = stoi(tmp); //stringתint 
				if(type == 0 && R == 0) //����ѵ���������� 
				{
					train_set[train_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 0 && R == 1) //������֤�������� 
				{
					validation_set[val_cnt].info[tmp_dimension] = tmp_num;
				}
				else if(type == 1) //������Լ������� 
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
		if(type == 0 && R == 0) //����ѵ�����Ľ�� 
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
		else if(type == 0 && R == 1) //������֤���Ľ�� 
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

int select_A(vector<data> v, int type, bool tmp[10]) //����һ�����ݼ���ѡ�������ʹ�õ����� 
{
	double index[10]; //������ÿһ�����Ե�ָ�� 
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
		for(int j = 0; j < 9; j ++) //����ch[j]�ڲ������� 
		{
			int tmp_pos;
			vector<int>::iterator it;
			it = find(ch[j].key.begin(), ch[j].key.end(), v[i].info[j]);
			if(it == ch[j].key.end()) //ͳ�����Կ��ܵ�ȡֵ��������������������ȡֵ 
			{
				ch[j].key.push_back(v[i].info[j]);
				tmp_pos = ch[j].key.size() - 1;	
			}
			else tmp_pos = it - ch[j].key.begin();
			
			ch[j].key_cnt[tmp_pos] ++;
			if(v[i].flag == 1) ch[j].positive_result[tmp_pos] ++;
			else ch[j].negative_result[tmp_pos] ++;
		}
		//���㵱ǰ���ݼ��ϵ�һЩ���� 
		if(v[i].flag == 1) total_positive ++;
		else total_negative ++;
	}
	double total_num = total_negative + total_positive;
	
	if(type == 0 || type == 1) //�����ID3����C4.5 
	{
		//������Ϣ�� 
		double HD = cal(total_positive/total_num) + cal(total_negative/total_num);
		double gx[10];
		memset(gx, 0, sizeof(gx));
		
		for(int i = 0; i < 9; i ++)
		{
			//���������� 
			for(int j = 0; j < ch[i].key.size(); j++)
			{
				if(ch[i].key_cnt[j] != 0)
				{
					double tmp1 =  ch[i].positive_result[j] / ch[i].key_cnt[j];
					double tmp2 =  ch[i].negative_result[j] / ch[i].key_cnt[j];
					gx[i] += (ch[i].key_cnt[j] / total_num) * (cal(tmp1) + cal(tmp2));
				}
            }
            index[i] = HD - gx[i]; //������Ϣ���� 
		}
		
        if(type == 1) //�����C4.5
        {
			for(int i = 0; i < 9; i ++)
			{
				//���������Ϣ 
				double split_info = 0;
				for(int j = 0; j < ch[i].key.size(); j ++)
				{
					split_info += cal(ch[i].key_cnt[j] / total_num);
				}
				if(split_info != 0) index[i] /= split_info; //������Ϣ������ 
				else index[i] = 0;
			}
		}

		double max_num = 0;
		int result = -1;
		for(int i = 0; i < 9; i ++) //ѡ��������Ϣ�������Ϣ������ 
		{
			if(tmp[i] == true && index[i] > max_num)
			{
				max_num = index[i];
				result = i;
			}
		}
		return result; //�������ʺϵ����� 
	}
	else if(type == 2) //�����CART 
	{
		for(int i = 0; i < 9; i ++)
		{
			//�������ϵ�� 
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
		for(int i = 0; i < 9; i ++) //ѡ������ϵ����С�ĵ� 
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
	//�ȸ��µ�ǰ�ڵ��ڵ����� 
	int A = select_A(N->ele, type, N->flag); //��ѡ��ǰ����ʹ�õ����� 
	N->pos_num = total_positive;
	N->neg_num = total_negative;
	N->A = A;
	//����Ѿ�û�п��õķ���㣬ֹͣ����
	if(A == -1) return;  
	//����ڵ���ֻ��һ�ֽ����ֹͣ����
	if(N->pos_num == 0 || N->neg_num == 0) return;  
	//���벢��ʼ�����е��ӽڵ� 
	for(int i = 0; i < ch[A].key.size(); i ++)
	{
		N->key.push_back(ch[A].key[i]);
		node* child = new node;
		N->children.push_back(child);
		for(int j = 0; j < 10; j ++) N->children.back()->flag[j] = N->flag[j];
		N->children.back()->flag[A] = false;
	}
	//��ʼ�����ݼ���������ӽڵ� 
	for(int i = 0; i < N->ele.size(); i ++)
	{
		vector<int>::iterator it; 
		it = find(N->key.begin(), N->key.end(), N->ele[i].info[A]);
		int pos = it - N->key.begin();
		data tmp_data;
		N->children[pos]->ele.push_back(tmp_data);	
		for(int j = 0; j < 9; j ++) //�����ӽڵ��ڵ����ݼ� 
		{
			N->children[pos]->ele.back().info.push_back(N->ele[i].info[j]);
		}
		//���ӽڵ��ڼ�¼�Ѿ�ʹ�ù������� 
		N->children[pos]->ele.back().flag = N->ele[i].flag;
	}
	//DFS�ݹ齨�� 
	for(int i = 0; i < N->children.size(); i ++) build_tree(N->children[i], type);
}

int search_tree(node *N, data D) //����ģ��Ԥ���µ����� 
{
	if(N->children.empty()) //����Ѿ��ҵ�Ҷ�ڵ� 
	{
		//���Ҷ�ڵ�洢����������ڸ�����ôԤ��Ϊ������֮��Ȼ 
		if(N->pos_num > N->neg_num) return 1;
		else return -1; 
	}
	else 
	{
		vector<int>::iterator it = find(N->key.begin(), N->key.end(), D.info[N->A]);
		if(it != N->key.end()) //����ҵ��˵�ǰ���ԣ�DFS������ѯ 
		{
			return search_tree(N->children[it - N->key.begin()], D);
		}
		else //���û���ҵ���ǰ���Ե�ȡֵ��ֱ���ڸýڵ����Ԥ�� 
		{
			if(N->pos_num > N->neg_num) return 1;
			else return -1; 
		}
	}
}

void val(int type) //������֤���Ͳ��Լ� 
{
	if(type == 0)
	{
		double hit = 0; //��¼һ����ȷ����Ŀ 
		for(int i = 0; i < validation_set.size(); i ++)
		{
			int tmp = search_tree(&root, validation_set[i]);
			vali_result.push_back(tmp);
			if(vali_result[i] == validation_set[i].flag) hit ++;
		}
		double accuarcy = hit / validation_set.size();
		cout<<"Accuracy = "<<accuarcy<<endl; //�����ȷ�� 
	}
	else if(type == 1)
	{
		for(int i = 0; i < test_set.size(); i ++)
		{
			test_result.push_back(search_tree(&root, test_set[i]));
		}
	}
}

void write_result(string file_name) //������Լ���Ԥ���� 
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
	int type = 1; //0��1��2�ֱ��ӦID3��C4.5��CART 
	double percentage_of_train = 0.99; //ѵ����ռ���뼯�ϵİٷֱ� 
	train_size = percentage_of_train * 787;
	val_size = 787 - train_size;
	
	string train_name = "DATA\\train.csv";
	string test_name = "DATA\\test.csv";
	string result_name = "DATA\\15352334_wujiawei.txt";
	
	srand (time(NULL)); //��ʼ��ʱ������� 
	data_in(train_name, 0); //����ѵ��������֤��
	//��ʼ�����ڵ� 
	for(int i = 0; i < train_set.size(); i ++) root.ele.push_back(train_set[i]);
	for(int i = 0; i < 9; i ++) root.flag[i] = true;
	//�Ӹ��ڵ㿪ʼ���� 
	build_tree(&root, type);
	cout<<"Build Tree Success!"<<endl;
	cout<<"Applying: ";
	if(type == 0) cout<<"ID3"<<endl;
	else if(type == 1) cout<<"C4.5"<<endl;
	else cout<<"CART"<<endl;
	val(0); //Ԥ����֤�� 
	data_in(test_name, 1); //������Լ� 
	val(1); //��֤���Լ� 
	write_result(result_name); //���Ԥ��Ĳ��Լ���� 
	return 0;
}

