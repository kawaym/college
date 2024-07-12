# -*- coding: utf-8 -*-
"""
Created on Fri Jul 12 11:01:27 2024

@author: kaway
"""

import matplotlib.pyplot as plt
import sys
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns
import matplotlib.cm as cm


from sklearn import preprocessing, linear_model
from sklearn.feature_selection import SelectKBest, f_regression
from sklearn.metrics import confusion_matrix, r2_score, classification_report
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.linear_model import SGDRegressor

# Utilitários

def printResults(results, targetY, name, inside=True):
    text = "Fora"
    if (inside): text = "Dentro"
    print(f"\nRegressor {name} ({text} da Amostra)\n")   
    
    print('R^2 Score: ', createRegressionReport(target = targetY, predicted = results))
    
def createResults(classifier, training_x, training_y, targetX):
    classifier.fit(training_x, training_y)
    return classifier, classifier.predict(targetX)

def createClassificationReport(target, predicted, output_dict=False):
    report = classification_report(y_true=target, y_pred=predicted, target_names=['Negative', 'Positive'], output_dict=output_dict)
    return report

def createRegressionReport(target, predicted):
    report = r2_score(y_true=target, y_pred=predicted)
    return report

def cramers_corrected_stat(confusion_matrix):
    """ calculate Cramers V statistic for categorial-categorial association.
        uses correction from Bergsma and Wicher, 
        Journal of the Korean Statistical Society 42 (2013): 323-328
    """
    chi2 = stats.chi2_contingency(confusion_matrix)[0]
    n = confusion_matrix.sum().sum()
    phi2 = chi2/n
    r,k = confusion_matrix.shape
    phi2corr = max(0, phi2 - ((k-1)*(r-1))/(n-1))    
    rcorr = r - ((r-1)**2)/(n-1)
    kcorr = k - ((k-1)**2)/(n-1)
    return np.sqrt(phi2corr / min( (kcorr-1), (rcorr-1)))

# Inicio

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_2"

trainingFile = "/data/conjunto_de_treinamento.csv"
testFile = "/data/conjunto_de_teste.csv"

#  ========== CRIA DATAFRAME PRINCIPAL, LIMPA COLUNAS MAL PREENCHIDAS E FAZ PRÉ-PROCESSAMENTO DOS DADOS ===========

typeDictionary = {}

training_data = pd.read_csv(path + trainingFile, index_col='Id')
unsupervised_test_data = pd.read_csv(path + testFile, index_col='Id')

target = 'preco'
print('Dados estatísticos do conjunto de dados')
print(training_data.describe(), '\n')

# Sanitiza os dados que podem conter nulos e converte valores para numéricos

encoder = preprocessing.OrdinalEncoder(encoded_missing_value=-1)
scaler = preprocessing.StandardScaler()
for column in training_data:
    if (column == 'preco'): continue
    if(training_data[column].dtypes != 'float64' and training_data[column].dtypes != 'int64'):
        training_data[[column]] = encoder.fit_transform(training_data[[column]])
        typeDictionary[column] = 'categorical'
        continue
    training_data[[column]] = scaler.fit_transform(training_data[[column]])
    training_data[[column]] = training_data[[column]].fillna(0)
    typeDictionary[column] = 'continuous'

for column in unsupervised_test_data:
    if(unsupervised_test_data[column].dtypes != 'float64' and unsupervised_test_data[column].dtypes != 'int64'):
        unsupervised_test_data[[column]] = encoder.fit_transform(unsupervised_test_data[[column]])
        typeDictionary[column] = 'categorical'
        continue
    unsupervised_test_data[[column]] = scaler.fit_transform(unsupervised_test_data[[column]])
    unsupervised_test_data[[column]] = unsupervised_test_data[[column]].fillna(0)
    typeDictionary[column] = 'continuous'

correlations = {}

for column in training_data:
    if column == target: continue
    if typeDictionary[column] == 'categorical':
        confusion = pd.crosstab(training_data[column], training_data[target])
        correlations[column] = cramers_corrected_stat(confusion)
    if typeDictionary[column] == 'continuous':
        correlations[column] = stats.pointbiserialr(training_data[column], training_data[target])[0]
        
npCorrelations = np.array(list(correlations.items()))
dfCorrelations = pd.DataFrame(data=[x for x in npCorrelations], columns=['id', 'correlation'])
dfCorrelations = dfCorrelations.sort_values(by=['correlation'])

# Seleciona as features mais performáticas
last_y = training_data.iloc[:,-1]

selector = SelectKBest(f_regression, k=4)
selector.fit(training_data.iloc[:, :-1], training_data.iloc[:,-1])
cols_idxs = selector.get_support(indices=True)
training_data = training_data.iloc[:,cols_idxs]
training_data.insert(len(training_data.columns), 'preco', last_y.values)

# Embaralha os dados para fazer as predições

training, supervised_test = train_test_split(training_data, test_size=0.3, random_state=124324)
training_x = training.iloc[:, :-1]
training_y = training.iloc[:, -1]
supervised_test_x = supervised_test.iloc[:, :-1]
supervised_test_y = supervised_test.iloc[:, -1]

unsupervised_test_x = unsupervised_test_data.iloc[:, :]

# Linear Regressor

regressor = LinearRegression()
LRegressor, results = createResults(regressor, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Linear")

# Stochastic Gradient Descent Regressor

regressor = SGDRegressor(loss='squared_error', alpha=0.1, penalty='l2')
GDRegressor, results = createResults(regressor, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Gradiente Estocástico")

# Saída

answer_y = GDRegressor.predict(unsupervised_test_x.iloc[:,cols_idxs])
answer = pd.DataFrame(data={'Id': [i for i in range(0, 2000)], 'preco': answer_y})
answer.to_csv('results.csv', index= False)

print(answer)