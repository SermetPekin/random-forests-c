import numpy as np
from sklearn.datasets import make_classification
import csv

# Generate a binary classification dataset
X, y = make_classification(
    n_samples=150,
    n_features=4,
    n_informative=2,
    n_redundant=0,
    n_classes=2,
    random_state=42
)

header = [f'feature_{i+1}' for i in range(X.shape[1])] + ['label']
rows = np.column_stack((X, y))

with open('data.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(header)
    writer.writerows(rows)

print("CSV file 'data.csv' created for binary classification.")
