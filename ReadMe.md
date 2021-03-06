# 决策树大作业

苏睿 2001111334

## 一、 编译运行

编程语言：C++

操作系统：Win10

编译器：mingw64 g++ 8.1.0



程序源码包含在 `decision_tree_src` 文件夹中，其中：

`dataset`：包含三个数据集`tic-tac-toe`，`balance-scale`，`nursery`

`main.cpp`：因为程序并不大，所以将所有内容都写在了一个文件中。

`makefile`： `g++ -o main main.cpp -std=c++17 -g`  



使用命令 `make` 编译源文件得到可执行文件`main.exe`

`./main [options]`:

​	`[-data <dataset_name>]` 选择执行的数据集，可选参数`tic-tac-toe`, `balance-scale`, `nursery`

​	`[-R]` 使用信息增益比例进行特征选择

​	`[-ts <train_set_size>]` 选择训练集占比，取值为`[0-1]`，默认值0.5

​	`[-d <max_depth>]` 限制决策树的深度，整数，默认值5

​	`[-ig <min_ig>]` 根据最小信息增益进行剪枝，默认值0.05

​	`[-s <min_samples>]` 根据最少样本数目剪枝，默认值10

例：`./main -data nursery -d 3 -ig 0.0 -s 20 -R` 表示使用nursery数据集进行使用，决策树深度最大为3，最小信息增益为0.0，最少样本数为20，使用信息增益比进行特征选择，训练集占比为默认值0.5.

执行结果如下

![image-20210526211910445](src/images/image-20210526211910445.png)

输出构建决策树使用的时间，树节点的总数，正确率





## 二、代码说明

程序的执行过程如下：

1. 对命令行参数进行解析，设置所需的全局变量参数，line 252-281

2. 读取`dataset`目录下对应数据集的文件内容，并将各特征值从字符串映射到整数上，line 285

   `read_file(path)`：读取文件，统计首行中逗号的数量，得到分类特征的数量，之后对每行依次处理，若某特征或类别字符串未出现过，则从0开始为其分配整数值，最终得到的数据集保存在`dataset`中，每条数据由结构体`Data`存储。如 `tic-tac-toe` 会得到如下的映射关系

   ```c++
   int class_num = 2;
   unordered_map<string, int> class_trans 
   {
       { "positive", 0 },
       { "negative", 1 }
   };
   
   int num_attribute = 9;
   vector<int> attributes_num {3, 3, 3, 3, 3, 3, 3, 3, 3};
   vector<unordered_map<string, int> > attributes_trans 
   {
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} },
       { { "x", 0 }, { "o", 1 }, { "b" , 2} }
   };
   ```

3. 随机打乱数据集，根据 `train_set_size` 划分出训练集 `train` 和测试集 `test`，line 287-289

   `divide_dataset` 对于每个class分别进行划分，保证在每个训练集和测试集中每个class所占比例相同，为了之后测试预剪枝算法的效果，这里固定随机种子，每次运行的结果均相同。

4. 构建决策树，并对此阶段计时，line 298-303

   树节点 `TreeNode` 类，其中 `TreeNode::child` 表示其子节点们的指针，`TreeNode::nattribute`表示该节点的分类根据的特征序号，`TreeNode::nclass` 指示是否为叶节点，若不为叶节点，则为-1，否则表示该叶节点所属的分类类别；

   构造函数`TreeNode::TreeNode()`递归地构建决策树，过程：统计出当前数据子集中占多数的类别`majority`；执行各种预剪枝策略，最小样本数目、已全部为同一类别、最大树深度；遍历还未使用过的分类特征，每个计算出信息增益，得到达到最大信息增益对应的分类特征；执行最小信息增益预剪枝；根据该特征划分子数据集，递归构建子树。

   `TreeNode::classify()`函数递归地对数据进行预测分类。

5. 在测试集上测试决策树的预测准确率，line 307-313

   ​		
   
   

## 三、实验结果与分析

1. 数据集特征的分析

   <center>tic-tac-toe</center>

   |  class   |  N   |  N[%]  |
   | :------: | :--: | :----: |
   | positive | 626  | 34.60% |
   | negative | 332  | 65.30% |

   | Feature | 1      | 2      | 3      | 4      | 5      | 6      | 7      | 8      | 9      |
   | ------- | ------ | ------ | ------ | ------ | ------ | ------ | ------ | ------ | ------ |
   |         | x: 418 | x: 378 | x: 418 | x: 378 | x: 458 | x: 378 | x: 418 | x: 378 | x: 418 |
   |         | o: 335 | o: 330 | o: 335 | o: 330 | o: 340 | o: 330 | o: 335 | o: 330 | o: 335 |
   |         | b: 205 | b: 250 | b: 205 | b: 250 | b: 160 | b: 250 | b: 205 | b: 250 | b: 205 |

   

   <center>balance-scale</center>

   | class |  N   |  N[%]  |
   | :---: | :--: | :----: |
   |   L   | 288  | 46.00% |
   |   B   |  49  | 7.80%  |
   |   R   | 288  | 46.00% |

   | Feature | Left-Weight | Left-Distance | Right-Weight | Right-Distance |
   | ------- | ----------- | ------------- | ------------ | -------------- |
   |         | 1 : 125     | 1 : 125       | 1 : 125      | 1 : 125        |
   |         | 2 : 125     | 2 : 125       | 2 : 125      | 2 : 125        |
   |         | 3 : 125     | 3 : 125       | 3 : 125      | 3 : 125        |
   |         | 4 : 125     | 4 : 125       | 4 : 125      | 4 : 125        |
   |         | 5 : 125     | 5 : 125       | 5 : 125      | 5 : 125        |

   

   <center>nursery</center>

   |   class    |  N   | N[%]  |
   | :--------: | :--: | :---: |
   | not_recom  | 4320 | 33.3% |
   |  priority  | 4266 | 32.9% |
   | recommend  |  2   | 0.0%  |
   | spec_prior | 4044 | 31.2% |
   | very_recom | 328  | 2.5%  |

   | Feature | parents            | has_nurs           | form              | children    | housing           | finance            | social               | health             |
   | ------- | ------------------ | ------------------ | ----------------- | ----------- | ----------------- | ------------------ | -------------------- | ------------------ |
   |         | unsual : 4320      | proper : 2592      | complete : 3240   | 1 : 3240    | convenient : 4320 | convienient : 6480 | nonprob : 4320       | recommended : 4320 |
   |         | pretentious : 4320 | less_proper : 2592 | completed : 3240  | 2 : 3240    | less_conv : 4320  | inconv : 6480      | slightly_prob : 4320 | priority : 4320    |
   |         | great_pret : 4320  | improper : 2592    | imcomplete : 3240 | 3 : 3240    | critical : 4320   |                    | problematic : 4320   | not_recom : 4320   |
   |         |                    | critical : 2592    | foster : 3240     | more : 3240 |                   |                    |                      |                    |
   |         |                    | very_crit : 2592   |                   |             |                   |                    |                      |                    |

   

2. 基于IG与IGR的决策树构造

   此阶段命令行参数使用 `-d 100 -ig 0.0 -s 0` 避免预剪枝

   由于数据集 balance-scale 中类别在特征前，这里预处理了一下，将类别放在特征后

   对训练集大小从0-1，以0.1为步长依次实验，（程序运行时间均短于0.2s）结果如下：

   <center>tic-tac-toe</center>

   |      |            | 0.1   | 0.2   | 0.3   | 0.4   | 0.5   | 0.6   | 0.7   | 0.8   | 0.9   |
   | ---- | ---------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
   | IG   | correct(%) | 68.25 | 71.58 | 78.42 | 81.25 | 84.13 | 84.64 | 89.58 | 88.08 | 82.47 |
   |      | nodes      | 58    | 118   | 145   | 178   | 190   | 235   | 265   | 298   | 283   |
   | IGR  | correct(%) | 67.32 | 71.32 | 79.02 | 84.38 | 87.27 | 87.76 | 90.63 | 90.16 | 83.51 |
   |      | nodes      | 61    | 121   | 148   | 160   | 190   | 235   | 268   | 298   | 283   |

   <center>balance-scale</center>

   |      |            | 0.1   | 0.2   | 0.3   | 0.4   | 0.5   | 0.6   | 0.7   | 0.8   | 0.9   |
   | ---- | ---------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
   | IG   | correct(%) | 52.92 | 55.18 | 57.63 | 56.38 | 54.95 | 55.95 | 50.79 | 50.00 | 44.44 |
   |      | nodes      | 61    | 131   | 206   | 266   | 361   | 416   | 441   | 481   | 501   |
   | IGR  | correct(%) | 49.91 | 56.37 | 55.13 | 55.85 | 54.95 | 53.97 | 50.79 | 50.00 | 44.44 |
   |      | nodes      | 61    | 131   | 211   | 266   | 361   | 416   | 441   | 481   | 501   |

   <center>nursery</center>

   |      |            | 0.1   | 0.2   | 0.3   | 0.4   | 0.5   | 0.6   | 0.7   | 0.8   | 0.9   |
   | ---- | ---------- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- |
   | IG   | correct(%) | 90.44 | 92.99 | 94.29 | 95.35 | 95.66 | 96.30 | 97.07 | 97.49 | 98.23 |
   |      | nodes      | 359   | 549   | 677   | 809   | 889   | 963   | 1053  | 1110  | 1129  |
   | IGR  | correct(%) | 89.35 | 92.55 | 93.97 | 94.88 | 95.62 | 96.45 | 96.94 | 97.26 | 98.15 |
   |      | nodes      | 354   | 551   | 677   | 803   | 884   | 953   | 1047  | 1114  | 1126  |

   可以看到使用信息增益IG和信息增益比例IGR的实验结果相近，三个数据集在训练集占0.7的时候正确率较高，因此我们选择训练集大小为0.7作为之后的baseline。

   程序执行时间 [0.001 - 0.076] 秒，时间复杂度O(KMN)，其中K为特征数，M为特征的种类数，N为数据集大小，即使是整个nursery数据集 O(5 * 8 * 12960)，因此程序执行时间合理，并且很快不需要加速。

   

3. 预剪枝策略

   我们注意到，预剪枝的参数对准确率的影响很大，甚至在多数情况下，使用预剪枝的结果比不使用预剪枝还要差，因此我们写了个python脚本 `test.py` 来搜索最优的预剪枝参数。（所有的训练集占比均取0.7）

   其执行方式为 `python test.py [dataset]` 

   我们对最大深度depth，从2起至20，以步长为2搜索，共9种情况；

   ​		对最小信息增益ig，从0.005起至0.05，以步长为0.005搜索，共9种情况；

   ​		对最小样本数s，从0起至20，以步长为2搜索，共10种情况；

   ​		对是否使用信息增益比例，均搜索，共2种情况；

   共计执行`main` 9 * 9 * 10 * 2 = 1620 次，得到三个数据集的最优参数如下：

   `tic-tac-toe` : 最高准确率 92.01% > 90.63%

   ![image-20210616142904968](src/images/image-20210616142904968.png)

   `balance-scale` :  最高准确率 68.25% > 50.79%

   ![image-20210616143111190](src/images/image-20210616143111190.png)

   `nursery` :  最高准确率 97.30% > 97.07%

   ![image-20210616143628270](src/images/image-20210616143628270.png)

   在选择最优的参数下，预剪枝的策略能够提升分类的准确率。
