#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <time.h>
#include "snake.h"


#ifndef FOOD_H
#define FOOD_H


struct s_food;
struct s_snake;

struct s_food *food_init(void);

void set_food_position(struct s_food *food, int x, int y);

int get_food_pos_x(struct s_food *food);

int get_food_pos_y(struct s_food *food);

void make_food(struct s_food *food,
               struct s_snake *snake,
               int snake_head_x,
               int snake_head_y);

bool check_food_collision(struct s_food *food,
                          int snake_head_x,
                          int snake_head_y);

#endif // FOOD_H

#ifndef MAIN_H
#define MAIN_H

#define WORLD_WIDTH 50
#define WORLD_HEIGHT 20
#define START_SNAKE_LENGHT 3

void iniciar_aleatorio(void);
int aleatorio(int a,int b);

bool U, D, R, L;
bool COLLISION;
typedef struct {
    unsigned short int x;
    unsigned short int y;
} s_coord;

#endif // MAIN_H


#ifndef SCORES_H
#define SCORES_H

void set_new_score(int usr_score);

#endif // SCORES_H


#ifndef SNAKE_H
#define SNAKE_H

#define TAM_MAX WORLD_HEIGHT*WORLD_WIDTH

struct s_body;
struct s_snake;
unsigned short int snake_lenght, snake_scores;

void set_snake_speed(struct s_snake *snake, int new_speed);

int get_snake_speed(struct s_snake *snake);

void set_snake_lenght(struct s_snake *snake, int new_lenght);

int get_snake_lenght(struct s_snake *snake);

struct s_snake *snake_init(void);

int move_snake(struct s_snake *snake, struct s_food *food);

int game_loop();

#endif // SNAKE_H


#ifndef WINDOW_H
#define WINDOW_H

#define MENUMAX 3

WINDOW *snake_world;
WINDOW *scores_win;
WINDOW *game_over_win;

void draw_menu(int item);

void init_game_window();

void init_main_window();

void init_highscores_window();

void show_game_over_window();

#endif // WINDOW_H




int final_score = 0;

/*

    function

*/

/* snake */


struct s_body
{
    s_coord position;
};

/*
 * A "classe" snake
 */
struct s_snake
{
    unsigned short int speed;
    unsigned short int lenght, scores;
    s_coord position;
    struct s_body body[TAM_MAX];
};

void set_snake_speed(struct s_snake *snake, int new_speed){
    snake->speed = new_speed;
}

int get_snake_speed(struct s_snake *snake){
    return snake->speed;
}

void set_snake_lenght(struct s_snake *snake, int new_lenght){
    snake->lenght = new_lenght;
}

int get_snake_lenght(struct s_snake *snake){
    return snake->lenght;
}

/*
 *Função que inicializa a snake, tornando
 */
struct s_snake *snake_init(void)
{
    struct s_snake *return_snake = malloc (sizeof(struct s_snake));
    return_snake->lenght = START_SNAKE_LENGHT;
    return_snake->position.x = (WORLD_WIDTH - return_snake->lenght) / 2;
    return_snake->position.y = (WORLD_HEIGHT - 1) / 2;
    return return_snake;
}

int move_snake(struct s_snake *snake, struct s_food *food)
{
    int i;
    /*
     * Declaração de váriaveis x e y que recebem as cordenadas da cabeça da snake.
     */
    int x = snake->body[snake->lenght - 1].position.x;
    int y = snake->body[snake->lenght - 1].position.y;

    wclear(snake_world);
    make_food(food,snake, x, y);

    /*
     * Laço que faz cada elemento do vetor body, receber o valor
     * do proximo elemento, o que faz com que a snake se movimente.
     * E também verifica se a cabeça da snake (primeiro elemento) possui
     * as mesmas cordenadas do resto do seu corpo (colisão com ela mesma)
     * caso sim, acabar o jogo.
     */
    for (i = 0; i < (snake->lenght - 1); i++)
    {
        snake->body[i] = snake->body[i + 1];
        mvwaddch(snake_world, snake->body[i].position.y, snake->body[i].position.x, 'o');

    }

   /*
    * Verifica se a snake colidiu com a borda
    */
    if (U == 1)
        y - 1 == 0 ? y = WORLD_HEIGHT - 2 : y--;
    if(D == 1)
        y + 1 == WORLD_HEIGHT - 1 ? y = 1 : y++;
    if(R == 1)
        x + 1 == WORLD_WIDTH - 1 ? x = 1 : x++;
    if(L == 1)
        x - 1 == 0 ? x = WORLD_WIDTH - 2 : x--;


    make_food(food,snake, x, y);
    for (i = 0; i <= snake->lenght - 1; i++){
        if(x == snake->body[i].position.x && y == snake->body[i].position.y){
            delwin(snake_world);
            endwin();
            show_game_over_window();
            return -1;
        }
    }
    /*
     * A cabeça da snake recebe novas cordenadas, ou seja um ++.
     * Esta parta faz com que a snake se movimente.
     */
    snake->body[snake->lenght - 1].position.x = x;
    snake->body[snake ->lenght - 1].position.y = y;

    /*
     * Checa se o usuario colidiu com a comida, retorna true caso sim,
     * e falso caso nao, e armazena o valor na variavel COLLISION que sera
     * analisada dentro da funçao make_food.
     */
    COLLISION = check_food_collision(food, x, y);

    /*
     * Printa a cabeça da snake, com suas novas cordenadas
     */
    mvwaddch(snake_world, y, x, '@');

    /*
     * Desenha o quadrado ao redor da tela.
     */
    box(snake_world, 0 , 0);
    mvwprintw(snake_world, 1,WORLD_WIDTH / 2, "%d", snake->lenght);
    wrefresh(snake_world);

    return 0;
}

int game_loop(){
    int ch, i, food_x, food_y, nextX, nextY, flag = 1;
    struct s_snake *snake = snake_init();
    struct s_food *food = food_init();
    snake->lenght = 5;
    set_snake_speed(snake, 200);
    timeout(get_snake_speed(snake));

    /*set_food_position(food, rand() % WORLD_WIDTH, rand() % WORLD_HEIGHT);
    mvwaddch(snake_world, get_food_pos_y(food), get_food_pos_x(food), 'X')*/
    /*
     * O tamanho da snake recebe o valor da váriavel snake_lenght a qual será
     * modificado durante a execução do jogo.
     */
    snake_lenght = snake->lenght;
    snake_scores = 1;
    for (i = 0; i < snake->lenght; i++)
    {
        snake->body[i].position.x = snake->position.x + i;
        snake->body[i].position.y = snake->position.y;
    }

    /*
     * Aqui inicia o loop do jogo, que faz com que tudo funcione, este switch
     * e para ler se as setas foram apertadas.
     */
    while ((ch = getch()) != 'x')
    {

        flag = move_snake(snake, food);

        if(flag == -1) break;

        if(ch != ERR)
        {
            switch(ch)
            {
            case KEY_UP:
                if (D == 0)
                {
                    L = 0, D = 0, R = 0,
                    U = 1;
                }
                break;
            case KEY_DOWN:
                if (U == 0)
                {
                    L = 0, D = 1, R = 0,
                    U = 0;
                }
                break;
            case KEY_RIGHT:
                if (L == 0)
                {
                    L = 0, D = 0, R = 1,
                    U = 0;
                }
                break;
            case KEY_LEFT:
                if (R == 0)
                {
                    L = 1, D = 0, R = 0,
                    U = 0;
                }
                break;
            default:
                break;
            }
        }
    }

    final_score = snake->lenght;
    return snake->lenght;
}




/* food */

struct s_food
{
    s_coord position;
};

struct s_food *food_init(void)
{
    struct s_food *return_food = malloc(sizeof(struct s_food));
    return return_food;
}

void set_food_position(struct s_food *food, int x, int y){
    food->position.x = x;
    food->position.y = y;
}

int get_food_pos_x(struct s_food *food){
    return food->position.x;
}

int get_food_pos_y(struct s_food *food){
    return food->position.y;
}

/*
 * Funçao que printa um 'x' na tela para representar o food. Caso a cabeça
 * da snake tenha as mesmas cordenadas que o food, entao a snake recebe
 * mais um em sua variavel lenght, ou seja mais um em tamanho
 */
void make_food(struct s_food *food,
               struct s_snake *snake,
               int snake_head_x,
               int snake_head_y)
{
    if (COLLISION == true){
        iniciar_aleatorio();

        /*
         *Gera um numero aleatorio que esteja dentro da janela, estes numeros
         * serao respectivamente as cordenadas do food
         */
        food->position.x = aleatorio(((COLS - WORLD_WIDTH) / 2) + 2, WORLD_WIDTH - 2);
        food->position.y = aleatorio(((LINES - WORLD_HEIGHT) / 2) + 2, WORLD_HEIGHT - 2);
        mvwaddch(snake_world, food->position.y, food->position.x, ' ');
    }
    mvwaddch(snake_world, food->position.y, food->position.x, 'x');

    if ((snake_head_x == food->position.x) && (snake_head_y == food->position.y)){
        snake_lenght+=4;
        snake_scores+=4;
        set_snake_lenght(snake, snake_lenght);
    }
}

/*
 * Funçao booleana que retorna true se a snake comeu o food, e false caso nao
 */
bool check_food_collision(struct s_food *food,
                          int snake_head_x,
                          int snake_head_y)
{
    if ((snake_head_x == food->position.x) && (snake_head_y == food->position.y))
    {
        return true;
    }
    else
        return false;

}

/* score */
void set_new_score(int usr_score)
{
    FILE *scores;

    scores = fopen ("scores.txt", "a");

    if(!scores)
     {
        printf( "Erro na abertura do arquivo");
        exit(0);
     }
     fprintf(scores,"%d\n", usr_score);
     fclose(scores);
}



/* window */
/*
 * Função que desenha, e manipula o menu da primeira janela.
 */
void draw_menu(int item)
{
    int i;
    char main_menu[] = "Snake Game!";
    char menu[MENUMAX][21] = {
        "INICIAR JOGO",
        " ",
        "SAIR",
    };
    curs_set(0);
    clear();
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLUE);
    mvaddstr(1, WORLD_WIDTH / 2 + 5, main_menu);
    for (i = 0; i < MENUMAX; i++)
    {
        if (i == item)
            attron(A_REVERSE);
        mvaddstr(WORLD_HEIGHT / 3 +(i * 2),WORLD_WIDTH / 2 + 5, menu[i]);
        attroff(A_REVERSE);
        refresh();
    }
}

/*
 * Função que inicializa a janela do jogo.
 */
void init_game_window()
{   
    int offsetx, offsety;
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    refresh();
    curs_set(0);

    offsetx = (COLS - WORLD_WIDTH) / 2;
    offsety = (LINES - WORLD_HEIGHT) / 2;

    snake_world = newwin(WORLD_HEIGHT,
                         WORLD_WIDTH,
                         offsety,
                         offsetx);

    /*Entra no loop do jogo*/
    game_loop();

    delwin(snake_world);

    endwin();
}

/*
 * Funçao que inicializa a primeira janela.
 */
void init_main_window()
{
    int key, menu_item = 0;
    curs_set(0);
    initscr();
    draw_menu(menu_item);
    keypad(stdscr,TRUE);
    noecho();
    do
    {
        key = getch();
        switch(key)
        {
        case KEY_DOWN:
            menu_item+=2;
            if(menu_item > MENUMAX-1) menu_item = 0;
            break;
        case KEY_UP:
            menu_item-=2;
            if(menu_item < 0) menu_item = MENUMAX -1;
            break;
        case '\n':
            if (menu_item == MENUMAX - 1){
                delwin(stdscr);
                endwin();
                exit(0);
            }
            else if (menu_item == 0){
                endwin();
                clear();
                init_game_window();
            }
            else if(menu_item == MENUMAX - 2){
            }
            break;
        default:
            break;
        }
        draw_menu(menu_item);
    } while(key != 'x');

    echo();
    endwin();
}

void show_game_over_window()
{
    int ch;
    initscr();
    noecho();
    cbreak();
    game_over_win = newwin(0,0,0,0);
    endwin();
    printf("Game Over!\nPlease Wait other gamer\n");
    //mvwaddstr(game_over_win, 10, 25,"Game Over!\n \t\twating for other player!\n\t");
    //move(0,0);
    wrefresh(game_over_win);
    endwin();
    getch();
    
    //exit(0);

    //wrefresh(game_over_win);
}




/* main */

void iniciar_aleatorio(void) {
    srand((unsigned int)time(NULL));
    rand();  rand();  rand(); rand();
}
/*
 * Funçao que gera um numero aleatorio, em um determinado intervalo
 */
int aleatorio(int a,int b){
    double r;
    r=(double)rand()/RAND_MAX;
    return (int)(a+(r*(b-a)));
}

int start (void){
    R = 1;
    COLLISION = true;
    //init_main_window();
    init_game_window();

    return final_score;
}
