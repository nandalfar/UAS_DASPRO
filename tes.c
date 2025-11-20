#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int x ;
    int y ;
} koordinat ;

const int MaxHeight = 20, MaxWidth = 60 ;

int main() {
    initscr() ;
    cbreak() ;
    noecho() ;

    // cek ukuran terminal user
    int heightT, widthT ;
    getmaxyx(stdscr, heightT, widthT) ;

    WINDOW * win ;
    if(widthT <= MaxWidth && heightT <= MaxHeight) {
        win = newwin(heightT, widthT, 0, 0) ;
    } else if(widthT <= MaxWidth) {
        win = newwin(MaxHeight, widthT, 0, 0) ;
    } else {
        win = newwin(MaxHeight, MaxWidth, 0, 0) ;
    }

    box(win, 0, 0) ;
    refresh() ;
    wrefresh(win) ;
    keypad(win, true) ;
    nodelay(win, true);
    curs_set(0) ; 
    srand(time(NULL)) ;

    koordinat kepala ;
    koordinat gerak ;
    koordinat apel ;
    int score = 0 ;
    bool input = false ;

    kepala.x = kepala.y = 1 ;
    gerak.x = 2 ;
    gerak.y = 0 ;
    apel.x = rand() % 60; apel.x += (apel.x == 0) ;
    apel.y = rand() % 20; apel.y += (apel.y == 0) ;
    
    mvwaddch(win, kepala.y, kepala.x, 'O') ;
    mvwaddch(win, apel.y, apel.x, '@') ;
    mvwprintw(win, 0, 25, " Score: %d ", score) ;
    do{
        int arrow = wgetch(win) ;
        
        // update arah jika ada input
        switch (arrow)
        {
            case KEY_UP:
            if(gerak.y == 1 || kepala.x == MaxWidth || kepala.x == 0) continue ;
            gerak.x = 0 ;
            gerak.y = -1 ;
            input = true ;
            break;
            
            case KEY_DOWN:
            if(gerak.y == -1 || kepala.x == MaxWidth || kepala.x == 0) continue ;
            gerak.x = 0 ;
            gerak.y = 1 ;
            input = true ;
            break;
            
            case KEY_RIGHT:
            if(gerak.x == -1 || kepala.y == MaxHeight || kepala.y == 0) continue ;
            gerak.x = 1 ;
            gerak.y = 0 ;
            input = true ;
            input = true ;
            break;
            
            case KEY_LEFT:
            if(gerak.x == 2 || kepala.y == MaxHeight || kepala.y == 0) continue ;
            gerak.x = -1 ;
            gerak.y = 0 ;
            input = true ;
            break;
            
            default:
            break; // jika tidak ada input, tetap gerak arah sebelumnya
        }
        
        if(arrow == '\e'){
            break ;
        }
        
        // clear posisi lama
        mvwaddch(win, kepala.y, kepala.x, ' ') ;
        
        // ular makan apel
        if((kepala.x == apel.x && kepala.y == apel.y) || (kepala.x-1 == apel.x && kepala.y-1 == apel.y)){
            apel.x = rand() % 60; apel.x += (apel.x==0) ;
            apel.y = rand() % 20; apel.y += (apel.y==0) ;
            score++ ;
            
            mvwaddch(win, apel.y, apel.x, '@') ;
            wrefresh(win) ;
        }
        
        // gerakkan ular
        kepala.x += gerak.x ; if(kepala.x >= MaxWidth) kepala.x = 1 ; if(kepala.x <= 0) kepala.x = MaxWidth-1 ;
        kepala.y += gerak.y ; if(kepala.y == MaxHeight) kepala.y = 1 ; if(kepala.y == 0) kepala.y = MaxHeight-1 ;
        
        // gambar posisi baru
        mvwaddch(win, kepala.y, kepala.x, 'O') ;
        wrefresh(win) ;
        
        // pastikan box tetap aman
        box(win, 0, 0) ;
        mvwprintw(win, 0, 25, " Score: %d ", score) ;
        wrefresh(win) ;

        // delay biar tidak terlalu cepat
        napms(120);

    } while(true);

    curs_set(1) ;
    getch();
    endwin();
    return 0;
}
     

/*
int main() {
    initscr() ;
    noecho() ;
    cbreak() ;

    int y, x ;

    getmaxyx(stdscr, y, x) ;

    printw("%d %d", y, x) ;

    getch() ;
    endwin() ;
}
*/

/*
int main() {
    initscr() ;

    int height, width, start_y, start_x ;
    height = 20 ;
    width = 20 ;
    start_y = start_x = 0 ;

    WINDOW * win = newwin(10, 10, 0, 0) ;
    WINDOW * apa = newwin(height, width, start_y, width+1) ;
    refresh();
    char a = '#' ;
    box(win, (int)a, (int)a) ;
    box(apa, 0,0) ;
    wrefresh(win) ;
    wrefresh(apa) ;
    int c = getch() ;
    mvwprintw(win, 0,5,"box") ;
    wrefresh(win) ;
    getch() ;
    endwin() ;

    return 0 ;
    } 
*/
