#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Agents strategies:

'''

import numpy as np
from wumpus import Agent, Action

moves = [
    Action.UP,
    Action.DOWN,
    Action.LEFT,
    Action.RIGHT
]

flashes = [
    Action.FLASH_UP,
    Action.FLASH_DOWN,
    Action.FLASH_LEFT,
    Action.FLASH_RIGHT
]


class PrudentAgent(Agent):
    '''
    Save the flashlight units
    '''
    def reset(self):
        self.fd = -1

    def getAction(self):
        x, y, breeze, smell, num_flash = self.state_
        if smell and num_flash:  # ----------------------- High risk of encountering a wumpus - use flash if possible
            if self.fd == -1:
                self.fd = np.random.randint(0,3)
                action = Action(flashes[self.fd])
            else:                # -----------------------  Move in the same direction as the flash
                action = Action(moves[self.fd])
                self.fd = -1
        else:                    # -----------------------  Random from moves
            action = Action(np.random.choice(list(moves)))
        
        return action

class state_encoding(object):
    '''
    Encoding of states 'BSF', 'ABSF', 'XYBSF'
    '''
    def __init__(self, dims):
        self.dims = dims

    def get_state(self, agent):
        '''
        retrieve the current state
        '''
        pass

    def get_state_index(self, agent):
        '''
        Get the given state flat index
        '''
        state = self.get_state(agent)
        # ---- beware X,Y starts from 1
        state_index = np.ravel_multi_index( state, dims=self.dims, order='C')
        return state_index


class BSF(state_encoding):
    def __init__(self, n_flash):
        state_encoding.__init__(self, [2, 2, n_flash+1])

    def get_state(self, agent):
        return agent.state_[2:]


class ABSF(state_encoding):
    def __init__(self, n_flash):
        state_encoding.__init__(self, [len(Action), 2, 2, n_flash+1])

    def get_state(self, agent):
        return [agent.previous_action-1] + agent.state_[2:]


class XYBSF(state_encoding):
    def __init__(self, grid_size, n_flash):
        state_encoding.__init__(self, [grid_size, grid_size, 2, 2, n_flash+1])

    def get_state(self, agent):
        return agent.state_


class epsGreedyAgent(Agent):
    '''
    Selects the best action with probability 1- eps
    eps=1 :    Random agent
    eps=0 :    Greedy agent
    '''
    def __init__(self, eps, encoding):
        self.eps = eps
        self.encoding = encoding
        self.Q = np.zeros((np.prod(encoding.dims), len(Action)))  
        self.Nvisits = np.zeros((np.prod(encoding.dims), len(Action)), dtype=np.int)  
        Agent.__init__(self)

    def reset(self):
        self.first_visit = True 
        self.current_action = Agent.getAction(self)

    def getAction(self):
        self.current_action = self.getActionBandit()
        return self.current_action

    def getActionBandit(self):
        '''
        Determine the best action and make a decision
        '''
        if np.random.uniform() < self.eps:
            # explore
            action = Action(np.random.randint(1,len(Action)))
        else:
            # exploit
            state_index = self.encoding.get_state_index(self)
            qstate = self.Q[state_index] / (self.Nvisits[state_index] + 1*(self.Nvisits[state_index]==0))
            action = Action(1 + np.argmax(qstate))
        return action

    def nextState(self, s, reward):
        # update Q
        if not self.first_visit:
            state_index = self.encoding.get_state_index(self)
            self.Q[state_index, self.current_action-1] += reward
            self.Nvisits[state_index, self.current_action-1] += 1.
        else:
            self.first_visit = False

        Agent.nextState(self, s, reward)
        self.previous_action = self.current_action  



def softmax(w):
    '''
    Array softmax
    '''
    e = np.exp(w - np.max(w))
    out = e / e.sum()
    return out


class softmaxAgent(epsGreedyAgent):
    def __init__(self, temperature, encoding):
        Agent.__init__(self)
        epsGreedyAgent.reset(self)
        self.temperature = temperature
        self.encoding = encoding
        self.Q = np.zeros((np.prod(encoding.dims), len(Action)))  
        self.Nvisits = np.zeros((np.prod(encoding.dims), len(Action)), dtype=np.int)  
        

    def getActionBandit(self):
        state_index = self.encoding.get_state_index(self)
        qstate = self.Q[state_index] / (self.Nvisits[state_index] + 1*(self.Nvisits[state_index]==0))
        smax = softmax(qstate / self.temperature)
        action= Action (1 + np.random.choice(len(Action), 1, p=smax))
        return action

class UCBAgent(epsGreedyAgent):
    def __init__(self, encoding):
        Agent.__init__(self)
        epsGreedyAgent.reset(self)
        self.encoding = encoding
        self.Q = np.zeros((np.prod(encoding.dims), len(Action)))  
        self.Nvisits = np.zeros((np.prod(encoding.dims), len(Action)), dtype=np.int)  
        
    def getActionBandit(self):
        state_index = self.encoding.get_state_index(self)
        # B_s = \hat\mu_s +\sqrt{2log(t)/ts(t)}
        t = np.sum(self.Nvisits)
        t = t +1*(t==0)
        ts = self.Nvisits[state_index] + 1*(self.Nvisits[state_index]==0)
        qstate = self.Q[state_index] / ts
        B = qstate + np.sqrt(2*np.log(t)/ts)
        action = Action(1 + np.argmax(B))
        return action

    def nextState(self, s, reward):
        #-------------------------------- Scale rewards in [0 1]
        s_reward = (float(reward) + 10) / 110  
        epsGreedyAgent.nextState(self, s, s_reward)
