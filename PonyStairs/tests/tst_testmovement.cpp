#include <QtTest>

// add necessary includes here

#include "../level.h"
#include "../player.h"
#include "../CommonTypes.h"

class TestMovement : public QObject
{
    Q_OBJECT

private:
    Level level ;
    const float dt = 1.0f/60.0f ;

public:
    TestMovement();
    ~TestMovement();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testWalls();
    void testStopLeftRight();
    void testJump();
};

TestMovement::TestMovement()
{

}

TestMovement::~TestMovement()
{

}

void TestMovement::initTestCase()
{
    level.loadFromFile("../Game/level1.dat") ;
}

void ComparePos(const Player & p, float x, float y) {
    const int PREC = 10 ;
    QCOMPARE((int)round(p.getPosX()*PREC),(int)(x*PREC)) ;
    QCOMPARE((int)round(p.getPosY()*PREC),(int)(y*PREC)) ;
}

void CompareCell(const Player & p, int x, int y) {
    QCOMPARE(p.getCellX(),x) ;
    QCOMPARE(p.getCellY(),y) ;
}

void TestMovement::cleanupTestCase()
{

}

void TestMovement::testWalls()
{
    QCOMPARE(level.getWidth(),22) ;
    QCOMPARE(level.getHeight(),19) ;

    QVERIFY2(level.isBlockAt(0,0),"wall-0-0") ;
    QVERIFY2(level.isBlockAt(0,1),"wall-0-1") ;
    QVERIFY2(level.isBlockAt(0,2),"wall-0-2") ;
    QVERIFY2(level.isBlockAt(1,0),"wall-1-0") ;
    QVERIFY2(!level.isBlockAt(1,1),"wall-1-1") ;
    QVERIFY2(level.isBlockAt(1,2),"wall-1-2") ;

    QVERIFY2(level.isBlockAt(level.getWidth()-1,0),"wall-w-0") ;
    QVERIFY2(level.isBlockAt(level.getWidth()-1,1),"wall-w-1") ;
    QVERIFY2(level.isBlockAt(level.getWidth()-1,2),"wall-w-2") ;
    QVERIFY2(level.isBlockAt(level.getWidth()-2,0),"wall-w1-0") ;
    QVERIFY2(!level.isBlockAt(level.getWidth()-2,1),"wall-w1-1") ;
    QVERIFY2(level.isBlockAt(level.getWidth()-2,2),"wall-w1-2") ;
}

void TestMovement::testStopLeftRight()
{
    Player player ;
    player.level = &level ;
    player.setInitial(5,1,dWait) ;

    ComparePos(player,5.5f,1.5f) ;
    CompareCell(player,5,1) ;

    float dist = 5.0f ;
    int steps = (dist/dt)/player.PLAYERV ;

    // update on wait
    for (int i=0; i<steps; i++) player.update(dt) ;
    ComparePos(player,5.5f,1.5f) ;
    CompareCell(player,5,1) ;

    // update on go right
    player.sendNewDirection(dRight) ;
    for (int i=0; i<steps; i++) player.update(dt) ;
    ComparePos(player,5.5f+dist,1.5f) ;
    CompareCell(player,5+dist,1) ;

    // update on go left and stop
    player.sendNewDirection(dLeft) ;
    for (int i=0; i<10*steps; i++) player.update(dt) ;
    ComparePos(player,1.5f,1.5f) ;
    CompareCell(player,1,1) ;

    // update on go right and stop
    player.sendNewDirection(dRight) ;
    for (int i=0; i<10*steps; i++) player.update(dt) ;
    ComparePos(player,level.getWidth()-2+0.5f,1.5f) ;
    CompareCell(player,level.getWidth()-2,1) ;

    // update on go left and stop
    player.sendNewDirection(dLeft) ;
    for (int i=0; i<10*steps; i++) player.update(dt) ;
    ComparePos(player,1.5f,1.5f) ;
    CompareCell(player,1,1) ;

}

void TestMovement::testJump()
{
    Player player ;
    player.level = &level ;

    player.setInitial(5,1,dWait) ;
    QVERIFY2(!player.canJump(dUp),"jump1") ;
    QVERIFY2(player.canJump(dDown),"jump2") ;

    player.setInitial(13,1,dWait) ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(!player.canJump(dDown),"jump4") ;

    player.playerx = 12.25f ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(player.canJump(dDown),"jump4") ;

    player.playerx = 12.75f ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(!player.canJump(dDown),"jump4") ;

    player.playerx = 13.25f ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(!player.canJump(dDown),"jump4") ;

    player.playerx = 13.75f ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(!player.canJump(dDown),"jump4") ;

    player.playerx = 14.25f ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(!player.canJump(dDown),"jump4") ;

    player.playerx = 14.75f ;
    QVERIFY2(!player.canJump(dUp),"jump3") ;
    QVERIFY2(player.canJump(dDown),"jump4") ;
}

QTEST_APPLESS_MAIN(TestMovement)

#include "tst_testmovement.moc"
