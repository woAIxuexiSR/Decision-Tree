#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <time.h>

using namespace std;

class Data
{
public:
    vector<int> attributes;
    int nclass;

public:
    Data() {}
    Data(const vector<int>& _a, int _c) : attributes(_a), nclass(_c) {}
};


int class_num;
unordered_map<string, int> class_trans;
int num_attribute;
vector<int> attributes_num;
vector<unordered_map<string, int> > attributes_trans;

vector<Data> dataset;

double cal_entropy(const vector<Data>& d)
{
    int n = d.size();

    vector<int> cnt(class_num);
    for(int i = 0; i < n; ++i)
        cnt[d[i].nclass]++;

    double entropy = 0.0;
    for(int i = 0; i < class_num; ++i)
    {
        if(cnt[i] > 0)
        {
            double prob = (double)cnt[i] / n;
            entropy += -prob * log2(prob);
        }
    }

    return entropy;
}


int node_count = 0;
int max_depth = 5;
double min_ig = 0.01;
int min_samples = 10;

class TreeNode
{
private:
    vector<TreeNode*> child;
    int nattribute;     // classify by the n-th attribute
    int nclass;         // if leaf, then return nclass; else -1
  
public:
    TreeNode() {}
    TreeNode(const vector<Data>& d, bool used[], int depth, bool IG_ratio)
    {
        node_count++;

        int majority = 0, max_num = 0;
        vector<int> cnt(class_num);
        for(int i = 0; i < d.size(); ++i)
            cnt[d[i].nclass]++;
        for(int i = 0; i < class_num; ++i)
        {
            if(cnt[i] > max_num)
            {
                max_num = cnt[i];
                majority = i;
            }
        }

        if(d.size() <= min_samples)
        {
            nclass = majority;
            return;
        }

        double H_d = cal_entropy(d);
        if(H_d == 0.0)
        {
            nclass = majority;
            return;
        }

        if(depth > max_depth)
        {
            nclass = majority;
            return;
        }

        int n = d.size();
        double max_value = 0.0;
        int idx = -1;
        for(int i = 0; i < num_attribute; ++i)
        {
            if(!used[i])
            {
                vector<vector<Data> > tmp(attributes_num[i]);
                for(int j = 0; j < n; ++j)
                    tmp[d[j].attributes[i]].push_back(d[j]);

                double IG = H_d;
                for(int j = 0; j < attributes_num[i]; ++j)
                    IG -= (double)tmp[j].size() / n * cal_entropy(tmp[j]);

                if(IG_ratio)
                {
                    double entropy = 0.0;
                    for(int j = 0; j < attributes_num[i]; ++j)
                    {
                        if(tmp[j].size() > 0)
                        {
                            double prob = (double)tmp[j].size() / n;
                            entropy += -prob * log2(prob);
                        }
                    }
                    IG /= entropy;
                }

                if(IG >= max_value)
                {
                    max_value = IG;
                    idx = i;
                }
            }
        }

        if(idx == -1 || max_value < min_ig)
        {
            nclass = majority;
            return;
        }

        used[idx] = true;
        vector<vector<Data> > tmp(attributes_num[idx]);
        for(int j = 0; j < n; ++j)
            tmp[d[j].attributes[idx]].push_back(d[j]);
        
        bool* tmpused = new bool[num_attribute];
        for(int i = 0; i < attributes_num[idx]; ++i)
        {
            memcpy(tmpused, used, sizeof(bool) * num_attribute);
            child.push_back(new TreeNode(tmp[i], tmpused, depth + 1, IG_ratio));
        }
        delete[] tmpused;

        nattribute = idx;
        nclass = -1;
    }

    int classify(const Data& d) const
    {
        if(nclass != -1) return nclass;
        
        int idx = d.attributes[nattribute];
        return child[idx]->classify(d);
    }

    ~TreeNode()
    {
        for(int i = 0; i < child.size(); ++i)
            if(child[i]) delete child[i];
    }
};

void read_file(const string& path)
{
    ifstream f(path, ios::in);
    if(!f)
    {
        cout << "Failed to open file " << path << endl;
        exit(-1);
    }

    string line;
    f >> line;

    num_attribute = count(line.begin(), line.end(), ',');
    attributes_num.resize(num_attribute, 0);
    attributes_trans.resize(num_attribute);

    do
    {
        int pos = 0;
        vector<int> attr;
        for (int i = 0; i < num_attribute; ++i)
        {
            int comma = line.find(',', pos);
            string s = line.substr(pos, comma - pos);

            if(attributes_trans[i].find(s) == attributes_trans[i].end())
            {
                attributes_trans[i][s] = attributes_num[i];
                attributes_num[i]++;
            }

            attr.push_back(attributes_trans[i][s]);

            pos = comma + 1;
        }

        string s = line.substr(pos, line.size() - pos);
        if(class_trans.find(s) == class_trans.end())
        {
            class_trans[s] = class_num;
            class_num++;
        }

        dataset.push_back(Data(attr, class_trans[s]));

    } while (f >> line);

}

void divide_dataset(vector<Data>& train, vector<Data>& test, double train_set_size)
{
    vector<vector<Data> > subset(class_num);
    for(int i = 0; i < dataset.size(); ++i)
        subset[dataset[i].nclass].push_back(dataset[i]);

    for(int i = 0; i < class_num; ++i)
    {
        int train_size = subset[i].size() * train_set_size;
        for(int j = 0; j < train_size; ++j)
            train.push_back(subset[i][j]);
        for(int j = train_size; j < subset[i].size(); ++j)
            test.push_back(subset[i][j]);
    }
}

int main(int argc, char** argv)
{
    bool IG_ratio = false;
    string dataset_name = "tic-tac-toe";
    double train_set_size = 0.5;


    for(int i = 0; i < argc; ++i)
    {
        if(!strcmp(argv[i], "-data"))
        {
            dataset_name = argv[i + 1];
            i += 1;
        }
        else if(!strcmp(argv[i], "-R"))
            IG_ratio = true;
        else if(!strcmp(argv[i], "-ts"))
        {
            train_set_size = atof(argv[i + 1]);
            i += 1;
        }
        else if(!strcmp(argv[i], "-d"))
        {
            max_depth = atoi(argv[i + 1]);
            i += 1;
        }
        else if(!strcmp(argv[i], "-ig"))
        {
            min_ig = atof(argv[i + 1]);
            i += 1;
        }
        else if(!strcmp(argv[i], "-s"))
        {
            min_samples = atoi(argv[i + 1]);
            i += 1;
        }
    }


    const string data_path = "./dataset/" + dataset_name + "/" + dataset_name + ".data";
    read_file(data_path);

    random_shuffle(dataset.begin(), dataset.end());
    vector<Data> train, test;
    divide_dataset(train, test, train_set_size);

    // int train_size = dataset.size() * train_set_size;
    // random_shuffle(dataset.begin(), dataset.end());
    // vector<Data> train {dataset.begin(), dataset.begin() + train_size};
    // vector<Data> test {dataset.begin() + train_size, dataset.end()};

    cout << train.size() << " " << test.size() << endl;

    clock_t start = clock();
    bool* used = new bool[num_attribute];
    memset(used, 0, sizeof(bool) * num_attribute);
    TreeNode* root = new TreeNode(train, used, 0, IG_ratio);
    delete[] used;
    clock_t end = clock();
    cout << "Time to build decision tree : " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "Total tree node number : " << node_count << endl;

    int correct = 0;
    for(int i = 0; i < test.size(); ++i)
    {
        int y = root->classify(test[i]);
        if(y == test[i].nclass)
            correct++;
    }
    cout << "Correct rate : " << (double)correct / test.size() << endl;

    if(root) delete root;

    return 0;
}