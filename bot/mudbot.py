#!/usr/bin/env python
import os
import sys
import time
import socket
import string


"""
Mud-Bot is a python irc bot that has been designed for ROM2.4b6
Its is just a simple bot that reads informatin from certain files
in the rom folder. This code can very easily be reworked to fit 
any number of needs.

Make a directory in your mud directory called "bot" and launch the
bot from there. The bot looks in "../player/" for the player files
if you do not keep your files there, you will have to edit the
code.

Complete the below config and launch the bot. if all goes well you
should see it in the channel in a few seconds.
type !help for a list of commands
"""


"""
Config
"""
HOST="irc.blinkenshell.org"
PORT=6667
NICK="MUD-Bot"
PASSWORD="firewall"
IDENT="mudbot"
REALNAME="MudiousRoboticus"
OWNER='KeeperX' #The bot owner's nick 
CHANNELINIT='#mud,#mudlog' #The default channels for the bot to join including the log channel
LOGCHANNEL='#mudlog' #Which channel should the log read to
readbuffer=""

"""
Initalize some variables
"""
logLine = ''
score=''
oldLine=[]
mudText =[]

"""
Connect
"""

s=socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
s.connect((HOST, PORT))
s.send("NICK %s\r\n" % NICK)
s.send("USER %s %s bla :%s\r\n" % (IDENT, HOST, REALNAME))
s.send ('PRIVMSG nickserv :identify '+PASSWORD+' \r\n')

s.send ( 'JOIN '+CHANNELINIT+' #mud\r\n' )

"""
open the logfiles
"""
mudLog = open ( '../area/nohup.out', 'r' )
logFile = open ( 'mudbot.log', 'a' )



"""
The LOOP
"""

while True:
    mudData = mudLog.readlines()
    tlen = len(mudData)
    start=int(tlen)-int(3)
    appl=mudData[start:tlen]
    mudText = string.join(appl , '\n')
    if oldLine != mudText:
        s.send ('PRIVMSG ' +  LOGCHANNEL + ' : ' + mudText + ' \r\n')
        oldLine = mudText
    else:
        oldLine = oldLine

        
    data = s.recv ( 4096 )
    if data.find ( ' ' ) == -1:
        continue
    logFile.write ( '\n\n' + data  )
    if data.find ( 'PING' ) != -1:
        s.send ( 'PONG ' + data.split() [ 1 ] + '\r\n' ) #this is required
    elif data.find ( 'PRIVMSG' ) != -1: #look for the command character as the first character of the line
        nick = data.split ( '!' ) [ 0 ].replace ( ':', '' )
        message = ':'.join ( data.split ( ':' ) [ 2: ] )
        destination = ''.join ( data.split ( ':' ) [ :2 ] ).split ( ' ' ) [ -2 ]

        """
        if you find "!" at position 0
        """
        if message.find ( '!' ) ==0:


            """
            The Help 
            """
            if message.find ( '!help' ) !=-1:
                result = message.replace  ( '!repeat' ,'' )
                s.send ('PRIVMSG ' +  destination + ' : Commands are :: \r\n')
                s.send ('PRIVMSG ' +  destination + ' : !help ::: prints this message \r\n')
                s.send ('PRIVMSG ' +  destination + ' : !score <playername> ::: Prints information about the player \r\n')
                s.send ('PRIVMSG ' +  destination + ' : !repeat <string> ::: repeats whatever you type\r\n')


            """
            The Repeater
            """
            if message.find ( '!repeat' ) !=-1: 
                result = message.replace  ( '!repeat' ,'' )
                s.send ('PRIVMSG ' +  destination + ' :' + result + '\r\n')


            """
            Player Information
            """
            if message.find ( '!score ' ) !=-1:
                player = message.replace ( '!score ','' )
                player = player.replace( '\r\n','' )
                player = player.lower()
                player = player.capitalize()
                filename = ('../player/'+player) 
                if os.path.exists(filename):
                    fileData = open ( filename, 'r')
                    for line in fileData:                            

                        if line.find ( 'Race' ) !=-1:
                            Race = line.replace ( 'Race','')
                            Race = Race.replace ( '\n','')
                            Race = Race.replace ( '\r','')
                            Race = Race.replace ( '\0','')
                            Race = Race.replace ( '~','')
                            Race = Race.replace ( ' ','')
                            s.send  ('PRIVMSG ' +  destination + ' :' + player + ' is a ' + Race + '\r\n')
                            

                        if line.find ( 'Cla' ) !=-1:
                            Cla = line.replace ( 'Cla','')
                            Cla = Cla.replace ( '\n','')
                            Cla = Cla.replace ( '\r','')
                            Cla = Cla.replace ( '\0','')
                            Cla = Cla.replace ( ' ','')
                            Cla = Cla.replace ( '0','Mage')
                            Cla = Cla.replace ( '1','Cleric')
                            Cla = Cla.replace ( '2','Thief')
                            Cla = Cla.replace ( '3','Warrior')
                            s.send  ('PRIVMSG ' +  destination + ' :' + player + ' is of the ' + Cla + ' class' + '\r\n')
                            

                        if line.find ( 'Levl' ) !=-1:
                            Levl = line.replace ( 'Levl','')
                            Levl = Levl.replace ( '\n','')
                            Levl = Levl.replace ( '\r','')
                            Levl = Levl.replace ( '\0','')
                            Levl = Levl.replace ( ' ','')
                            s.send  ('PRIVMSG ' +  destination + ' :' + player + ' is level ' + Levl + '\r\n')
                            

                        if line.find ( 'Exp' ) !=-1:
                            Exp = line.replace ( 'Exp','')
                            Exp = Exp.replace ( '\n','')
                            Exp = Exp.replace ( '\r','')
                            Exp = Exp.replace ( '\0','')
                            Exp = Exp.replace ( ' ','')
                            s.send  ('PRIVMSG ' +  destination + ' :' + player + ' has ' + Exp + ' experience points' + '\r\n')


                        if line.find ( 'Titl' ) !=-1:
                            Titl = line.replace ( 'Titl','')
                            Titl = Titl.replace ( '\n','')
                            Titl = Titl.replace ( '\r','')
                            Titl = Titl.replace ( '\0','')
                            Titl = Titl.replace ( '~','')
                            s.send  ('PRIVMSG ' +  destination + ' :' + player + ' is known as -' + player + Titl + '\r\n')
                            
                        if message.find ( 'Plyd ' ) !=-1:
                            Plyd = line.replace ( 'Plyd ','')
                            Plyd = Plyd.replace ( '\n','')
                            Plyd = Plyd.replace ( '\r','')
                            Plyd = Plyd.replace ( '\0','')
                            Plyd = Plyd.replace ( ' ','')
                            Plyd = (Plyd / 60)/60
                            s.send  ('PRIVMSG ' +  destination + ' :' + player + ' has played for ' + Plyd + ' Hours' + '\r\n')

                       
                                        
                else:
                    s.send ('PRIVMSG ' +  destination + ' : Im sorry that player does not exist' + '\r\n')

        print '(',destination,')', nick + ':', message
    continue

