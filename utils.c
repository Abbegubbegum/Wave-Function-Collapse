#include "common.h"

// Vector Functions
bool v_eq_v(Vector2 v1, Vector2 v2)
{
    return (v1.x == v2.x && v1.y == v2.y);
};

float angle(Vector2 p1, Vector2 p2)
{
    return atan2f((SCREEN_HEIGHT - p2.y) - (SCREEN_HEIGHT - p1.y), p2.x - p1.x);
}

int sqr_dist(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy;
}

float dist(Vector2 p0, Vector2 p1)
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;

    return sqrt(x * x + y * y);
}

// CONVERSION FUNCTIONS //////////////////////////////////////////////////////////
int city_coord2index(int x, int y)
{
    return y * CITY_COLS + x;
}

Vector2 city_index2coord(int i)
{
    return (Vector2){
        .x = i % CITY_COLS,
        .y = i / CITY_COLS};
}

Vector2 city_coord2screen(int x, int y)
{
    return (Vector2){
        .x = x * CITY_TILE_SIZE,
        .y = y * CITY_TILE_SIZE,
    };
}

int house_coord2index(int x, int y)
{
    return y * HOUSE_COLS + x;
}

Vector2 house_index2coord(int i)
{
    return (Vector2){
        .x = i % HOUSE_COLS,
        .y = i / HOUSE_COLS,
    };
}

Vector2 house_coord2screen(int x, int y)
{
    return (Vector2){
        .x = HOUSE_X_EDGE_OFFSET + x * HOUSE_TILE_SIZE,
        .y = HOUSE_Y_EDGE_OFFSET + y * HOUSE_TILE_SIZE,
    };
}

// MAP PLACEMENT FUNCTIONS ///////////////////////////////////////////////////////////////////////////

bool city_tile_is_placable(wfc_tile_t tile, int x, int y)
{
    // CHECK ABOVE TILE
    if (city_tiles[city_coord2index(x, y - 1)].tile != NULL)
    {
        if (city_tiles[city_coord2index(x, y - 1)].tile->side_rules.bottom != tile.side_rules.top)
        {
            return false;
        }
    }

    // CHECK BELOW TILE
    if (city_tiles[city_coord2index(x, y + 1)].tile != NULL)
    {
        if (city_tiles[city_coord2index(x, y + 1)].tile->side_rules.top != tile.side_rules.bottom)
        {
            return false;
        }
    }

    // CHECK LEFT TILE
    if (city_tiles[city_coord2index(x - 1, y)].tile != NULL)
    {
        if (city_tiles[city_coord2index(x - 1, y)].tile->side_rules.right != tile.side_rules.left)
        {
            return false;
        }
    }

    // CHECK RIGHT TILE
    if (city_tiles[city_coord2index(x + 1, y)].tile != NULL)
    {
        if (city_tiles[city_coord2index(x + 1, y)].tile->side_rules.left != tile.side_rules.right)
        {
            return false;
        }
    }

    return true;
}

bool city_tile_is_connected(int x, int y)
{
    // CHECK TOP TILE
    if (city_tiles[city_coord2index(x, y - 1)].tile != NULL && city_tiles[city_coord2index(x, y - 1)].tile->side_rules.bottom != 0)
    {
        return true;
    }

    // CHECK BELOW TILE
    if (city_tiles[city_coord2index(x, y + 1)].tile != NULL && city_tiles[city_coord2index(x, y + 1)].tile->side_rules.top != 0)
    {
        return true;
    }

    // CHECK LEFT TILE
    if (city_tiles[city_coord2index(x - 1, y)].tile != NULL && city_tiles[city_coord2index(x - 1, y)].tile->side_rules.right != 0)
    {
        return true;
    }

    // CHECK RIGHT TILE
    if (city_tiles[city_coord2index(x + 1, y)].tile != NULL && city_tiles[city_coord2index(x + 1, y)].tile->side_rules.left != 0)
    {
        return true;
    }

    return false;
}