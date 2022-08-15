#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "all_sfml.h"
#include "level.h"
#include "player.h"
#include "seqgen.h"
#include "countdown.h"

using namespace sf ;

class Game : public QObject
{
    Q_OBJECT
private:
    Font font ;
    Sprite spr_player ;
    Sprite spr_block ;
    Sprite spr_stair ;
    Sprite spr_crystall ;
    Sprite spr_crystall_big ;
    Sprite spr_list_teleport ;
    Sprite spr_list_damage ;
    Sprite spr_list_protect ;
    Sprite spr_protect ;
    Sprite spr_spawn1 ;
    Sprite spr_spawn2 ;
    QMap<QString,Sprite> spr_monsters ;
    const int CELL_WIDTH=40 ;
    const int CELL_HEIGHT=40 ;
    int teleport_cnt ;
    int damage_cnt ;
    int protect_cnt ;
    int crystall_cnt ;
    GameState gamestate ;
    float timepass ;
    const float PROTECT_DURATION=10.0f ;
    const float DAMAGE_DURATION=4.0f ;
    Countdown protect ;
    Countdown damage ;
    sf::VertexArray laser;
    int laserstage ;

    Player player ;
    Level level ;

    QList<Player> monsters ;

    float nextspawn ;
    const float SPAWN_INTERVAL=10.0f ;

    SeqGen gen_monster ;
    SeqGen gen_portal ;

    void drawLabel(RenderTarget & target, const QString & str, int x, int y, const Font & font, int size) ;
    void drawSprite(RenderTarget & target, Sprite & sprite, int x, int y) ;

    void loadRes() ;
    void initLevel() ;
public:
    explicit Game(QObject *parent = nullptr);
    static sf::String makeStr(const QString & str) ;
    void loadFileTo(sf::Sprite & sprite, const QString &str) ;
    void loadFileCenteredTo(sf::Sprite & sprite, const QString &str) ;
    //void setFPS(unsigned int Afps) ;
    bool Init() ;
    bool Frame(float dt, std::vector<Event> events, int mx, int my) ;
    bool Render(sf::RenderTarget & target) ;
    bool UnInit() ;
    ~Game();

private slots:
    void grabCrystall() ;

signals:

};

#endif // GAME_H
