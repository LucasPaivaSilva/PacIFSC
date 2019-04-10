#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "objects.h"
#include <stdio.h>

//Variáveis Globais--------------------------------------------------------------------------------------------------------------------------------------------//
const int WIDTH = 1024;
const int HEIGHT = 768;
int pos_x = 32;
int pos_y = 32;
enum KEYS {UP, DOWN, LEFT, RIGHT, SPACE, ESCAPE, ENTER};
enum STATE {MENU, PLAYING, GAMEOVER};

//Variáveis para Sprite----------------------------------------------------------------------------------------------------------------------------------------//
const int maxFrame = 3;
int curFrame = 0;
int frameCount = 0;
int frameDelay = 5;
int frameWidth = 26;
int frameHeight = 28;
int sprite = 0;


//Matriz contendo o mapa do jogo, cada número representa o estado de cada "bloco"
uint64_t Map[24][22] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,3,0,
    0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,
    0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,
    0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,
    0,1,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,1,0,
    0,1,0,1,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,0,1,0,
    0,1,1,1,0,0,1,0,0,0,5,5,0,0,0,1,0,0,1,1,1,0,
    0,0,0,1,0,0,1,1,1,1,5,5,1,1,1,1,0,0,1,0,0,0,
    6,1,1,1,1,1,1,1,0,1,5,5,1,0,1,1,1,1,1,1,1,4,
    0,0,0,1,0,0,1,1,1,1,5,5,1,1,1,1,0,0,1,0,0,0,
    0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,
    0,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,
    0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,
    0,1,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,
    0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,
    0,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,0,
    0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,1,0,
    0,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,3,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
//Matriz de posicionamento, posteriormente utilizada para as funçoes relacionadas com posicionamento
uint64_t Position[22][24];


//Protótipos de funçoes-------------------------------------------------------------------------------------------------------------------------------------------//
//Desenha o mapa na tela
void DrawMap(ALLEGRO_BITMAP *parede, ALLEGRO_BITMAP *comida, ALLEGRO_BITMAP *cafe, ALLEGRO_BITMAP *seta, ALLEGRO_BITMAP *seta2);

//Inicia o mapa de posicionamento
void InitPoseMap();

//Funções do Player(Pacman)
void InitPacman (Pacman &Player1); // Inicia o pacman e preenche os valores das variáveis do struct do mesmo
void MovePacman (bool keys[], Pacman &Player1);// Move o pacman de acordo com o input do teclado
void DrawPacman(Pacman &Player1, ALLEGRO_BITMAP *pacmano, int curFrame, int frameWidth, int frameHeight);//Desenha o pacman com o respectivo sprite
void CheckScore(Pacman &Player1, Enemy Ghosts[]);//Checa a score do pacman


//Funçoes dos inimigos(Ghost)
void InitGhosts (Enemy Ghosts[]);//Incia os fantamsmas e preenche os valores das variáveis do struct dos mesmos
void DrawGhosts (Enemy Ghosts[],ALLEGRO_BITMAP *enemy1, ALLEGRO_BITMAP *enemy2, ALLEGRO_BITMAP *enemy3, ALLEGRO_BITMAP *enemy4, int timer);//Desenha os fantamas
void MoveGhosts (Enemy Ghosts[], Pacman &Player1);//Move os fantamas e toma as decisoes baseado na inteligencia dos mesmos
void CheckIfVulnerable(Pacman &Player1, Enemy Ghosts[]);//Checa se o player comeu o cafe, caso sim, deixa os inimigos vulnerávies
void MakeInvulnerable(Enemy Ghosts[]);//Deixa os inimigos Invulneráveis novamente
void ResetGhostPosition(Enemy Ghosts[], int x);//Reseta a posição dos fantasmas para a original

//Funçoes do jogo
void SwitchLevel(Pacman &Player1, Enemy Ghosts[]);//Troca para o próximo nivel e faz as alteraçoes necessárias
void RestartGame(Pacman &Player1);// Reinicia o jogo
int CheckColision(Pacman &Player1, Enemy Ghosts[], ALLEGRO_SAMPLE *pacman_eatghost);//Checa a colisão(mais explicado na própria função)







int main(void)
{
    //Variáveis Primitivas
    const int FPS = 60;//FPS do jogo
    int GameIsOver = 0;//Indica se o jogo foi perdido
    int FirstTime = 0;//Váriavel criada de modo a rodar os incializadores apneas uma vez
    int finalScore = 0;//Socre final, a soma das comidas + fantasmas(comidos)
    bool done = false;
    bool redraw = true;
    int timer2 = 0;//Timer1
    int timer3 = 0;//Timer2
    int timer4 = 0;//Timer3
    bool keys[7] = {false, false, false, false, false, false, false};
    int state = MENU;//Estado inicial, Menu

    int x = WIDTH/2;
    int y = HEIGHT/2;

    //Variáveis de objeto
    Pacman Player1;//Cria o Player1 por meio do struct Pacman
    Enemy Ghosts[4];//Cria os fantasmas por meio do struct Enemy

    //Variáveis do Allegro
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_FONT *fonteG = NULL;
    ALLEGRO_FONT *fonteGG = NULL;
    ALLEGRO_BITMAP *parede = NULL;
    ALLEGRO_BITMAP *seta = NULL;
    ALLEGRO_BITMAP *seta2 = NULL;
    ALLEGRO_BITMAP *seta3 = NULL;
    ALLEGRO_BITMAP *seta4 = NULL;
    ALLEGRO_BITMAP *comida = NULL;
    ALLEGRO_BITMAP *enemy1 = NULL;
    ALLEGRO_BITMAP *enemy2 = NULL;
    ALLEGRO_BITMAP *enemy3 = NULL;
    ALLEGRO_BITMAP *enemy4 = NULL;
    ALLEGRO_BITMAP *cafe = NULL;

    //Variáveis do Allegro Relacionadas a Audio
    ALLEGRO_BITMAP *pacmanfinal = NULL;
    ALLEGRO_SAMPLE *pacman_death = NULL;
    ALLEGRO_SAMPLE *pacman_eatghost = NULL;
    ALLEGRO_SAMPLE *pacman_beginning = NULL;
    ALLEGRO_SAMPLE *pacman_theme = NULL;
    ALLEGRO_SAMPLE_INSTANCE *pacman_theme_instance = NULL;



    //Funçoes de incialização
    if(!al_init())
    {
        return -1;
        printf("Erro ao inciar o allegro!\n");
    }

    if(!al_install_audio())
    {
        return -1;
        printf("Erro ao iniciar os sistemas de Audio!\n");
    }

    if(!al_init_acodec_addon())
    {
        return -1;
        printf("Erro ao iniciar os sistemas de codec de Audio!\n");
    }

    if (!al_reserve_samples(1))
    {
        return -1;
        printf("Erro ao reservar as samples de Audio!\n");
    }

    display = al_create_display(WIDTH, HEIGHT);

    if(!display)
    {
        return -1;
        printf("Erro ao criar o display!\n");
    }

    if(!al_init_primitives_addon())
    {
        return -1;
        printf("Erro inciar os addons primitivos!\n");
    }

    if(!al_install_keyboard())
    {
        return -1;
        printf("Erro instalar/iniciar o teclado!\n");
    }

    if(!al_init_image_addon())
    {
        return -1;
        printf("Erro iniciar o addon de imagem!\n");
    }

    al_init_font_addon();
    al_init_ttf_addon();

    //Carrega os arquivos utilizados
    parede = al_load_bitmap("Misk/parede.bmp");
    comida = al_load_bitmap("Misk/Comida.bmp");
    enemy1 = al_load_bitmap("enemyBMP/enemy1.png");
    enemy2 = al_load_bitmap("enemyBMP/enemy2.png");
    enemy3 = al_load_bitmap("enemyBMP/enemy3.png");
    enemy4 = al_load_bitmap("enemyBMP/enemy4.png");
    pacmanfinal = al_load_bitmap("Misk/pacman2.png");
    seta = al_load_bitmap("Misk/seta.png");
    seta2 = al_load_bitmap("Misk/seta2.png");
    seta3 = al_load_bitmap("Misk/seta3.png");
    seta4 = al_load_bitmap("Misk/seta4.png");
    cafe = al_load_bitmap("Misk/cafe.png");
    fonte = al_load_font("Fonts/8bits.ttf", 30, 0);
    fonteG = al_load_font("Fonts/8bits.ttf", 70, 0);
    fonteGG = al_load_font("Fonts/8bits.ttf", 110, 0);

    //Converte e faz as masks para os arquivos necessários
    al_convert_mask_to_alpha(comida, al_map_rgb(255,0,255));
    al_convert_mask_to_alpha(enemy1, al_map_rgb(255,0,255));
    al_convert_mask_to_alpha(enemy2, al_map_rgb(255,0,255));
    al_convert_mask_to_alpha(enemy3, al_map_rgb(255,255,255));
    al_convert_mask_to_alpha(enemy4, al_map_rgb(255,0,255));

    //Parte de carregamento e configuração de audio
    al_reserve_samples(10);//Reserva 10 samples, mais do que o suficiente para o que vai ser usado
    pacman_death = al_load_sample("Audio/pacman_death.wav");//Carrega o som do pacman morrendo
    pacman_eatghost = al_load_sample("Audio/pacman_eatghost.wav");//carrega o som do pacmna comendo um fantamsma
    pacman_beginning = al_load_sample("Audio/pacman_beginning.wav");//carrega o som do jogo iniciando
    pacman_theme = al_load_sample("Audio/pacman_theme.wav");//carrega o tema do pacman
    pacman_theme_instance = al_create_sample_instance(pacman_theme);//cria uma sample instance e coloca o tema dentro
    al_set_sample_instance_playmode(pacman_theme_instance, ALLEGRO_PLAYMODE_LOOP);//configura o playmode da sample instance, nesse caso loop
    al_attach_sample_instance_to_mixer(pacman_theme_instance, al_get_default_mixer());//da o "atach" da sample_instance ao mixer


    event_queue = al_create_event_queue();//cria o event queue
    timer = al_create_timer(1.0 / FPS);//cria o timer baseado no FPS


    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    al_start_timer(timer);//Inicia o timer
    InitPacman(Player1);//Inicia o player1
    InitGhosts(Ghosts);//Inicia os fantamas
    InitPoseMap();//Incia o mapa de posicionamento
    al_play_sample(pacman_beginning,1,0,1,ALLEGRO_PLAYMODE_ONCE, NULL);//Toca o som do inicio do pacman


    while(!done)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_ESCAPE:
                keys[ESCAPE] = true;
                break;
            case ALLEGRO_KEY_UP:
                if (Position[Player1.Line][Player1.Column-1] == 0)
                {
                    break;
                }
                keys[UP] = true;
                break;
            case ALLEGRO_KEY_DOWN:
                if (Position[Player1.Line][Player1.Column+1] == 0)
                {
                    break;
                }
                keys[DOWN] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                if (Position[Player1.Line+1][Player1.Column] == 0)
                {
                    break;
                }
                keys[RIGHT] = true;
                break;
            case ALLEGRO_KEY_LEFT:
                if (Position[Player1.Line-1][Player1.Column] == 0)
                {
                    break;
                }
                keys[LEFT] = true;
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = true;
                break;
            case ALLEGRO_KEY_ENTER:
                keys[ENTER] = true;
                break;

            }
        }
        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                break;
            case ALLEGRO_KEY_DOWN:
                break;
            case ALLEGRO_KEY_RIGHT:
                break;
            case ALLEGRO_KEY_LEFT:
                break;
            case ALLEGRO_KEY_SPACE:
                keys[SPACE] = false;
                break;
            case ALLEGRO_KEY_ESCAPE:
                keys[ESCAPE] = false;
                break;
            case ALLEGRO_KEY_ENTER:
                keys[ENTER] = false;
                break;

            }
        }

        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            done = true;
        }

        else if (ev.type == ALLEGRO_EVENT_TIMER)
        {
            redraw=true;
            MovePacman(keys, Player1);//Move o pacman
            timer2++;//Adiciona ao timer
            if ((timer2%Ghosts[0].Velocity)==0)//Velocidade dos fantamas é controlada por aqui, quanto menos o valor do .Velocity, maior a velocidade dos mesmos
            {
                //Movimentação dos fantasmas é definida aqui
                MoveGhosts(Ghosts, Player1);
            }
            if (Ghosts[0].Vulnerable || Ghosts[1].Vulnerable || Ghosts[2].Vulnerable || Ghosts[3].Vulnerable)//Chega se algum dos fantamas esta Vulnerável
            {
                timer3++;//Adiciona ao timer 3, que é o timer do tempo que os fantamas ficam vulneráveis
                if ((timer3%400==0))//checa o tempo passado
                {
                    MakeInvulnerable(Ghosts);//deixa os fantamas invulneráveis novamente
                }
            }
            if (state == MENU)
            {
                if(keys[ENTER])
                    state = PLAYING;
                if(keys[ESCAPE])
                    done = true;
            }
            else if (state == PLAYING)
            {
                if(FirstTime == 0)//Roda apenas ao entrar no mapa pela primeira vez(de cada jogo)
                {
                    InitPacman(Player1);//Inicia o pacman
                    InitGhosts(Ghosts); //Inicia os fantasmas
                    al_play_sample_instance(pacman_theme_instance);//Começa a tocar o tema
                    FirstTime = 1;//Joga firstTime para 1, de modo a não entrar no If novamente
                }

                if(keys[ESCAPE])
                    state = GAMEOVER;
            }
            else if (state == GAMEOVER)
            {
                if(keys[ENTER])
                    done = true;
                else if (keys[SPACE])
                {
                    state = MENU;
                }
            }
            //Realiza a contagem dos frames do sprite, referente a linha selecionada
            if (++frameCount >= frameDelay) {
                if (++curFrame >= maxFrame)
                    curFrame = 0;

                frameCount = 0;
            }

            x-= 5;

            if (x <= 0 - frameWidth)
                x = WIDTH;
        }



        if(redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;


            if (state == MENU)
            {
                al_draw_text(fonteG, al_map_rgb(0,0,255), 190, 50, ALLEGRO_ALIGN_CENTER, "COMO JOGAR:");
                al_draw_bitmap(seta, 120, 160, 0);
                al_draw_bitmap(seta2, 40, 160, 0);
                al_draw_bitmap(seta3, 80, 120, 0);
                al_draw_bitmap(seta4, 80, 160, 0);
                al_draw_text(fonte, al_map_rgb(255,255,255), 180, 120, 0, "MOVIMENTAM");
                al_draw_text(fonte, al_map_rgb(255,255,255), 180, 145, 0, "O PERSONAGEM");
                al_draw_text(fonte, al_map_rgb(255,255,255), 180, 170, 0, "PARA TODOS OS LADOS");
                al_draw_text(fonte, al_map_rgb(255,40,40), 240, 220, ALLEGRO_ALIGN_CENTER, "PRESSIONE ESC A QUALQUER MOMENTO");
                al_draw_text(fonte, al_map_rgb(255,40,40), 240, 245, ALLEGRO_ALIGN_CENTER, "PARA SAIR DO JOGO");
                al_draw_text(fonte, al_map_rgb(255,255,0), 240, 280, ALLEGRO_ALIGN_CENTER, "APERTE ENTER PARA SER");
                al_draw_text(fonte, al_map_rgb(255,255,0), 240, 305, ALLEGRO_ALIGN_CENTER, "REDIRECIONADO A PARTIDA");
                al_draw_text(fonte, al_map_rgb(0,255,0), 240, 340, ALLEGRO_ALIGN_CENTER, "BOA SORTE!");
                al_draw_text(fonteG, al_map_rgb(0,0,255), 280, 400, ALLEGRO_ALIGN_CENTER, "DESENVOLVIDO POR:");
                al_draw_text(fonte, al_map_rgb(255,255,255), 40, 460, 0, "EDUARDO BORGES SIQUEIRA");
                al_draw_text(fonte, al_map_rgb(255,255,255), 40, 490, 0, "LUCAS PAIVA DA SILVA");
                al_draw_text(fonteG, al_map_rgb(0,0,255), 250, 550, ALLEGRO_ALIGN_CENTER, "COM O AUXILIO DE:");
                al_draw_text(fonte, al_map_rgb(255,255,255), 40, 610, 0, "PROFESSOR FERNANDO SANTANA PACHECO");
                al_draw_text(fonte, al_map_rgb(255,255,255), 40, 640, 0, "MIKE GEIG (FixByProximity)");
                al_draw_text(fonte, al_map_rgb(255,255,255), 40, 670, 0, "TURMA 722 (2018/1)");
                al_draw_text(fonteG, al_map_rgb(0,0,255), 780, 50, ALLEGRO_ALIGN_CENTER, "INIMIGOS:");
                al_draw_bitmap(enemy1, 650, 160, 0);
                al_draw_bitmap(enemy2, 650, 240, 0);
                al_draw_bitmap(enemy3, 650, 320, 0);
                al_draw_bitmap(enemy4, 650, 400, 0);
                al_draw_text(fonte, al_map_rgb(255,255,255), 700, 160, 0, "RELATORIO DE PI");
                al_draw_text(fonte, al_map_rgb(255,255,255), 700, 240, 0, "PACMAN IRRITADO");
                al_draw_text(fonte, al_map_rgb(255,255,255), 700, 320, 0, "IFSC");
                al_draw_text(fonte, al_map_rgb(255,255,255), 700, 400, 0, "ELETRONICA");
                al_draw_text(fonteG, al_map_rgb(0,0,255), 760, 500, ALLEGRO_ALIGN_CENTER, "HISTORIA:");
                al_draw_text(fonte, al_map_rgb(255,255,255), 630, 570, 0, "COMO UM BOM ESTUDANTE, ");
                al_draw_text(fonte, al_map_rgb(255,255,255), 630, 600, 0, "VOCE DEVERA USAR O CAFE");
                al_draw_text(fonte, al_map_rgb(255,255,255), 630, 630, 0, "PARA ACABAR COM SEUS MAIORES");
                al_draw_text(fonte, al_map_rgb(255,255,255), 630, 660, 0, "INIMIGOS, AS TAREFAS!");
            }
            else if (state == PLAYING)
            {
                if(GameIsOver == 0)
                {
                    GameIsOver = CheckColision(Player1, Ghosts, pacman_eatghost);
                    CheckIfVulnerable(Player1, Ghosts);
                    CheckScore(Player1, Ghosts);
                    DrawMap(parede, comida, cafe, seta, seta2);
                    DrawPacman(Player1, pacmanfinal, curFrame, frameWidth, frameHeight);
                    DrawGhosts(Ghosts, enemy1, enemy2, enemy3, enemy4, timer2);
                    al_draw_textf(fonteG, al_map_rgb(255,255,255), 760, 5, 0, "PONTOS");
                    al_draw_textf(fonteGG, al_map_rgb(255,255,255), 800, 150, 0, "%d", Player1.Score+Player1.GhostScore);

                }
                else
                {
                    al_play_sample(pacman_death,1,0,1,ALLEGRO_PLAYMODE_ONCE, NULL);
                    al_stop_sample_instance(pacman_theme_instance);
                    finalScore = Player1.Score + Player1.GhostScore;
                    FirstTime = 0;
                    state = GAMEOVER;
                }

            }
            else if (state == GAMEOVER)
            {
                InitPacman(Player1);
                InitGhosts(Ghosts);
                InitPoseMap();
                GameIsOver = 0;
                al_draw_text(fonteGG, al_map_rgb(255,0,0), WIDTH / 2, 200, ALLEGRO_ALIGN_CENTER, "DERROTA");
                al_draw_text(fonteG, al_map_rgb(255,255,255), WIDTH / 2, 300, ALLEGRO_ALIGN_CENTER, "pressione ENTER para sair");
                al_draw_text(fonteG, al_map_rgb(255,255,255), WIDTH / 2, 350, ALLEGRO_ALIGN_CENTER, "ou");
                al_draw_text(fonteG, al_map_rgb(255,255,255), WIDTH / 2, 400, ALLEGRO_ALIGN_CENTER, "pressione SPACE para ir ao menu");
                al_draw_textf(fonteG, al_map_rgb(0,0,255), WIDTH / 2, 600, ALLEGRO_ALIGN_CENTER,"high score: %d", finalScore);
            }

            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }
    }
    al_destroy_sample_instance(pacman_theme_instance);
    al_destroy_sample(pacman_theme);
    al_destroy_sample(pacman_beginning);
    al_destroy_sample(pacman_death);
    al_destroy_sample(pacman_eatghost);
    al_uninstall_audio();
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_display(display);

    return 0;
}

//-----------------------------------------------------------------------------------------------------------------------//

//Essa Função desenha o mapa
void DrawMap(ALLEGRO_BITMAP *parede, ALLEGRO_BITMAP *comida, ALLEGRO_BITMAP *cafe, ALLEGRO_BITMAP *seta, ALLEGRO_BITMAP *seta2)
{
    //Cria as varáveis de Linha e Coluna
    int MapLine, MapColumn;
    int x = 0, y = 0;

    for (MapLine=0; MapLine<22; MapLine++)
    {
        for (MapColumn=0; MapColumn<24; MapColumn++)
        {
            //Desenha a parede
            if (Position[MapLine][MapColumn] == 0)
                al_draw_bitmap(parede, x, y, 0);
            //Desenha a comida
            if (Position[MapLine][MapColumn] == 1)
                al_draw_bitmap(comida, x, y, 0);
            //Desenha os cafes
            if (Position[MapLine][MapColumn] == 3)
                al_draw_bitmap(cafe, x, y, 0);
            //Desenha uma das setas(das laterais)
            if (Position[MapLine][MapColumn] == 4)
                al_draw_bitmap(seta, x, y, 0);
            //Deseha a outra seta
            if (Position[MapLine][MapColumn] == 6)
                al_draw_bitmap(seta2, x, y, 0);
            y += 32;
        }
        x += 32;
        y = 0;
    }
}

//Essa função inicia o pacman
void InitPacman (Pacman &Player1)
{
    //Inicia o player na linha 1 e na coluna 1([1,1])
    Player1.Column = 1;
    Player1.Line = 1;
    //Incia a Score em 0
    Player1.Score = 0;
    //Inicia a GhostScore(Fantasmas comidos) em 0
    Player1.GhostScore = 0;
}

//Essa função Move o pacman
void MovePacman (bool keys[], Pacman &Player1)
{

    bool movimento = false;

    //Checa o que tipo de bloco está ao redor do pacman
    if (Position[Player1.Line][Player1.Column-1] != 0 && Position[Player1.Line][Player1.Column-1] != 5 && movimento == false)
    {
        //checa se a tecla esta para cima
        if (keys[UP] == 1)
        {
            //move o pacman e atualiza o sprite
            sprite = 0;
            Player1.Column--;
            movimento = true;
            keys[UP] = 0;
        }
    }

    //Checa o que tipo de bloco está ao redor do pacman
    if (Position[Player1.Line][Player1.Column+1] != 0 && Position[Player1.Line][Player1.Column+1] != 5 && movimento == false)
    {
        //checa se a tecla esta para baixo
        if (keys[DOWN] == 1)
        {
            //move o pacman e atualiza o sprite
            sprite = 2;
            Player1.Column++;
            movimento = true;
            keys[DOWN] = 0;
        }
    }

    //Checa o que tipo de bloco está ao redor do pacman
    if (Position[Player1.Line-1][Player1.Column] != 0 && Position[Player1.Line-1][Player1.Column] != 5 && movimento == false)
    {
        //checa se a tecla esta para esquerda
        if (keys[LEFT] == 1)
        {
            //move o pacman e atualiza o sprite
            sprite = 1;
            Player1.Line--;
            movimento = true;
            keys[LEFT] = 0;
        }
    }

    //Checa o que tipo de bloco está ao redor do pacman
    if (Position[Player1.Line+1][Player1.Column] != 0 && Position[Player1.Line+1][Player1.Column] != 5 && movimento == false)
    {
        //checa se a tecla esta para direita
        if (keys[RIGHT] == 1)
        {
            //move o pacman e atualiza o sprite
            sprite = 3;
            Player1.Line++;
            movimento = true;
            keys[RIGHT] = 0;
        }
    }
    //Chega se o player esta nos corredores laterais, caso sim, troca a posição do pacman para o outro lado
    if (Player1.Line == 0 && Player1.Column == 11)
    {
        Player1.Line = 20;
        Player1.Column = 11;
    }
    if (Player1.Line == 21 && Player1.Column == 11)
    {
        Player1.Line = 1;
        Player1.Column = 11;
    }

}

//Essa função checa a score do pacman, aumenta a mesma e caso necessario, troca o nivel
void CheckScore(Pacman &Player1, Enemy Ghosts[])
{
    //chega se tem comida ou cafe na posição
    if (Position[Player1.Line][Player1.Column] == 1 || Position[Player1.Line][Player1.Column] == 3)
    {
        //troca para o quadrado preto
        Position[Player1.Line][Player1.Column] = 2;
        //aumenta a score
        Player1.Score++;

    }
    //caso o player coma todas a comidas disponiveis(230), troca o level
    if (Player1.Score % 230 == 0)
    {
        //troca para o proximo nivel
        SwitchLevel(Player1, Ghosts);
    }
}

//Função que troca o nivel
void SwitchLevel(Pacman &Player1, Enemy Ghosts[])
{
    //Reinicia a posiÇão do player([1,1])
    Player1.Line = 1;
    Player1.Column = 1;
    //Reincia o mapa de posição
    InitPoseMap();
    //Reinicia os fantamas
    for(int x=0; x<=3; x++)
    {
        //Reinicia a posição dos fantasmas
        ResetGhostPosition(Ghosts, x);
        //Muda a velocidade dos fantasmas
        //Primeiro if impede que ela passe de um certo valor de velocidade
        if (Ghosts[x].Velocity == 1)
        {
            Ghosts[x].Velocity = 1;
        }
        //Case seja maior que 10, faz em passos de -5
        else if(Ghosts[x].Velocity > 10)
        {
            Ghosts[x].Velocity -= 5;
        }
        //Caso contrario, faz em passos de -1
        else
        {
            Ghosts[x].Velocity -= 1;
        }
    }

}

//Essa função inicia  o mapa de posição
void InitPoseMap()
{
    for (int x=0; x<22; x++)
    {
        for (int y=0; y<24; y++)
        {
            Position[x][y]=Map[y][x];//Faz o inverso do mapa normal
        }
    }
}

//Essa função desenha o pacman com os sprites
void DrawPacman(Pacman &Player1, ALLEGRO_BITMAP *pacmanfinal, int curFrame, int frameWidth, int frameHeight)
{
    al_draw_bitmap_region(pacmanfinal,  curFrame * frameWidth, sprite * frameHeight, frameWidth, frameHeight, Player1.Line * 32, Player1.Column * 32, 0);
}

//Essa função incia os fantasmas
void InitGhosts (Enemy Ghosts[])
{
    //Inicia o tipo, vulnerabilidade e velocidade para todos os fantamas
    for(int x=0; x<=3; x++)
    {
        Ghosts[x].Type = x;
        Ghosts[x].Vulnerable = false;
        Ghosts[x].Velocity = 30;
    }
    //Inicia a posição dos fantasmas individualmente
    Ghosts[0].Line = 10;
    Ghosts[0].Column = 10;
    Ghosts[1].Line = 10;
    Ghosts[1].Column = 12;
    Ghosts[2].Line = 11;
    Ghosts[2].Column = 10;
    Ghosts[3].Line = 11;
    Ghosts[3].Column = 12;
}

//Desenha os fantamas
void DrawGhosts (Enemy Ghosts[],ALLEGRO_BITMAP *enemy1, ALLEGRO_BITMAP *enemy2, ALLEGRO_BITMAP *enemy3, ALLEGRO_BITMAP *enemy4, int timer)
{
    //Incialmente chega se o fantama esta vulneravel
    //Caso esteja, intercala entre desenhar o mesmo e nao desenhar
    //Isso é feito de modo a dar o efeito dos fantasmas piscando
    if ((!Ghosts[0].Vulnerable))
    {
        al_draw_bitmap(enemy1, Ghosts[0].Line*32, Ghosts[0].Column*32, 0);
    }
    else if(timer%10)
    {
        al_draw_bitmap(enemy1, Ghosts[0].Line*32, Ghosts[0].Column*32, 0);
    }
    if ((!Ghosts[1].Vulnerable))
    {
        al_draw_bitmap(enemy2, Ghosts[1].Line*32, Ghosts[1].Column*32, 0);
    }
    else if(timer%10)
    {
        al_draw_bitmap(enemy2, Ghosts[1].Line*32, Ghosts[1].Column*32, 0);
    }
    if ((!Ghosts[2].Vulnerable))
    {
        al_draw_bitmap(enemy3, Ghosts[2].Line*32, Ghosts[2].Column*32, 0);
    }
    else if(timer%10)
    {
        al_draw_bitmap(enemy3, Ghosts[2].Line*32, Ghosts[2].Column*32, 0);
    }
    if ((!Ghosts[3].Vulnerable))
    {
        al_draw_bitmap(enemy4, Ghosts[3].Line*32, Ghosts[3].Column*32, 0);
    }
    else if(timer%10)
    {
        al_draw_bitmap(enemy4, Ghosts[3].Line*32, Ghosts[3].Column*32, 0);
    }
}

//Move os fantamas por meio de uma inteligencia artificial basica
void MoveGhosts (Enemy Ghosts[], Pacman &Player1)
{
    //Cria as variáveis de decisão
    int decisionX;//Decisão para o eixo X
    int decisionY;//Decisão para o eixo Y
    int Fdecision;//DeciSão final
    int random;//Número random
    //Cria um laço para passar pelos fantamas
    for(int x=0; x<=3; x++)
    {
        //Inicia as váriaveis de decisão em 0
        decisionX = 0;
        decisionY = 0;
        Fdecision = 0;
        //Checa se o fantasma está vulnerável ou não
        //Isso define o comporamento do fantama
        //Caso esteja invulnerável, ele tende a seguir o pacman
        //Caso esteja vulnerável, ele para de seguir o pacman e assume um comportamento aleatório

        //CASO INVULNERÁVEL//
        if(!Ghosts[x].Vulnerable)
        {
            //Primeiramnete define em que quadrante o pacman está em relação ao fantasma
            //Com isso toma uma pre decisao pra onde ir
            //tambem leva em conta se é possivel se mover para o quadrande em questão

            //Verifica Se o X do player é >= ao X do Fantasma e verifica se é possivel se mover +1 na Linha
            if(Position[Ghosts[x].Line+1][Ghosts[x].Column] != 0 && Player1.Line>=Ghosts[x].Line)
            {
                //Caso isso aconteça, define a decisão X como 1;
                decisionX = 1;
            }
            //Senão(Se o X do player é < ao X do Fantasma), verifica se é possivel se mover -1 na linha
            else if(Position[Ghosts[x].Line-1][Ghosts[x].Column] != 0)
            {
                //Caso isso aconteça, define a decisão X como 2;
                decisionX = 2;
            }
            //Faz a mesma coisa para a coluna, mesmo principio
            if(Position[Ghosts[x].Line][Ghosts[x].Column+1] != 0 && Player1.Column>=Ghosts[x].Column)
            {
                //Define a decisao Y como 3
                decisionY = 3;
            }
            else if((Position[Ghosts[x].Line][Ghosts[x].Column-1]) != 0)
            {
                //Ou como 4
                decisionY = 4;
            }

            //Apos isso o fantasma vai ter um valor pra decisao X e um para a Y
            //Todavia, ele so pode se mexer em um eixo
            //Logo, é feita uma seleção randomica(50% de chance para cada)
            if(rand() % 2 == 0)
            {
                //Fedicion assume o valor de decisionX
                Fdecision = decisionX;
            }
            else
            {
                //Fedicion assume o valor de decisionY
                Fdecision = decisionY;
            }

            //A variável randomica tem um valor para cada fantasma
            //lembrando que cada ghost tem um type
            //que vai de 0 até 3
            //isso é somado com 1 para definir a variável random
            random = Ghosts[x].Type + 1;

            //Em seguida é a feito um switch case
            //cada case é para uma das decisoes
            //logo
            //Case 1: referente a decisionX
            //Case 2: referente a decisionX
            //Case 3: referente a decisionY
            //Case 4: referente a decisionY
            //O que define para que case vai é a Fdecision, que foi escolhida anteriormente
            switch (Fdecision)
            {
            case 1:
                //Faz outra verificação, gera um número randomico, no range da variável random
                //lembrando que a variável random tem um valor diferente para cada fantasma
                //Com isso, certo fantasmas terão probabilidades diferentes de entrar no if
                //Isso gera uma chance de movimento aleatório, dando mais dinamica ao jogo
                if(rand() % random >= 0)
                {
                    //ao entrar no if, se adiciona +1 na linha, Fantasma se move
                    Ghosts[x].Line+=1;
                }
                else if(Position[Ghosts[x].Line-1][Ghosts[x].Column] != 0)
                {
                    //caso contrário, se subtrai -1 na linha, Fantasma se move
                    Ghosts[x].Line-=1;
                }
                break;
                //O mesmo se aplica para o case 2
            case 2:
                if(rand() % random >= 0)
                {
                    Ghosts[x].Line-=1;
                }
                else if(Position[Ghosts[x].Line+1][Ghosts[x].Column] != 0)
                {
                    Ghosts[x].Line+=1;
                }
                break;
                //O mesmo se aplica para o case 3
            case 3:
                if(rand() % random >= 0)
                {
                    Ghosts[x].Column+=1;
                }
                else if((Position[Ghosts[x].Line][Ghosts[x].Column-1]) != 0)
                {
                    Ghosts[x].Column-=1;
                }
                break;
                //e para o case 4
            case 4:
                if(rand() % random >= 0)
                {
                    Ghosts[x].Column-=1;
                }
                else if(Position[Ghosts[x].Line][Ghosts[x].Column+1] != 0)
                {
                    Ghosts[x].Column+=1;
                }
                break;
            }
        }
        //CASO VULNERÁVEL//
        else
        {
            //O fantasmsa faz a mesma verificação de quadrantes
            //Porem ao inves de se referenciar as cordenadas de player1
            //se refencia a uma cornenada gerada randomicamente
            //para cada um dos fantasmas existe uma cordenada randomica nova

            //novamente se utiliza os decisionX e Y
            if(Position[Ghosts[x].Line+1][Ghosts[x].Column] != 0 && (rand() % 20)>=Ghosts[x].Line)
            {
                decisionX = 1;
            }
            else if(Position[Ghosts[x].Line-1][Ghosts[x].Column] != 0)
            {
                decisionX = 2;
            }
            if(Position[Ghosts[x].Line][Ghosts[x].Column+1] != 0 && (rand() % 20)>=Ghosts[x].Column)
            {
                decisionY = 3;
            }
            else if((Position[Ghosts[x].Line][Ghosts[x].Column-1]) != 0)
            {
                decisionY = 4;
            }

            //Em seguida se escolhe de maneira randomica o decisionX e decisionY(50% de chance, meio a meio)
            if(rand() % 2 == 0)
            {
                Fdecision = decisionX;
            }
            else
            {
                Fdecision = decisionY;
            }
            //Novamente se usa o switch case
            //Porem dessa vez nao existe uma chance randomica em cada case
            switch (Fdecision)
            {
            case 1:
                //adiciona +1 na linha, Fantasma se move
                Ghosts[x].Line+=1;
                break;
            case 2:
                //adiciona -1 na linha, Fantasma se move
                Ghosts[x].Line-=1;
                break;
            case 3:
                //adiciona +1 na coluna, Fantasma se move
                Ghosts[x].Column+=1;
                break;
            case 4:
                //adiciona -1 na coluna, Fantasma se move
                Ghosts[x].Column-=1;
                break;
            }
        }
    }
}

//Checa se a posição do player, caso ele esteja no café, deixa os fantamas vulneráveis
void CheckIfVulnerable(Pacman &Player1, Enemy Ghosts[])
{
    //Checa se o player esta no café
    if(Position[Player1.Line][Player1.Column] == 3)
    {
        //deixa os fantamas vulneráveis
        for(int x=0; x<=3; x++)
        {
            Ghosts[x].Vulnerable = true;
        }
    }
}

//Essa função deixa os fantasmas invvulneráveis
void MakeInvulnerable(Enemy Ghosts[])
{
    //cria um laço para passar por todos os fantasmas
    for(int x=0; x<=3; x++)
    {
        Ghosts[x].Vulnerable = false;
    }
}

//Essa função reseta a posição e um fantasma
void ResetGhostPosition(Enemy Ghosts[], int x)
{
    //X é o indice do fanatasma a ter a posição resetada
    switch (x)
    {
    //para o fantasma 1
    case 0:
        Ghosts[0].Line = 10;
        Ghosts[0].Column = 10;
        break;
    //para o fantasma 2
    case 1:
        Ghosts[1].Line = 10;
        Ghosts[1].Column = 12;
        break;
    //para o fantasma 3
    case 2:
        Ghosts[2].Line = 11;
        Ghosts[2].Column = 10;
        break;
    //para o fantasma 4
    case 3:
        Ghosts[3].Line = 11;
        Ghosts[3].Column = 12;
        break;
    }
}

//Checa a colisão e as possiveis combinaçoes e consequencias relacionadas com a mesma
int CheckColision(Pacman &Player1, Enemy Ghosts[], ALLEGRO_SAMPLE *pacman_eatghost)
{
    //Cria a variável dec(detection), que conta o número de colisoes com fantasmas vulneráveis
    int dec = 0;
    //Faz um laço de modo a verificar todos os fantasmas
    for(int x=0; x<=3; x++)
    {
        //Verifica se o fantasma está na mesma posição do player
        if((Player1.Line == Ghosts[x].Line)&&(Player1.Column == Ghosts[x].Column))
        {
            //caso esteja, verifica se o fantasma está vulnerável
            if(Ghosts[x].Vulnerable)
            {
                //se o fantasma esiver vulnerável
                //Resenta a posiçao do mesmo
                ResetGhostPosition(Ghosts, x);
                //Toca o som do fantasma comido
                al_play_sample(pacman_eatghost, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                //Adiciona +40 ao GhostScore
                Player1.GhostScore += 40;
                //Deixa o fantasma Invulnerável
                Ghosts[x].Vulnerable = false;
            }
            else
            {
                //se o fantasma estiver invulnerável, adiciona 1 a variável dec
                dec++;
            }
        }
    }
    if (dec!=0)
    {
        return 1;
        //Ao retornar 1 indica que houve colisão e que os fantasmas estavam invulneráveis
        //Logo, GameOver
    }
    else
    {
        return 0;
        //Não houve colisão ou houve e o fantama estava vulnerável
        //Logo, Segue la pelota
    }
}
