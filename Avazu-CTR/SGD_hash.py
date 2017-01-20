# -*- coding: utf-8 -*-
"""
Created on Thu Dec 11 14:57:36 2014

@author: mahaelbayad
"""

# -*- coding: utf-8 -*-
"""
Created on Wed Dec 10 15:35:44 2014
"""
import pandas as pd
from scipy.sparse import hstack, coo_matrix
from sklearn.feature_extraction import FeatureHasher
from sklearn.linear_model import SGDClassifier, LogisticRegression
from sklearn.cross_validation import train_test_split
from sklearn import metrics
from sklearn.preprocessing import StandardScaler
import time

def datevector(n):
  if n==0 : return [0,0,0,0,0,0,1]
  if n==1 : return [0,0,0,0,0,1,0]
  if n==2 : return [0,0,0,0,1,0,0]
  if n==3 : return [0,0,0,1,0,0,0]
  if n==4 : return [0,0,1,0,0,0,0]
  if n==5 : return [0,1,0,0,0,0,0]
  if n==6 : return [1,0,0,0,0,0,0]

#reading data & hashing
data=pd.read_csv('train_sample_5ml.csv', dtype=str , nrows=50000)
data_f=data.drop('click',1)
features=[ u'id', u'C1', u'banner_pos', u'site_id', u'site_domain',  u'site_category', u'app_id', u'app_domain', u'app_category',u'device_id', u'device_ip', u'device_model', u'device_type', u'device_conn_type', u'C14', u'C15', u'C16', u'C17', u'C18', u'C19', u'C20', u'C21']
##Special treatement for hour
X_hours = [[int(time.strptime(d,"%y%m%d%H").tm_hour),int(time.strptime(d, "%y%m%d%H").tm_wday)] for d in data['hour']]

"""
X_hours = [np.concatenate(([int(time.strptime(d,"%y%m%d%H").tm_hour)], datevector(int(time.strptime(d, "%y%m%d%H").tm_wday)))) for d in data['hour']]
"""
scaler = StandardScaler()
X_hours=coo_matrix(scaler.fit_transform(X_hours))
###the hasher
hasher = FeatureHasher(n_features=2**18, input_type="string")
#############
features_matrices = []
for feature in features:
  features_matrices.append(hasher.transform(k for k in data_f[feature].values))
X_cat=hstack(features_matrices)
X_train=hstack([X_cat,X_hours])
print(X_train)
Y_train=data['click'].apply(lambda x: int(x)).values
#cross-validation
# split into training / test sets
X_train, X_test, Y_train, Y_test = train_test_split(X_train, Y_train, test_size=0.7)

# fit SDG
#clf = SGDClassifier(loss="log", penalty="l2",shuffle=True, n_iter=700)
clf = LogisticRegression(penalty="l2", C=100)
clf.fit(X_train, Y_train)
"""
#load test and predict in chunks
test=pd.read_csv('avazu_test.csv', dtype=str,chunksize=10000)
submission_df = pd.DataFrame(columns=['id','click'])
for chunk in test:
  chunk_matrices = []
  for feature in features:
    chunk_matrices.append(hasher.transform(k for k in chunk[feature].values))
  X_chunk_hours = [np.concatenate(([int(time.strptime(d,"%y%m%d%H").tm_hour)], datevector(int(time.strptime(d, "%y%m%d%H").tm_wday)))) for d in chunk['hour']]
  X_chunk=hstack(chunk_matrices,X_chunk_hours)
  Y_chunk=clf.predict_proba(X_chunk)
  predicted_chunk = pd.DataFrame()
  predicted_chunk['id'] = chunk['id']
  predicted_chunk['click'] = [pred[1] for pred in Y_chunk]
  submission_df = submission_df.append(predicted_chunk, ignore_index=True)
submission_df.to_csv("subm.csv", index=False)
"""
# predict on the test set
Y_pred = clf.predict_proba(X_test)
Y_pred_pro = [pred[1] for pred in Y_pred]
"""
print metrics.classification_report(Y_test, Y_pred)
"""
print metrics.log_loss(Y_test, Y_pred_pro)




    
    