#pragma once

#define WINDOW_WIDTH 1280															// Window width in pixels
#define WINDOW_HEIGHT 720															// Window height in pixels
#define RATIO float(float(WINDOW_WIDTH) / float(WINDOW_HEIGHT))						// Aspect ratio (16:9)
#define BLOCK_WIDTH 16																// Width of each block
#define BLOCK_HEIGHT 16																// Height of each block
#define BLOCKS_HORIZONTAL (WINDOW_WIDTH / BLOCK_WIDTH)								// Number of blocks horizontally
#define BLOCKS_VERTICAL (WINDOW_HEIGHT / BLOCK_HEIGHT)								// Number of blocks vertically
#define BLOCKS_EXTRA_HORIZONTAL 1													// Extra horizontal buffer blocks
#define BLOCKS_EXTRA_VERTICAL 1														// Extra vertical buffer blocks
#define BLOCKS_TOTAL_HORIZONTAL (BLOCKS_HORIZONTAL + (2 * BLOCKS_EXTRA_HORIZONTAL))	// Total horizontal blocks
#define BLOCKS_TOTAL_VERTICAL (BLOCKS_VERTICAL + (2 * BLOCKS_EXTRA_VERTICAL))		// Total vertical blocks
#define IMAGE_WIDTH (BLOCKS_TOTAL_HORIZONTAL * BLOCK_WIDTH)							// Width of entire render area
#define IMAGE_HEIGHT (BLOCKS_TOTAL_VERTICAL * BLOCK_HEIGHT)							// Height of entire render area