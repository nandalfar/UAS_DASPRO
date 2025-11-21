#include <ncurses.h>
#include <stdlib.h>
#include <time.h>


typedef struct {
    int x ;
    int y ;
} koordinat ;

void init() ;
bool cek1(koordinat a, koordinat b) ;
void spawn_apel() ;
void input() ;
void update() ;
void draw() ;

int MaxHeight = 25, MaxWidth = 70 ;
int score = 0 ;
int pBadan = 0 ;
koordinat kepala, apel, gerak, badan[1000] ;
bool lanjut = true ;
WINDOW * win ;

int main() {
    initscr() ;
    cbreak() ;
    noecho() ;
    
    curs_set(0) ;

    init() ;
    while(lanjut) {
        napms(120) ;
        input() ;
        update() ;
        draw() ;
        napms(50) ;
        input() ;
        update() ;
        draw() ;
    }

    curs_set(1) ;
    getch() ;
    endwin();
    return 0;
}

void init() {
    // cek ukuran terminal user
    int heightT, widthT ;
    getmaxyx(stdscr, heightT, widthT) ;
    
    // buat window untuk game snake
    if(widthT <= MaxWidth && heightT <= MaxHeight) {
        MaxHeight = heightT - 2 ;
        MaxWidth = widthT - 2 ; 
    } else if (widthT <= MaxWidth) {
        MaxWidth = widthT - 2 ; 
    } else if (heightT <= MaxHeight) {
        MaxHeight = heightT - 2 ;
    }
    
    win = newwin(MaxHeight, MaxWidth, 0, 0) ;
    
    box(win, 0, 0) ;
    refresh() ;
    wrefresh(win) ;
    keypad(win, true) ;
    nodelay(win, true) ;
    srand(time(NULL)) ;

    kepala.x = kepala.y = 1 ;
    gerak.x = 1 ; gerak.y = 0 ;
    spawn_apel() ;

    mvwaddch(win, kepala.y, kepala.x, '>') ;
    mvwaddch(win, apel.y, apel.x, '@') ;
    wrefresh(win) ;
}

bool cek1 (koordinat a, koordinat b) {
    if(a.x != b.x) return false ;
    if(a.y != b.y) return false ;

    return true ;
}

void spawn_apel() {
    apel.x = 1 + rand() % (MaxWidth-2) ; 
    apel.y = 1 + rand() % (MaxHeight-2) ;
    while(cek1(apel, kepala)) {
        apel.x = 1 + rand() % (MaxWidth-2); 
        apel.y = 1 + rand() % (MaxHeight-2) ;
    }
}

void input() {
    int x = wgetch(win) ;

    switch (x)
    {
    case KEY_UP:
        if(gerak.y == 1 || kepala.x >= MaxWidth || kepala.x <= 0) return ;
        gerak.x = 0 ;
        gerak.y = -1 ;
        break;
        
    case KEY_DOWN:
        if(gerak.y == -1 || kepala.x >= MaxWidth || kepala.x <= 0) return ;
        gerak.x = 0 ;
        gerak.y = 1 ;
        break;
        
    case KEY_RIGHT:
        if(gerak.x == -1 || kepala.y >= MaxHeight || kepala.y <= 0) return ;
        gerak.x = 1 ;
        gerak.y = 0 ;
        break;
        
    case KEY_LEFT:
        if(gerak.x == 1 || kepala.y >= MaxHeight || kepala.y <= 0) return ;
        gerak.x = -1 ;
        gerak.y = 0 ;
        break;

    case '\e':
        lanjut = false ;
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

    mvwaddch(win, kepala.y, kepala.x, ' ') ;
    if(pBadan > 0) mvwaddch(win, badan[pBadan-1].y, badan[pBadan-1].x, ' ') ;
    wrefresh(win) ;
    
    if(pBadan > 0) {
        for (int i=pBadan-1; i>0; i--) {
            badan [i] = badan [i-1] ;
        }
        badan[0] = kepala ;
    }
    if(score/3 > pBadan) {
        if(pBadan == 0) {
            badan [pBadan].y = kepala.y - gerak.y ;
            badan [pBadan].x = kepala.x - gerak.x ;
        }else {
            badan [pBadan].y = badan[pBadan-1].y - gerak.y ;
            badan [pBadan].x = badan[pBadan-1].x - gerak.x ;
        }
        pBadan++ ;
    }
    kepala.x += gerak.x ; if(kepala.x == 0) kepala.x = MaxWidth - 2 ; if(kepala.x == MaxWidth-1) kepala.x = 1 ;
    kepala.y += gerak.y ; if(kepala.y == 0) kepala.y = MaxHeight - 2 ; if(kepala.y == MaxHeight-1) kepala.y = 1 ;
}

void draw() {
    if(gerak.x == 1) {
        mvwaddch(win, kepala.y, kepala.x, '>') ;
    }else if (gerak.x == -1) {
        mvwaddch(win, kepala.y, kepala.x, '<') ;
    } else if (gerak.y == 1) {
        mvwaddch(win, kepala.y, kepala.x, 'v') ;
    } else {
        mvwaddch(win, kepala.y, kepala.x, '^') ;
    }
    
    for(int i=0; i<pBadan; i++) {
        mvwaddch(win, badan[i].y, badan[i].x, 'O') ;
    }
    mvwaddch(win, apel.y, apel.x, '@') ;
    box(win, 0, 0) ;
    mvwprintw(win, 0, (MaxWidth/2)-(5 + (score/10) ), " Score: %d ", score) ;
    wrefresh(win) ;
}