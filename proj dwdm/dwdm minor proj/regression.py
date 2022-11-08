

from sklearn.linear_model import LogisticRegression
from sklearn.datasets.samples_generator import make_blobs
from sklearn import metrics



train_x = X[:80]
train_y = Y[:80]


test_x = X[80:]
test_y = Y[80:]


model = LogisticRegression()
model.fit(train_x, train_y)


predicted = model.predict(test_x)

for i in range(len(test_x)):
	print("X=%s, Predicted=%s, Actual=%s" % (test_x[i], predicted[i], test_y[i]))
 
print("Accuracy:",metrics.accuracy_score(test_y, predicted))
print("Precision:",metrics.precision_score(test_y, predicted))
