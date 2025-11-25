#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

extern int startY, startX ;
extern bool lanjut ;

typedef struct {
    int x ;
    int y ;
} koordinat ;

int gameHeight = 25, gameWidth = 80 ;
int score ;
int pBadan ;
koordinat kepala, apel, gerak, badan[1000] ;
WINDOW * snakeWin ;


void printWcolor(WINDOW * curWin, int y, int x, char *s, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    mvwprintw(curWin, y, x, "%s", s) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

void charWcolor(WINDOW * curWin, int y, int x, char s, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    mvwaddch(curWin, y, x, s) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

void colorBox(WINDOW * curWin, int color) {
    wattron(curWin, COLOR_PAIR(color)) ;
    box(curWin, 0, 0) ;
    wattroff(curWin, COLOR_PAIR(color)) ;
}

void delWin(WINDOW * tmp) {
    wclear(tmp) ;
    wrefresh(tmp) ;
    delwin(tmp) ;
    refresh() ;
}

bool cek1 (koordinat a, koordinat b) {
    if(a.x != b.x) return false ;
    if(a.y != b.y) return false ;
    
    return true ;
}

bool cek_mati() {
    for(int i=0; i<pBadan; i++) {
        if(kepala.x == badan[i].x && kepala.y == badan[i].y) {
            lanjut = false ;
            return true ;
        }
    }

    if(kepala.x == 0 || kepala.x == gameWidth-1 || kepala.y == 0 || kepala.y == gameHeight-1){
        lanjut = false ;
        return true ;
    }
    return false ;
}
void spawn_apel() {
    apel.x = 1 + rand() % (gameWidth - 2) ; 
    apel.y = 1 + rand() % (gameHeight - 2) ;
    while(cek1(apel, kepala)) {
        apel.x = 1 + rand() % (gameWidth - 2); 
        apel.y = 1 + rand() % (gameHeight - 2) ;
    }
}

void initSnake() {
    lanjut = true ;
    pBadan = 0 ;
    score = 0 ;
    snakeWin = newwin(gameHeight, gameWidth, startY, startX) ;
    box(snakeWin, 0, 0) ;
    refresh() ;
    wrefresh(snakeWin) ;
    keypad(snakeWin, true) ;
    nodelay(snakeWin, true) ;
    srand(time(NULL)) ;
    
    kepala.x = startX + 1, kepala.y = startY + 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;
    
    charWcolor(snakeWin, kepala.y, kepala.x, '>', 2) ;
    charWcolor(snakeWin, apel.y, apel.x, '@', 1) ;
    wrefresh(snakeWin) ;
}

void endGame() {
    curs_set(1) ;
    endwin() ;
}



void input() {
    int x = wgetch(snakeWin) ;
    
    switch (x)
    {
        case KEY_UP:
        if(gerak.y == 1) return ;
        gerak.x = 0 ;
        gerak.y = -1 ;
        break;
        
        case KEY_DOWN:
        if(gerak.y == -1) return ;
        gerak.x = 0 ;
        gerak.y = 1 ;
        break;
        
        case KEY_RIGHT:
        if(gerak.x == -1) return ;
        gerak.x = 1 ;
        gerak.y = 0 ;
        break;
        
        case KEY_LEFT:
        if(gerak.x == 1) return ;
        gerak.x = -1 ;
        gerak.y = 0 ;
        break;
        
        case '\e':
        endGame() ;
        exit(0) ;
        break;
        
        default:
        break;
    }
}

void update() {
    if(cek1 (apel, kepala)) {
        spawn_apel() ;
        score++ ;
    }
    
    if(cek_mati()){
        return ;
    }
    
    mvwaddch(snakeWin, kepala.y, kepala.x, ' ') ;
    if(pBadan > 0) mvwaddch(snakeWin, badan[pBadan-1].y, badan[pBadan-1].x, ' ') ;
    wrefresh(snakeWin) ;
    
    if(pBadan > 0) {
        for (int i=pBadan-1; i>0; i--) {
            badan [i] = badan [i-1] ;
        }
        badan[0] = kepala ;
    }
    if(score/1 > pBadan) {
        if(pBadan == 0) {
            badan [pBadan].y = kepala.y - gerak.y ;
            badan [pBadan].x = kepala.x - gerak.x ;
        }else {
            badan [pBadan].y = badan[pBadan-1].y - gerak.y ;
            badan [pBadan].x = badan[pBadan-1].x - gerak.x ;
        }
        pBadan++ ;
    }
    kepala.x += gerak.x ;
    kepala.y += gerak.y ;
}


void draw() {
    if(gerak.x == 1) {
        charWcolor(snakeWin, kepala.y, kepala.x, '>', 2) ;
    }else if (gerak.x == -1) {
        charWcolor(snakeWin, kepala.y, kepala.x, '<', 2) ;
    } else if (gerak.y == 1) {
        charWcolor(snakeWin, kepala.y, kepala.x, 'v', 2) ;
    } else {
        charWcolor(snakeWin, kepala.y, kepala.x, '^', 2) ;
    }
    
    for(int i=0; i<pBadan; i++) {
        printWcolor(snakeWin, badan[i].y, badan[i].x, "●", 2) ;
    }
    charWcolor(snakeWin, apel.y, apel.x, '@', 1) ;
    colorBox(snakeWin, 3) ;
    mvwprintw(snakeWin, 0, (gameWidth/2)-(5 + (score/10) ), " Score: %d ", score) ;
    wrefresh(snakeWin) ;
}