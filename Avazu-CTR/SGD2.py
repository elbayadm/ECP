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
from sklearn.naive_bayes import MultinomialNB
from sklearn import metrics
from sklearn.preprocessing import StandardScaler, normalize
import datetime

def wday(d):
  n=int(d.weekday())
  if n==0:return [0,0,0,0,0,0,1]
  if n==1:return [0,0,0,0,0,1,0]
  if n==2:return [0,0,0,0,1,0,0]
  if n==3:return [0,0,0,1,0,0,0]
  if n==4:return [0,0,1,0,0,0,0]
  if n==5:return [0,1,0,0,0,0,0]
  if n==6:return [1,0,0,0,0,0,0]
    
def generate_features(features, dataframe,hasher,scaler, fit=False):
    feature_matrices = []
    for f in features:
        feature_matrices.append(hasher.transform(k for k in dataframe[f].values))
    X_ctg=hstack(feature_matrices)
    X_hours = [[float(d.day),float(d.hour),float(d.hour)*float(d.weekday())]+wday(d) for d in dataframe['hour']]
    #X_enhanced=[[(int(d,16)%1021)]for d in dataframe['site_category']]
    X_features=coo_matrix(hstack([X_ctg,X_hours]))
#    if fit:
#      X_features=scaler.fit_transform(X_features)
#    else: X_features=scaler.transform(X_features)
   # return normalize(X_features,axis=1)
    return X_features
#Parameters
features=[ u'C1', u'site_domain', u'site_category',u'banner_pos' ,u'device_type', u'app_category', u'device_conn_type', u'C14', u'C15', u'C16', u'C17', u'C18', u'C19', u'C20', u'C21']
scaler = StandardScaler(with_mean=False)
hasher = FeatureHasher(n_features=210, non_negative=True, input_type="string")
clf = MultinomialNB()

#reading data in chunks and partially fitting the model
parse = lambda x: datetime.datetime.strptime(x, '%y%m%d%H')
data = pd.read_csv('chdata.csv', parse_dates=[2], date_parser=parse, dtype=str, chunksize=50000) 
i=0
for chunk in data:
  chunk_f=chunk.drop('click',1)
  X_chunk_train=generate_features(features,chunk_f,hasher,scaler, fit=True)
  Y_chunk_train=chunk['click'].apply(lambda x: int(x)).values
  clf.partial_fit(X_chunk_train, Y_chunk_train,[0,1])
  i+=1
  print('trained chunk:' + str(i) )




#load test and predict in chunks
#test=pd.read_csv('avazu_test.csv', dtype=str,chunksize=10000)
test=pd.read_csv('chdata2.csv', parse_dates=[2],iterator=True, date_parser=parse, dtype=str, chunksize=50000) 
submission_df = pd.DataFrame(columns=['id','click'])
j=0
for chunk in test:
  X_chunk_test=generate_features(features,chunk, hasher, scaler)
  Y_chunk=clf.predict_proba(X_chunk_test)
  Y_chunk=[pred[1] for pred in Y_chunk]
  Y_chunk_pred_cat=clf.predict(X_chunk_test)
  Y_chunk_test=chunk['click'].apply(lambda x: int(x)).values
  j+=1
  print('predicted chunk:'+str(j) )
  
  
  print("SDG log loss prediction")
  print metrics.log_loss(Y_chunk_test, Y_chunk)
  print metrics.classification_report(Y_chunk_test, Y_chunk_pred_cat)
  """
  predicted_chunk = pd.DataFrame()
  predicted_chunk['id'] = chunk['id']
  predicted_chunk['click'] = [pred[1] for pred in Y_chunk]
  submission_df = submission_df.append(predicted_chunk, ignore_index=True)
  j=+1
  print('predicted chunk:'+str(j) )
submission_df.to_csv("subm_chunked.csv", index=False)
"""







    
    