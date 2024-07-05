import matplotlib.pyplot as plt
import sys
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns
from enum import Enum
from sklearn import preprocessing

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_1"

trainingFile = "/data/conjunto_de_treinamento.csv"

#  ========== CRIA DATAFRAME PRINCIPAL, LIMPA COLUNAS MAL PREENCHIDAS E FAZ PRÉ-PROCESSAMENTO DOS DADOS ===========

typeDictionary = {}

data = pd.read_csv(path + trainingFile, index_col='id_solicitante')
data = data.drop(['grau_instrucao', 'possui_telefone_celular', 'qtde_contas_bancarias_especiais'], axis=1)
target = 'inadimplente'
#print('Dados estatísticos do conjunto de dados')
#print(data.describe(), '\n')

# Sanitiza os dados que podem conter nulos e converte valores para numéricos

encoder = preprocessing.OrdinalEncoder(encoded_missing_value=-1)
scaler = preprocessing.StandardScaler()
for column in data:
    if(data[column].dtypes != 'float64' and data[column].dtypes != 'int64'):
        data[[column]] = encoder.fit_transform(data[[column]])
        typeDictionary[column] = 'categorical'
        continue
    data[[column]] = scaler.fit_transform(data[[column]])
    data[[column]] = data[[column]].fillna(0)
    typeDictionary[column] = 'continuous'

print(data)

correlations = {}

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

for column in data:
    if column == target: continue
    if typeDictionary[column] == 'categorical':
        confusion_matrix = pd.crosstab(data[column], data[target])
        correlations[column] = cramers_corrected_stat(confusion_matrix)
    if typeDictionary[column] == 'continuous':
        correlations[column] = stats.pointbiserialr(data[column], data[target])[0]
        
npCorrelations = np.array(list(correlations.items()))
dfCorrelations = pd.DataFrame(data=[x for x in npCorrelations], columns=['id', 'correlation'])
dfCorrelations = dfCorrelations.sort_values(by=['correlation'])
print(dfCorrelations)

sns.scatterplot(data=dfCorrelations, x='id', y='correlation')