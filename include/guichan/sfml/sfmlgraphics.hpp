#ifndef GCN_SFMLGRAPHICS_HPP
#define GCN_SFMLGRAPHICS_HPP

#include "guichan/color.hpp"
#include "guichan/graphics.hpp"
#include "guichan/platform.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf
{
    class RenderTarget;
}

namespace gcn
{
    class Image;
    class Rectangle;

    /**
     * SFML implementation of the Graphics.
     */
    class GCN_EXTENSION_DECLSPEC SFMLGraphics : public Graphics
    {
    public:

        // Needed so that drawImage(gcn::Image *, int, int) is visible.
        using Graphics::drawImage;

        /**
         * Constructor.
         */
        SFMLGraphics(sf::RenderTarget& target);

        /**
         * Sets the RenderTarget to draw to. The target can be any RenderTarget.
         * This funtion also pushes a clip areas corresponding to the dimension 
         * of the target.
         *
         * @param target the target to draw to.
         */
        virtual void setRenderTarget(sf::RenderTarget& target);

        /**
         * Gets the RenderTarget.
         *
         * @return the RenderTarget.
         */
        virtual sf::RenderTarget& getRenderTarget() const;

        // Inherited from Graphics

        virtual void _beginDraw();

        virtual void _endDraw();

        virtual bool pushClipArea(Rectangle area);

        virtual void popClipArea();

        virtual void drawImage(const Image* image,
                               int srcX,
                               int srcY,
                               int dstX,
                               int dstY,
                               int width,
                               int height);

        virtual void drawPoint(int x, int y);

        virtual void drawLine(int x1, int y1, int x2, int y2);

        virtual void drawRectangle(const Rectangle& rectangle);

        virtual void fillRectangle(const Rectangle& rectangle);

        virtual void setColor(const Color& color);

        virtual const Color& getColor() const;

    protected:
        /**
         * Calculates the correct coordinates to apply a clipping rectangle, 
         * relative to the current render target's view.
         */
        virtual void calculateScissoring();

        sf::RenderTarget* mTarget;
        sf::Vector2f mSize;
        sf::Sprite mSprite;
        sf::Color mSfmlColor;
        Color mColor;
        bool mAlpha;
    };
}

#endif // end GCN_SFMLGRAPHICS_HPP