

# initiate chessboard
from ChessBoard import ChessBoard
import subprocess, time
import threading
import os, pygame,math
from pygame.locals import *

from pprint import pprint

from pygame.color import Color

maxchess = ChessBoard()# create an instance of Chessboard 

#serial

import serial
ser = serial.Serial("COM4", 9600)   # open serial port that Arduino is using
print ser                           # print serial config

print "Sending serial data"


# Reminder to close the connection when finished
if(ser.isOpen()):
   print "Serial connection is still open."



# initiate stockfish chess engine

engine = subprocess.Popen(
    'stockfish 7 x64.exe',
    universal_newlines=True,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,)

def get():
    
    stx=""
    engine.stdin.write('isready\n')
    print('\nengine:')
    while True :
        text = engine.stdout.readline().strip()
        if text == 'readyok':
            break
        if text !='':   
            print('\t'+text)
        if text[0:8] == 'bestmove':
        
            return text
def sget():
    
    stx=""
    engine.stdin.write('isready\n')
    print('\nengine:')
    while True :
        text = engine.stdout.readline().strip()
        #if text == 'readyok':
         #   break
        if text !='':   
            print('\t'+text)
        if text[0:8] == 'bestmove':
            mtext=text
            return mtext
def getboard():
    """ gets a text string from the board """
##    btxt=raw_input("\n Enter a board message: ").lower()
    btxt = ser.readline()
    print('\n sent from board'+btxt)
    print('\n Zia , This was recieved from avr:'+btxt)
    return btxt
    
def sendboard(stxt):
    """ sends a text string to the board """
    print("\n send to board:" +stxt)
    print('\n sent to board:'+stxt)
    print('\n yaaaaee  '+stxt)    
    ser.write("<"+stxt+">")

def newgame():
    get ()
    put('uci')
    get ()
    put('setoption name Skill Level value ' +skill)
    get ()
    put('setoption name Hash value 128')
    get()
    put('setoption name Best Book Move value true')
    get()
    put('setoption name OwnBook value true')
    get()
    put('uci')
    get ()
    put('ucinewgame')
    maxchess.resetBoard()
    fmove=""
    return fmove


gameResults = ["","WHITE WINS!","BLACK WINS!","STALEMATE","DRAW BY THE FIFTHY MOVES RULE","DRAW BY THE THREE REPETITION RULE"]


def bmove(fmove):
    """ assume we get a command of the form ma1a2 from board"""    
    fmove=fmove
    # Get a move from the board
    brdmove = bmessage[1:5].lower()
    # now validate move
    # if invalid, get reason & send back to board
      #  maxchess.addTextMove(move)
    if maxchess.addTextMove(brdmove) == False :
                        etxt = "error"+ str(maxchess.getReason())+brdmove
                        maxchess.printBoard()
                        sendboard(etxt)
                        return fmove
                       
#  elif valid  make the move and send Fen to board
    
    else:
        maxchess.printBoard()
        # maxfen = maxchess.getFEN()
        # sendboard(maxfen)
       # remove line below when working
       # raw_input("\n\nPress the enter key to continue")
        print ("fmove")
        print(fmove)
        print ("brdmove")
        print(brdmove)
        fmove =fmove+" " +brdmove

        cmove = "position startpos moves"+fmove
        print (cmove)

            #        if fmove == True :
            #                move = "position startpos moves "+move
            #        else:
            #               move ="position fen "+maxfen

        # put('ucinewgame') 
        # get()

       
        put(cmove)
        # send move to engine & get engines move

        
        put("go movetime " +movetime)

        text = sget()
        print (text)
        smove = text[9:13]
        hint = text[21:25]
        if maxchess.addTextMove(smove) != True :
                        stxt = "e"+ str(maxchess.getReason())+move
                        maxchess.printBoard()
                        sendboard(stxt)

        else:
                        if maxchess.isGameOver():
                           temp=fmove
                           fmove =temp+" " +smove
                           stx = smove + gameResults[maxchess.getGameResult()] ## +hint   # took this out     
                           sendboard(stx)
                           maxchess.printBoard()

                           print ("computer move: " +smove + "stx:" + stx)
                           return fmove
                        else:
                           temp=fmove
                           fmove =temp+" " +smove
                           Ischeck="unchecked"
                           if maxchess.isCheck():
                              Ischeck="checked"
                           
                           stx = smove+hint+Ischeck      
                           sendboard(stx)
                           maxchess.printBoard()
              
                           print ("computer move:"+smove+"sent to board:" + stx)
                           return fmove
        

def put(command):
    print('\nyou:\n\t'+command)
    engine.stdin.write(command+'\n')



# assume new game
print ("\n Chess Program \n")
skill = "19"
movetime = "6000"
fmove = newgame()


def worker():
   global fmove,bmessage
   
   while True:

       # Get  message from board
       bmessage = getboard()
       # Message options   Move, Newgame, level, style
       code = bmessage[0]
       
      
       
       # decide which function to call based on first letter of txt
       fmove=fmove
       if code == 'm': fmove = bmove(fmove)
       elif code == 'n': newgame()
       elif code == 'l': level()
       elif code == 's': style()
       else :  sendboard('error at option')

try:    
   t = threading.Thread(target=worker)
   t.daemon = True
   t.start()
except:
   t.exit()
   print("Failed")


   
pygame.init()   
def gui():
    while True:
        pieces = {}    
        board = maxchess.getBoard()
        turn = maxchess.getTurn()

        screen = pygame.display.set_mode((900, 480),1)
        pygame.display.set_caption('ChessBoard Client')

        # load all images
        pieces = [{},{}]
        pieces[0]["r"] = pygame.image.load("./img/brw.png")                
        pieces[0]["n"] = pygame.image.load("./img/bnw.png")                
        pieces[0]["b"] = pygame.image.load("./img/bbw.png")                
        pieces[0]["k"] = pygame.image.load("./img/bkw.png")                
        pieces[0]["q"] = pygame.image.load("./img/bqw.png")                
        pieces[0]["p"] = pygame.image.load("./img/bpw.png")                
        pieces[0]["R"] = pygame.image.load("./img/wrw.png")                
        pieces[0]["N"] = pygame.image.load("./img/wnw.png")                
        pieces[0]["B"] = pygame.image.load("./img/wbw.png")                
        pieces[0]["K"] = pygame.image.load("./img/wkw.png")                
        pieces[0]["Q"] = pygame.image.load("./img/wqw.png")                
        pieces[0]["P"] = pygame.image.load("./img/wpw.png")                
        pieces[0]["."] = pygame.image.load("./img/w.png")                
        pieces[1]["r"] = pygame.image.load("./img/brb.png")                
        pieces[1]["n"] = pygame.image.load("./img/bnb.png")                
        pieces[1]["b"] = pygame.image.load("./img/bbb.png")                
        pieces[1]["k"] = pygame.image.load("./img/bkb.png")                
        pieces[1]["q"] = pygame.image.load("./img/bqb.png")                
        pieces[1]["p"] = pygame.image.load("./img/bpb.png")                
        pieces[1]["R"] = pygame.image.load("./img/wrb.png")                
        pieces[1]["N"] = pygame.image.load("./img/wnb.png")                
        pieces[1]["B"] = pygame.image.load("./img/wbb.png")                
        pieces[1]["K"] = pygame.image.load("./img/wkb.png")                
        pieces[1]["Q"] = pygame.image.load("./img/wqb.png")                
        pieces[1]["P"] = pygame.image.load("./img/wpb.png")                
        pieces[1]["."] = pygame.image.load("./img/b.png")                

        clock = pygame.time.Clock()

        posRect = pygame.Rect(0,0,60,60)
        board = maxchess.getBoard() 
        mousePos = [-1,-1]
        markPos = [-1,-1]
        validMoves = []
        gameResults = ["","WHITE WINS!","BLACK WINS!","STALEMATE","DRAW BY THE FIFTHY MOVES RULE","DRAW BY THE THREE REPETITION RULE"]

       
            
        for event in pygame.event.get():
            if event.type == QUIT:
                return
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    return


            board = maxchess.getBoard()
            turn = maxchess.getTurn()
            markPos[0] = -1
            validMoves = [] 
                                                                  



        if maxchess.isGameOver():
           pygame.display.set_caption("Game Over! (Reason:%s)" % gameResults[maxchess.getGameResult()])
           validMove = []
           markPos[0] = -1
           markPos[1] = -1
        else:
           pygame.display.set_caption('ChessBoard Client') 
                                       
        y = 0
        for rank in board:
           x = 0
           for p in rank:
               screen.blit(pieces[(x+y)%2][p],(x*60,y*60))
               x+=1
           y+=1             

        if markPos[0] != -1:
           posRect.left = markPos[0]*60
           posRect.top = markPos[1]*60
           pygame.draw.rect(screen, (255,255,0),posRect, 4)


        background = pygame.Surface((480,480))
        background = background.convert()
        background.fill((250, 250, 250))
        font = pygame.font.Font(None, 36)
        text = font.render("Timer", 1, (10, 10, 10))
        textpos = text.get_rect()
        textpos.centerx = background.get_rect().centerx
        background.blit(text, textpos)


        

       # Blit everything to the screen
        screen.blit(background, (480, 0))                          
        pygame.display.flip()
        
gui()

