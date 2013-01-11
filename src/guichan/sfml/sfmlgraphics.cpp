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
        mContextView = mTarget->getView();
        mSize = mContextView.getSize();

        setColor(Color());
    }

    void SFMLGraphics::_beginDraw()
    {
        // Save the view before drawing
        mContextView = mTarget->getView();
        mSize = mContextView.getSize();

        pushClipArea(Rectangle(0, 0, static_cast<int>(mSize.x), static_cast<int>(mSize.y)));
    }

    void SFMLGraphics::_endDraw()
    {
        popClipArea();

        // Restore the view after drawing
        mTarget->setView(mContextView);
    }

    void SFMLGraphics::setRenderTarget(sf::RenderTarget& target)
    {
        mTarget = &target;
        mContextView = mTarget->getView();
        mSize = mContextView.getSize();
    }

    bool SFMLGraphics::pushClipArea(Rectangle area)
    {
        bool result = Graphics::pushClipArea(area);

        if (result)
        {
            const sf::View clippingView = convertClipRectangleToView(mClipStack.top());
            mTarget->setView(clippingView);
        }

        return result;
    }

    void SFMLGraphics::popClipArea()
    {
        Graphics::popClipArea();

        if (mClipStack.empty())
        {
            return;
        }

        const sf::View clippingView = convertClipRectangleToView(mClipStack.top());
        mTarget->setView(clippingView);
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

        sf::Vertex point(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)), mSfmlColor);

        mTarget->draw(&point, 1, sf::Points);
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

        sf::Vertex line[2] = 
        {
            sf::Vertex(sf::Vector2f(static_cast<float>(x1), static_cast<float>(y1)), mSfmlColor),
            sf::Vertex(sf::Vector2f(static_cast<float>(x2), static_cast<float>(y2)), mSfmlColor)
        };

        mTarget->draw(line, 2, sf::Lines);
    }

    void SFMLGraphics::drawRectangle(const Rectangle& rectangle)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        const float x = static_cast<float>(rectangle.x + top.xOffset);
        const float y = static_cast<float>(rectangle.y + top.yOffset);
        const float w = static_cast<float>(rectangle.width);
        const float h = static_cast<float>(rectangle.height);

        sf::Vertex rect[5] = 
        {
            sf::Vertex(sf::Vector2f(x, y), mSfmlColor),
            sf::Vertex(sf::Vector2f(x + w, y), mSfmlColor),
            sf::Vertex(sf::Vector2f(x + w, y + h), mSfmlColor),
            sf::Vertex(sf::Vector2f(x, y + h), mSfmlColor),
            sf::Vertex(sf::Vector2f(x, y), mSfmlColor)
        };

        mTarget->draw(rect, 5, sf::Lines);
    }

    void SFMLGraphics::fillRectangle(const Rectangle& rectangle)
    {
        if (mClipStack.empty())
        {
            throw GCN_EXCEPTION("Clip stack is empty, perhaps you called a draw function outside of _beginDraw() and _endDraw()?");
        }

        const ClipRectangle& top = mClipStack.top();

        const float x = static_cast<float>(rectangle.x + top.xOffset);
        const float y = static_cast<float>(rectangle.y + top.yOffset);
        const float w = static_cast<float>(rectangle.width);
        const float h = static_cast<float>(rectangle.height);

        sf::Vertex rect[4] = 
        {
            sf::Vertex(sf::Vector2f(x, y), mSfmlColor),
            sf::Vertex(sf::Vector2f(x + w, y), mSfmlColor),
            sf::Vertex(sf::Vector2f(x + w, y + h), mSfmlColor),
            sf::Vertex(sf::Vector2f(x, y + h), mSfmlColor)
        };

        mTarget->draw(rect, 4, sf::Quads);
    }

    void SFMLGraphics::setColor(const Color& color)
    {
        mColor = color;
        mSfmlColor = sf::Color(mColor.r, mColor.g, mColor.b, mColor.a);
    }

    const Color& SFMLGraphics::getColor() const
    {
        return mColor;
    }

    sf::View SFMLGraphics::convertClipRectangleToView(const ClipRectangle& rectangle) const
    {
        const sf::Vector2f& currentViewSize = mContextView.getSize();

        sf::FloatRect clipRectangle;
        clipRectangle.top = static_cast<float>(rectangle.y);
        clipRectangle.left = static_cast<float>(rectangle.x);
        clipRectangle.width = static_cast<float>(rectangle.width);
        clipRectangle.height = static_cast<float>(rectangle.height);

        // Use normalized viewport coordinates to emulate clipping
        // Special case: if the viewport size is 0, then the width/height is 0
        sf::FloatRect clipViewport;
        clipViewport.top = currentViewSize.y == 0 ? 0 : (rectangle.y / currentViewSize.y);
        clipViewport.left = currentViewSize.x == 0 ? 0 : (rectangle.x / currentViewSize.x);
        clipViewport.width = currentViewSize.x == 0 ? 0 : (rectangle.width / currentViewSize.x);
        clipViewport.height = currentViewSize.y == 0 ? 0 : (rectangle.height / currentViewSize.y);

        sf::View clippingView(clipRectangle);
        clippingView.setViewport(clipViewport);

        return clippingView;
    }
}