#include "guichan/sfml/sfmlgraphics.hpp"

#if defined (_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined (__amigaos4__)
#include <mgl/gl.h>
#define glVertex3i glVertex3f
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "guichan/exception.hpp"
#include "guichan/image.hpp"
#include "guichan/sfml/sfmlimage.hpp"

#include <SFML/Graphics.hpp>

namespace gcn
{
    SFMLGraphics::SFMLGraphics(sf::RenderTarget& target)
        : mTarget(&target)
    {
        mAlpha = false;
        mSize = mTarget->getView().getSize();

        setColor(Color());
    }

    void SFMLGraphics::_beginDraw()
    {
        mSize = mTarget->getView().getSize();

        glEnable(GL_SCISSOR_TEST);
        pushClipArea(Rectangle(0, 0, static_cast<int>(mSize.x), static_cast<int>(mSize.y)));
    }

    void SFMLGraphics::_endDraw()
    {
        popClipArea();
        glDisable(GL_SCISSOR_TEST);
    }

    void SFMLGraphics::setRenderTarget(sf::RenderTarget& target)
    {
        mTarget = &target;
        mSize = mTarget->getView().getSize();
    }

    bool SFMLGraphics::pushClipArea(Rectangle area)
    {
        bool result = Graphics::pushClipArea(area);

        calculateScissoring();

        return result;
    }

    void SFMLGraphics::popClipArea()
    {
        Graphics::popClipArea();

        if (mClipStack.empty())
        {
            return;
        }

        calculateScissoring();
    }

    sf::RenderTarget& SFMLGraphics::getRenderTarget() const {
        return *mTarget;
    }

    void SFMLGraphics::drawImage(const Image* image,
                                int srcX,
                                int srcY,
                                int dstX,
                                int dstY,
                                int width,
                                int height)
    {
        const SFMLImage* srcImage = dynamic_cast<const SFMLImage*>(image);

        if (srcImage == NULL)
        {
            throw GCN_EXCEPTION("Trying to draw an image of unknown format, must be an SFMLImage.");
        }

        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        dstX += top.xOffset;
        dstY += top.yOffset;

        const sf::IntRect srcRect(srcX, srcY, width, height);

        mSprite.setTexture(*srcImage->getTexture(), false);
        mSprite.setTextureRect(srcRect);
        mSprite.setPosition(static_cast<float>(dstX), static_cast<float>(dstY));
        
        mTarget->draw(mSprite);
    }

    void SFMLGraphics::drawPoint(int x, int y)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        x += top.xOffset;
        y += top.yOffset;

        sf::VertexArray point(sf::Points, 1);

        point[0] = sf::Vertex(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)), mSfmlColor);

        mTarget->draw(point);
    }

    void SFMLGraphics::drawLine(int x1, int y1, int x2, int y2)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        x1 += top.xOffset;
        y1 += top.yOffset;
        x2 += top.xOffset;
        y2 += top.yOffset;

        sf::VertexArray line(sf::Lines, 2);

        line[0] = sf::Vertex(sf::Vector2f(static_cast<float>(x1), static_cast<float>(y1)), mSfmlColor);
        line[1] = sf::Vertex(sf::Vector2f(static_cast<float>(x2), static_cast<float>(y2)), mSfmlColor);

        mTarget->draw(line);
    }

    void SFMLGraphics::drawRectangle(const Rectangle& rectangle)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();
        const float outlineThickness = 1.0f;

        sf::Vector2f size(static_cast<float>(rectangle.width) - outlineThickness, static_cast<float>(rectangle.height) - outlineThickness);
        sf::Vector2f position(static_cast<float>(top.xOffset + rectangle.x), static_cast<float>(top.yOffset + rectangle.y));
        
        sf::RectangleShape rectShape(size);
        rectShape.setPosition(position);
        rectShape.setOutlineColor(mSfmlColor);
        rectShape.setOutlineThickness(outlineThickness);
        rectShape.setFillColor(sf::Color::Transparent);

        mTarget->draw(rectShape);
    }

    void SFMLGraphics::fillRectangle(const Rectangle& rectangle)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        sf::Vector2f size(static_cast<float>(rectangle.width), static_cast<float>(rectangle.height));
        sf::Vector2f position(static_cast<float>(top.xOffset + rectangle.x), static_cast<float>(top.yOffset + rectangle.y));
        
        sf::RectangleShape rectShape(size);
        rectShape.setPosition(position);
        rectShape.setFillColor(mSfmlColor);

        mTarget->draw(rectShape);
    }

    void SFMLGraphics::setColor(const Color& color)
    {
        mColor = color;
        mSfmlColor = sf::Color(mColor.r, mColor.g, mColor.b, mColor.a);

        mAlpha = color.a != 255;
    }

    const Color& SFMLGraphics::getColor() const
    {
        return mColor;
    }

    void SFMLGraphics::calculateScissoring()
    {
        // Determine "world" coordinates
        const int worldLeft = mClipStack.top().x;
        const int worldTop = static_cast<int>(mSize.y) - mClipStack.top().y - mClipStack.top().height;
        const int worldRight = worldLeft + mClipStack.top().width;
        const int worldBottom = worldTop + mClipStack.top().height;

        // Scale the coordinate according to the view ratio
        const sf::Vector2f worldPosition = sf::Vector2f(static_cast<float>(worldLeft), static_cast<float>(worldTop));
        const sf::Vector2f worldSize = sf::Vector2f(static_cast<float>(worldRight), static_cast<float>(worldBottom));
        const sf::Vector2i targetPosition = mTarget->mapCoordsToPixel(worldPosition);
        const sf::Vector2i targetSize = mTarget->mapCoordsToPixel(worldSize);

        // Determine "target" coordinates
        const int renderedX = targetPosition.x;
        const int renderedY = targetPosition.y;
        const int renderedWidth = targetSize.x - targetPosition.x;
        const int renderedHeight = targetSize.y - targetPosition.y;

        glScissor(renderedX,
                  renderedY,
                  renderedWidth,
                  renderedHeight);
    }
}