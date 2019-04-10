//Object IDS
//Structs
struct Pacman
{
    int Line;//Linha do pacman
    int Column;//Column do pacman
    int Score;//Score do pacman
    int GhostScore;//GhostScore do pacman
};
struct Enemy
{
    int Line;//Linha do enemy
    int Column;//Column do enemy
    bool Vulnerable;//Vulnerable state
    int Type;//Type, reflete na inteligencia
    int Velocity;//Velocidade do enemy
};
