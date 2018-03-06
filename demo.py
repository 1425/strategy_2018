#!/usr/bin/python3

#from scipy.stats import skellam
from scipy import stats
import sys

def skellam_cdf(a,b,c):
    return stats.skellam.cdf(a,b,c)

def calculator():
    f=stats.skellam.cdf
    r=sys.stdin.readline
    while True:
        v=r()
        if v=='':
            return
        print(f(*map(float,v.split())))
        sys.stdout.flush()

def to_file():
    f=open('out.txt','w')
    f.write(str(skellam_cdf(*map(float,sys.argv[1:]))))

if __name__=='__main__':
    #print('demo')
    #for i in range(10):
    #    for j in range(10):
    #        skellam_cdf(i,j,-1)

    calculator()

    #to_file()
    #print('done')
