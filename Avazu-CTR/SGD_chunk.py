# -*- coding: utf-8 -*-
"""
Created on Fri Dec 12 15:21:34 2014

@author: mahaelbayad
"""

parse = lambda x: datetime.datetime.strptime(x, '%y%m%d%H')
data = pd.read_csv('train_sample_5ml.csv', parse_dates=[2], date_parser=parse, dtype=str, chunksize=10000) 
for chunk in data:
  chunk_f=chunk.drop('click',1)
  X_chunk_train=generate_features(features,chunk_f,hasher,scaler, fit=True)
  Y_chunk_train=chunk['click'].apply(lambda x: int(x)).values
  clf.partial_fit(X_chunk_train, Y_chunk_train)
  
  
features=[ u'id', u'C1', u'banner_pos', u'site_id', u'site_domain',  u'site_category', u'app_id', u'app_domain', u'app_category',u'device_id', u'device_ip', u'device_model', u'device_type', u'device_conn_type', u'C14', u'C15', u'C16', u'C17', u'C18', u'C19', u'C20', u'C21']
scaler = StandardScaler()
hasher = FeatureHasher(n_features=2**18, input_type="string")


#############
X_train=generate_features(features,data_f,hasher,scaler, fit=True)
Y_train=data['click'].apply(lambda x: int(x)).values