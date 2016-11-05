import socket
import urllib
from bs4 import BeautifulSoup

cli_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cli_socket.connect( ('irc.freenode.net', 6667) )
cli_socket.send(bytes('NICK helloooooo \r\n'))
cli_socket.send(bytes('USER beforeAT 8 * :realname \r\n'))
cli_socket.send(bytes('JOIN #CN_Demo ILoveTA \r\n'))

def send_(msg):
    cli_socket.send(bytes('PRIVMSG #CN_Demo :%s \r\n' % msg))

while True:
    line = cli_socket.recv(1024)
    if line[:4] == 'PING':
        send(bytes(line.replace('PING', 'PONG')), cli_socket)
        continue
    
    line = line.rstrip()
    if len(line) < 2:
        print 'Error: len(line) < 2, line:', line
        break

    print line
    
    if 'PRIVMSG' in line:
        nickname = line[1 : line.find('!~')]
        msg = line.split(':')[-1]
        if msg == '@help':
            send_('@repeat <String> (%s)' % nickname)
            send_('@cal <Expression> (%s)' % nickname)
            send_('@play <Robot Name> (%s)' % nickname)
            send_('@guess <Integer> (%s)' % nickname)
            send_('@youtube <Song> (%s)' % nickname)
        elif msg.startswith('@repeat '): # no word after @repeat accepted?
            msg = msg[8:]
            send_('%s (%s)' % (msg, nickname))
        elif msg.startswith('@cal '):
            msg = msg[5:].replace('^', '**')
            try:
                ans = eval(msg)
                send_('%f (%s)' % (ans, nickname))
            except:
                send_('Format Error (%s)' % nickname)
        
    
cli_socket.close()
