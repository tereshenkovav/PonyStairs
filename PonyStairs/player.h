#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include "CommonTypes.h"
#include "level.h"

class Player
{    
private:

    Direction lasthorzdir ;
    Direction cachedcmd ;
    bool isDirsOrtho(Direction newdir2) ;
    int cmd2sig(Direction dir) ;
    void cacheDir2PlayerDir() ;
    bool fixXifCrossed(float stopx, float dx) ;
    bool fixYifCrossed(float stopy, float dy) ;


public:
    Player();

    const float PLAYERV=2.0f ;
// tmp public
    float playerx ;
    float playery ;
    Direction playerdir ;

    QString spritecode ;

    QPoint lastswitchpoint ;
    bool isAwayLastSwitchPoint() const ;

    Level * level ;
    int getCellX() const ;
    int getCellY() const ;
    float getPosX() const ;
    float getPosY() const ;
    void setInitial(int playerx, int playery, Direction dir) ;
    void sendNewDirection(Direction dir) ;
    bool isLeftRotated() const;
    bool isIntersectWith(const Player & player) const;
    void update(float dt) ;
    bool canJump(Direction dir) const ;
    void doJump(Direction dir) ;

};

#endif // PLAYER_H
