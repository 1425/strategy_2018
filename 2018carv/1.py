#!/usr/bin/env python

from common_utils.util import print_lines,filter_unique

def parse():
    lines=open('prelim_sched.txt','r').read().splitlines()
    out=[]
    while len(lines):
        s=lines[0]
        #if 'Qual'==s[:4] and s!='Qualification Match Schedule':
        
        if ':' in s and '-' not in s and len(s.split())==2:
            x={'time':s}
            lines=lines[2:]
            x['match_num']=int(lines[0])
            lines=lines[1:]
            red1=lines[0]
            lines=lines[1:]
            red2=lines[0]
            lines=lines[1:]
            red3=lines[0]
            lines=lines[1:]
            blue1=lines[0]
            lines=lines[1:]
            blue2=lines[0]
            lines=lines[1:]
            blue3=lines[0]
            lines=lines[1:]
            def parse_team(s):
                if s[-1]=='*':
                    return {'#':int(s[:-1]),'surrogate':1}
                return {'#':int(s),'surrogate':0}
            x['red']=map(parse_team,[red1,red2,red3])
            x['blue']=map(parse_team,[blue1,blue2,blue3])
            out.append(x)
        else:
            lines=lines[1:]
    return out

def split1(s):
    #print 's:',s
    out=[]
    while len(s):
        a=''
        while len(s) and s[0]!=',':
            if s[0]=='"':
                s=s[1:]
                while len(s) and s[0]!='"':
                    a+=s[0]
                    s=s[1:]
                s=s[1:] #skip closing " and ,
            else:
                a+=s[0]
                s=s[1:]
        s=s[1:]
        out.append(a)
    return out


def caleb():
    lines=open('caleb.csv','r').read().splitlines()
    labels=filter(len,lines[3].split(','))
    print 'labels:',labels
    def process_data((key,value)):
        def value1():
            if key in ['nickname','home Championship','event',"Chairman's Strength (mCA)"]:
                return value
            if key in ['total Points','winning Margin','ranking Points','auto Points',
                'teleop Excluding Endgame Points',
                'endgame Points','Elo Points','auto Elo Points','scale Elo Points',
                'switch and Scale Elo Points','win','opponent Switch Deactivated Seconds',
                'placeholder','auto Run Points','auto Run Rate','auto Switch Ownership Seconds','auto Switch At Zero',
                'auto Scale Ownership Seconds','auto Scale Ownership Margin',
                'auto Ownership Points','auto Ownership Points Margin','auto Quest Ranking Point',
                'teleop Switch Ownership Seconds','teleop Switch Force Seconds',
                'teleop Switch Boost Seconds','teleop Scale Ownership Seconds','teleop Scale Force Seconds',
                'teleop Scale Boost Seconds','teleop Ownership Points','teleop Ownership Points Margin',
                'vault Force Played','vault Force Total','average Force Efficiency','vault Levitate Played',
                'vault Levitate Total','average Levitate Efficiency','vault Boost Played','vault Boost Total',
                'average Boost Efficiency','vault Points','parking Points','climbing Points','levitate Points',
                'climb Rate','adjusted Levitate Rate','adjusted Climb Or Levitate Rate',
                'adjusted Individual Endgame Points','face The Boss Ranking Point','foul Count','fouls Drawn','tech Foul Count',
                'tech Fouls Drawn','foul Points','foul Points Drawn','tba Ranking Points Earned',
                ]:
                try:
                    return float(value)
                except:
                    print '"%s" "%s"'%(key,value)
                    raise
            try:
                return int(value)
            except:
                print 'key:',key
                raise
        return key,value1()
    out=[]
    for line in lines[4:]:
        print_lines(zip(labels,split1(line)))
        z=map(process_data,zip(labels,split1(line)))
        out.append(dict(z))
        #print_lines(z)
        print
        #=dict(zip(labels,line.split(',')))
        #rint d
    return out


if __name__=='__main__':
    #print_lines(parse())

    c=caleb()

    p=parse()

    def team_pts(team_num):
        #int->float
        f=filter(lambda y: y['team Number']==team_num,c)
        if len(f)==0:
            #This appears because some teams did not play for the first time until after when the data is from.
            f=0 #print 'f:',f,x
        else:
            f=f[-1]['total Points']
        return f


    def show_team(x):
        def surrogate_marker():
            if x['surrogate']:
                return '*'
            return ''
        return str(x['#'])+surrogate_marker()+' ('+str(team_pts(x['#']))+')'

    def show_alliance(a):
        return map(show_team,a)

    for p1 in p:
        def get_total(alliance_color):
            return sum(map(lambda x: team_pts(x['#']),p1[alliance_color]))

        print '\t'.join(map(str,
            [p1['match_num'],p1['time']]+
            show_alliance(p1['red'])+
            show_alliance(p1['blue'])+
            [get_total('red'),get_total('blue')]
            ))

    def get_team_rp(team):
        #int->float
        f=filter(lambda y: y['team Number']==team,c)
        if len(f)==0:
            return 0
        return f[-1]['ranking Points']

    team_rp={}
    for p1 in p:
        def add_rp(alliance):
            teams_playing=map(lambda x: x['#'],p1[alliance])
            total=sum(map(get_team_rp,teams_playing))

            #TO DEAL W/ surrogate matches.
            teams_counting=map(lambda x: x['#'],filter(lambda y: not y['surrogate'],p1[alliance]))
            for team in teams_counting:
                if team not in team_rp:
                    team_rp[team]=0
                team_rp[team]+=total

        add_rp('red')
        add_rp('blue')

    for a in reversed(sorted(map(lambda x: (x[1],x[0]),team_rp.iteritems()))):
        print a
