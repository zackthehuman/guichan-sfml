#include "guichan/sfml/sfmlimage.hpp"

#include "guichan/exception.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

namespace gcn
{
    SFMLImage::SFMLImage(sf::Texture* texture, bool autoFree)
    {
        mAutoFree = autoFree;
        mTexture = texture;
    }

    SFMLImage::~SFMLImage()
    {
        if (mAutoFree)
        {
            free();
        }
    }

    sf::Texture* SFMLImage::getTexture() const
    {
        return mTexture;
    }

    int SFMLImage::getWidth() const
    {
        if (mTexture == NULL)
        {
            throw GCN_EXCEPTION("Trying to get the width of a non loaded image.");
        }

        return mTexture->getSize().x;
    }

    int SFMLImage::getHeight() const
    {
        if (mTexture == NULL)
        {
            throw GCN_EXCEPTION("Trying to get the height of a non loaded image.");
        }

        return mTexture->getSize().y;
    }

    Color SFMLImage::getPixel(int x, int y)
    {
        if (mTexture == NULL)
        {
            throw GCN_EXCEPTION("Trying to get a pixel from a non loaded image.");
        }

        // TODO: Implement getPixel
        //       Potential way to implement is to convert to Image and then return
        //       the pixel after converting its format.


        // For now return none more black.
        return Color();
    }

    void SFMLImage::putPixel(int x, int y, const Color& color)
    {
        if (mTexture == NULL)
        {
            throw GCN_EXCEPTION("Trying to put a pixel in a non loaded image.");
        }

        // TODO: Imeplement setPixel
        //       Potential way to implement is to convert to Image, update the pixel
        //       and then update the Texture with the new pixel array.
    }

    void SFMLImage::convertToDisplayFormat()
    {
        if (mTexture == NULL)
        {
            throw GCN_EXCEPTION("Trying to convert a non loaded image to display format.");
        }

        // Do nothing since we can't change the display format.
    }

    void SFMLImage::free()
    {
        if(mTexture != NULL) {
            delete mTexture;
        }
    }
}
