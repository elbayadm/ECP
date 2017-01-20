# -*- coding: utf-8 -*-
"""
Created on Sat Dec 13 08:30:25 2014

@author: mahaelbayad
"""
import pandas as pd
import numpy as np
import datetime


from sklearn.preprocessing import normalize, StandardScaler, PolynomialFeatures
from sklearn.cross_validation import train_test_split
from sklearn.naive_bayes import MultinomialNB
from sklearn.decomposition import PCA
from sklearn import metrics
from math import fabs


parse = lambda x: datetime.datetime.strptime(x, '%y%m%d%H')
data = pd.read_csv('chdata.csv', parse_dates=[2], date_parser=parse, dtype=str, nrows=5000) 
data_f=data.drop('click',1)
Y=data['click'].apply(lambda x: int(x)).values

#Raw inputs
features=[u'id', u'C1', u'banner_pos', u'site_id', u'site_domain', u'site_category', u'app_id', u'app_domain', u'app_category', u'device_id', u'device_ip', u'device_model', u'device_type', u'device_conn_type', u'C14', u'C15', u'C16', u'C17', u'C18', u'C19', u'C20', u'C21']
#X_hours = [[float(d.day)*float(d.hour)*float(d.weekday()),float(d.day)*float(d.weekday()),float(d.hour),float(d.hour)*float(d.weekday()),float(d.weekday())]for d in data_f['hour']]
X_hours= [[float(d.day),float(d.weekday()),float(d.hour%12)]for d in data_f['hour']]
matrix_f=[]
for f in features:
 matrix_f.append([fabs(int(str(k),16)) for k in data_f[f]])
matrix_f=np.transpose(matrix_f)
inputs=np.concatenate((X_hours,matrix_f),axis=1)
"""
#polynomial features
poly=PolynomialFeatures(2,interaction_only=False)
inputs=poly.fit_transform(inputs)
print(inputs.shape)
"""
#Preprocessing:
"""
#scaling
inputs_log=normalize(inputs,axis=0)
scaler = StandardScaler()
scaler.fit_transform(inputs_log)
"""
"""
#PCA
inputs_log=inputs
pca=PCA(whiten=True,n_components=60)
inputs_log=pca.fit_transform(inputs_log)
print(inputs_log.shape)
"""
#cross validation
X_train1, X_test1, Y_train1, Y_test1 = train_test_split(inputs, Y, test_size=0.7)


#ModelMultiNBC
clf = MultinomialNB()
clf.fit(X_train1, Y_train1)
Y_pred = clf.predict(X_test1)
Y_proba = clf.predict_proba(X_test1)
Y_proba = [pred[1] for pred in Y_proba]
print metrics.classification_report(Y_test1, Y_pred)
print metrics.log_loss(Y_test1, Y_proba)

