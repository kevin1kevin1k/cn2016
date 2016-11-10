import socket
import urllib
from bs4 import BeautifulSoup
import random

config = open('config')
for line in config:
    exec(line)
my_nickname = 'helloooooo'

cli_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
cli_socket.connect( ('irc.freenode.net', 6667) )
cli_socket.send(bytes('NICK %s \r\n' % my_nickname))
cli_socket.send(bytes('USER beforeAT 8 * :realname \r\n'))
cli_socket.send(bytes('JOIN %s %s \r\n' % (CHAN, CHAN_KEY)))

def send_(msg):
    cli_socket.send(bytes('PRIVMSG %s :%s \r\n' % (CHAN, msg)))

player = ''
while True:
    line = cli_socket.recv(1024)
    if line[:4] == 'PING':
        cli_socket.send(bytes(line.replace('PING', 'PONG')))
        continue
    
    line = line.rstrip()
    if len(line) < 2:
        print 'Error: len(line) < 2, line:', line
        break

    print line
    
    if 'PRIVMSG' in line:
        nickname = line[1 : line.find('!')]
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
                        send_('Game over. (%s)' % nickname)
                        player = ''
                    elif answer < guess:
                        send_('Smaller. (%d times left) (%s)' % (times, nickname))
                    else:
                        send_('Larger. (%d times left) (%s)' % (times, nickname))
            except:
                send_('Format Error (%s)' % nickname)
        elif msg.startswith('@youtube '):
            query = urllib.urlencode({'search_query': msg[9:]})
            url = 'http://www.youtube.com/results?' + query
            html = urllib.urlopen(url)
            soup = BeautifulSoup(html, 'lxml')
            try:
                song = soup.find(class_='yt-lockup-title').find('a')
                song_url = 'http://www.youtube.com' + song['href']
                song_title = song['title']
                send_('[%s] %s (%s)' % (song_url, song_title, nickname))
            except:
                send_('No result found. (%s)' % nickname)
        
    
cli_socket.close()
