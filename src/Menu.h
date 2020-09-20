#ifndef F214AE3A_4338_459F_9F8D_CB4C2B3B8F48
#define F214AE3A_4338_459F_9F8D_CB4C2B3B8F48

#include "Gui.h"
#include "Text.h"

class Action : public Text
{
public:
    Action(int newTrigger, const std::string &newText, std::function<void(void)> newCallback, std::function<bool(void)> newVisible)
        : Text(newText), trigger(newTrigger), callback(newCallback), visible(newVisible)
    {
        setColor({1.f, 1.f, 1.f, 0.5f});
    }
    void draw(Window *window) const override
    {
        if (isVisible())
            Text::draw(window);
    }
    void consumeKeyEvent(int key)
    {
        if (key == trigger && isVisible())
            callback();
    }
    bool isVisible() const
    {
        return visible();
    }

private:
    friend class Menu;
    int trigger;
    std::function<void(void)> callback;
    std::function<bool(void)> visible;
};

class Menu : public DrawableObject
{
public:
    void draw(Window *window) const override
    {
        for (const auto &action : actions)
            action.draw(window);
    }

    void update(float delta) override
    {
        glm::vec2 temp(20.f, 0.f);
        for (auto &action : actions)
        {
            action.setPosition({-1.f, 0.f});
            if (action.isVisible())
            {
                action.setPositionOffset(temp);
                temp.y += Text::glyphHeight * 1.5f;
            }
            action.update(delta);
        }
    }

    void consumeKeyEvent(int key)
    {
        for (auto &action : actions)
            action.consumeKeyEvent(key);
    }

    std::vector<Action>::iterator insert(const Action &action)
    {
        return actions.insert(actions.end(), action);
    }

    void erase(std::vector<Action>::iterator it)
    {
        actions.erase(it);
    }

private:
    std::vector<Action> actions;
};

#endif /* F214AE3A_4338_459F_9F8D_CB4C2B3B8F48 */
