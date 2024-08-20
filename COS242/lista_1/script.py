import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

df = pd.read_csv("results.csv", delimiter=",")
df[['user', 'system', 'total']] = df[['user', 'system', 'total']].apply(pd.to_numeric)

sns.lineplot(df, x='size', y='total', hue='prints')
plt.savefig('graph.png')
plt.show()


print(df)