guichan-sfml
============

A set of classes which provide an SFML-powered backend for [Guichan](http://gitorious.org/guichan). 

## Implemented Features ##

* `SFMLFont`: `sf::Font` rendering, `gcn::Graphics::Alignment` supported
* `SFMLGraphics`: Drawing images, lines, points, and rectangles (outline or filled), clip rectangles are emulated using `sf::View`s
* `SFMLImage`: Wrapper for `sf::Texture` which also supports pixel reading/manipulation
* `SFMLImageLoader`: Load images via SFML's `loadFromFile`
* `SFMLInput`: Input events (keyboard, mouse, mouse wheel, window focus)

## Example Usage ##

The following example shows how to create an SFML window and integrate Guichan. A few widgets are also created; including
one that draws an image and one that uses fonts.

```c++
#include <guichan/sfml.hpp>
#include <guichan/gui.hpp>
#include <guichan/widgets/button.hpp>
#include <guichan/widgets/checkbox.hpp>
#include <guichan/widgets/container.hpp>
#include <guichan/widgets/icon.hpp>

#include <SFML/Graphics.hpp>

int main() {
    sf::VideoMode videoMode(640, 480, 32);
    sf::RenderWindow window(videoMode, "Guichan SFML Test");

    gcn::SFMLGraphics guiGraphics(window);
    gcn::SFMLFont guiFont("arial.ttf", 10);
    gcn::SFMLInput guiInput;
    gcn::SFMLImageLoader guiImageLoader;

    gcn::Image::setImageLoader(&guiImageLoader);
    gcn::Widget::setGlobalFont(&guiFont);

    gcn::Gui gui;
    gcn::Container topContainer;
    gcn::Icon guiIcon("sfml-small.png");
    gcn::Button guiButton("Clicky!");
    gcn::CheckBox guiCheckbox;

    gui.setInput(&guiInput);
    gui.setGraphics(&guiGraphics);
    gui.setTop(&topContainer);

    guiButton.setWidth(100);
    guiButton.setHeight(40);

    guiCheckbox.setWidth(16);
    guiCheckbox.setHeight(16);

    topContainer.setSize(400, 300);
    topContainer.add(&guiIcon, 50, 50);
    topContainer.add(&guiButton, 100, 100);
    topContainer.add(&guiCheckbox, 210, 100);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            guiInput.pushInput(event, window);
        }

        window.clear();

        gui.logic();
        gui.draw();

        window.display();
    }

    return 0;
}
```
