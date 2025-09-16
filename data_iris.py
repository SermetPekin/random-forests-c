import numpy as np
from sklearn.datasets import load_iris
import csv

# Load the Iris dataset (classic for classification)
data = load_iris()

X = data['data']
y = data['target']

# Filter to only include samples with label 0 or 1
mask = (y == 0) | (y == 1)
X_bin = X[mask]
y_bin = y[mask]

header = [f'feature_{i+1}' for i in range(X.shape[1])] + ['label']
rows = np.column_stack((X_bin, y_bin))

with open('iris_binary.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(header)
    writer.writerows(rows)

print("CSV file 'iris_binary.csv' created from sklearn's iris dataset (labels 0 and 1 only).")
