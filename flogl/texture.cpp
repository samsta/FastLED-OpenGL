#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WITH_GLEW
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

namespace flogl {

struct DdsPixelformat {
  unsigned dwSize;
  unsigned dwFlags;
  unsigned dwFourCC;
  unsigned dwRGBBitCount;
  unsigned dwRBitMask;
  unsigned dwGBitMask;
  unsigned dwBBitMask;
  unsigned dwABitMask;
};

struct DdsHeader{
  unsigned           dwSize;
  unsigned           dwFlags;
  unsigned           dwHeight;
  unsigned           dwWidth;
  unsigned           dwPitchOrLinearSize;
  unsigned           dwDepth;
  unsigned           dwMipMapCount;
  unsigned           dwReserved1[11];
  DdsPixelformat     ddspf;
  unsigned           dwCaps;
  unsigned           dwCaps2;
  unsigned           dwCaps3;
  unsigned           dwCaps4;
  unsigned           dwReserved2;
};

GLuint loadDDS(const unsigned char* binary_image, unsigned size)
{
   const unsigned HEADER_SIZE = 128;
   if (size < HEADER_SIZE)
   {
      printf("Not enough bytes (%u) for header size (%u)\n", size, HEADER_SIZE);
      return 0;
   }

   if (strncmp((const char*)binary_image, "DDS ", 4) != 0)
   {
      printf("image doesn't start with 'DDS '\n");
      return 0;
   }

   DdsHeader hdr;
   memcpy(&hdr, binary_image + 4, sizeof(hdr));
   
   unsigned int height      = hdr.dwHeight;
   unsigned int width       = hdr.dwWidth;
   unsigned int mipMapCount = hdr.dwMipMapCount;
   unsigned int fourCC      = hdr.ddspf.dwFourCC;
   
   const unsigned char *buffer = binary_image + HEADER_SIZE;
   unsigned int format;
   switch(fourCC) 
   { 
   case FOURCC_DXT1: 
      format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
      break; 
   case FOURCC_DXT3: 
      format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
      break; 
   case FOURCC_DXT5: 
      format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
      break; 
   default: 
      return 0; 
   }

   // Create one OpenGL texture
   GLuint textureID;
   glGenTextures(1, &textureID);

   // "Bind" the newly created texture : all future texture functions will modify this texture
   glBindTexture(GL_TEXTURE_2D, textureID);
   glPixelStorei(GL_UNPACK_ALIGNMENT,1);  
   
   unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
   unsigned int offset = 0;

   /* load the mipmaps */ 
   for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
   { 
      unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
      glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
         0, size, buffer + offset); 
    
      offset += size; 
      width  /= 2; 
      height /= 2; 

      // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
      if(width < 1) width = 1;
      if(height < 1) height = 1;

   } 

   return textureID;
}

}
