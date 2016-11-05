import socket
import urllib
from bs4 import BeautifulSoup

def send_(string, skt):
    skt.send(bytes(string + ' \r\n'))

cli_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cli_socket.connect( ('irc.freenode.net', 6667) )
cli_socket.send(bytes('NICK helloooooo \r\n'))
cli_socket.send(bytes('USER beforeAT 8 * :realname \r\n'))
cli_socket.send(bytes('JOIN #CN_Demo ILoveTA \r\n'))

while True:
    line = cli_socket.recv(1024).rstrip()
    if len(line) < 2:
        print 'Error: len(line) < 2, line:', line
        break
    print line
    # cli_socket.send_(bytes('PRIVMSG #CN_Demo ' + 'hello ' + '\r\n'))
    if line[:4] == 'PING':
        send_(line.replace('PING', 'PONG'), cli_socket)
    
    if 'PRIVMSG' in line:
        nickname = line[1 : line.find('!~')]
        msg = line.split(':')[-1]
        if msg == '@help': # more text accepted?
            send_('PRIVMSG #CN_Demo :@repeat <String> (%s)' % nickname, cli_socket)
            send_('PRIVMSG #CN_Demo :@cal <Expression> (%s)' % nickname, cli_socket)
            send_('PRIVMSG #CN_Demo :@play <Robot Name> (%s)' % nickname, cli_socket)
            send_('PRIVMSG #CN_Demo :@guess <Integer> (%s)' % nickname, cli_socket)
            send_('PRIVMSG #CN_Demo :@youtube <Song> (%s)' % nickname, cli_socket)
        elif msg.startswith('@repeat '): # no word after @repeat accepted?
            # this replace all @repeat, which is not good
            msg = msg.replace('@repeat ', '')
            send_('PRIVMSG #CN_Demo :%s (%s)' % (msg, nickname), cli_socket)
        elif msg.startswith('@cal'):
            msg = msg.replace('@cal', '').replace('^', '**')
            try:
                ans = eval(msg)
                send_('PRIVMSG #CN_Demo :%f (%s)' % (ans, nickname), cli_socket)
            except:
                send_('PRIVMSG #CN_Demo :Format Error (%s)' % nickname, cli_socket)
        
    
cli_socket.close()
