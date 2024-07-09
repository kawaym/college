import matplotlib.pyplot as plt
import sys
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns
from enum import Enum

from sklearn import preprocessing
from sklearn.neighbors import KNeighborsClassifier

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_1"

trainingFile = "/data/conjunto_de_treinamento.csv"
testFile = "/data/conjunto_de_teste.csv"

#  ========== CRIA DATAFRAME PRINCIPAL, LIMPA COLUNAS MAL PREENCHIDAS E FAZ PRÉ-PROCESSAMENTO DOS DADOS ===========

typeDictionary = {}

training_data = pd.read_csv(path + trainingFile, index_col='id_solicitante')
training_data = training_data.drop(['grau_instrucao', 'possui_telefone_celular', 'qtde_contas_bancarias_especiais'], axis=1)
test_data = pd.read_csv(path + testFile, index_col='id_solicitante')
test_data = test_data.drop(['grau_instrucao', 'possui_telefone_celular', 'qtde_contas_bancarias_especiais'], axis=1)

target = 'inadimplente'
#print('Dados estatísticos do conjunto de dados')
#print(data.describe(), '\n')

# Sanitiza os dados que podem conter nulos e converte valores para numéricos

encoder = preprocessing.OrdinalEncoder(encoded_missing_value=-1)
scaler = preprocessing.StandardScaler()
for column in training_data:
    if(training_data[column].dtypes != 'float64' and training_data[column].dtypes != 'int64'):
        training_data[[column]] = encoder.fit_transform(training_data[[column]])
        typeDictionary[column] = 'categorical'
        continue
    training_data[[column]] = scaler.fit_transform(training_data[[column]])
    training_data[[column]] = training_data[[column]].fillna(0)
    typeDictionary[column] = 'continuous'

for column in test_data:
    if(test_data[column].dtypes != 'float64' and test_data[column].dtypes != 'int64'):
        test_data[[column]] = encoder.fit_transform(test_data[[column]])
        typeDictionary[column] = 'categorical'
        continue
    test_data[[column]] = scaler.fit_transform(test_data[[column]])
    test_data[[column]] = test_data[[column]].fillna(0)
    typeDictionary[column] = 'continuous'

print(training_data)

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

for column in training_data:
    if column == target: continue
    if typeDictionary[column] == 'categorical':
        confusion_matrix = pd.crosstab(training_data[column], training_data[target])
        correlations[column] = cramers_corrected_stat(confusion_matrix)
    if typeDictionary[column] == 'continuous':
        correlations[column] = stats.pointbiserialr(training_data[column], training_data[target])[0]
        
npCorrelations = np.array(list(correlations.items()))
dfCorrelations = pd.DataFrame(data=[x for x in npCorrelations], columns=['id', 'correlation'])
dfCorrelations = dfCorrelations.sort_values(by=['correlation'])

# Embaralha os dados para fazer as predições

shuffled_data = training_data.sample(frac=1, random_state=3213)
training_x = shuffled_data.iloc[:, :-1].values
training_y = shuffled_data.iloc[:, -1].values

test_x = test_data.iloc[:, :].values

# CLASSSIFICADOR KNN

classifier = KNeighborsClassifier(n_neighbors=1, weights='uniform')
classifier = classifier.fit(training_x, training_y)
answer_training_y = classifier.predict(training_x)
answer_test_y = classifier.predict(test_x)

print("\nClassificador KNN (Dentro da Amostra)\n")
total = len(training_y)
acertos = sum(answer_training_y == training_y)
erros = sum(answer_training_y != training_y)

print("Total de amostras: ", total)
print("Repostas corretas: ", acertos)
print("Respostas erradas: ", erros)

acuracia = acertos / total

print("Acurácia = %.1f %%" % (100*acuracia))
print("Taxa Erro = %4.1f %%" % (100*(1-acuracia)))