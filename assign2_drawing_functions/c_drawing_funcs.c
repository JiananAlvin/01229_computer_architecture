/*
 * C implementations of drawing functions (and helper functions)
 * CSF Assignment 2 MS1
 * Jianan Xu
 * jxu147@jhu.edu
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "drawing_funcs.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////

/*
 * Check x and y coordinates to determine whether 
 * they are in bounds in the specified image.
 *
 * Parameters:
 *   img     - pointer to struct Image
 *   x       - x coordinate (pixel column)
 *   y       - y coordinate (pixel row)
 *
 * Returns:
 *   1       - (x, y) is in bounds in the image
 *   0       - (x, y) is out bounds of the image
 */
int32_t in_bounds(struct Image *img, int32_t x, int32_t y) {
  return ((x >= 0 && x < img->width) && (y >= 0 && y < img->height)) ? 1 : 0;
}; 

/*
 * Compute the index of a pixel in an image’s data
 * array given its x and y coordinates.
 *
 * Parameters:
 *   img     - pointer to struct Image
 *   x       - x coordinate (pixel column)
 *   y       - y coordinate (pixel row)
 *
 * Returns:
 *   the index of a pixel in an image’s data array
 */
uint32_t compute_index(struct Image *img, int32_t x, int32_t y) {
  return (y*(img->width) + x);
};

/*
* Check whether a value is constrained to between min 
* and max. 
* 
* Parameters:
*   value   - a 32-bit signed integer
*   min     - lower bound
*   max     - upper bound
*
* Return:
*   1        - val is greater than equal to min and less 
*              than or equal to max
*   0        - val is less than min or greater than max
*/
int32_t clamp(int32_t val, int32_t min, int32_t max) {
  return (val >= min && val <= max);
}; 

/*
* Get the red component of a pixel color value.
*
* Parameters:
*   color   - uint32_t color value
*
* Return:
*   the red component of a pixel color value
*/
uint8_t get_r(uint32_t color) {
  return (color >> 24);
};

/*
* Get the green component of a pixel color value.
*
* Parameters:
*   color   - uint32_t color value
*
* Return:
*   the green component of a pixel color value
*/
uint8_t get_g(uint32_t color) {
  return (color >> 16);
};

/*
* Get the blue component of a pixel color value.
*
* Parameters:
*   color   - uint32_t color value
*
* Return:
*   the blue component of a pixel color value
*/
uint8_t get_b(uint32_t color) {
  return (color >> 8);
};

/*
* Get the alpha component of a pixel color value.
*
* Parameters:
*   color   - uint32_t color value
*
* Return:
*   the alpha component of a pixel color value
*/
uint8_t get_a(uint32_t color) {
  return color;
};

/*
* Blend foreground and background color component values
* using a specified alpha (opacity) value.
* 
* Parameters:
*   fg      - the foreground color component value
*   bg      - the background color component value
*   alpha   - the alpha value of the foreground color
*
* Return:
*   the blended color component value
*/
uint8_t blend_components(uint32_t fg, uint32_t bg, uint32_t alpha) {
  return ((alpha*fg + (255 - alpha)*bg)/255);
};

/*
* Blend foreground and background colors using the foreground
* color’s alpha value to produce an opaque color.
* 
* Parameters:
*   fg      - the foreground color value
*   bg      - the background color value
*
* Return:
*   the blended color value
*/
uint32_t blend_colors(uint32_t fg, uint32_t bg) {
  uint8_t r = blend_components(get_r(fg), get_r(bg), get_a(fg));
  uint8_t g = blend_components(get_g(fg), get_g(bg), get_a(fg));
  uint8_t b = blend_components(get_b(fg), get_b(bg), get_a(fg));
  uint8_t a = 255;
  return ((r << 24) + (g << 16) + (b << 8) + a);
};

/*
* draws a single pixel to a destination image, blending the 
* specified foregroudn color with the existing background color,
* at a specified pixel index.
*
* Parameters:
*   img     - pointer to struct Image
*   index   - the index of the destination pixel
*   color   - uint32_t color value
*/
void set_pixel(struct Image *img, uint32_t index, uint32_t color) {
  uint32_t bg = img->data[index];
  img->data[index] = blend_colors(color, bg);
};

/*
* Square an int64_t value.
*
* Parameters:
*   x       - an int64_t value
*
* Return:
*   the result of squaring an int64_t value
*/
int64_t square(int64_t x) {
  return x * x;
};

/*
* Get the sum of the squares of the x and y distances
* between two points.
*
* Parameters:
*   x1      - x coordinate of one point
*   y1      - y coordinate of one point
*   x2      - x coordinate the other point
*   y2      - y coordinate the other point
*
* Return:
*    the sum of the squares of the x and y distances 
*    between two points
*/
int64_t square_dist(int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
  return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
};

////////////////////////////////////////////////////////////////////////
// API functions
////////////////////////////////////////////////////////////////////////

//
// Draw a pixel.
//
// Parameters:
//   img   - pointer to struct Image
//   x     - x coordinate (pixel column)
//   y     - y coordinate (pixel row)
//   color - uint32_t color value
//
void draw_pixel(struct Image *img, int32_t x, int32_t y, uint32_t color) {
  if (in_bounds(img, x, y)) {
    uint32_t index = compute_index(img, x, y);
    set_pixel(img, index, color);
  }
}

//
// Draw a rectangle.
// The rectangle has rect->x,rect->y as its upper left corner,
// is rect->width pixels wide, and rect->height pixels high.
//
// Parameters:
//   img     - pointer to struct Image
//   rect    - pointer to struct Rect
//   color   - uint32_t color value
//
void draw_rect(struct Image *img,
               const struct Rect *rect,
               uint32_t color) {
  for (int px = rect->x; px < rect->x + rect->width; px++) {
    for (int py = rect->y; py < rect->y + rect->height; py++) {
      draw_pixel(img, px, py, color);
    }
  }
}

//
// Draw a circle.
// The circle has x,y as its center and has r as its radius.
//
// Parameters:
//   img     - pointer to struct Image
//   x       - x coordinate of circle's center
//   y       - y coordinate of circle's center
//   r       - radius of circle
//   color   - uint32_t color value
//
void draw_circle(struct Image *img,
                 int32_t x, int32_t y, int32_t r,
                 uint32_t color) {
  for (int px = 0; px < img->width; px++) {
    for (int py = 0; py < img->height; py++) {
      if (square_dist(px, py, x, y) <= r * r) {
        draw_pixel(img, px, py, color);
      }
    }
  }
}  

//
// Draw a tile by copying all pixels in the region
// enclosed by the tile parameter in the tilemap image
// to the specified x/y coordinates of the destination image.
// No blending of the tile pixel colors with the background
// colors should be done.
//
// Parameters:
//   img     - pointer to Image (dest image)
//   x       - x coordinate of location where tile should be copied
//   y       - y coordinate of location where tile should be copied
//   tilemap - pointer to Image (the tilemap)
//   tile    - pointer to Rect (the tile)
//
void draw_tile(struct Image *img,
               int32_t x, int32_t y,
               struct Image *tilemap,
               const struct Rect *tile) {
  if (clamp(tile->x, 0, tilemap->width - 1) 
    && clamp(tile->x + tile->width - 1, 0, tilemap->width - 1)
    && clamp(tile->y, 0, tilemap->height - 1)
    && clamp(tile->y + tile->height - 1, 0, tilemap->height - 1)) {
    int32_t deltax = tile->x - x;
    int32_t deltay = tile->y - y;
    for (int px = tile->x; px < tile->x + tile->width; px++) {
      for (int py = tile->y; py < tile->y + tile->height; py++) {
        if (in_bounds(img, px - deltax, py - deltay)) {
          uint32_t index_tailmap = compute_index(tilemap, px, py);
          uint32_t index_destination = compute_index(img, px - deltax, py - deltay);
          img->data[index_destination] = tilemap->data[index_tailmap];
        }
      }
    } 
  } 
}

//
// Draw a sprite by copying all pixels in the region
// enclosed by the sprite parameter in the spritemap image
// to the specified x/y coordinates of the destination image.
// The alpha values of the sprite pixels should be used to
// blend the sprite pixel colors with the background
// pixel colors.
//
// Parameters:
//   img       - pointer to Image (dest image)
//   x         - x coordinate of location where sprite should be copied
//   y         - y coordinate of location where sprite should be copied
//   spritemap - pointer to Image (the spritemap)
//   sprite    - pointer to Rect (the sprite)
//
void draw_sprite(struct Image *img,
                 int32_t x, int32_t y,
                 struct Image *spritemap,
                 const struct Rect *sprite) {
  if (clamp(sprite->x, 0, spritemap->width - 1) 
    && clamp(sprite->x + sprite->width - 1, 0, spritemap->width - 1)
    && clamp(sprite->y, 0, spritemap->height - 1)
    && clamp(sprite->y + sprite->height - 1, 0, spritemap->height - 1)) {
    int32_t deltax = sprite->x - x;
    int32_t deltay = sprite->y - y;
    for (int px = sprite->x; px < sprite->x + sprite->width; px++) {
      for (int py = sprite->y; py < sprite->y + sprite->height; py++) {
        if (in_bounds(img, px - deltax, py - deltay)) {
          uint32_t index_spritemap = compute_index(spritemap, px, py);
          uint32_t index_destination = compute_index(img, px - deltax, py - deltay);
          img->data[index_destination] = blend_colors(spritemap->data[index_spritemap], img->data[index_destination]);
        }
      }
    } 
  } 
}
