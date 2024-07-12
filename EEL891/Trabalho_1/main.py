import matplotlib.pyplot as plt
import sys
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns
import matplotlib.cm as cm


from sklearn import preprocessing, linear_model
from sklearn.feature_selection import SelectKBest, f_classif
from sklearn.metrics import confusion_matrix, classification_report
from sklearn.neighbors import KNeighborsClassifier
from sklearn.model_selection import train_test_split, cross_val_predict
from sklearn.naive_bayes import BernoulliNB
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.ensemble import HistGradientBoostingClassifier
from sklearn.ensemble import VotingClassifier
from sklearn.cluster import KMeans
from xgboost import XGBClassifier
from catboost import CatBoostClassifier

# Utilitários

def printResults(results, targetY, name, inside=True):
    text = "Fora"
    if (inside): text = "Dentro"
    print(f"\nClassificador {name} ({text} da Amostra)\n")

    createMatrixConfusion(target = targetY, predicted = results, name = name)    
    
    print(createClassificationReport(target = targetY, predicted = results))
    
def createResults(classifier, training_x, training_y, targetX):
    classifier.fit(training_x, training_y)
    return classifier, classifier.predict(targetX)

def createClassificationReport(target, predicted, output_dict=False):
    report = classification_report(y_true=target, y_pred=predicted, target_names=['Negative', 'Positive'], output_dict=output_dict)
    return report

def createMatrixConfusion(target, predicted, name="Modelo de Classificação"):
    labels = ['Negativo', 'Positivo']
    matrix = confusion_matrix(y_true=target, y_pred=predicted)
    sns.heatmap(matrix, annot=True, cmap=cm.gray, xticklabels=labels, yticklabels=labels, fmt=".10g")
    plt.title(f"Matriz de Confusão para {name}")
    plt.show()
    return matrix    

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

selector = SelectKBest(f_classif, k=7)
selector.fit(training_data.iloc[:, :-1], training_data.iloc[:,-1])
cols_idxs = selector.get_support(indices=True)
training_data = training_data.iloc[:,cols_idxs]
training_data.insert(len(training_data.columns), 'inadimplente', last_y.values)

# Embaralha os dados para fazer as predições

training, supervised_test = train_test_split(training_data, test_size=0.001, random_state=31212)
training_x = training.iloc[:, :-1]
training_y = training.iloc[:, -1]
supervised_test_x = supervised_test.iloc[:, :-1]
supervised_test_y = supervised_test.iloc[:, -1]

unsupervised_test_x = unsupervised_test_data.iloc[:, :]

# KNN Classifier 

k_number = 1
accuracy = 0.0

for k in range(1, 31):

    classifier = KNeighborsClassifier(n_neighbors=k, weights="distance")
    classifier = classifier.fit(training_x, training_y)
    _, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)
    
    report = createClassificationReport(target=supervised_test_y, predicted=results, output_dict=True)
    
    accuracy_extracted = report['accuracy']
    
    if (accuracy_extracted > accuracy):
        k_number = k
        accuracy = accuracy_extracted

classifier = KNeighborsClassifier(n_neighbors=k_number, weights='uniform')
KNNClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "KNN")

# Naive Bayes Classifier

classifier = BernoulliNB(alpha=1.0)
BNBClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Bernoulli Naive Bayes")

# Decision Tree Classifier 

feature_number = 1
depth_number = 1
accuracy = 0.0

for k in range(1, 7):
    classifier = DecisionTreeClassifier(criterion='gini', max_features=k, max_depth=7)
    classifier = classifier.fit(training_x, training_y)
    _, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)
    
    report = createClassificationReport(target=supervised_test_y, predicted=results, output_dict=True)
    
    accuracy_extracted = report['accuracy']
    
    if (accuracy_extracted > accuracy):
        feature_number = k
        accuracy = accuracy_extracted

for k in range(1, 10):
    classifier = DecisionTreeClassifier(criterion='gini', max_features=feature_number, max_depth=k)
    classifier = classifier.fit(training_x, training_y)
    _, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)
    
    report = createClassificationReport(target=supervised_test_y, predicted=results, output_dict=True)
    
    accuracy_extracted = report['accuracy']
    
    if (accuracy_extracted > accuracy):
        depth_number = k
        accuracy = accuracy_extracted

classifier = DecisionTreeClassifier(criterion='gini', max_features=feature_number, max_depth=7)
DTClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Árvore de Decisão")

# Decision Forest Classifier

classifier = ExtraTreesClassifier(n_estimators=100, max_features=feature_number)
DFClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Floresta de Decisão Extra")

# Random Forest Classifier

classifier = RandomForestClassifier(n_estimators=100, criterion='gini', max_features='sqrt', max_depth=depth_number)
RFClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Floresta de Decisão Aleatória")

# Neural Network Classifier

classifier = MLPClassifier(max_iter=1000)
NNClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Resdes Neurais")

# Gradient Boosting Classifier

classifier = HistGradientBoostingClassifier(learning_rate=0.3, max_features=1.0)
GBClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Boosting de Gradiente")

# Ada Boosting Classifier

# classifier = AdaBoostClassifier(estimator=DecisionTreeClassifier(), learning_rate=0.1, n_estimators=100)
# ABClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

# printResults(results, supervised_test_y, "Boosting Ada")

# XGBoost Classifier

classifier = XGBClassifier()
XGClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "XGBoosting")

# CatBoost Classifier

classifier = CatBoostClassifier(logging_level='Silent')
CBClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "CatBoosting")

# Voting Classifier

estimators = [('knn', KNNClassifier), 
              ('bnb', BNBClassifier), 
              ('dt', DTClassifier), 
              ('gb', GBClassifier), 
              # ('df', DFClassifier), 
              ('rf', RFClassifier), 
              ('xg', XGClassifier),
              ('cb', CBClassifier),
              ('nn', NNClassifier)]
estimators_weights = [1 for _ in estimators]

classifier = VotingClassifier(estimators=estimators, voting='soft', weights=estimators_weights)
VClassifier, results = createResults(classifier, training_x, training_y, targetX = supervised_test_x)

printResults(results, supervised_test_y, "Votação")
answer_y = VClassifier.predict(unsupervised_test_x.iloc[:,cols_idxs])
answer = pd.DataFrame(data={'id_solicitante': [i for i in range(20001, 25001)], 'inadimplente': answer_y})
answer.to_csv('results.csv', index= False)
print(answer)