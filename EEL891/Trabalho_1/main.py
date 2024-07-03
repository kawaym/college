import matplotlib.pyplot as plt
import sys
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns
from enum import Enum

if sys.platform.startswith('win32'):
    path="E:/Projetos/college/EEL891/Trabalho_1"

trainingFile = "/data/conjunto_de_treinamento.csv"

#  ========== CRIA DATAFRAME PRINCIPAL, LIMPA COLUNAS MAL PREENCHIDA E FAZ PRÉ-PROCESSAMENTO DOS DADOS ===========

stateDictionary = {'RJ': 1, 'RS': 2, 'BA': 3, 'CE': 4, 'PE': 5, 'PR': 6, 'PB': 7, 'SP': 8, 'MG': 9, 'PI': 10, 
                   'RO': 11, 'PA': 12, 'MT': 13, 'AL': 14, 'RN': 15, 'DF': 16, 'MA': 17, 'SC': 18, 'AM': 19,
                   'ES': 20, 'MS': 21, 'TO': 22, 'GO': 23, 'AC': 24, 'AP': 25, 'SE': 26, 'RR': 27}
sexDictionary = {'N': 0, 'M': 1, 'F': 2}
booleanDictionary = {'N': 0, 'Y': 1}
methodDictionary = {'presencial': 0, 'internet': 1, 'correio': 2}
variableTypeDictionary = {}

data = pd.read_csv(path + trainingFile, index_col='id_solicitante')
data = data.drop(['grau_instrucao', 'possui_telefone_celular', 'qtde_contas_bancarias_especiais'], axis=1)

print('Dados estatísticos do conjunto de dados')
print(data.describe(), '\n')

# Sanitiza os dados que podem conter nulos e converte valores para inteiros a fim de facilitar o cálculo de correlação posteriormente
for column in data:
    if (data[column].dtypes != 'float64' and data[column].dtypes != 'int64'):
        data[column] = data[column].apply(lambda x: 0 if x == ' ' else x)
        data[column] = data[column].apply(lambda x: sexDictionary[x] if x in sexDictionary else x)
        data[column] = data[column].apply(lambda x: stateDictionary[x] if x in stateDictionary else x)
        data[column] = data[column].apply(lambda x: booleanDictionary[x] if x in booleanDictionary else x)
        data[column] = data[column].apply(lambda x: methodDictionary[x] if x in methodDictionary else x)
        data[column] = data[column].apply(lambda x: int(x))
        variableTypeDictionary[column] = 'categorical'
    else:
        variableTypeDictionary[column] = 'continuous'

print(data.head())

# Gera gráficos de cada coluna para visualização

#sns.scatterplot(x=data['renda_mensal_regular'], y=data['valor_patrimonio_pessoal'], hue=data['inadimplente'])

#sns.scatterplot(x=data['valor_patrimonio_pessoal'], y=data['renda_mensal_regular'], hue=data['inadimplente'])
sns.lmplot(data=data, x='valor_patrimonio_pessoal', y='renda_mensal_regular', hue='inadimplente')
plt.yscale('log')
plt.xscale('log')

#sns.barplot(x=data['inadimplente'], y=data['valor_patrimonio_pessoal'], hue=data['sexo'])

#sns.swarmplot(x=data['inadimplente'], y=data['sexo'])
#plt.show()

# Calcula os indíces de correlação com a coluna-alvo, retorna as cinco maiores correlações

#corrcoefs = []

#for column in data:
#    if (column == 'inadimplente'): break
#    if (column == 'id_solicitante'): continue
#    if (variableTypeDictionary[column] == 'categorical'):
#    corr = data[column].corr(data['inadimplente'], method='spearman')
#    else:
#        corr = stats.pointbiserialr(data[column], data['inadimplente'])
#    corrcoefs.append([column, corr])


#corrcoefs.sort(reverse=True, key = (lambda x: abs(x[1])))

#selectedCoefs = [corrcoefs[i] for i in range(0,7)]

#print(selectedCoefs)

# Gera gráficos das sete maiores colunas com suas respectivas correlações, ajuda na visualização
