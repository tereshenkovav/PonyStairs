#include <QCoreApplication>

#include "all_sfml.h"
#include <fstream>
#include <vector>
#include <time.h>
#include "game.h"
#include <QDebug>
#include <QDir>

using namespace sf ;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDir::setCurrent("../data") ;

    std::ofstream file( "SFML.log" );
    sf::err().rdbuf(file.rdbuf()) ;

    sf::RenderWindow window(sf::VideoMode(1024, 768),
                            Game::makeStr("Рарити и рубины")) ;
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    srand((unsigned int)time(NULL)) ;

    Game game ;
    game.Init() ;

    Clock clock ;

    float lasttime = clock.getElapsedTime().asSeconds() ;

    while (window.isOpen())
    {
        float newtime = clock.getElapsedTime().asSeconds() ;
        float dt = newtime-lasttime ;
        lasttime = newtime ;

        //game.setFPS(60) ;

        std::vector<Event> events ;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close() ;
                break ;
            }
            if (event.type == sf::Event::KeyPressed)
                if (event.key.code==Keyboard::Escape) {
                    window.close() ;
                    break ;
            }
            events.push_back(event) ;
        }

        int mx=Mouse::getPosition(window).x ;
        int my=Mouse::getPosition(window).y ;

        if (!game.Frame(dt,events,mx,my)) window.close() ;

        window.clear();
        game.Render(window) ;
        window.display();
    }

    game.UnInit() ;

    a.quit() ;
    return EXIT_SUCCESS ;
}
