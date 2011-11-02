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
HOST="irc.overloadx.net"
PORT=6667
NICK="bot69"
PASSWORD="password"
IDENT="mudbot"
REALNAME="MudiousRoboticus"
OWNER='KeeperX' #The bot owner's nick 
CHANNELINIT='#bot' #The default channels for the bot to join including the log channel
readbuffer=""

"""
Initalize some variables
"""
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

s.send ( 'JOIN '+CHANNELINIT+' '+CHANNELINIT+'\r\n' )

"""
open the logfiles
"""



"""
The LOOP
"""

while True:
    data = s.recv ( 4096 )
    if data.find ( ' ' ) == -1:
        continue
    if data.find ( 'PING' ) != -1:
        s.send ( 'PONG ' + data.split() [ 1 ] + '\r\n' ) #this is required
    elif data.find ( 'PRIVMSG' ) != -1: #look for the command character as the first character of the line
        nick = data.split ( '!' ) [ 0 ].replace ( ':', '' )
        message = ':'.join ( data.split ( ':' ) [ 2: ] )
        destination = ''.join ( data.split ( ':' ) [ :2 ] ).split ( ' ' ) [ -2 ]

        """
        if you find "warez" at position 0
        """
        if message.find ( 'warez' ) :
            result = message.replace  ( '!repeat' ,'' )
            s.send ('PRIVMSG ' +  destination + ' : the riaa hates Illeagle file sharing \r\n')
            s.send ('PRIVMSG ' +  destination + ' : stop stealing music \r\n')
            s.send ('PRIVMSG ' +  destination + ' : bitch \r\n')
            s.send ('PRIVMSG ' +  destination + ' : yeah i called you a bitch\r\n')


            
        print '(',destination,')', nick + ':', message
    continue


