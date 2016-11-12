import socket
import urllib
from bs4 import BeautifulSoup
import random
import re

def send_(msg):
    cli_socket.send(bytes('PRIVMSG %s :%s \r\n' % (CHAN, msg)))

def get_nym(query, _nym, num=5):
    html = urllib.urlopen('http://www.thesaurus.com/browse/' + query)
    soup = BeautifulSoup(html, 'lxml')
    word_list = []
    for i in [3, 2, 1]:
        word_list += soup.find_all('a', attrs={'data-category': re.compile('"name": "relevant-%d".*' % (i * _nym))})
    result = [word.span.text for word in word_list][:num]
    if result == []:
        return 'No result found.'
    return ', '.join(result)

if __name__ == '__main__':
    config = open('config')
    for line in config:
        exec(line)
    my_nickname = 'Thesaurus'

    cli_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cli_socket.connect( ('irc.freenode.net', 6667) )
    cli_socket.send(bytes('NICK %s \r\n' % my_nickname))
    cli_socket.send(bytes('USER beforeAT 8 * :realname \r\n'))
    cli_socket.send(bytes('JOIN %s %s \r\n' % (CHAN, CHAN_KEY)))
    
    send_("Hi, I'm %s" % my_nickname)

    player = ''
    while True:
        line = cli_socket.recv(1024)
        if line[:4] == 'PING':
            cli_socket.send(bytes(line.replace('PING', 'PONG')))
            continue
        
        line = line.rstrip('\r\n')
        if len(line) < 2:
            print 'Error: len(line) < 2, line:', line
            break

        print line
        
        if 'PRIVMSG' in line:
            nickname = line[1 : line.find('!')]
            msg = line.split(':')[-1]
            if msg.startswith('@help'):
                send_('@repeat <String> (%s)' % nickname)
                send_('@cal <Expression> (%s)' % nickname)
                send_('@play <Robot Name> (%s)' % nickname)
                send_('@guess <Integer> (%s)' % nickname)
                send_('@synonym <Word> <Number>(default 5) (%s)' % nickname)
                send_('@antonym <Word> <Number>(default 5) (%s)' % nickname)
            elif msg.startswith('@repeat '):
                msg = msg[8:]
                send_('%s (%s)' % (msg, nickname))
            elif msg.startswith('@cal '):
                msg = msg[5:].replace('^', '**')
                try:
                    ans = eval(msg)
                    send_('%f (%s)' % (ans, nickname))
                except:
                    send_('Format Error (%s)' % nickname)
            elif msg.startswith('@play ' + my_nickname):
                if player == nickname:
                    send_('You are playing. (%s)' % nickname)
                elif player == '':
                    player = nickname
                    answer = random.randint(0, 100)
                    times = 5
                    send_('Game start! Guess the number from 0 to 100, inclusive (%d times left) (%s)' % (times, nickname))
            elif msg.startswith('@guess ') and player == nickname:
                try:
                    guess = int(msg[7:])
                    times -= 1
                    if answer == guess:
                        send_('Congratulations! (%s)' % nickname)
                        player = ''
                    else:
                        if times == 0:
                            send_('Game over. Answer is %d. (%s)' % (answer, nickname))
                            player = ''
                        elif answer < guess:
                            send_('Smaller. (%d times left) (%s)' % (times, nickname))
                        else:
                            send_('Larger. (%d times left) (%s)' % (times, nickname))
                except:
                    send_('Format Error (%s)' % nickname)
            elif msg.startswith('@synonym ') or msg.startswith('@antonym '):
                if msg.startswith('@synonym '):
                    _nym = 1
                else:
                    _nym = -1
                query = msg[9:].split()
                if len(query) == 1:
                    send_('%s (%s)' % (get_nym(query[0], _nym), nickname))
                elif len(query) == 2:
                    try:
                        num = int(query[1])
                        send_('%s (%s)' % (get_nym(query[0], _nym, num), nickname))
                    except:
                        send_('Format Error (%s)' % nickname)
                else:
                    send_('Format Error (%s)' % nickname)
        
    cli_socket.close()
