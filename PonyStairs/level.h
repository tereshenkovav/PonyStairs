#ifndef LEVEL_H
#define LEVEL_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPoint>
#include "CommonTypes.h"

class Level
{    
private:
    enum CellType { ctFree, ctBlock, ctStair, ctCrystall } ;
    QMap<int,CellType> map ;
    QList<QPoint> teleports ;
    QList<QPoint> damages ;
    QList<QPoint> protects ;    
    QPoint startpos ;
    int width ;
    int height ;
    int total_crystall ;
public:
    Level();
    QList<QPoint> spawn_points ;
    bool loadFromFile(const QString & filename) ;
    bool isBlockAt(int x, int y) const ;
    bool isStairAt(int x, int y) const;
    bool isProtectAt(int x, int y) const;
    bool isDamageAt(int x, int y) const;
    bool isTeleportAt(int x, int y) const;
    bool isCrystallAt(int x, int y) const;
    void grabItemAt(int x, int y) ;
    void grabCrystallAt(int x, int y) ;
    int getWidth() const ;
    int getHeight() const ;
    int getTotalCrystall() const ;
    QPoint getStartPos() const ;
    bool canJumpUp(float playerx, int playery) const ;
    bool canJumpDown(float playerx, int playery) const ;
    QList<QPoint> getCellForLaser(int playerx, int playery, bool isleft) ;

};

#endif // LEVEL_H
