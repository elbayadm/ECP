
from datetime import datetime
from csv import DictReader
from math import exp, log, sqrt
import random


# TL; DR, the main training process starts on line: 250,
# you may want to start reading the code from there


##############################################################################
# parameters #################################################################
##############################################################################

# A, paths
train = 'avazu_train.csv'               # path to training file
test = 'avazu_test.csv'                 # path to testing file
submission = 'subFTRL2.csv'  # path of to be outputted submission file

# B, model
alpha = .15  # learning rate
beta = 1.1   # smoothing parameter for adaptive learning rate
L1 = 1.1     # L1 regularization, larger value means more regularized
L2 = 1.1     # L2 regularization, larger value means more regularized

# C, feature/hash trick and data processing
D = 2 ** 22             # number of weights to use
interaction = True     # whether to enable poly2 feature interactions
dropout=0.8
sparse=False
dayfeature=True
dayfilter=None
device_counters=False
verbose=3
# D, training/validation
epoch = 1       # learn training data for N passes
holdout = 100  # use every N training instance for holdout validation


##############################################################################
# class, function, generator definitions #####################################
##############################################################################

class ftrl_proximal(object):
    ''' Our main algorithm: Follow the regularized leader - proximal

        In short,
        this is an adaptive-learning-rate sparse logistic-regression with
        efficient L1-L2-regularization

        Reference:
        http://www.eecs.tufts.edu/~dsculley/papers/ad-click-prediction.pdf
    '''

    def __init__(self, alpha, beta, L1, L2, D, 
                 interaction=False, dropout = 1.0, sparse = False,
                 dayfeature = True,
                 device_counters = False):
        # parameters
        self.alpha = alpha
        self.beta = beta
        self.L1 = L1
        self.L2 = L2
        self.dayfeature = dayfeature
        self.device_counters = device_counters

        # feature related parameters
        self.D = D
        self.interaction = interaction
        self.dropout = dropout        

        # model
        # n: squared sum of past gradients
        # z: weights
        # w: lazy weights
        self.n = [0.] * D
        self.z = [0.] * D
        
        if sparse:
            self.w = {}
        else:
            self.w = [0.] * D  # use this for execution speed up
    def _indices(self, x):
        ''' A helper generator that yields the indices in x

            The purpose of this generator is to make the following
            code a bit cleaner when doing feature interaction.
        '''
        for i in x:
            yield i

        if self.interaction:
            D = self.D
            L = len(x)
            for i in xrange(1, L):  # skip bias term, so we start at 1
                for j in xrange(i+1, L):
                    # one-hot encode interactions with hash trick
                    yield abs(hash(str(x[i]) + '_' + str(x[j]))) % D

    def predict(self, x, dropped = None):
        ''' Get probability estimation on x

            INPUT:
                x: features

            OUTPUT:
                probability of p(y = 1 | x; w)
        '''
        # params
        dropout = self.dropout

        # model
        w = self.w

        # wTx is the inner product of w and x
        wTx = 0.
        for j, i in enumerate(self._indices(x)):
            
            if dropped != None and dropped[j]:
                continue
           
            wTx += w[i]
        
        if dropped != None: wTx /= dropout 

        # bounded sigmoid function, this is the probability estimation
        return 1. / (1. + exp(-max(min(wTx, 35.), -35.)))

    def update(self, x, y):
        ''' Update model using x, p, y

            INPUT:
                x: feature, a list of indices
                p: click probability prediction of our model
                y: answer

            MODIFIES:
                self.n: increase by squared gradient
                self.z: weights
        '''

        # parameters
        alpha = self.alpha
        beta = self.beta
        L1 = self.L1
        L2 = self.L2

        # model
        n = self.n
        z = self.z
        w = self.w  # no need to change this, it won't gain anything
        dropout = self.dropout

        ind = [ i for i in self._indices(x)]
        
        if dropout == 1:
            dropped = None
        else:
            dropped = [random.random() > dropout for i in xrange(0,len(ind))]
        
        p = self.predict(x, dropped)

        # gradient under logloss
        g = p - y

        # update z and n
        for j, i in enumerate(ind):

            # implement dropout as overfitting prevention
            if dropped != None and dropped[j]: continue

            sigma = (sqrt(n[i] + g * g) - sqrt(n[i])) / alpha
            z[i] += g - sigma * w[i]
            n[i] += g * g
            
            sign = -1. if z[i] < 0 else 1.  # get sign of z[i]

            # build w on the fly using z and n, hence the name - lazy weights -
            if sign * z[i] <= L1:
                # w[i] vanishes due to L1 regularization
                w[i] = 0.
            else:
                # apply prediction time L1, L2 regularization to z and get w
                w[i] = (sign * L1 - z[i]) / ((beta + sqrt(n[i])) / alpha + L2)


def logloss(p, y):
    ''' FUNCTION: Bounded logloss

        INPUT:
            p: our prediction
            y: real answer

        OUTPUT:
            logarithmic loss of p given y
    '''

    p = max(min(p, 1. - 10e-15), 10e-15)
    return -log(p) if y == 1. else -log(1. - p)


def data(path, D, dayfilter = None, dayfeature = True, counters = False):
    ''' GENERATOR: Apply hash-trick to the original csv row
                   and for simplicity, we one-hot-encode everything

        INPUT:
            path: path to training or testing file
            D: the max index that we can hash to

        YIELDS:
            ID: id of the instance, mainly useless
            x: a list of hashed and one-hot-encoded 'indices'
               we only need the index since all values are either 0 or 1
            y: y = 1 if we have a click, else we have y = 0
    '''

    device_ip_counter = {}
    device_id_counter = {}

    for t, row in enumerate(DictReader(open(path))):
        # process id
        ID = row['id']
        del row['id']

        # process clicks
        y = 0.
        if 'click' in row:
            if row['click'] == '1':
                y = 1.
            del row['click']
 
        # turn hour really into hour, it was originally YYMMDDHH

        date = row['hour'][0:6]
        row['hour'] = row['hour'][6:]
        
        #       stderr.write("_%s_" % date)
        
        if dayfilter != None and not date in dayfilter:
            continue

        if dayfeature: 
            # extract date
            row['wd'] = str(int(date) % 7)
            row['wd_hour'] = "%s_%s" % (row['wd'], row['hour'])            

        if counters:
            d_ip = row['device_ip']
            d_id = row ["device_id"]
            try:
                device_ip_counter [d_ip] += 1
                device_id_counter [d_id] += 1
            except KeyError:
                device_ip_counter [d_ip] = 1
                device_id_counter [d_id] = 1                
            row["ipc"] = str(min(device_ip_counter[d_ip], 8))
            row["idc"] = str(min(device_id_counter[d_id], 8))

        # build x
        x = [0]  # 0 is the index of the bias term
        for key in row:
            value = row[key]

            # one-hot encode everything with hash trick
            index = abs(hash(key + '_' + value)) % D
            x.append(index)

        yield t, ID, x, y

##############################################################################
# start training #############################################################
##############################################################################

start = datetime.now()

# initialize ourselves a learner
learner=ftrl_proximal(alpha,beta,L1, L2, D, interaction,
                           dropout,
                           sparse,
                           dayfeature,
                           device_counters)
# start training
for e in xrange(epoch):
       loss = 0.
       count = 0
       next_report = 10000
       c = 0
       for t, ID, x, y in data(train, D, 
                               dayfilter = dayfilter,
                               dayfeature = dayfeature,
                               counters =device_counters):
           # data is a generator
           #  t: just a instance counter
           # ID: id provided in original data
           #  x: features
           #  y: label (click)
            
           # step 1, get prediction from learner
            
           if t % holdout == 0:
                # step 2-1, calculate holdout validation loss
                #           we do not train with the holdout data so that our
                #           validation loss is an accurate estimation of
                #           the out-of-sample error
                p = learner.predict(x)
                loss += logloss(p, y)
                count += 1
           else:
               # step 2-2, update learner with label (click) information
               learner.update(x, y)
            
           c += 1 
           if verbose > 2 and c >= next_report:
               print(' %s\tencountered: %d/%d\tcurrent logloss: %f\n' % (
                     datetime.now(), c, t, loss/count))
               next_report *= 2

       if count != 0:
           print('Epoch %d finished, %d/%d samples per pass, holdout logloss: %f, elapsed time: %s\n' % (
                    e, c, t, loss/count, str(datetime.now() - start)))
       else:
           print('Epoch %d finished, %d/%d samples per pass, suspicious holdout logloss: %f/%f, elapsed time: %s\n' % (
                  e, c, t, loss, count, str(datetime.now() - start)))

##############################################################################
# start testing, and build Kaggle's submission file ##########################
##############################################################################

with open(submission, 'w') as outfile:
    outfile.write('id,click\n')
    for t, date, ID, x, y in data(test, D):
        p = learner.predict(x)
        outfile.write('%s,%s\n' % (ID, str(p)))
