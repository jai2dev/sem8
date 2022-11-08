
import csv
import networkx as nximport 
import collections
import numpy as np
import matplotlib.pyplot as plt
import math
import networkx as nx

def read_csv_file(file_name):
	with open(file_name, newline='') as csvfile:
	    data = list(csv.reader(csvfile))

	data = sorted(data,key = lambda x: int(x[0]))

	return data

def compressed_dictionaty(data,upto_tim):
	dit = {}
	compress = 0

	for x in data:
		if(int(x[0])>upto_tim):
			break

		st = x[1]
		if st in dit:
			pass
		else:
			dit[st] = compress
			compress = compress + 1
	
		st = x[2]
		if st in dit:
			continue
		else:
			dit[st] = compress
			compress = compress + 1

	return dit

def plot_graph(G):
	nx.draw(G, with_labels=True, font_weight='bold')
	plt.show()

upto_tim = 100

G = nx.DiGraph()

de = collections.deque([0,0])

mini_check = 0.05

data = read_csv_file('input.csv')

dit = compressed_dictionaty(data,upto_tim)

def getting_warning_list(p=0.5,q=0.33,k=3):

	graph_no_list = []
	Gk = [0]*k

	de = [0]*k

	prev_dependents = 3

	for i in range(k):
		temp_lis = [0]*prev_dependents
		de[i] = collections.deque(temp_lis)

	for x in dit:
		G.add_node(dit[x])

	n = len(G)

	sizes = [0]*k
	for i in range(k):
		Gk[i] = []
		source = [0]*n
		dest = [0]*n
		for x in G:
			y = np.random.binomial(1, p)
			if(y):
				source[x] = 1
			y = np.random.binomial(1, q)
			if(y):
				dest[x] = 1
			Gk[i]=[source, dest]

		for nodes in G:
			if(source[nodes] or dest[nodes]):
				sizes[i] = sizes[i] + 1

	prev = 0
	sum = 0
	sz = len(dit)

	def mean_sq_error(de):
		mean_sq = 0

		for itr in de:
			mean_sq = mean_sq + (rat - itr)*(rat - itr)

		mean_sq = math.sqrt(mean_sq)

		if(mean_sq >= mini_check*sizes[i]):
			return 1
			
		return 0

	def mean_ab_error(de):
		mean_ab = 0

		for itr in de:
			mean_sq = mean_ab + abs(rat - itr)

		if(mean_sq >= mini_check*sizes[i]):
			return 1
			
		return 0

	X = []
	Y = []

	for x in data:

		if(int(x[0])>upto_tim):
			break

		u = dit[x[1]]
		v = dit[x[2]]
		
		sum = sum + 1
		G.add_weighted_edges_from([(u,v,1)])
		
		if(int(x[0]) == prev):
			continue

		prev = int(x[0])

		flag = 0

		error_list = [0]*k
	  
		XX = []

		for i in range(k):

			if(sizes[i] == 0):
				continue

			source = Gk[i][0]
			dest = Gk[i][1]
			score = 0

			g = nx.DiGraph()

			for nodes in G:
				if(source[nodes] or dest[nodes]):
					g.add_node(nodes)

			for e in G.edges():
				uu = e[0] 
				vv = e[1]
				if(source[uu] and dest[vv]):
					g.add_weighted_edges_from([(uu,vv,1)])
					score += 1

			XX += [score]
			rat = score

			if(mean_sq_error(de[i])):
				flag = 1

			error_list[i] = rat

		X.append(XX)
		Y.append(flag)
		

		if(flag):
			# print("Warning")
			graph_no_list.append(int(x[0]))
			# print(error_list)
		else:
			for i in range(k):
				de[i].append(error_list[i])
				de[i].popleft()
		
		# print(de)
		
		G.clear()

		for x in dit:
			G.add_node(dit[x])

	return X, Y, graph_no_list

X, Y, original_error_list = getting_warning_list(1,1,1)
print(original_error_list)

X, Y, get_other_list1 = getting_warning_list(0.5,0.33,1)
print(get_other_list1)

X, Y, get_other_list2 = getting_warning_list(0.5,0.33,20)
print(get_other_list2)

error1 = 0
for i in original_error_list:
	if(get_other_list1.count(i) == 0):
		error1 = error1 + 1

error2 = 0
for i in original_error_list:
	if(get_other_list2.count(i) == 0):
		error2 = error2 + 1

error1 = error1/len(original_error_list) * 100.00
error2 = error2/len(original_error_list) * 100.00

print(error1,error2)
label = ['First','Second']
index = np.arange(len(label))
plt.bar([0,1], [error1,error2])
plt.xlabel('Type')
plt.ylabel('Error Percentage')
plt.xticks(index, label)
plt.title('Comparision')
plt.show()

from medoids import Medoid
from sklearn.linear_model import LogisticRegression
from sklearn.datasets import make_blobs
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

