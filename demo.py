#!/usr/bin/python3.4

#from scipy.stats import skellam
from scipy import stats
import sys

def skellam_cdf(a,b,c):
    return stats.skellam.cdf(a,b,c)

def calculator():
    while True:
        #print('going to read a line',file=sys.stderr)
        v=sys.stdin.readline()
        #print('have line: \"%s\"'%v,file=sys.stderr)
        if v=='exit\n' or v=='':
            #print('done!')
            return
        a,b,c=map(float,v.split())
        print(skellam_cdf(a,b,c))
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
