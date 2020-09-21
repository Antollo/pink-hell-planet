#ifndef A15B603B_ED5B_45A4_96FF_36ED181556A2
#define A15B603B_ED5B_45A4_96FF_36ED181556A2

#include "ShapeArray.h"
#include "Text.h"
#include "PlayableObject.h"
#include "Camera.h"
#include "ProgressBar.h"

class Crosshair : public Drawable
{
public:
    Crosshair(Camera &c)
        : camera(c),
          lines1(ShapeArray::Type::lines),
          lines2(ShapeArray::Type::lines),
          dots1(ShapeArray::Type::points),
          dots2(ShapeArray::Type::points),
          triangles(ShapeArray::Type::triangles),
          healthBar(10, 1.f, "hp: "),
          reloadBar(4, 1.f),
          powerBar(4, 1.f)
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
            dots1.insert(dots1.end(), {cosf(float(i) * pi / 8.f) * 100.f, sinf(float(i) * pi / 8.f) * 100.f});

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
            dots2.insert(dots2.end(), {cosf(float(i) * pi / 16.f) * 500.f, sinf(float(i) * pi / 16.f) * 500.f});

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

        distance.setPositionOffset({240.f, 0.f});
        distance.setColor({1.f, 1.f, 1.f, 0.5f});

        wind.setPositionOffset({-240.f - wind.size() * Text::glyphWidth, 0.f});
        wind.setColor({1.f, 1.f, 1.f, 0.5f});

        healthBar.setPosition({-0.2f, 1.f});
        healthBar.setPositionOffset({0.f, -20.f - Text::glyphHeight});
        healthBar.setColor({1.f, 1.f, 1.f, 0.5f});

        reloadBar.setPositionOffset({240.f, -Text::glyphHeight});
        reloadBar.setColor({1.f, 1.f, 1.f, 0.5f});

        powerBar.setPositionOffset({-240.f - powerBar.size() * Text::glyphWidth, -Text::glyphHeight});
        powerBar.setColor({1.f, 1.f, 1.f, 0.5f});
    }

    void update(float delta)
    {
        auto player = camera.getPlayer();
        if (player != nullptr)
        {
            const float alpha = player->getAlpha();

            lines1.update(delta);
            dots1.update(delta);

            lines2.update(delta);
            dots2.update(delta);
            triangles.update(delta);

            lines1.setColor({1.f, 1.f, 1.f, 0.5f * alpha});
            dots1.setColor({1.f, 1.f, 1.f, 0.5f * alpha});

            lines2.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - alpha)});
            dots2.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - alpha)});
            triangles.setColor({1.f, 1.f, 1.f, 0.02f * (1.f - alpha)});
            distance.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - alpha)});
            wind.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - alpha)});
            reloadBar.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - alpha)});
            powerBar.setColor({1.f, 1.f, 1.f, 0.5f * (1.f - alpha)});

            if (i == 0)
            {
                glm::vec3 v = player->getRaycastFront();
                if (std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z))
                    distance.setText("inf");
                else
                    distance.setText(std::to_string(int(glm::distance(player->getPosition(), v) + 0.5f)));
                wind.setText(std::to_string(std::abs(Wind::getStrength())).substr(0, 4));
                wind.setPositionOffset({-240.f - wind.size() * Text::glyphWidth, 0.f});
            }
            i++;
            if (i > 10)
                i = 0;

            healthBar.setValue(player->getHP() / PlayableObject::maxHP);
            healthBar.update(delta);

            reloadBar.setValue(player->getReloadState());
            powerBar.setValue(player->getFullPowerState());

            reloadBar.update(delta);
            powerBar.update(delta);
        }
    }

protected:
    void draw(Window *window) const override
    {
        if (camera.getPlayer() != nullptr)
        {
            lines1.draw(window);
            dots1.draw(window);

            lines2.draw(window);
            dots2.draw(window);
            triangles.draw(window);
            distance.draw(window);
            wind.draw(window);

            healthBar.draw(window);
            reloadBar.draw(window);
            powerBar.draw(window);
        }
    }

private:
    int i = 0;
    Camera &camera;
    ShapeArray lines1, lines2;
    ShapeArray dots1, dots2;
    ShapeArray triangles;
    Text distance, wind;
    ProgressBar healthBar, reloadBar, powerBar;
    glm::vec3 pos;
};

#endif /* A15B603B_ED5B_45A4_96FF_36ED181556A2 */
