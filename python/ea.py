#-*- coding:utf-8 -*-

# json模块
import json
# 随机数模块
import random
# 时间模块
import time
# 高性能数组模块
import numpy as np
# 利用scipy库中pdist函数计算各种距离
# 参考: https://www.cnblogs.com/denny402/p/7027954.html  (博客)
#       https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.distance.pdist.html (官方文档)
# 提示: 测试发现后发现scipy的距离度量的代码在实现上有2种:
#       1、使用scipy.spatial.distance模块的distance.pdist()函数: distance.pdist(X, 'seuclidean', V=V)
#       2、使用scipy.spatial.distance模块的各类距离函数: seuclidean(x, y, V)
#       使用pdist函数注意,第3个参数一定要使用命名参数的写法,否则程序会出现比较诡异的错误!!!
#       比如: distance.pdist(X, 'seuclidean', V=V)  的第3个参数必须写成  V=xxx  ，而不是  xxx
from scipy.spatial import distance

# 遗传算法模块inspyred
from inspyred import ec
from inspyred import swarm
from inspyred.ec import terminators
from inspyred.ec import observers
from inspyred.ec import analysis

# 表格美化打印输出模块
from prettytable import PrettyTable

# 适应值曲线绘制模块
import ea_utils

# 自定义距离度量函数
def my_distance(x, y, dist_type=1):
    X = np.vstack([x, y])
    d = [0.0]
    if dist_type == 1:
        # 1. 欧氏距离(Euclidean Distance)
        d = distance.pdist(X)
    elif dist_type == 2:
        # 2. 曼哈顿距离(Manhattan Distance)
        d = distance.pdist(X, 'cityblock')
    elif dist_type == 3:
        # 3. 切比雪夫距离(Chebyshev Distance)
        d = distance.pdist(X, 'chebyshev')
    elif dist_type == 4:
        # 4. 闵可夫斯基距离(Minkowski Distance)
        d = distance.pdist(X, 'minkowski', p=2)
    elif dist_type == 5:
        # 5. 标准化欧氏距离 (Standardized Euclidean distance)
        #   a.计算方差V
        V = np.var(X, axis=0, ddof=1)
        #   hack: 用10的负7次幂替换0元素,避免出现除0的情况!!!
        V[V<1e-7] = 1e-7
        #   b.调用distance模块的pdist函数计算距离
        d = distance.pdist(X, 'seuclidean', V=V)  # 返回的是一个数组
        # d[0] = distance.seuclidean(x, y, V)     # 返回的是一个浮点数
    elif dist_type == 6:
        # 6. 马氏距离(Mahalanobis Distance)
        d = distance.pdist(X.T, 'mahalanobis')
    elif dist_type == 7:
        # 7. 夹角余弦(Cosine)
        d = 1 - distance.pdist(X, 'cosine')
    elif dist_type == 8:
        # 8. 皮尔逊相关系数(Pearson correlation)
        d[0] = np.corrcoef(X)[0][1]
    elif dist_type == 9:
        # 9. 汉明距离(Hamming distance)
        d = distance.pdist(X, 'hamming')
    elif dist_type == 10:
        # 10. 杰卡德相似系数(Jaccard similarity coefficient)
        d = distance.pdist(X, 'jaccard')
    elif dist_type == 11:
        # 11. 布雷柯蒂斯距离(Bray Curtis Distance)
        d = distance.pdist(X, 'braycurtis')
    else:
        # 1. 欧氏距离(Euclidean Distance)
        d = distance.pdist(X)
    # d是一个数组,包含1个元素
    return d[0]

# 实际问题类
class Problem:
    # 构造函数
    # 参数:
    #   lb, ub -- 下限和上限数组
    #   var_types  -- 变量类型数组(数组中的元素为0表示整数,1则表示实数)
    def __init__(self, lb, ub, var_types, X0, dist_type=1):
        # Problem类的成员变量: 下限
        self.lb = lb
        # Problem类的成员变量: 上限
        self.ub = ub
        # Problem类的成员变量: 变量类型(整数或实数)
        self.var_types = var_types
        # Problem类的成员变量: 编码长度
        self.var_num = len(self.var_types)
        # 目标值
        self.X0 = X0
        # 距离度量类型
        self.dist_type = dist_type
        # 解码函数1(从一个区间映射到另一个区间)
        # 参考: https://blog.csdn.net/touch_dream/article/details/62076236
        # self.f = lambda i, x: (self.ub[i]-self.lb[i])/(self.var_max-self.var_min)*(x-self.var_min)+self.lb[i]
        # 解码函数2
        self.f = lambda i, x: x
        # 如果变量类型值取0,则该变量为一个整数,使用int进行强制转换
        self.g = lambda i, x: int(self.f(i, x)) if self.var_types[i] == 0 else self.f(i, x)

    # 返回变量的上下限
    def bounder(self):
        return ec.Bounder(self.lb, self.ub)
        
    # 随机生成一个在上下限范围内的编码
    # random -- 随机数生成器对象
    def rand(self, rng):
        return [rng.uniform(self.lb[i], self.ub[i]) for i in range(self.var_num)]
    
    # 将个体编码(取值范围[0,1])解码为"真实变量"(取值范围[lb, ub])
    # 数学计算公式: f=(b-a)*x+a, 其中x范围[0,1], f范围[a, b]
    def decode(self, X):
        return [self.g(i, x) for i,x in enumerate(X)]

    # Todo: 在这里实现适应值函数!!!
    def fitness(self, X):
        # 染色体进行解码
        X1 = self.decode(X)
        # 计算解码后的变量X1与目标值X0之间的距离, 作为个体X的适应值
        #   X1              -- 当前个体对应的变量
        #   self.X0         -- 目标值
        #   self.dist_type  -- 距离度量方法(默认采用欧氏距离)
        return my_distance(X1, self.X0, self.dist_type)
    
# 自定义个体生成器
# 注: 下面es.evolve()函数中的所有参数构成了一个词典args
def my_generator(random, args):
    # 跳转到Problem::rand()成员函数
    return args['problem'].rand(random)

# 自定义个体适应值计算函数
def my_fitness(X, args):
    # 跳转到Problem::fitness()成员函数
    return args['problem'].fitness(X)

# 自定义种群适应值计算函数
# 注: 参数candidates是一个数组,它由多个个体组成
def my_evaluator(candidates, args):
    #依次计算每个个体的适应值
    fitness = []
    for X in candidates:
        # 将浮点数四舍五入,强制转换为0和1,并计算个体的适应值
        score = my_fitness(X, args)
        #添加到列表
        fitness.append(score)
    return fitness

def minimum_fitness_terminator(population, num_generations, num_evaluations, args):
    if num_generations > args['max_generations']:
      return True
    min_fitness = args.get('minimum_fitness', 0.01)
    pop_fitness = my_evaluator([x.candidate for x in population], args)
    print pop_fitness
    return max(pop_fitness) < min_fitness

# 自定义终止条件
# 根据指定的类型来选择终止条件函数
def my_terminator(terminator_type):
    TerminatorDict = {
        # 按进化代数终止: 表示进化多少代之后就终止,无论是否达到最优,也不考虑算法是否收敛
        1:terminators.generation_termination,
        # 按适应值计算次数终止: 表示适应值计算多少次后就就终止,无论是否达到最优,也不考虑算法是否收敛
        2:terminators.evaluation_termination,
        # 按种群的平均适应值终止: 表示种群的平均适应值连续多少代没有显著变化就终止,因为进化过程已经收敛或者假收敛了
        3:terminators.average_fitness_termination,
        # 自定义终止条件(按最大相对误差达到一定精度后终止)
        4:minimum_fitness_terminator
    }
    if not (terminator_type in TerminatorDict):
        terminator_type = 1
    return TerminatorDict[terminator_type]

# 遗传算法主框架
def runGA(args):
    #随机数初始化
    rng = random.Random()
    #如果设置的seed是一个固定值,那么优化算法的结果始终是固定不变的!!!
    #利用当前时间作为随机数种子,就可以看到不同的优化结果!!!
    rng.seed(int(time.time()))

    #选择优化算法
    #inspyred提供了多种智能优化算法
    #例如GA(遗传算法)、NSGA2(多目标优化算法)、DEA(微分进化算法)、SA(模拟退火算法)、PSO(粒子群算法)等等
    algo = args.get('algo', 1)
    es = None
    if algo == 1:
        # 遗传算法
        es = ec.GA(rng)
    elif algo == 2:
        # 粒子群算法
        es = swarm.PSO(rng)
        es.topology = swarm.topologies.ring_topology
    elif algo == 3:
        # 模拟退火算法
        es = ec.SA(rng)
    elif algo == 4:
        # 差分进化算法
        es = ec.DEA(rng)
    else:
        # 默认采用遗传算法
        es = ec.GA(rng)

    #设置终止条件
    es.terminator = my_terminator(args['terminator'])

    #设置数据观察者
    #file_observer是inspyred提供了一个数据接口,它会收集进化过程中的数据,并写入到文件
    #我们可以通过这些文件来生成进化曲线,观察进化过程和优化结果是否满足我们的需要
    es.observer = observers.file_observer
    #es.observer = observers.plot_observer

    # 新建种群数据记录文件
    stat_file = open(args['stat_file'], 'w')
    ind_file = open(args['ind_file'], 'w')   
    
    # 优化计算
    final_pop = es.evolve(
        # 自定义算子及参数
        generator = my_generator,                              # 自定义个体生成函数
        evaluator = my_evaluator,                              # 自定义个体适应值计算函数
        problem =  args['problem'],                            # 自定义问题对象参数(个体生成、适应值计算均由该对象负责)
        minimum_fitness = args.get('minimum_fitness', 0.01),   # 自定义参数 - 最小适应值(自定义的minimum_fitness_terminator()使用)
        
        # 通用参数
        pop_size = args.get('popsize', 10),                    # 种群规模
        maximize = False,                                      # 求最大优化问题还是最小优化问题
        bounder = args['problem'].bounder(),                # 变量取值范围(每个分量的范围都限制在0~1内)
        max_evaluations = args.get('max_evaluations', 1200),   # 最大适应值计算次数
        max_generations = args.get('max_generations', 20),     # 最大进化代数
        statistics_file = stat_file,                           # 设置进化统计数据要写入的文件(es.observer=observers.file_observer专用)
        individuals_file = ind_file,                           # 设置进化统计数据要写入的文件(es.observer=observers.file_observer专用)
        
        # 遗传算法GA参数
        mutation_rate = args.get('mutation_rate', 0.75),       # 变异率
        cross_rate = args.get('cross_rate', 0.015),            # 交叉率
        # num_elites = args.get('num_elites', 0),                # 保留精英个数
        # num_crossover_points = args.get('num_crossover_points', 1), # 交叉点个数(默认单点交叉)
        
        # 粒子群算法PSO参数
        neighborhood_size = args.get('neighborhood_size', 5)  # 邻域范围
        # cognitive_rate = args.get('cognitive_rate', 2.1)      # the rate at which the particle’s current position influences its movement (default 2.1)
        # social_rate = args.get('social_rate', 2.1)            # the rate at which the particle’s neighbors influence its movement (default 2.1)
        # inertia = args.get('inertia', 0.5)                    # the inertia constant to be used in the particle updating (default 0.5)
    
        # 模拟退火算法SA参数
        # cooling_rate = args.get('cooling_rate', 0.618)        # a real-valued coefficient in the range (0, 1) by which the temperature should be reduced
        # mutation_rate = args.get('mutation_rate', 0.1)        # the rate at which mutation is performed (default 0.1)
        # gaussian_mean = args.get('gaussian_mean', 0)          # the mean used in the Gaussian function (default 0)
        # gaussian_stdev = args.get('gaussian_stdev', 1)        # the standard deviation used in the Gaussian function (default 1)

        # 差分进化算法DEA参数
        # num_selected = args.get('num_selected', 2)            # the number of individuals to be selected (default 2)
        # tournament_size = args.get('tournament_size', 2)      # the tournament size (default 2)
        # crossover_rate = args.get('crossover_rate', 1.0)      # the rate at which crossover is performed (default 1.0)
        # mutation_rate = args.get('mutation_rate', 0.1)        # the rate at which mutation is performed (default 0.1)
        # gaussian_mean = args.get('gaussian_mean', 0)          # the mean used in the Gaussian function (default 0)
        # gaussian_stdev = args.get('gaussian_stdev', 1)        # the standard deviation used in the Gaussian function (default 1)
    )
    # 关闭种群数据记录文件
    stat_file.close()
    ind_file.close()

    # 种群排序
    # 如果maximize=True,则final_pop.sort()按照从小到大的顺序排列,适应值最小的在第一个位置,最差的个体排在首位
    # 如果maximize=False,则final_pop.sort()按照从大到小的顺序排列,适应值最大的在第一个位置,还是最差的个体排在首位
    # 因此,使用final_pop.sort()的时候,一定要加reverse=True这个参数!!!(适应值始终与目标函数一致,最大或最小)
    final_pop.sort(reverse = True)
    # 打印最优解(排序后的种群中第1个个体)
    return final_pop[0].candidate

# 计算误差
def cacl_error(X1, X0):
    # 记录绝对误差和相对误差
    AE, RE = [], []
    # 数据个数
    n = len(X0)
    for i in range(n):
        err1 = X1[i]-X0[i]
        err2 = (X1[i]-X0[i])/X0[i]*100.0
        # 记录绝对和相对误差
        AE.append(abs(err1))
        RE.append(abs(err2))
    # 返回绝对误差和相对误差
    return AE, RE

import sys
def table_print(X1, X0, AE, RE):
    # 定义表格
    tbl = PrettyTable([u"编号", u"最优解", u"目标值", u"绝对误差", u"相对误差%"], encoding=sys.stdout.encoding)
    tbl.align[u"最优解"] = "1"  # 左对齐
    tbl.align[u"目标值"] = "1"  # 左对齐
    tbl.align[u"绝对误差"] = "1"  # 左对齐
    tbl.align[u"相对误差"] = "1"  # 左对齐
    tbl.padding_width = 1  # 填充宽度

    # 数据个数
    n = len(X0)
    for i in range(n):
        # 添加行
        x1 = "%.2f" % (X1[i])
        x0 = "%.2f" % (X0[i])
        absolute_error = "%.2f" % (AE[i])
        relative_error = "%.2f%%" % (RE[i])
        tbl.add_row([i+1, x1, x0, absolute_error, relative_error])

    # 打印误差信息
    print u'最大绝对误差: %.2f, 最大相对误差: %.2f%%' % (np.max(AE), np.max(RE))
    print u'平均绝对误差: %.2f, 平均相对误差: %.2f%%' % (np.mean(AE), np.mean(RE))
    # 打印表格
    print tbl

# 主函数
def main():
    # 开始时间
    start = time.time()

    # 参数设置
    config = {
        #---------------- 自定义参数: 变量及范围  -----------------#
        # 变量类型(0-整数,1-实数) 
        "var_types":[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],       
        # 下限(数组中的每一位代表一个变量的下限)
        "lb":[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.045, 19.095, 285.095, 3800.095, 47500.095, 5.795, 66.595, 760.095, 8550.095, 9500.095, 1055.64, 9.785, 9.88, 9.975, 10.07, 10.165, 10.26, 10.355, 19],
        # 上限(数组中的每一位代表一个变量的上限)
        "ub":[10, 10, 10, 10, 10, 10, 10, 10, 10, 15, 1.155, 21.105, 315.105, 4200.105, 52500.105, 6.405, 73.605, 840.105, 9450.105, 10500.105, 1166.76, 10.815, 10.92, 11.025, 11.13, 11.235, 11.34, 11.445, 21],
        # 目标值
        "X0":[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1.1, 20.1, 300.1, 4000.1, 50000.1, 6.1, 70.1, 800.1, 9000.1, 10000.1, 1111.2, 10.3, 10.4, 10.5, 10.6, 10.7, 10.8, 10.9, 20.0],
        
        # "lb":[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1.045, 19.095, 28.5095, 38.00095, 47.500095, 5.795, 66.595, 76.0095, 85.50095, 95.00095, 10.5564, 9.785, 9.88, 9.975, 10.07, 10.165, 10.26, 10.355, 19],
        # "ub":[10, 10, 10, 10, 10, 10, 10, 10, 10, 15, 1.155, 21.105, 31.5105, 42.00105, 52.500105, 6.405, 73.605, 84.0105, 94.50105, 105.00105, 11.6676, 10.815, 10.92, 11.025, 11.13, 11.235, 11.34, 11.445, 21],
        # "X0":[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 1.1, 20.1, 30.01, 40.001, 50.0001, 6.1, 70.1, 80.01, 90.001, 100.001, 11.112, 10.3, 10.4, 10.5, 10.6, 10.7, 10.8, 10.9, 20],

        # "lb":[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9.785, 9.88, 9.975, 10.07, 10.165, 10.26, 10.355, 19, 10.5564, 9.785, 10.5564, 9.785, 9.88, 9.975, 10.07, 10.165, 10.26, 10.355, 19],
        # "ub":[10, 10, 10, 10, 10, 10, 10, 10, 10, 15, 10.815, 10.92, 11.025, 11.13, 11.235, 11.34, 11.445, 21, 11.6676, 10.815, 11.6676, 10.815, 10.92, 11.025, 11.13, 11.235, 11.34, 11.445, 21],
        # "X0":[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10.3, 10.4, 10.5, 10.6, 10.7, 10.8, 10.9, 20, 11.112, 10.3, 11.112, 10.3, 10.4, 10.5, 10.6, 10.7, 10.8, 10.9, 20],

        #---------------- 自定义参数: 距离度量方法及终止条件  -----------------#
        "algo": 2,                      # 算法选择
                                        #   1. 遗传算法GA
                                        #   2. 粒子群算法PSO
                                        #   3. 模拟退火算法SA
                                        #   4. 差分进化算法DEA
        
        "dist_type": 5,                 # 距离度量方法(1-表示采用欧氏距离))
                                        #   1. 欧氏距离(Euclidean Distance)
                                        #   2. 曼哈顿距离(Manhattan Distance)
                                        #   3. 切比雪夫距离(Chebyshev Distance )
                                        #   4. 闵可夫斯基距离(Minkowski Distance)
                                        #   5. 标准化欧氏距离(Standardized Euclidean distance)
                                        #   6. 马氏距离(Mahalanobis Distance)
                                        #   7. 夹角余弦(Cosine)
                                        #   8. 皮尔逊相关系数(Pearson correlation)
                                        #   9. 汉明距离(Hamming distance)
                                        #   10. 杰卡德相似系数(Jaccard similarity coefficient)
                                        #   11. 布雷柯蒂斯距离(Bray Curtis Distance)        
        
        "terminator": 1,                # 终止条件(1-表示按最大进化代数终止迭代)
                                        #   1. 按最大进化代数终止迭代
                                        #   2. 按最大适应值计算次数终止迭代
                                        #   3. 按种群平均适应值收敛终止迭代
                                        #   4. 自定义终止条件

        #---------------- 进化算法通用参数(遗传、粒子群、蚁群、模拟退火等)  -----------------#
        "popsize": 50,                  # 种群规模
        "max_generations": 1000,        # 最大进化代数
        "max_evaluations": 1000,        # 最大适应值计算次数(暂未使用)
        "stat_file": "stat.csv",        # 每一代的进化参数记录
        "ind_file": "ind.csv",          # 每一代的进化参数记录

        #---------------- 遗传算法GA参数 --------------------#
        "cross_rate": 0.75,             # 交叉率(默认1.0)
        "mutation_rate": 0.015,         # 变异率(默认0.1)
        # "num_selected": 50              # 被选择的个体数(默认等于种群规模popsize)
        # "num_crossover_points": 1       # 交叉点个数(默认单点交叉,等于1)
        # "num_elites": 0                 # 启用精英策略,保留n个最优个体到下一代(默认0)

        #---------------- 粒子群算法PSO参数 -----------------#
        "neighborhood_size": 5          # 邻域范围
        # "cognitive_rate": 2.1           # the rate at which the particle’s current position influences its movement (default 2.1)
        # "social_rate": 2.1              # the rate at which the particle’s neighbors influence its movement (default 2.1)
        # "inertia": 0.5                  # the inertia constant to be used in the particle updating (default 0.5)
        
        #---------------- 模拟退火算法SA参数 ------------------#
        # "temperature": 100              # the initial temperature
        # "cooling_rate": 0.618           # a real-valued coefficient in the range (0, 1) by which the temperature should be reduced
        # "mutation_rate": 0.1            # the rate at which mutation is performed (default 0.1)
        # "gaussian_mean": 0              # the mean used in the Gaussian function (default 0)
        # "gaussian_stdev": 1             # the standard deviation used in the Gaussian function (default 1)

        #---------------- 差分进化算法DEA参数 ------------------#
        # "num_selected": 2               # the number of individuals to be selected (default 2)
        # "tournament_size": 2            # the tournament size (default 2)
        # "crossover_rate": 1.0           # the rate at which crossover is performed (default 1.0)
        # "mutation_rate": 0.1            # the rate at which mutation is performed (default 0.1)
        # "gaussian_mean": 0              # the mean used in the Gaussian function (default 0)
        # "gaussian_stdev": 1             # the standard deviation used in the Gaussian function (default 1)
    }

    # 定义问题对象并添加到config词典中
    # 个体生成、适应值计算均由该对象负责,详见Problem类的定义
    config['problem'] = Problem(config['lb'], config['ub'], config['var_types'], config['X0'], config['dist_type'])

    # 遗传算法计算得到"最优"个体
    best_X = runGA(config)

    # 结束时间
    stop = time.time()
    print u'计算耗时:', "%.1fs" % (stop-start)

    # 打印计算得到的最优解及适应值
    print u'最优适应值:', config['problem'].fitness(best_X)
    # print u'最优个体:', config['problem'].decode(best_X)

    # 表格打印目标值、最优解，以及两者的绝对误差、相对误差
    X0 = config['X0']                         # 目标值
    X1 = config['problem'].decode(best_X)     # 最优解
    # 计算绝对误差和相对误差
    AE, RE = cacl_error(X1, X0)
    # 表格打印
    table_print(X1, X0, AE, RE)

    # 绘制图形
    ea_utils.plot(config['stat_file'], 'ea.png', False)

if __name__ == "__main__":
    # 主函数入口
    main()