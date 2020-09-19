#ifndef LIFE_GAME_H
#define LIFE_GAME_H

#define GRID_SIZE     15
#define GRID_COUNT_X  10
#define GRID_COUNT_Y  10
#define GRID_POS_X(x) (GRID_SIZE * (x) + 1 + (x))
#define GRID_POS_Y(y) (GRID_SIZE * (y) + 1 + (y))
#define CANVAS_WIDTH  (GRID_SIZE * GRID_COUNT_X + GRID_COUNT_X + 1)
#define CANVAS_HEIGHT (GRID_SIZE * GRID_COUNT_Y + GRID_COUNT_Y + 1)

void the_game_of_life_widget(void);

#endif  // LIFE_GAME_H
