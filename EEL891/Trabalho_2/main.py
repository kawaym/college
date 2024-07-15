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
from sklearn.preprocessing import OrdinalEncoder
from sklearn.feature_selection import SelectKBest, f_regression
from sklearn.metrics import confusion_matrix, r2_score, classification_report
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.linear_model import SGDRegressor
from sklearn.linear_model import LogisticRegression
from sklearn.svm import LinearSVR
from sklearn.svm import SVR

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

def cramers_corrected_stat(confusion):
    """ calculate Cramers V statistic for categorial-categorial association.
        uses correction from Bergsma and Wicher, 
        Journal of the Korean Statistical Society 42 (2013): 323-328
    """
    chi2 = stats.chi2_contingency(confusion_matrix)[0]
    n = confusion.sum().sum()
    phi2 = chi2/n
    r,k = confusion.shape
    phi2corr = max(0, phi2 - ((k-1)*(r-1))/(n-1))    
    rcorr = r - ((r-1)**2)/(n-1)
    kcorr = k - ((k-1)**2)/(n-1)
    return np.sqrt(phi2corr / min( (kcorr-1), (rcorr-1)))

# Inicio

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_2"

trainingFile = "/data/conjunto_de_treinamento.csv"
testFile = "/data/conjunto_de_teste.csv"

data = pd.read_csv(path + trainingFile)
target_col = 'preco'
target = data.preco

median_by_bairro = data.groupby('bairro')['preco'].median().to_dict()
sorted_bairros = sorted(median_by_bairro, key=median_by_bairro.get)
dict_posicoes = {bairro: posicao for posicao, bairro in enumerate(sorted_bairros, start=1)}
order_bairro = [bairro for bairro, posicao in sorted(dict_posicoes.items(), key=lambda item: item[1])]

# A coluna diferenciais é redundante, pois seus dados já estão codificados nas diversas colunas

data = data.drop([target_col, 'diferenciais'], axis=1)

test_data = pd.read_csv(path + testFile)
data_description = data.describe()

print("Contagem de valores nulos do dataset de treinamento")
print(data.isna().sum(), '\n\n\n')

# Box plot para checagem de outliers

sns.boxplot(data, y=target, log_scale=True)
plt.show()

# Não existem valores nulos, portanto todas as features podem continuar no plot

for column in data:
    print(f'Coluna {column} do dataset, caracteristicas: ')
    print(f'Tipo: {data[column].dtypes}')
    if (data[column].dtypes == 'object'):
        print(f'Valores únicos: {data[column].unique()}')
        print(f'Contagem por valor: {data[column].value_counts()}')
    else:
        print(f'Média: {data[column].mean()}')
    print('\n\n')


# Ainda temos três features categóricas, podemos explorar mais sobre elas utilizando gráficos de barra

sns.histplot(data=data, x='tipo')
plt.show()

sns.barplot(data=data, x='tipo', y=target)
plt.show()

sns.histplot(data=data, x='tipo_vendedor')
plt.show()

sns.barplot(data=data, x='tipo_vendedor', y=target)
plt.show()

f, ax = plt.subplots(figsize = (20, 30))
sns.barplot(data=data, x=target, y='bairro')
plt.show()

# One Hot Encoding

onehot = pd.get_dummies(data, columns = ['tipo', 'tipo_vendedor'], dtype=int)
data = pd.concat([data, onehot], axis='columns')
data = data.loc[:,~data.columns.duplicated()].copy()
data = data.drop(['tipo', 'tipo_vendedor', 'tipo_vendedor_Imobiliaria', 'tipo_Loft'], axis='columns')

onehot_test = pd.get_dummies(test_data, columns = ['tipo', 'tipo_vendedor'], dtype=int)
test_data = pd.concat([test_data, onehot_test], axis='columns')
test_data = test_data.loc[:,~test_data.columns.duplicated()].copy()
test_data = test_data.drop(['tipo', 'tipo_vendedor', 'tipo_vendedor_Imobiliaria', 'tipo_Loft'], axis='columns')

# Ordinal Encoding
encoder = OrdinalEncoder()
data[['bairro']] = encoder.fit_transform(data[['bairro']])
test_data[['bairro']] = encoder.fit_transform(test_data[['bairro']])

print(data)

f, ax = plt.subplots(figsize = (30,30))
sns.heatmap(data.corr(), annot=True, linewidths = .5, fmt = '.1f', ax=ax)
plt.show()