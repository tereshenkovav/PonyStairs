#include "game.h"
#include <math.h>
#include <QDebug>
#include <QFile>

Game::Game(QObject *parent) : QObject(parent)
{

}

sf::String Game::makeStr(const QString &str)
{
    QByteArray bb = str.toUtf8();
    return String::fromUtf8(bb.constBegin(),bb.constEnd());
}

void Game::loadFileTo(sf::Sprite & sprite, const QString & filename)
{
    Texture * tex = new Texture() ;
    tex->loadFromFile(makeStr(filename)) ;
    sprite.setTexture(*tex) ;
}

void Game::loadFileCenteredTo(sf::Sprite & sprite, const QString & filename)
{
    Texture * tex = new Texture() ;
    tex->loadFromFile(makeStr(filename)) ;
    sprite.setTexture(*tex) ;
    sprite.setOrigin(tex->getSize().x/2,tex->getSize().y/2) ;
}


void Game::drawLabel(RenderTarget & target, const QString & str, int x, int y, const Font & font, int size)
{
    Text text(makeStr(str),font,size) ;
    text.setPosition(x,y) ;
    target.draw(text) ;
}

void Game::drawSprite(RenderTarget & target, Sprite & sprite, int x, int y)
{
    sprite.setPosition(x,y) ;
    target.draw(sprite) ;
}

void Game::loadRes()
{
    font.loadFromFile("fonts/arial.ttf") ;

    loadFileCenteredTo(spr_player,"sprites/rarity.png") ;
    loadFileTo(spr_block,"sprites/block.png") ;
    loadFileTo(spr_stair,"sprites/stair.png") ;
    loadFileTo(spr_crystall,"sprites/diamond.png") ;
    loadFileTo(spr_crystall_big,"sprites/diamond_big.png") ;
    loadFileTo(spr_list_teleport,"sprites/list_teleport.png") ;
    loadFileTo(spr_list_damage,"sprites/list_damage.png") ;
    loadFileTo(spr_list_protect,"sprites/list_protect.png") ;
    loadFileCenteredTo(spr_protect,"sprites/shield.png") ;
    loadFileTo(spr_spawn1,"sprites/spawn1.png") ;
    loadFileTo(spr_spawn2,"sprites/spawn2.png") ;

    Sprite monster ;
    loadFileCenteredTo(monster,"sprites/monster1.png") ;
    spr_monsters.insert("monster1",monster) ;
    loadFileCenteredTo(monster,"sprites/monster2.png") ;
    spr_monsters.insert("monster2",monster) ;
    loadFileCenteredTo(monster,"sprites/monster3.png") ;
    spr_monsters.insert("monster3",monster) ;

    laser.setPrimitiveType(sf::Lines) ;
    laser.resize(2) ;

}

void Game::initLevel() {
    level.loadFromFile("levels/level1.dat") ;
    player.setInitial(level.getStartPos().x(),level.getStartPos().y(),dWait) ;

    teleport_cnt=0 ;
    protect_cnt=0 ;
    damage_cnt=0 ;

    monsters.clear() ;

    if (QFile::exists("dev_mode")) {
        teleport_cnt=10 ;
        protect_cnt=10 ;
        damage_cnt=10 ;
    }
    crystall_cnt=0 ;

    gamestate=gsNorm ;
    timepass = 0 ;

    nextspawn = SPAWN_INTERVAL ;

    gen_portal.setN(level.spawn_points.count()) ;
    gen_monster.setN(spr_monsters.count()) ;
}

bool Game::Init() {
    loadRes() ;
    player.level = &level ;
    initLevel() ;
    return true ;    
}

bool Game::Frame(float dt, std::vector<Event> events, int mx, int my)
{    
    Q_UNUSED(mx)
    Q_UNUSED(my)
    if (gamestate!=gsNorm) {
        if (Keyboard::isKeyPressed(Keyboard::F5)) initLevel() ;
        return true ;
    }

    for (Event event: events) {
        if (event.type == sf::Event::KeyPressed) {            
            if (event.key.code==Keyboard::Up) player.sendNewDirection(dUp) ;
            if (event.key.code==Keyboard::Down) player.sendNewDirection(dDown) ;
            if (event.key.code==Keyboard::Left) player.sendNewDirection(dLeft) ;
            if (event.key.code==Keyboard::Right) player.sendNewDirection(dRight) ;

            if ((protect_cnt>0)&&(!protect.isActive())) {
                if (event.key.code==Keyboard::Num3) {
                    protect.upset(PROTECT_DURATION) ;
                    protect_cnt-- ;
                }
            }
            if ((damage_cnt>0)&&(!damage.isActive())) {
                if (event.key.code==Keyboard::Num4) {
                    damage.upset(DAMAGE_DURATION) ;
                    damage_cnt-- ;
                }
            }
            if (teleport_cnt>0) {
                Direction jumpdir = dWait ;
                if (event.key.code==Keyboard::Num1) jumpdir=dUp ;
                if (event.key.code==Keyboard::Num2) jumpdir=dDown ;
                if (jumpdir!=dWait)
                    if (player.canJump(jumpdir)) {
                        player.doJump(jumpdir) ;
                        teleport_cnt-- ;
                    }
            }
        }
    }

    if (level.isProtectAt(player.getCellX(),player.getCellY())) {
        level.grabItemAt(player.getCellX(),player.getCellY()) ;
        protect_cnt++ ;
    }
    if (level.isTeleportAt(player.getCellX(),player.getCellY())) {
        level.grabItemAt(player.getCellX(),player.getCellY()) ;
        teleport_cnt++ ;
    }
    if (level.isDamageAt(player.getCellX(),player.getCellY())) {
        level.grabItemAt(player.getCellX(),player.getCellY()) ;
        damage_cnt++ ;
    }

    if (level.isCrystallAt(player.getCellX(),player.getCellY())) {
        level.grabCrystallAt(player.getCellX(),player.getCellY()) ;
        this->metaObject()->invokeMethod(this,"grabCrystall") ;
        //crystall_cnt++ ;
    }

    player.update(dt) ;

    if (crystall_cnt==level.getTotalCrystall()) gamestate=gsWin ;

    protect.update(dt) ;
    damage.update(dt) ;
    nextspawn-=dt ;
    timepass+=dt ;

    if (nextspawn<=0) {
        nextspawn = SPAWN_INTERVAL ;
        QPoint spawnp = level.spawn_points[gen_portal.getNext()] ;
        Player monster ;
        monster.level = &level ;
        monster.setInitial(spawnp.x(),spawnp.y(),dWait) ;
        monster.spritecode=spr_monsters.keys()[gen_monster.getNext()] ;
        monsters.append(monster) ;
    }

    for (int i=0; i<monsters.count(); i++) {
        if (monsters[i].playerdir==dWait) {
            if (rand()%2==0)
                monsters[i].sendNewDirection(dLeft) ;
            else
                monsters[i].sendNewDirection(dRight) ;
        }
        if (monsters[i].isAwayLastSwitchPoint()) {
            QList<Direction> dirs ;
            if ((monsters[i].playerdir==dLeft)||(monsters[i].playerdir==dRight)) {

            if (!level.isBlockAt(monsters[i].getCellX(),monsters[i].getCellY()-1))
                dirs.append(dUp) ;
            if (!level.isBlockAt(monsters[i].getCellX(),monsters[i].getCellY()+1))
                dirs.append(dDown) ;
            }
            if ((monsters[i].playerdir==dUp)||(monsters[i].playerdir==dDown)) {

            if (!level.isBlockAt(monsters[i].getCellX()-1,monsters[i].getCellY()))
                dirs.append(dLeft) ;
            if (!level.isBlockAt(monsters[i].getCellX()+1,monsters[i].getCellY()))
                dirs.append(dRight) ;
            }
            if (dirs.count()>0) {
              int idx = rand()%(dirs.count()+1) ;
              if (idx<dirs.count()) monsters[i].sendNewDirection(dirs[idx]) ;
              monsters[i].lastswitchpoint = QPoint(monsters[i].getCellX(),monsters[i].getCellY()) ;
            }
        }

        if (monsters[i].isIntersectWith(player))
            if (!protect.isActive()) gamestate = gsFail ;

        monsters[i].update(dt) ;
    }

    if (damage.isActive()) {
        auto cells = level.getCellForLaser(player.getCellX(),player.getCellY(),player.isLeftRotated()) ;
        int i=0 ;
        while (i<monsters.count()) {
            if (cells.contains(QPoint(monsters[i].getCellX(),monsters[i].getCellY())))
                monsters.removeAt(i) ;
            else
                i++ ;
        }
    }

    laserstage = ((int)(timepass*10)) % 2 ;

    return true ;
}

bool Game::Render(sf::RenderTarget & target) {
    drawLabel(target,"Рарити и рубины",890,20,font,16) ;
    drawLabel(target,"Собери их все!",890,80,font,16) ;

    for (int i=0; i<level.getWidth(); i++)
        for (int j=0; j<level.getHeight(); j++) {
            Sprite * spr = NULL;
            if (level.isBlockAt(i,j)) spr = &spr_block ;
            if (level.isStairAt(i,j)) spr = &spr_stair ;
            if (level.isCrystallAt(i,j)) spr = &spr_crystall ;
            if (spr!=NULL) drawSprite(target,*spr,i*CELL_WIDTH,j*CELL_HEIGHT) ;
            spr = NULL ;
            if (level.isTeleportAt(i,j)) spr = &spr_list_teleport ;
            if (level.isProtectAt(i,j)) spr = &spr_list_protect ;
            if (level.isDamageAt(i,j)) spr = &spr_list_damage ;
            if (spr!=NULL) drawSprite(target,*spr,i*CELL_WIDTH,j*CELL_HEIGHT) ;
        }

    foreach(auto p, level.spawn_points)
        if ((int)(p.y())/2 % 2==0)
            drawSprite(target,spr_spawn1,p.x()*CELL_WIDTH,p.y()*CELL_HEIGHT) ;
        else
            drawSprite(target,spr_spawn2,p.x()*CELL_WIDTH,p.y()*CELL_HEIGHT) ;

    for (int i=0; i<monsters.count(); i++) {
        spr_monsters[monsters[i].spritecode].setScale(monsters[i].isLeftRotated()?-1.0f:1.0f,1.0f) ;
        drawSprite(target,spr_monsters[monsters[i].spritecode],
                monsters[i].getPosX()*CELL_WIDTH,monsters[i].getPosY()*CELL_HEIGHT) ;
    }

    spr_player.setScale(player.isLeftRotated()?-1.0f:1.0f,1.0f) ;
    drawSprite(target,spr_player,player.getPosX()*CELL_WIDTH,player.getPosY()*CELL_HEIGHT) ;

    if (protect.isActive())
        drawSprite(target,spr_protect,player.getPosX()*CELL_WIDTH,player.getPosY()*CELL_HEIGHT) ;

    if (damage.isActive()) {
        laser[0].color = laserstage==1?Color::Blue:Color::White;
        laser[1].color = laserstage==1?Color::Blue:Color::White;
        auto cells = level.getCellForLaser(player.getCellX(),player.getCellY(),player.isLeftRotated()) ;
        if (cells.count()>0) {
        for (int i=-1; i<=1; i++) {
            if (player.isLeftRotated()) {
                laser[0].position = sf::Vector2f(player.getPosX()*CELL_WIDTH-CELL_WIDTH/2,player.getPosY()*CELL_HEIGHT+i);
                laser[1].position = sf::Vector2f(cells.last().x()*CELL_WIDTH,player.getPosY()*CELL_HEIGHT+i);
            }
            else {
                laser[0].position = sf::Vector2f(player.getPosX()*CELL_WIDTH+CELL_WIDTH/2,player.getPosY()*CELL_HEIGHT+i);
                laser[1].position = sf::Vector2f(cells.last().x()*CELL_WIDTH+CELL_WIDTH,player.getPosY()*CELL_HEIGHT+i);
            }
        target.draw(laser) ;
        }
        }
    }

    drawSprite(target,spr_list_teleport,910,200) ;
    drawLabel(target,"x "+QString::number(teleport_cnt),960,200,font,26) ;

    drawSprite(target,spr_list_protect,910,250) ;
    drawLabel(target,"x "+QString::number(protect_cnt),960,250,font,26) ;

    drawSprite(target,spr_list_damage,910,300) ;
    drawLabel(target,"x "+QString::number(damage_cnt),960,300,font,26) ;

    drawSprite(target,spr_crystall_big,910,125) ;
    drawLabel(target,QString::number(100*crystall_cnt/level.getTotalCrystall())+" %",960,125,font,26) ;

    drawLabel(target,"УПРАВЛЕНИЕ",890,400,font,16) ;
    drawLabel(target,"1 - прыжок вверх",890,420,font,16) ;
    drawLabel(target,"(желтый свиток)",890,440,font,16) ;
    drawLabel(target,"2 - прыжок вниз",890,460,font,16) ;
    drawLabel(target,"(желтый свиток)",890,480,font,16) ;
    drawLabel(target,"3 - силовой щит",890,500,font,16) ;
    drawLabel(target,"(синий свиток)",890,520,font,16) ;
    drawLabel(target,"4 - роголазер",890,540,font,16) ;
    drawLabel(target,"(красный свиток)",890,560,font,16) ;
    drawLabel(target,"Esc - выход",890,580,font,16) ;

    drawLabel(target,"Время: "+QString::number((int)timepass)+" с",900,700,font,20) ;

    if (gamestate!=gsNorm) {
        drawLabel(target,"F5 - рестарт",895,640,font,22) ;
        if (gamestate==gsWin) drawLabel(target,"Победа!",895,610,font,22) ;
        if (gamestate==gsFail) drawLabel(target,"Поражение!",895,610,font,22) ;
    }
    return true ;
}

bool Game::UnInit()
{
    return true ;
}

Game::~Game()
{
}

void Game::grabCrystall()
{
    crystall_cnt++ ;
}
