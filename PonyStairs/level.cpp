#include "level.h"
#include <QFile>
#include <math.h>
#include <QDebug>

Level::Level()
{

}

bool Level::loadFromFile(const QString &filename)
{
    map.clear() ;
    total_crystall = 0 ;

    QFile file(filename) ;
    if (!file.exists()) return false ;
    if (!file.open(QIODevice::ReadOnly)) return false ;

    QString line = file.readLine() ;
    width = line.toInt() ;
    line = file.readLine() ;
    height = line.toInt() ;
    for (int y=0; y<height; y++) {
        line = file.readLine() ;
        for (int x=0; x<width; x++) {
            CellType ct=((x+y/2)%2==0)?ctCrystall:ctFree ;
            if (line.at(x).toLatin1()=='#') ct = ctStair ;
            if (line.at(x).toLatin1()=='=') ct = ctBlock ;
            if (line.at(x).toLatin1()=='T') {
                teleports.append(QPoint(x,y)) ;
                ct = ctFree ;
            }
            if (line.at(x).toLatin1()=='P') {
                protects.append(QPoint(x,y)) ;
                ct = ctFree ;
            }
            if (line.at(x).toLatin1()=='D') {
                damages.append(QPoint(x,y)) ;
                ct = ctFree ;
            }
            if (line.at(x).toLatin1()=='W') {
                spawn_points.append(QPoint(x,y)) ;
                ct = ctFree ;
            }
            if (line.at(x).toLatin1()=='S') {
                startpos = QPoint(x,y) ;
                ct = ctFree ;
            }
            if (ct==ctCrystall) total_crystall++ ;
            map.insert(y*width+x,ct) ;
        }
    }

    file.close() ;
    return true ;
}

int Level::getWidth() const
{
    return width ;
}

int Level::getHeight() const
{
    return height ;
}

int Level::getTotalCrystall() const
{
    return total_crystall ;
}

QPoint Level::getStartPos() const
{
    return startpos ;
}

bool Level::canJumpUp(float playerx, int playery) const
{
    if (isBlockAt(trunc(playerx),playery-2)) return false ;
    if (isBlockAt(trunc(playerx)+1,playery-2)) return false ;
    return true ;
}

bool Level::canJumpDown(float playerx, int playery) const
{
    if (isBlockAt(trunc(playerx),playery+2)) return false ;
    if (isBlockAt(trunc(playerx)+1,playery+2)) return false ;
    return true ;
}

QList<QPoint> Level::getCellForLaser(int playerx, int playery, bool isleft)
{
    QList<QPoint> res ;
    if (isleft) {
        int i=playerx-1 ;
        while (!isBlockAt(i,playery)) {
            res.append(QPoint(i,playery)) ;
            i-- ;
            if (i<0) break ;
        }
    }
    else {
        int i=playerx+1 ;
        while (!isBlockAt(i,playery)) {
            res.append(QPoint(i,playery)) ;
            i++ ;
            if (i>width-1) break ;
        }
    }
    return res ;

}

bool Level::isBlockAt(int x, int y) const
{    
    if ((x<0)||(x>=width)||(y<0)||(y>=height)) return true ;
    return (map[y*width+x]==ctBlock) ;
}

bool Level::isStairAt(int x, int y) const
{
    if ((x<0)||(x>=width)||(y<0)||(y>=height)) return false ;
    return map[y*width+x]==ctStair ;
}

bool Level::isCrystallAt(int x, int y) const
{
    if ((x<0)||(x>=width)||(y<0)||(y>=height)) return false ;
    return map[y*width+x]==ctCrystall ;
}

bool Level::isProtectAt(int x, int y) const
{
    return protects.contains(QPoint(x,y)) ;
}

bool Level::isTeleportAt(int x, int y) const
{
    return teleports.contains(QPoint(x,y)) ;
}

void Level::grabItemAt(int x, int y)
{
    QPoint p(x,y) ;
    teleports.removeOne(p) ;
    protects.removeOne(p) ;
    damages.removeOne(p) ;
}

void Level::grabCrystallAt(int x, int y)
{
    map[y*width+x]=ctFree ;
}

bool Level::isDamageAt(int x, int y) const
{
    return damages.contains(QPoint(x,y)) ;
}
