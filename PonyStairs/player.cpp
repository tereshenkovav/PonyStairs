#include "player.h"
#include <math.h>

Player::Player()
{

}

bool Player::isDirsOrtho(Direction newdir) {
    if ((playerdir==dUp)||(playerdir==dDown)) return (newdir==dLeft)||(newdir==dRight) ;
    if ((newdir==dUp)||(newdir==dDown)) return (playerdir==dLeft)||(playerdir==dRight) ;
    return false ;
}

int Player::cmd2sig(Direction dir) {
    if (dir==dLeft) return -1 ;
    if (dir==dRight) return 1 ;
    if (dir==dUp) return -1 ;
    if (dir==dDown) return 1 ;
    return 0 ;
}

void Player::cacheDir2PlayerDir()
{
    playerdir = cachedcmd ;
    cachedcmd = dWait ;
}

bool Player::fixXifCrossed(float stopx, float dx)
{    
    if (playerdir==dLeft) {
        float borderx = playerx-HALF ;
        if (borderx+dx<stopx) {
            playerx = stopx+HALF ;
            return true ;
        }
        return false ;
    }
    else {
        float borderx = playerx+HALF ;
        if (borderx+dx>stopx) {
            playerx = stopx-HALF ;
            return true ;
        }
        return false ;
    }
}

bool Player::fixYifCrossed(float stopy, float dy)
{
    if (playerdir==dUp) {
        float bordery = playery-HALF ;
        if (bordery+dy<stopy) {
            playery = stopy+HALF ;
            return true ;
        }
        return false ;
    }
    else {
        float bordery= playery+HALF ;
        if (bordery+dy>stopy) {
            playery = stopy-HALF ;
            return true ;
        }
        return false ;
    }

}

bool Player::isAwayLastSwitchPoint() const
{
    return (lastswitchpoint.x()!=getCellX())||(lastswitchpoint.y()!=getCellY()) ;
}

void Player::setInitial(int playerx, int playery, Direction dir)
{
    this->playerx = playerx+HALF ;
    this->playery = playery+HALF ;
    playerdir = dir ;
    cachedcmd = dWait ;
    lasthorzdir = dRight ;
    lastswitchpoint = QPoint(-1,-1) ;
}

void Player::sendNewDirection(Direction dir)
{
    if (isDirsOrtho(dir)) cachedcmd = dir ; else playerdir = dir ;
}

bool Player::isLeftRotated() const
{
    return lasthorzdir==dLeft ;
}

bool Player::isIntersectWith(const Player &player) const
{
    return (abs(playerx-player.playerx)<1.0f)&&
           (abs(playery-player.playery)<1.0f) ;
}

void Player::update(float dt)
{

    if ((cachedcmd==dDown)||(cachedcmd==dUp))
        if (!level->isBlockAt(getCellX(),getCellY()+cmd2sig(cachedcmd))) {
            if (playerdir==dRight) {
                if (fixXifCrossed(getCellX()+1,PLAYERV*dt)) cacheDir2PlayerDir() ;
            }
            else {
                if (fixXifCrossed(getCellX(),-PLAYERV*dt)) cacheDir2PlayerDir() ;
            }
        }

    if ((cachedcmd==dLeft)||(cachedcmd==dRight))
        if (!level->isBlockAt(getCellX()+cmd2sig(cachedcmd),getCellY())) {
            if (playerdir==dDown) {
                if (fixYifCrossed(getCellY()+1,PLAYERV*dt)) cacheDir2PlayerDir() ;
            }
            else {
                if (fixYifCrossed(getCellY(),-PLAYERV*dt)) cacheDir2PlayerDir() ;
            }
        }

    if (playerdir==dLeft)
        if (level->isBlockAt(getCellX()-1,getCellY()))
            if (fixXifCrossed(getCellX(),-PLAYERV*dt)) playerdir=dWait ;

    if (playerdir==dRight)
        if (level->isBlockAt(getCellX()+1,getCellY()))
            if (fixXifCrossed(getCellX()+1,PLAYERV*dt)) playerdir=dWait ;

    if (playerdir==dUp)
        if (level->isBlockAt(getCellX(),getCellY()-1))
            if (fixYifCrossed(getCellY(),-PLAYERV*dt)) playerdir=dWait ;

    if (playerdir==dDown)
        if (level->isBlockAt(getCellX(),getCellY()+1))
            if (fixYifCrossed(getCellY()+1,PLAYERV*dt)) playerdir=dWait ;

    if (playerdir==dLeft) playerx-=PLAYERV*dt ;
    if (playerdir==dRight) playerx+=PLAYERV*dt ;
    if (playerdir==dUp) playery-=PLAYERV*dt ;
    if (playerdir==dDown) playery+=PLAYERV*dt ;

    if (isAwayLastSwitchPoint()) lastswitchpoint = QPoint(-1,-1) ;

    if ((playerdir==dLeft)||(playerdir==dRight)) lasthorzdir=playerdir ;
}

bool Player::canJump(Direction dir) const
{
    int dy = (dir==dUp?-2:2) ;
    int p1 = trunc(playerx-0.49f) ;
    return (!level->isBlockAt(p1,getCellY()+dy)) &&
           (!level->isBlockAt(p1+1,getCellY()+dy)) ;
}

void Player::doJump(Direction dir)
{
    int dy = (dir==dUp?-2:2) ;
    playery=getCellY()+dy+HALF ;
}

int Player::getCellX() const
{
    return trunc(playerx) ;
}

int Player::getCellY() const
{
    return trunc(playery) ;
}

float Player::getPosX() const
{
    return playerx ;
}

float Player::getPosY() const
{
    return playery ;
}
