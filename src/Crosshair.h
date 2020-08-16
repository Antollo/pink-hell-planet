#ifndef A15B603B_ED5B_45A4_96FF_36ED181556A2
#define A15B603B_ED5B_45A4_96FF_36ED181556A2

#include "ShapeArray.h"
#include "PlayableObject.h"

class Crosshair : public Drawable
{
public:
    Crosshair(PlayableObject *&p)
        : player(p),
          lines1(ShapeArray::Type::lines),
          dots1(ShapeArray::Type::points),
          lines2(ShapeArray::Type::lines),
          dots2(ShapeArray::Type::points),
          triangles(ShapeArray::Type::triangles)
    {
        lines1.insert(lines1.end(), {

                                        {-60.f, 0.f},
                                        {-20.f, 0.f},
                                        {60.f, 0.f},
                                        {20.f, 0.f},
                                        {0.f, -80.f},
                                        {0.f, -20.f}

                                    });
        lines1.setWidth(4.f);
        lines1.setColor({1.f, 1.f, 1.f, 0.5f});

        for (int i = 0; i < 16; i++)
            dots1.insert(dots1.end(), {cosf(float(i) * 3.14f / 8.f) * 100.f, sinf(float(i) * 3.14f / 8.f) * 100.f});

        dots1.insert(dots1.end(), {0.f, 0.f});

        dots1.setWidth(4.f);
        dots1.setColor({1.f, 1.f, 1.f, 0.5f});

        lines2.insert(lines2.end(), {
                                        /*

                                      - -

                                      */
                                        {-200.f, 0.f},
                                        {-20.f, 0.f},
                                        {200.f, 0.f},
                                        {20.f, 0.f},
                                        /*


                                       |
                                      */
                                        {0.f, -300.f},
                                        {0.f, -20.f},
                                        /*
                                      |  |
                                      |  |
                                      |  |
                                      */
                                        {-220.f, 300.f},
                                        {-220.f, -300.f},
                                        {220.f, 300.f},
                                        {220.f, -300.f},
                                        /*
                                      -  -

                                      -  -
                                      */
                                        {-200.f, 320.f},
                                        {-100.f, 320.f},
                                        {-200.f, -320.f},
                                        {-100.f, -320.f},
                                        {200.f, 320.f},
                                        {100.f, 320.f},
                                        {200.f, -320.f},
                                        {100.f, -320.f},

                                    });
        lines2.setWidth(4.f);
        lines2.setColor({1.f, 1.f, 1.f, 0.5f});

        for (int i = 0; i < 32; i++)
            dots2.insert(dots2.end(), {cosf(float(i) * 3.14f / 16.f) * 500.f, sinf(float(i) * 3.14f / 16.f) * 500.f});

        dots2.insert(dots2.end(), {
                                      /*
                                    
                                     .
                                    
                                    */
                                      {0.f, 0.f},
                                      /*
                                    .  .
                                    
                                    .  .
                                    */
                                      /*{-220.f, 320.f},
                                    {-220.f, -320.f},
                                    {220.f, 320.f},
                                    {220.f, -320.f}*/
                                  });
        /*

         .
         .
        */
        for (int i = 0; i < 10; i++)
            dots2.insert(dots2.end(), {30.f, -float(i * 30 + 25)});

        dots2.setWidth(4.f);
        dots2.setColor({1.f, 1.f, 1.f, 0.5f});

        triangles.insert(triangles.end(), {

                                              {-200.f, 300.f},
                                              {-200.f, -300.f},
                                              {200.f, 300.f},

                                              {-200.f, -300.f},
                                              {200.f, -300.f},
                                              {200.f, 300.f}

                                          });

        triangles.setColor({1.f, 1.f, 1.f, 0.02f});
    }
    void update()
    {
        lines1.setPosition();
        dots1.setPosition();
        lines1.setColor({1.f, 1.f, 1.f, 0.5f * player->getAlpha()});
        dots1.setColor({1.f, 1.f, 1.f, 0.5f * player->getAlpha()});

        lines2.setPosition();
        dots2.setPosition();
        triangles.setPosition();
        lines2.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - player->getAlpha())});
        dots2.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - player->getAlpha())});
        triangles.setColor({1.f, 1.f, 1.f, 0.02f * (1.f - player->getAlpha())});
    }

protected:
    void draw(Window *window) const override
    {
        lines1.draw(window);
        dots1.draw(window);

        lines2.draw(window);
        dots2.draw(window);
        triangles.draw(window);
    }

private:
    ShapeArray lines1, lines2;
    ShapeArray dots1, dots2;
    ShapeArray triangles;
    PlayableObject *&player;
};

#endif /* A15B603B_ED5B_45A4_96FF_36ED181556A2 */
