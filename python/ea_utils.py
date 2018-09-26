#-*- coding:utf-8 -*-

import matplotlib.pyplot as plt
import matplotlib as mpl

# 设置全局图形效果
# mpl.rcParams['font.family']='sans-serif'
# mpl.rcParams['font.sans-serif']=['Microsoft YaHei']
# 必要设置: 保证图形中的中文正常显示!!!
mpl.rcParams['axes.unicode_minus']=False
# 不显示图例
mpl.rcParams['legend.frameon']=False
# 定义中文字体
ch_font=mpl.font_manager.FontProperties(fname='simhei.ttf')

# seaborn模块(在matplotlib模块的基础上封装的更高级一些)
# 要注意的是一旦导入了seaborn，matplotlib的默认作图风格就会被覆盖成seaborn的格式
# import seaborn as sns
# sns.set_style("whitegrid")

def _CaclPlotRowColNum(n):
    if n == 1:
        #1*1
        return 111
    elif n == 2:
        #2*1
        return 211
    elif n == 3:
        #3*1
        return 311
    elif n == 4:
        #2*2
        return 221
    elif n == 5:
        #3*2
        return 321
    elif n == 6:
        #3*2
        return 321
    elif n == 7:
        #4*2
        return 241
    elif n == 8:
        #4*2
        return 421
    elif n == 9:
        #3*3
        return 331
    else:
        #3*3
        return 331

def _PlotFitness(names, datas, bPlotInOne):
    if len(names) < 2 or len(datas) < 2 or len(names) != len(datas):
        return

    xname = names[0]
    xdata = datas[0]

    plotNum = _CaclPlotRowColNum(len(names)-1)

    for yname, ydata in zip(names, datas):
        if yname == xname:
            continue

        if not bPlotInOne:
            plt.subplot(plotNum)
            # plt.figure(plotNum)
            plotNum = plotNum + 1

        plt.plot(xdata, ydata, label=yname)
        plt.xlabel(xname, fontproperties=ch_font)

        if not bPlotInOne:
            # 设置y轴标签
            plt.ylabel(yname, fontproperties = ch_font)
            # plt.title(yname, fontproperties=ch_font)
            # 设置图例
            plt.legend(loc = "upper right", prop = ch_font)        
            # 显示网格
            plt.grid(True)

    if bPlotInOne:
        # 设置y轴标签
        plt.ylabel(u'适应值', fontproperties = ch_font)
        # 设置图例
        plt.legend(loc = "upper right", prop = ch_font)
        #plt.axes(40,160,0,0.3)

def _GetBestXgDatas(stat_file_name):
    gen = []
    best = []
    worst = []
    average = []
    stdev = []
    f = open(stat_file_name, 'r')
    for line in f:
        data = line.split(',')
        gen.append(int(data[0]))

        a,b,c,d = float(data[2]), float(data[3]), float(data[5]), float(data[6])
        #a=a+1
        #b=b+1
        #c=c+1
        #d=d+1
        #a = math.log10(a)
        #b = math.log10(b)
        #c = math.log10(c)
        #d = math.log10(d)
        worst.append(a)
        best.append(b)
        average.append(c)
        stdev.append(d)
    f.close()
    return gen, best,worst, average, stdev

def plot(statfile, png_file,  bPlotInOne=False):
    #绘制4个曲线图
    #X轴--进化代数(Generation)
    #Y轴分别是:种群的最优适应值(Best Fitness), 种群的最差适应值(Worst Fitness), 种群的平均适应值(Average Fitness), 种群适应值的方差(Standard Deviation)
    # names = ['Generation', 'Best Fitness', 'Worst Fitness', 'Average Fitness', 'Standard Deviation']
    names = [u'进化代数', u'种群最优适应值', u'种群最坏适应值', u'种群平均适应值', u'种群适应值方差']
    #从stat.csv中提取数据
    gen, best, worst, average,stdev = _GetBestXgDatas(statfile)
    datas = [gen, best, worst, average, stdev]
    #绘制4个曲线图
    #PlotFitness()函数的第3个参数表示是否将4个图画在一个图表里(1个figure有4个子图)
    #如果单独显示,可以将True改成False
    _PlotFitness(names, datas, bPlotInOne)

    #plt.legend(loc='upper left' ) 
    # plt.savefig(png_file, dpi=100)
    plt.show()
