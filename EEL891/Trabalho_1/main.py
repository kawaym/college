import matplotlib.pyplot as plt
import sys
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns

from sklearn import preprocessing
from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import train_test_split
from sklearn.feature_selection import RFE
from sklearn.naive_bayes import BernoulliNB
from sklearn.tree import DecisionTreeClassifier

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_1"

trainingFile = "/data/conjunto_de_treinamento.csv"
testFile = "/data/conjunto_de_teste.csv"

#  ========== CRIA DATAFRAME PRINCIPAL, LIMPA COLUNAS MAL PREENCHIDAS E FAZ PRÉ-PROCESSAMENTO DOS DADOS ===========

typeDictionary = {}

training_data = pd.read_csv(path + trainingFile, index_col='id_solicitante')
training_data = training_data.drop(['grau_instrucao', 'possui_telefone_celular', 'qtde_contas_bancarias_especiais'], axis=1)
unsupervised_test_data = pd.read_csv(path + testFile, index_col='id_solicitante')
unsupervised_test_data = unsupervised_test_data.drop(['grau_instrucao', 'possui_telefone_celular', 'qtde_contas_bancarias_especiais'], axis=1)

target = 'inadimplente'
#print('Dados estatísticos do conjunto de dados')
#print(data.describe(), '\n')

# Sanitiza os dados que podem conter nulos e converte valores para numéricos

encoder = preprocessing.OrdinalEncoder(encoded_missing_value=-1)
scaler = preprocessing.StandardScaler()
for column in training_data:
    if (column == 'inadimplente'): continue
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

training, supervised_test = train_test_split(training_data, test_size=0.3)
training_x = training.iloc[:, :-1].values
training_y = training.iloc[:, -1].values
supervised_test_x = supervised_test.iloc[:, :-1].values
supervised_test_y = supervised_test.iloc[:, -1].values

unsupervised_test_x = unsupervised_test_data.iloc[:, :].values

# CLASSSIFICADOR KNN

# classifier = KNeighborsClassifier(n_neighbors=5, weights='uniform')
# classifier = classifier.fit(training_x, training_y)
# answer_training_y = classifier.predict(training_x)

# print("\nClassificador KNN (Dentro da Amostra)\n")
# total = len(training_y)
# acertos = sum(answer_training_y == training_y)
# erros = sum(answer_training_y != training_y)

# print("Total de amostras: ", total)
# print("Repostas corretas: ", acertos)
# print("Respostas erradas: ", erros)

# acuracia = acertos / total

# print("Acurácia = %.1f %%" % (100*acuracia))
# print("Taxa Erro = %4.1f %%" % (100*(1-acuracia)))

# answer_supervised_test_y = classifier.predict(supervised_test_x)

# print("\nClassificador KNN (Fora da Amostra)\n")
# total = len(supervised_test_y)
# acertos = sum(answer_supervised_test_y == supervised_test_y)
# erros = sum(answer_supervised_test_y != supervised_test_y)

# print("Total de amostras: ", total)
# print("Repostas corretas: ", acertos)
# print("Respostas erradas: ", erros)

# acuracia = acertos / total

# print("Acurácia = %.1f %%" % (100*acuracia))
# print("Taxa Erro = %4.1f %%" % (100*(1-acuracia)))

# def get_weights(A):
#     with np.errstate(divide='ignore'):
#         B = 1 / np.sqrt(A)
#     return B


# print("\n  K TREINO  TESTE ERRTRN ERRTST")
# print(" -- ------ ------ ------ ------")

# for k in range(1, 31):
#     # for k in range(10,501,10):

#     classifier = KNeighborsClassifier(n_neighbors=k, weights="uniform")
#     classifier = classifier.fit(training_x, training_y)

#     answer_training_y = classifier.predict(training_x)
#     answer_supervised_test_y = classifier.predict(supervised_test_x)

#     acuracia_treino = sum(answer_training_y == training_y)/len(training_y)
#     acuracia_teste = sum(answer_supervised_test_y == supervised_test_y) / len(supervised_test_y)


#     print(
#         "%3d" % k,
#         "%6.1f" % (100*acuracia_treino),
#         "%6.1f" % (100*acuracia_teste),
#         "%6.1f" % (100*(1-acuracia_treino)),
#         "%6.1f" % (100*(1-acuracia_teste))
#     )


classifier = KNeighborsClassifier(n_neighbors=17, weights='uniform')
classifier = classifier.fit(training_x, training_y)
results = classifier.predict(supervised_test_x)

print("\nClassificador Bernoulli Naive Bayes (Dentro da Amostra)\n")
total = len(results)
acertos = sum(results == supervised_test_y)
erros = sum(results != supervised_test_y)

print("Total de amostras: ", total)
print("Repostas corretas: ", acertos)
print("Respostas erradas: ", erros)

acuracia = acertos / total

print("Acurácia = %.1f %%" % (100*acuracia))
print("Taxa Erro = %4.1f %%" % (100*(1-acuracia)))

# Naive Bayes Classifier

classifier = BernoulliNB(alpha=1.0)
classifier = classifier.fit(training_x, training_y)
results = classifier.predict(supervised_test_x)

print("\nClassificador Bernoulli Naive Bayes (Dentro da Amostra)\n")
total = len(results)
acertos = sum(results == supervised_test_y)
erros = sum(results != supervised_test_y)

print("Total de amostras: ", total)
print("Repostas corretas: ", acertos)
print("Respostas erradas: ", erros)

acuracia = acertos / total

print("Acurácia = %.1f %%" % (100*acuracia))
print("Taxa Erro = %4.1f %%" % (100*(1-acuracia)))

# Decision Tree Classifier 

classifier = DecisionTreeClassifier(criterion='gini', max_features=17, max_depth=7)
classifier = classifier.fit(training_x, training_y)
results = classifier.predict(supervised_test_x)

print("\nClassificar Árvore de Decisão (Dentro da Amostra)\n")
total = len(results)
acertos = sum(results == supervised_test_y)
erros = sum(results != supervised_test_y)

print("Total de amostras: ", total)
print("Respostas corretas: ", acertos)
print("Respostas erradas: ", erros)

acuracia = acertos / total

print("Acurácia = %.1f %%" % (100*acuracia))
print("Taxa Erro = %4.1f %%" % (100*(1-acuracia)))