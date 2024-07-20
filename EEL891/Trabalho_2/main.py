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

from sklearn.preprocessing import OrdinalEncoder, MinMaxScaler
from sklearn.model_selection import cross_val_predict, GridSearchCV
from sklearn.metrics import confusion_matrix, r2_score, classification_report, mean_squared_error
from sklearn.linear_model import LinearRegression
from sklearn.linear_model import SGDRegressor
from sklearn.ensemble import RandomForestRegressor
from sklearn.ensemble import ExtraTreesRegressor
from sklearn.neighbors import KNeighborsRegressor
from sklearn.neural_network import MLPRegressor
from sklearn.ensemble import GradientBoostingRegressor
from sklearn.ensemble import AdaBoostRegressor
from sklearn.svm import LinearSVR
from sklearn.svm import SVR
from xgboost import XGBRegressor
from catboost import CatBoostRegressor

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

def metrics(true, predict):
    RMSE = np.sqrt(mean_squared_error(true, predict))
    RMSPE = (np.sqrt(np.mean(np.square((true - predict) / true))))
    R2   = r2_score(true, predict)
    return RMSE,RMSPE,R2

# Inicio

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_2"

trainingFile = "/data/conjunto_de_treinamento.csv"
testFile = "/data/conjunto_de_teste.csv"

figures = path + '/figures'

data = pd.read_csv(path + trainingFile)
target_col = 'preco'
target = data.preco

# A coluna diferenciais é redundante, pois seus dados já estão codificados nas diversas colunas

data = data.drop(['diferenciais'], axis=1)

test_data = pd.read_csv(path + testFile)
data_description = data.describe()

test_data = test_data.drop(['diferenciais'], axis=1)

print("Contagem de valores nulos do dataset de treinamento")
print(data.isna().sum(), '\n\n\n')

# Box plot para checagem de outliers

sns.boxplot(data, y=target, log_scale=True)
plt.savefig(f'{figures}/outliers_boxplot.png')
plt.show()

# Não existem valores nulos, portanto todas as features podem continuar no plot

# for column in data:
#     print(f'Coluna {column} do dataset, caracteristicas: ')
#     print(f'Tipo: {data[column].dtypes}')
#     if (data[column].dtypes == 'object'):
#         print(f'Valores únicos: {data[column].unique()}')
#         print(f'Contagem por valor: {data[column].value_counts()}')
#     else:
#         print(f'Média: {data[column].mean()}')
#     print('\n\n')


# Ainda temos três features categóricas, podemos explorar mais sobre elas utilizando gráficos de barra

sns.histplot(data=data, x='tipo')
plt.savefig(f'{figures}/tipo_histplot.png')
plt.show()

sns.barplot(data=data, x='tipo', y=target)
plt.savefig(f'{figures}/tipo_barplot.png')
plt.show()

sns.histplot(data=data, x='tipo_vendedor')
plt.savefig(f'{figures}/tipo_vendedor_histplot.png')
plt.show()

sns.barplot(data=data, x='tipo_vendedor', y=target)
plt.savefig(f'{figures}/tipo_vendedor_barplot.png')
plt.show()

f, ax = plt.subplots(figsize = (20, 30))
sns.barplot(data=data, x=target, y='bairro')
plt.savefig(f'{figures}/bairro_preco_barplot.png')
plt.show()

# One Hot Encoding

bairro_count = data['bairro'].value_counts()
relevant_bairros = bairro_count[bairro_count >= 100]
relevant_bairros.index = ('bairro_' + relevant_bairros.index)

for dataset in [data, test_data]:
    dummies = pd.get_dummies(dataset['bairro'], prefix = 'bairro')
    dataset[relevant_bairros.index] = dummies[relevant_bairros.index]

pd.concat([data['bairro'], data[relevant_bairros.index]], axis=1).head()

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

# Coloca o valor em escala logaritmica
sns.boxplot(x=target)
plt.show()
data[target_col] = np.log(data[target_col])
target = data[target_col]
sns.boxplot(x=target)
plt.savefig(f'{figures}/preco_log_barplot.png')
plt.show()

correlations = data.corr(method='pearson')
target = data[target_col]
# data = data.drop([target_col], axis = 'columns')

# Escala o restantes das features

sns.scatterplot(data, x='area_util', y=target)
plt.savefig(f'{figures}/area_util_scatterplot.png')
plt.show()

sns.scatterplot(data, x='area_extra', y=target)
plt.savefig(f'{figures}/area_extra_scatterplot.png')

plt.show()

data['area_util'] = np.log(data['area_util'])
data['area_extra'] = np.log1p(data['area_extra'])

test_data['area_util'] = np.log(test_data['area_util'])
test_data['area_extra'] = np.log1p(test_data['area_extra'])

sns.scatterplot(data, x='area_util', y=target)
plt.savefig(f'{figures}/area_util_log_scatterplot.png')
plt.show()

sns.scatterplot(data, x='area_extra', y=target)
plt.savefig(f'{figures}/area_extra_log_scatterplot.png')
plt.show()

data['comodos'] = data['quartos'] + data['vagas'] 
# + data['suites']
# data = data.drop(['quartos', 'vagas', 'suites'], axis = 'columns')
test_data['comodos'] = test_data['quartos'] + test_data['vagas'] 
# + test_data['suites']
# test_data = test_data.drop(['quartos', 'vagas', 'suites'], axis = 'columns')

sns.scatterplot(data, x='comodos', y=target)
plt.savefig(f'{figures}/comodos_scatterplot.png')
plt.show()

quartos_suites = data['quartos'] - data['suites']

for dataset in [data, test_data]:
    quartos_suites = dataset['quartos'] - dataset['suites']
    dataset['razao_quarto_vaga'] = np.where(quartos_suites == 0, 0, dataset['vagas'] / quartos_suites)

data['soma_diferenciais'] = data[['churrasqueira', 'estacionamento', 'piscina','playground', 'quadra', 's_festas','s_jogos', 's_ginastica' ,'sauna']].sum(axis=1)
test_data['soma_diferenciais'] = test_data[['churrasqueira', 'estacionamento', 'piscina','playground', 'quadra', 's_festas','s_jogos', 's_ginastica' ,'sauna']].sum(axis=1)

correlations = data.corr()
print(abs(correlations[target_col]).sort_values(ascending=False))

data = data.drop([target_col, 'tipo_Quitinete'], axis = 'columns')

# f, ax = plt.subplots(figsize = (30, 30))
# sns.heatmap(data.corr(), annot=True, fmt='.1f')
# plt.savefig(f'{figures}/correlations_heatmap.png')
# plt.show()

# Selecionar melhores features para treinamento do modelo

chosen_columns = [
                  'comodos', 
                  'suites', 
                  'area_util', 
                  'vista_mar', 
                  'bairro_Boa Viagem', 
                  'bairro_Casa Forte', 
                  'tipo_Casa', 
                  'soma_diferenciais', 
                  'razao_quarto_vaga',
                  # 'bairro' Causa overfitting
                  ]
data = data[chosen_columns]
sns.set(font_scale=2.0)
f, ax = plt.subplots(figsize = (30, 30))
sns.heatmap(data.corr(), annot=True, fmt='.1f')
plt.savefig(f'{figures}/correlations_heatmap.png')
plt.show()
sns.set(font_scale=1.0)


# Scaling

scaler = MinMaxScaler()
scaler.fit(data)
x = scaler.transform(data)

# Seleção de modelos de regressão
jobs = -1
names = ['LinearRegression', 'SGDRegressor', 'RandomForestRegressor', 
         'ExtraTreesRegressor', 'KNeighborsRegressor',
         'MLPRegressor', 'GradientBoostingRegressor', 'LinearSVR', 'SVR', 'AdaBoostRegressor', 'XGBRegressor', 
         # 'CatBoostRegressor'
         ]

regressors = [
        LinearRegression(n_jobs=jobs), SGDRegressor(), 
        RandomForestRegressor(n_jobs=jobs), ExtraTreesRegressor(n_jobs=jobs), KNeighborsRegressor(n_jobs=jobs),
        MLPRegressor(), GradientBoostingRegressor(learning_rate=0.1, loss='huber', max_depth=6), LinearSVR(), SVR(), AdaBoostRegressor(),
        XGBRegressor(), 
        # CatBoostRegressor()
        ]

RMSE,RMSPE,R2 = [],[],[]
for regressor in regressors:
    cv_pred = cross_val_predict(regressor, x, target, cv=4, n_jobs=jobs)
    
    RMSE.append(np.sqrt(mean_squared_error(target, cv_pred)))
    RMSPE.append((np.sqrt(np.mean(np.square((target - cv_pred) / target)))))
    R2.append(r2_score(target, cv_pred))

scores = pd.DataFrame({
    'Regressor': names,
    'RMSE': RMSE,
    'RMSPE': RMSPE,
    'R2': R2
})

scores.sort_values('RMSPE',ascending=True)

print(scores)

parameters = {
    # 'loss': ['huber', 'squared_error'],
    # 'learning_rate': [x / 10.0 for x in range(1, 10, 1)],
    # 'criterion': ['friedman_mse', 'squared_error'],
    # 'max_depth': list(range(1, 9)),
    # 'max_features': ['sqrt', 'log2']
              }
grid = GridSearchCV(GradientBoostingRegressor(learning_rate=0.2, loss='huber', max_depth=5, max_features='log2'), 
                    parameters, cv=10)
grid.fit(x, target)
print(grid.best_params_)

chosen_regressor = GradientBoostingRegressor(learning_rate=0.2, loss='huber', max_depth=5, max_features='log2')

cv_pred = cross_val_predict(chosen_regressor, x, target, cv=6,n_jobs=jobs)

RMSE,RMSPE,R2 = metrics(target, cv_pred)
print('RMSE: %.4f / RMSPE: %.4f / R2: %.4f' % (RMSE,RMSPE,R2))

# Scatterplot entre a resposta correta e a resposta do modelo
plt.scatter(target, cv_pred, alpha = .4)
plt.plot([target.min(), target.max()], [target.min(), target.max()], 'k--', lw=2)
plt.xlabel('Real',fontsize=15)
plt.ylabel('Predito',fontsize=15)
plt.savefig(f'{figures}/real_vs_predito.png')
plt.show()

df = pd.DataFrame({
    'True Values': np.exp(target),
    'Predicted': np.floor(np.exp(cv_pred)),
})

test_data = test_data[chosen_columns]
x_test = scaler.transform(test_data)

chosen_regressor.fit(x, target)

response = np.floor(np.exp(chosen_regressor.predict(x_test)))

answer = pd.DataFrame({
    'Id': test_data.index,
    'preco': response
})

answer.to_csv('results.csv', index= False)
print(answer.head(9))