from numpy import *
import numpy as np
import pandas as pd
from math import log
import operator

def entropy(dataSet):
    counts=len(dataSet)
    labelCounts={}
    for featVec in dataSet:
        currentLabel=featVec[-1]
        if currentLabel not in labelCounts.keys():
            labelCounts[currentLabel]=0
        labelCounts[currentLabel]+=1
    entropy=0.0
    for key in labelCounts:
        prob=float(labelCounts[key])/counts
        entropy-=prob*log(prob,2)
    return entropy

# def vi(dataSet):
#     k=len(dataSet)
#     ki={}
#     for featVec in dataSet:
#         currentLabel=featVec[-1]
#         if currentLabel not in ki.keys():
#             ki[currentLabel]=0
#         ki[currentLabel]+=1
#     vi=1.0
#     for key in ki:
#         prob=float(ki[key])/k
#         vi*=prob
#     return vi

def divideDataSet(dataSet,axis,value):
    retDataSet=[]
    for featVec in dataSet:
        if featVec[axis]==value:
            reducedFeatVec=featVec[:axis]
            reducedFeatVec.extend(featVec[axis+1:])
            retDataSet.append(reducedFeatVec)
    return retDataSet

def inforGain(dataSet,labels):
    Features=len(dataSet[0])
    baseEntropy=entropy(dataSet)
    bestFeature=-1; bestInforGain=0.0
    for i in range(Features):
        #print(i)
        featureList=[example[i] for example in dataSet]
        uniqueValues=set(featureList)
        newEntropy=0.0
        for value in uniqueValues:
            subDataSet=divideDataSet(dataSet,i,value)
            prob=len(subDataSet)/float(len(dataSet))
            newEntropy+=prob*entropy(subDataSet)
        inforGain=baseEntropy-newEntropy
        if inforGain>bestInforGain:
            bestInfoGain=inforGain
            bestFeature=i
    return bestFeature

# def varianceImpurity(dataSet,labels):
#     Features=len(dataSet[0])-1
#     baseVi=vi(dataSet)
#     bestFeature=-1
#     bestVarianceImpurity=0.0
#     bestDivideDict={}
#     for i in range(Features):
#         featureList=[example[i] for example in dataSet]
#         uniqueValues=set(featList)
#         newVi=1.0
#         for value in uniqueValues:
#             subDataSet=divideDataSet(dataSet,i,value)
#             prob=len(subDataSet)/float(dataSet)
#             newVi+=prob*vi(subDataSet)
#         varianceImpurity=baseVi-newVi
#         if varianceImpurit<bestVarianceImpurity:
#             bestVarianceImpurity=varianceImpurity
#             bestFeature=i
#     return bestFeature

# def majorityCount(classList):
#        classCount={}
#        for vote in classList:
#            if vote not in classCount.keys():
#                calssCount[vote]=0
#            classCount[vote]+=1
#        return max(classCount)

def growTree(dataSet,labels,training_set,labels_full):
    classList=[example[-1] for example in dataSet]
    if classList.count(classList[0])==len(classList):
        return classList[0]
    if len(dataSet[0])==1:
        return majorityCnt(classList)
    bestFeature=inforGain(dataSet,labels)
    bestFeatureLabel=labels[bestFeature]
    decisionTree={bestFeatureLabel:{}}
    featureValues=[example[bestFeature] for example in dataSet]
    uniqueValues=set(featureValues)
    if type(dataSet[0][bestFeature]).__name__=='str':
        currentLabel=labels_full.index(labels[bestFeature])
        featureValuesFull=[exmaple[currentLabel] for exmaple in training_set]
        uniqueValuesFull=set(featureValuesFull)
    del(labels[bestFeature])
    for value in uniqueValues:
        subLabels=labels[:]
        if type(dataSet[0][bestFeature]).__name__=='str':
            uniqueValuesFull.remove(value)
        decisionTree[bestFeatureLabel][value]=growTree(divideDataSet\
         (dataSet,bestFeature,value),subLabels,training_set,labels_full)
    if type(dataSet[0][bestFeature]).__name__=='str':
        for value in uniqueValuesFull:
            decisionTree[bestFeatureLabel][value]=majorityCount(classList)
    return decisionTree

def classify(inputTree,featureLabels,testVec):
    #firstStr=inputTree.keys()[0]
    firstStr=list(inputTree.keys())[0]
    if '<=' in firstStr:
        featureValues=float(re.compile("(<=.+)").search(firstStr).group()[2:])
        featureKey=re.compile("(.+<=)").search(firstStr).group()[:-2]
        secondDict=inputTree[firstStr]
        featureIndex=featureLabels.index(featureKey)
        if testVec[featureIndex]<=featureValues:
            judge=1
        else:
            judge=0
        for key in secondDict.keys():
            if judge==int(key):
                if type(secondDict[key]).__name__=='dict':
                    classLabel=classify(secondDict[key],featureLabels,testVec)
                else:
                    classLabel=secondDict[key]
    else:
        secondDict=inputTree[firstStr]
        featureIndex=featureLabels.index(firstStr)
        for key in secondDict.keys():
            if testVec[featureIndex]==key:
                if type(secondDict[key]).__name__=='dict':
                    classLabel=classify(secondDict[key],featureLabels,testVec)
                else:
                    classLabel=secondDict[key]
    return classLabel

def testing(decisionTree,test_set,labels):
    error=0.0
    for i in range(len(test_set)):
        if classify(decisionTree,labels,test_set[i])!=test_set[i][-1]:
            error+=1
    return float(error)/len(test_set)


# def testingMajor(major,test_set):
#     error=0.0
#     for i in range(len(test_set)):
#         if major!=test_set[i][-1]:
#             error+=1
#     return float(error)

# def getNumNonLeafs(decisionTree):
#     numNonLeafs=0
#     #firstStr=decisionTree.keys()[0]
#     firstStr=list(decisionTree.keys())[0]
#     secondDict=decisionTree[firstStr]
#     for key in secondDict.keys():
#         if type(secondDict[key]).__name__=='dict':
#             numNonLeafs+=getNumNonLeafs(secondDict[key])
#             numNonLeafs+=1
#     return numNonLeafs
#
# def postPruning(decisionTree,dataSet,vadiation_set,labels):
#     bestTree=decisionTree
#     for i in range(1:L):
#         newTree=decisionTree
#         M=random.randint(1,K)
#         for j in range(1,M):
#             N=getNumNonLeafs(newTree)
#
#             P=random.randint(1,N)
#
#
#         if testing(newTree,validation_set,labels)<testing(bestTree,validation_set,labels):
#             bestTree=newTree
#     return bestTree


# def printTree(tree, d = 0):
#     if (tree == None or len(tree) == 0):
#         print("\t" * d, "-")
#     else:
#         for key, val in tree.items():
#             if (isinstance(val, dict)):
#                 print("\t" * d, key)
#                 printTree(tree[key][val], d+1)
#             else:
#                 print("\t" * d, key, "(" + val + ")")

# def postPruningTree(inputTree,dataSet,validation_set,labels):
#     #firstStr=decisionTree.keys()[0]
#     firstStr=list(inputTree.keys())[0]
#     secondDict=inputTree[firstStr]
#     classList=[example[-1] for example in dataSet]
#     featureKey=copy.deepcopy(firstStr)
#     if '<=' in firstStr:
#         featureKey=re.compile("(.+<=)").search(firstStr).group()[:-2]
#         featureValues=float(re.compile("(<=.+)").search(firstStr).group()[2:])
#     labelIndex=labels.index(featureKey)
#     temp_labels=copy.deepcopy(labels)
#     del(labels[labelIndex])
#     for key in secondDict.keys():
#         if type(secondDict[key]).__name__=='dict':
#             if type(dataSet[0][labelIndex]).__name__=='str':
#                 inputTree[firstStr][key]=postPruningTree(secondDict[key],\
#                  divideDataSet(dataSet,labelIndex,key),divideDataSet(data_test,labelIndex,key),copy.deepcopy(labels))
#     if testing(inputTree,validation_set,temp_labels)<=testingMajor(majorityCnt(classList),validation_set):
#         return inputTree
#     return majorityCnt(classList)

# def print_tree(node, depth=0):
# 	if isinstance(node, dict):
# 		print('%s[X%d < %.3f]' % ((depth*' ', (node['index']+1), node['value'])))
# 		print_tree(node['left'], depth+1)
# 		print_tree(node['right'], depth+1)
# 	else:
# 		print('%s[%s]' % ((depth*' ', node)))

df=pd.read_csv('../data_sets2/training_set.csv')
data=df.values[:,:-1].tolist()
training_set=data[:]
labels=df.columns.values[:-1].tolist()
labels_full=labels[:]
decisionTree=growTree(data,labels,training_set,labels_full)
#printTree (decisionTree,0)
#firstStr=list(decisionTree.keys())[0]
#print(firstStr)

# dv=pd.read_csv('/Users/yushimeng/Documents/Courses/6475MLQE/hw/hw1/data_sets1/validation_set.csv')
# validation=dv.values[:,1:].tolist()
# validation_set=validation[:]
# postPruningTree=postPruningTree(decisionTree,validation,validation_set,labels)
#
dt=pd.read_csv('../data_sets2/test_set.csv')
testdata=dt.values[:,:-1].tolist()
test_set=testdata[:]
test_label=dt.columns.values[0:-1].tolist()
print(testing(decisionTree,test_set,test_label))
#print(len(data))

# import plotTree
# plotTree.createPlot(decisionTree)
