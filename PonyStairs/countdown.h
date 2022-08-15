#ifndef COUNTDOWN_H
#define COUNTDOWN_H

class Countdown
{
private:
    float left ;
    bool oncereach ;
public:
    Countdown(void);
    void upset(float interval) ;
    void update(float dt) ;
    bool isActive() ;
    bool onceReachNol() ;
    void reset() ;
    ~Countdown(void);

};

#endif // COUNTDOWN_H
