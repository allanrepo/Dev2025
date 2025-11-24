#pragma once
#include "Tile.h"
#include "Rect.h"
#include <optional>

namespace navigation
{
	namespace tile
	{
		enum class Direction
		{
			Up,
			Down,
			Left,
			Right,
			UpLeft,
			UpRight,
			DownLeft,
			DownRight
		};

		struct Constraints
		{
			// TODO:
			// tilelayer shouldn't have to be templated. assign a base tile class and just let it be inherited
			// right now this checks if tile.index = 1 for obstacles. again that is hard coded. must be tile.IsWalkable() from base/interface tile class
			// clean this function. it's messy, variables are repeatedly created, etc...
			// put this in separate file
			template<typename T>
			static bool IsPinchBlocked(
				const component::tile::TileLayer<T>& tilemap,
				const component::tile::TileCoord& curr,
				const component::tile::TileCoord& next,
				const spatial::SizeF& footprintSize,
				const spatial::SizeF& tileSize)
			{
				int NextRow = next.row;
				int NextCol = next.col;
				int currRow = curr.row;
				int currCol = curr.col;

				// Compute movement direction
				int dRow = NextRow - currRow;
				int dCol = NextCol - currCol;
				navigation::tile::Direction dir = navigation::tile::Direction::Down;
				if (dRow == -1 && dCol == 0)      dir = navigation::tile::Direction::Up;
				else if (dRow == 1 && dCol == 0)  dir = navigation::tile::Direction::Down;
				else if (dRow == 0 && dCol == -1) dir = navigation::tile::Direction::Left;
				else if (dRow == 0 && dCol == 1)  dir = navigation::tile::Direction::Right;
				else if (dRow == -1 && dCol == -1) dir = navigation::tile::Direction::UpLeft;
				else if (dRow == -1 && dCol == 1)  dir = navigation::tile::Direction::UpRight;
				else if (dRow == 1 && dCol == -1)  dir = navigation::tile::Direction::DownLeft;
				else if (dRow == 1 && dCol == 1)   dir = navigation::tile::Direction::DownRight;
				else
				{
					return false; // invalid neighbor
				}

				// quick sanity check for footprints larger than single tile but smaller than 2 tiles.
				// this checks if the footprint is moving at direction from current to next tile and checks if it can fit or not fit into the next tile
				// if cannot fit, can bail out here quickly. no point resolving since we know it's guaranteed to fail.
				{
					bool isFootprintWiderThanTile = footprintSize.width > tileSize.width;
					bool isFootprintTallerThanTile = footprintSize.height > tileSize.height;

					// check if next tile is pinched by obstacles
					if (isFootprintWiderThanTile)
					{
						bool isNextRightAdjTileWalkable = tilemap.IsValidTile(NextRow, NextCol + 1) && (tilemap.GetTile(NextRow, NextCol + 1).index != 1);
						bool isNextLefttAdjTileWalkable = tilemap.IsValidTile(NextRow, NextCol - 1) && (tilemap.GetTile(NextRow, NextCol - 1).index != 1);
						if (!isNextRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}
					}

					if (isFootprintTallerThanTile)
					{
						bool isNextBottomAdjTileWalkable = tilemap.IsValidTile(NextRow + 1, NextCol) && (tilemap.GetTile(NextRow + 1, NextCol).index != 1);
						bool isNextTopAdjTileWalkable = tilemap.IsValidTile(NextRow - 1, NextCol) && (tilemap.GetTile(NextRow - 1, NextCol).index != 1);
						if (!isNextBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}
					}

					// this checks for pinch where obstacles are not in same column or row
					switch (dir)
					{
					case navigation::tile::Direction::Up:
					{
						if (!isFootprintWiderThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 0N1
						// 1C0
						//  ^
						bool isCurrLeftAdjTileWalkable = tilemap.IsValidTile(currRow, currCol - 1) && (tilemap.GetTile(currRow, currCol - 1).index != 1);
						bool isNextRightAdjTileWalkable = tilemap.IsValidTile(NextRow, NextCol + 1) && (tilemap.GetTile(NextRow, NextCol + 1).index != 1);
						if (!isCurrLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 1N0
						// 0C1
						//  ^
						bool isCurrRightAdjTileWalkable = tilemap.IsValidTile(currRow, currCol + 1) && (tilemap.GetTile(currRow, currCol + 1).index != 1);
						bool isNextLefttAdjTileWalkable = tilemap.IsValidTile(NextRow, NextCol - 1) && (tilemap.GetTile(NextRow, NextCol - 1).index != 1);
						if (!isCurrRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 0N1
						// 0C0
						// 100
						//  ^
						bool isCurrBottomLeftAdjTileWalkable = tilemap.IsValidTile(currRow + 1, currCol - 1) && (tilemap.GetTile(currRow + 1, currCol - 1).index != 1);
						if (!isCurrBottomLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 1N0
						// 0C0
						// 001
						//  ^
						bool isCurrBottomRightAdjTileWalkable = tilemap.IsValidTile(currRow + 1, currCol + 1) && (tilemap.GetTile(currRow + 1, currCol + 1).index != 1);
						if (!isCurrBottomRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					case navigation::tile::Direction::Down:
					{
						if (!isFootprintWiderThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 1C0
						// 0N1
						bool isCurrLeftAdjTileWalkable = tilemap.IsValidTile(currRow, currCol - 1) && (tilemap.GetTile(currRow, currCol - 1).index != 1);
						bool isNextRightAdjTileWalkable = tilemap.IsValidTile(NextRow, NextCol + 1) && (tilemap.GetTile(NextRow, NextCol + 1).index != 1);
						if (!isCurrLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 0C1
						// 1N0
						bool isCurrRightAdjTileWalkable = tilemap.IsValidTile(currRow, currCol + 1) && (tilemap.GetTile(currRow, currCol + 1).index != 1);
						bool isNextLefttAdjTileWalkable = tilemap.IsValidTile(NextRow, NextCol - 1) && (tilemap.GetTile(NextRow, NextCol - 1).index != 1);
						if (!isCurrRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 001
						// 0C0
						// 1N0
						bool isCurrTopRightAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol + 1) && (tilemap.GetTile(currRow - 1, currCol + 1).index != 1);
						if (!isCurrTopRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 100
						// 0C0
						// 0N1
						bool isCurrTopLeftAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol - 1) && (tilemap.GetTile(currRow - 1, currCol - 1).index != 1);
						if (!isCurrTopLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					case navigation::tile::Direction::Right:
					{
						if (!isFootprintTallerThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  10
						// >CN
						//  01		
						bool isNextBottomAdjTileWalkable = tilemap.IsValidTile(NextRow + 1, NextCol) && (tilemap.GetTile(NextRow + 1, NextCol).index != 1);
						bool isCurrTopAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol) && (tilemap.GetTile(currRow - 1, currCol).index != 1);
						if (!isCurrTopAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  01
						// >CN
						//  10		
						bool isNextTopAdjTileWalkable = tilemap.IsValidTile(NextRow - 1, NextCol) && (tilemap.GetTile(NextRow - 1, NextCol).index != 1);
						bool isCurrBottomAdjTileWalkable = tilemap.IsValidTile(currRow + 1, currCol) && (tilemap.GetTile(currRow + 1, currCol).index != 1);
						if (!isCurrBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  100
						// >0CN
						//  001							
						bool isCurrTopLeftAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol - 1) && (tilemap.GetTile(currRow - 1, currCol - 1).index != 1);
						if (!isCurrTopLeftAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  001
						// >0CN
						//  100							
						bool isCurrBottomLeftAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol + 1) && (tilemap.GetTile(currRow - 1, currCol + 1).index != 1);
						if (!isCurrBottomLeftAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					case navigation::tile::Direction::Left:
					{
						if (!isFootprintTallerThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 01
						// NC<
						// 10		
						bool isNextBottomAdjTileWalkable = tilemap.IsValidTile(NextRow + 1, NextCol) && (tilemap.GetTile(NextRow + 1, NextCol).index != 1);
						bool isCurrTopAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol) && (tilemap.GetTile(currRow - 1, currCol).index != 1);
						if (!isCurrTopAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 10
						// NC<
						// 01		
						bool isNextTopAdjTileWalkable = tilemap.IsValidTile(NextRow - 1, NextCol) && (tilemap.GetTile(NextRow - 1, NextCol).index != 1);
						bool isCurrBottomAdjTileWalkable = tilemap.IsValidTile(currRow + 1, currCol) && (tilemap.GetTile(currRow + 1, currCol).index != 1);
						if (!isCurrBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 001
						// NC0<
						// 100							
						bool isCurrTopRighttAdjTileWalkable = tilemap.IsValidTile(currRow - 1, currCol + 1) && (tilemap.GetTile(currRow - 1, currCol + 1).index != 1);
						if (!isCurrTopRighttAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 100
						// NC0<
						// 001							
						bool isCurrBottomRightAdjTileWalkable = tilemap.IsValidTile(currRow + 1, currCol + 1) && (tilemap.GetTile(currRow + 1, currCol + 1).index != 1);
						if (!isCurrBottomRightAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					default:
						break;
					}
				}
			
				return false;
			}
		};
	};

	namespace tile1
	{
		enum class Direction
		{
			Up,
			Down,
			Left,
			Right,
			UpLeft,
			UpRight,
			DownLeft,
			DownRight
		};

		struct Constraints
		{
			// TODO:
			// tilelayer shouldn't have to be templated. assign a base tile class and just let it be inherited
			// right now this checks if tile.index = 1 for obstacles. again that is hard coded. must be tile.IsWalkable() from base/interface tile class
			// clean this function. it's messy, variables are repeatedly created, etc...
			// put this in separate file
			static bool IsPinchBlocked(
				const component::tile1::TileLayer& tilemap,
				const component::tile1::Tileset& tileset,
				const component::tile::TileCoord& curr,
				const component::tile::TileCoord& next,
				const spatial::SizeF& footprintSize,
				const spatial::SizeF& tileSize)
			{
				int NextRow = next.row;
				int NextCol = next.col;
				int currRow = curr.row;
				int currCol = curr.col;

				// Compute movement direction
				int dRow = NextRow - currRow;
				int dCol = NextCol - currCol;
				navigation::tile::Direction dir = navigation::tile::Direction::Down;
				if (dRow == -1 && dCol == 0)      dir = navigation::tile::Direction::Up;
				else if (dRow == 1 && dCol == 0)  dir = navigation::tile::Direction::Down;
				else if (dRow == 0 && dCol == -1) dir = navigation::tile::Direction::Left;
				else if (dRow == 0 && dCol == 1)  dir = navigation::tile::Direction::Right;
				else if (dRow == -1 && dCol == -1) dir = navigation::tile::Direction::UpLeft;
				else if (dRow == -1 && dCol == 1)  dir = navigation::tile::Direction::UpRight;
				else if (dRow == 1 && dCol == -1)  dir = navigation::tile::Direction::DownLeft;
				else if (dRow == 1 && dCol == 1)   dir = navigation::tile::Direction::DownRight;
				else
				{
					return false; // invalid neighbor
				}

				// quick sanity check for footprints larger than single tile but smaller than 2 tiles.
				// this checks if the footprint is moving at direction from current to next tile and checks if it can fit or not fit into the next tile
				// if cannot fit, can bail out here quickly. no point resolving since we know it's guaranteed to fail.
				{
					bool isFootprintWiderThanTile = footprintSize.width > tileSize.width;
					bool isFootprintTallerThanTile = footprintSize.height > tileSize.height;

					// check if next tile is pinched by obstacles
					if (isFootprintWiderThanTile)
					{
						bool isNextRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow, NextCol + 1);
						bool isNextLefttAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow, NextCol - 1);
						if (!isNextRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}
					}

					if (isFootprintTallerThanTile)
					{
						bool isNextBottomAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow + 1, NextCol);
						bool isNextTopAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow - 1, NextCol);
						if (!isNextBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}
					}

					// this checks for pinch where obstacles are not in same column or row
					switch (dir)
					{
					case navigation::tile::Direction::Up:
					{
						if (!isFootprintWiderThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 0N1
						// 1C0
						//  ^
						bool isCurrLeftAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow, currCol - 1);
						bool isNextRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow, NextCol + 1);
						if (!isCurrLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 1N0
						// 0C1
						//  ^
						bool isCurrRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow, currCol + 1);
						bool isNextLefttAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow, NextCol - 1);
						if (!isCurrRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 0N1
						// 0C0
						// 100
						//  ^
						bool isCurrBottomLeftAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow + 1, currCol - 1);
						if (!isCurrBottomLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 1N0
						// 0C0
						// 001
						//  ^
						bool isCurrBottomRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow + 1, currCol + 1);
						if (!isCurrBottomRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					case navigation::tile::Direction::Down:
					{
						if (!isFootprintWiderThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 1C0
						// 0N1
						bool isCurrLeftAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow, currCol - 1);
						bool isNextRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow, NextCol + 1);
						if (!isCurrLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 0C1
						// 1N0
						bool isCurrRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow, currCol + 1);
						bool isNextLefttAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow, NextCol - 1);
						if (!isCurrRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 001
						// 0C0
						// 1N0
						bool isCurrTopRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol + 1);
						if (!isCurrTopRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  v
						// 100
						// 0C0
						// 0N1
						bool isCurrTopLeftAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol - 1);
						if (!isCurrTopLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					case navigation::tile::Direction::Right:
					{
						if (!isFootprintTallerThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  10
						// >CN
						//  01		
						bool isNextBottomAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow + 1, NextCol);
						bool isCurrTopAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol);
						if (!isCurrTopAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  01
						// >CN
						//  10		
						bool isNextTopAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow - 1, NextCol);
						bool isCurrBottomAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow + 1, currCol);
						if (!isCurrBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  100
						// >0CN
						//  001							
						bool isCurrTopLeftAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol - 1);
						if (!isCurrTopLeftAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//  001
						// >0CN
						//  100							
						bool isCurrBottomLeftAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol + 1);
						if (!isCurrBottomLeftAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					case navigation::tile::Direction::Left:
					{
						if (!isFootprintTallerThanTile) break;

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 01
						// NC<
						// 10		
						bool isNextBottomAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow + 1, NextCol);
						bool isCurrTopAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol);
						if (!isCurrTopAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 10
						// NC<
						// 01		
						bool isNextTopAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, NextRow - 1, NextCol);
						bool isCurrBottomAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow + 1, currCol);
						if (!isCurrBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 001
						// NC0<
						// 100							
						bool isCurrTopRighttAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow - 1, currCol + 1);
						if (!isCurrTopRighttAdjTileWalkable && !isNextBottomAdjTileWalkable)
						{
							return true;
						}

						// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						// 100
						// NC0<
						// 001							
						bool isCurrBottomRightAdjTileWalkable = component::tile1::IsWalkable(tilemap, tileset, currRow + 1, currCol + 1);
						if (!isCurrBottomRightAdjTileWalkable && !isNextTopAdjTileWalkable)
						{
							return true;
						}
						break;
					}
					default:
						break;
					}
				}

				return false;
			}
		};
	};
};

namespace navigation
{
	namespace tile
	{
        // Defines the reference point for a footprint's position.
        // Used to interpret the footprint's RectF relative to its position.
        enum class Anchor 
        {
            Center,       // Position is the geometric center of the footprint
            TopLeft,      // Position is the top-left corner
            TopRight,     // Position is the top-right corner
            BottomLeft,   // Position is the bottom-left corner
            BottomRight   // Position is the bottom-right corner
        };

        // Represents the occupied area (footprint) of an actor on a tilemap.
        // Holds position, size, and anchor, and can produce a Rect<T> for collision/clearance checks.
        template<typename T = float>
        struct Footprint 
        {
            spatial::Pos<T> position;           // Anchor position in world space
            spatial::Size<T> size;              // Width/height of the footprint
			Anchor anchor = Anchor::Center;     // Anchor point for position interpretation

            // Returns the axis-aligned rectangle representing the footprint in world space.
            math::geometry::Rect<T> GetRect() const 
            {
                switch (anchor) 
                {
                case Anchor::Center:
                    return {
                        position.x - size.width / static_cast<T>(2),
                        position.y - size.height / static_cast<T>(2),
                        position.x + size.width / static_cast<T>(2),
                        position.y + size.height / static_cast<T>(2)
                    };
                case Anchor::TopLeft:
                    return {
                        position.x,
                        position.y,
                        position.x + size.width,
                        position.y + size.height
                    };
                case Anchor::TopRight:
                    return {
                        position.x - size.width,
                        position.y,
                        position.x,
                        position.y + size.height
                    };
                case Anchor::BottomLeft:
                    return {
                        position.x,
                        position.y - size.height,
                        position.x + size.width,
                        position.y
                    };
                case Anchor::BottomRight:
                    return {
                        position.x - size.width,
                        position.y - size.height,
                        position.x,
                        position.y
                    };
                default:
                    return {
                        position.x,
                        position.y,
                        position.x + size.width,
                        position.y + size.height
                    };
                }
            }
        };

        // Resolves footprint placement on a tile layer.
        // Performs validation (walkability, clearance) and resolution (nudging).
        class FootprintResolver 
		{
		public:
			enum class CostStrategy 
			{ 
				EuclidianSquared, 
				NormalizedEuclidianSquared, 
				GeometricDistance,	// not implemented yet
				Composite			// not implemented yet
			};

            // Construct a resolver with explicit parameters.
            FootprintResolver(
				std::function<bool(int, int)> isWalkable,								// Predicate to test walkability of a tile
				float epsilon = 0.01f,													// Tolerance for floating-point edge cases
				float maxHorizontalNudge = 1.0f,										// Maximum horizontal distance allowed for nudging
				float maxVerticalNudge = 1.0f,											// Maximum vertical distance allowed for nudging
                bool allowAnchorOverlap = false,										// Whether anchors may overlap blocked tiles
				CostStrategy costStrategy = CostStrategy::NormalizedEuclidianSquared	// cost calculation strategy
            ): 
                m_epsilon(epsilon),
				m_maxHorizontalNudge(maxHorizontalNudge),
				m_maxVerticalNudge(maxVerticalNudge),
                m_allowAnchorOverlap(allowAnchorOverlap),
				m_isWalkable(isWalkable),
				m_currCostStrategyFunc(nullptr)
            {
				// assign the cost strategy function 
				switch (costStrategy)
				{
				case CostStrategy::NormalizedEuclidianSquared:
					m_currCostStrategyFunc = &FootprintResolver::CostStrategy_NormalizedEuclidianSquared;
					break;
				case CostStrategy::EuclidianSquared:
					m_currCostStrategyFunc = &FootprintResolver::CostStrategy_EuclidianSquared;
					break;
				default:
					break;
				}
            }

			// Checks if a footprint is valid without resolution.
			template< typename T>
			bool IsValid(
				const component::tile::TileLayer<T>& tileLayer,         // Tile layer to operate on
				const spatial::SizeF& tileSize,                         // Size of each tile in world units
				const Footprint<float>& footPrint                      // Footprint bounds (position, size, anchor)
			) const
			{
				// get the footprint rect based on anchor which is assumed to be in world space (tilemap coordinate)
				math::geometry::RectF footPrintBounds = footPrint.GetRect();

				// calculate the tile coordinates covered by the footprint
				// right and bottom uses std::ceil and subtract -1 to follow half open method. this ensures footprint that are exact tile size covers only single tile
				int left =		static_cast<int>(std::floor(footPrintBounds.left / tileSize.width));
				int top =		static_cast<int>(std::floor(footPrintBounds.top / tileSize.height));
				int right =		static_cast<int>(std::ceil(footPrintBounds.right / tileSize.width)) - 1;
				int bottom =	static_cast<int>(std::ceil(footPrintBounds.bottom / tileSize.height)) - 1;

				// iterate through all tiles covered by the footprint
				for (int row = top; row <= bottom; ++row)
				{
					for (int col = left; col <= right; ++col)
					{
						component::tile::TileCoord tileCoord{ row, col };

						// Check if the tile is within the bounds of the tile layer
						if (!tileLayer.IsValidTile(row, col) || !m_isWalkable(row, col))
						{
							return false;
						}
					}
				}

				return true;
			}

            // Attempts to resolve placement of a footprint.
			template< typename T>
            bool TryResolveFirstFind(
                const component::tile::TileLayer<T>& tileLayer,
                const spatial::SizeF& tileSize,
                const Footprint<float>& footPrint,
				Footprint<float>& outFootPrint) const
			{
				// check if current position is safe
				if (IsValid(tileLayer, tileSize, footPrint))
				{
					outFootPrint = footPrint;
					return true; // already safe
				}

				// get the footprint rect based on anchor which is assumed to be in world space (tilemap coordinate)
				math::geometry::RectF footPrintBounds = footPrint.GetRect();

				// quick reject: if anchor position of footprint is inside an unwalkable tile, then no point trying to nudge
				if(!m_allowAnchorOverlap)
				{
					component::tile::TileCoord centerTileCoord
					{
						static_cast<int>(std::floor(footPrint.position.y / tileSize.height)),
						static_cast<int>(std::floor(footPrint.position.x / tileSize.width))
					};

					if (!tileLayer.IsValidTile(centerTileCoord) || !m_isWalkable(centerTileCoord.row, centerTileCoord.col))
					{
						return false; // center position is inside an unwalkable tile
					}
				}

				// get the top-left and bottom-right tile coords of the footprint at current position
				component::tile::TileCoord topLeftTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.top / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.left / tileSize.width))
				};

				component::tile::TileCoord bottomRightTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.bottom / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.right / tileSize.width))
				};

				// attempt to nudge along each edge of the footprint
				float nudgeY = 0;
				float nudgeX = 0;
				{
					// check footprint's top edge if safe to land. if not, nudge downwards incrementally outwards snapping at the tile edges
					nudgeY = 0;
					for (int step = 0; nudgeY < m_maxVerticalNudge; step++)
					{
						// calculate the nudge distance to align with bottom edge of the tile above. the + 1 is because we want the tile below. + epsilon to avoid being exactly on the edge
						nudgeY = (topLeftTileCoord.row + 1 + step) * tileSize.height - footPrintBounds.top + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.y += nudgeY;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}

					// check footprint's right edge if safe to land. if not, nudge leftwards incrementally outwards snapping at the tile edges
					nudgeX = 0;
					for (int step = 0; nudgeX < m_maxHorizontalNudge; step++)
					{
						// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
						nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - step) * tileSize.width + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.x -= nudgeX;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}

					// check footprint's bottom edge if safe to land. if not, nudge upwards incrementally outwards snapping at the tile edges
					nudgeY = 0;
					for (int step = 0; nudgeY < m_maxVerticalNudge; step++)
					{
						// calculate the nudge distance to align with top edge of the tile below. the + epsilon is to avoid being exactly on the edge
						nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - step) * tileSize.height + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.y -= nudgeY;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}

					// check footprint's left edge if safe to land. if not, nudge rightwards incrementally outwards snapping at the tile edges
					nudgeX = 0;
					for (int step = 0; nudgeX < m_maxHorizontalNudge; step++)
					{
						// calculate the nudge distance to align with right edge of the tile to the right. the + epsilon is to avoid being exactly on the edge
						nudgeX = (topLeftTileCoord.col + 1 + step) * tileSize.width - footPrintBounds.left + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.x += nudgeX;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}
				}

				// handle corner cases
				{
					// attempt to nudge the position of footprint at the top-left corner tile
					{
						// starting at tile where top-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where top-left of footprint is, nudge downwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								// start at row + 1 because row = 0 is the original position. we already know it's unsafe
								nudgeY = (topLeftTileCoord.row + row + 1) * tileSize.height - footPrintBounds.top + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y += nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									outFootPrint = nudgedFootprint;
									return true;
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-right corner tile
					{
						// starting at tile where bottom-right of footprint is, nudge leftwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

							// while at current tile column where bottom-right of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x -= nudgeX;
								nudgedFootprint.position.y -= nudgeY;
								
								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									outFootPrint = nudgedFootprint;
									return true;
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the top-right corner tile
					nudgeX = 0;
					for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
					{
						// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
						nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

						// while at current tile column where top-right of footprint is, nudge downwards incrementally outwards snapping at the tile edges
						nudgeY = 0;
						for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
						{
							nudgeY = (topLeftTileCoord.row + 1 + row) * tileSize.height - footPrintBounds.top + m_epsilon;

							// nudge footprint based on current tile column and row and see if safe to land
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.x -= nudgeX;
							nudgedFootprint.position.y += nudgeY;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								outFootPrint = nudgedFootprint;
								return true;
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-left corner tile
					{
						// starting at tile where bottom-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where bottom-left of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									outFootPrint = nudgedFootprint;
									return true;
								}
							}
						}
					}
				}

				return false;
			}
    
			// Attempts to resolve placement of a footprint.
			template< typename T>
			bool TryResolve(
				const component::tile::TileLayer<T>& tileLayer,
				const spatial::SizeF& tileSize,
				const Footprint<float>& footPrint,
				Footprint<float>& outFootPrint) const
			{
				// check if current position is safe
				if (IsValid(tileLayer, tileSize, footPrint))
				{
					outFootPrint = footPrint;
					return true; // already safe
				}

				// get the footprint rect based on anchor which is assumed to be in world space (tilemap coordinate)
				math::geometry::RectF footPrintBounds = footPrint.GetRect();

				// quick reject: if anchor position of footprint is inside an unwalkable tile, then no point trying to nudge
				if (!m_allowAnchorOverlap)
				{
					component::tile::TileCoord anchorTileCoord
					{
						static_cast<int>(std::floor(footPrint.position.y / tileSize.height)),
						static_cast<int>(std::floor(footPrint.position.x / tileSize.width))
					};

					if (!tileLayer.IsValidTile(anchorTileCoord) || !m_isWalkable(anchorTileCoord.row, anchorTileCoord.col))
					{
						return false; // center position is inside an unwalkable tile
					}
				}

				// get the top-left and bottom-right tile coords of the footprint at current position
				component::tile::TileCoord topLeftTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.top / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.left / tileSize.width))
				};

				component::tile::TileCoord bottomRightTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.bottom / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.right / tileSize.width))
				};

				// lambda wrapper for calling the current cost strategy 
				auto costOf = [this, &footPrint](const Footprint<float>& candidate) -> float 
					{
					return (this->*m_currCostStrategyFunc)(footPrint, candidate);
					};

				// used to compare candidate nudged footprint if this is the best
				float bestCost = std::numeric_limits<float>::infinity();
				auto consider = [&](const Footprint<float>& candidate, Footprint<float>& current)
					{
						float candidateCost = costOf(candidate);
						if (candidateCost < bestCost)
						{
							bestCost = candidateCost;
							current = candidate;
							return true;
						}
						return false;
					};


				// attempt to nudge along each edge of the footprint
				navigation::tile::Footprint bestFootprint;
				float nudgeY = 0;
				float nudgeX = 0;
				{
					nudgeY = 0;
					// DEBUG: TODO: 
					// max nudge is not being respected. i think what happens is that the latest calculated nudge can overlap max nudge
					// and wh
					for (int step = 0; nudgeY < m_maxVerticalNudge; step++)
					{
						// check footprint's top edge if safe to land. if not, nudge downwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with bottom edge of the tile above. the + 1 is because we want the tile below. + epsilon to avoid being exactly on the edge
							nudgeY = (topLeftTileCoord.row + 1 + step) * tileSize.height - footPrintBounds.top + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.y += nudgeY;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}

						// check footprint's bottom edge if safe to land. if not, nudge upwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with top edge of the tile below. the + epsilon is to avoid being exactly on the edge
							nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - step) * tileSize.height + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.y -= nudgeY;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}
					}

					nudgeX = 0;
					for (int step = 0; nudgeX < m_maxHorizontalNudge; step++)
					{
						// check footprint's right edge if safe to land. if not, nudge leftwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - step) * tileSize.width + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.x -= nudgeX;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}

						// check footprint's left edge if safe to land. if not, nudge rightwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with right edge of the tile to the right. the + epsilon is to avoid being exactly on the edge
							nudgeX = (topLeftTileCoord.col + 1 + step) * tileSize.width - footPrintBounds.left + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.x += nudgeX;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}
					}
				}

				// handle corner cases
				{
					// attempt to nudge the position of footprint at the top-left corner tile
					{
						// starting at tile where top-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where top-left of footprint is, nudge downwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								// start at row + 1 because row = 0 is the original position. we already know it's unsafe
								nudgeY = (topLeftTileCoord.row + row + 1) * tileSize.height - footPrintBounds.top + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y += nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-right corner tile
					{
						// starting at tile where bottom-right of footprint is, nudge leftwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

							// while at current tile column where bottom-right of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x -= nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the top-right corner tile
					{
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

							// while at current tile column where top-right of footprint is, nudge downwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = (topLeftTileCoord.row + 1 + row) * tileSize.height - footPrintBounds.top + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x -= nudgeX;
								nudgedFootprint.position.y += nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-left corner tile
					{
						// starting at tile where bottom-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where bottom-left of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}
				}				

				if (bestCost < std::numeric_limits<float>::infinity())
				{
					outFootPrint = bestFootprint;
					return true;
				}
				else
				{
					return false;
				}
			}

		private:
			// aliasing the cost strategy function signature. this function is to calculate the cost between original footprint position and candidate 
			using costStrategyFunc = float (FootprintResolver::*)(const Footprint<float>& original, const Footprint<float>& candidate) const;

			// NOTE: 
			// the epsilon is set to 0.01 by default. this might be too small for large footprints, and too big for small footprints
			// but in most cases, this is good enough. it is up to you to tune it to the right value depending on the size of your footprints
			float m_epsilon;

			// NOTE:
			// large footprints may cover large number of tiles. this could be a performance issue. handle with care and be aware of the size of the 
			// footprint and tile
			float m_maxHorizontalNudge;
			float m_maxVerticalNudge;
			bool m_allowAnchorOverlap;
			std::function<bool(int, int)> m_isWalkable;
			costStrategyFunc m_currCostStrategyFunc;

			// strategy to calculate cost. calculate the squared distance between position of original and candidate footprint
			float CostStrategy_EuclidianSquared(const Footprint<float>& original, const Footprint<float>& candidate) const
			{
				const float dx = candidate.position.x - original.position.x;
				const float dy = candidate.position.y - original.position.y;
				return (dx * dx + dy * dy);
			}

			// strategy to calculate cost. calculate the squared normalized distance between position of original and candidate footprint
			// this considers the aspect ratio of the footprint. for cases where aspect ratio is high e.g. width = 100, height = 10, this
			// uses the weight of the width and height in calculating the cost
			// halfwidth = original.size.width / 2
			// halfheight = original.size.height / 2
			// dx = (original.pos.x - candidate.pos.x) / halfwidth
			// dy = (original.pos.x - candidate.pos.y) / halfheight
			// cost = dx * dx + dy + dy
			float CostStrategy_NormalizedEuclidianSquared(const Footprint<float>& original, const Footprint<float>& candidate) const
			{
				const float halfWidth = original.size.width / 2;
				const float halfHeight = original.size.height / 2;

				const float dx = (candidate.position.x - original.position.x) / halfWidth;
				const float dy = (candidate.position.y - original.position.y) / halfHeight;

				return (dx * dx + dy * dy);
			}
		};
	}


	namespace tile1
	{
		// Defines the reference point for a footprint's position.
		// Used to interpret the footprint's RectF relative to its position.
		enum class Anchor
		{
			Center,       // Position is the geometric center of the footprint
			TopLeft,      // Position is the top-left corner
			TopRight,     // Position is the top-right corner
			BottomLeft,   // Position is the bottom-left corner
			BottomRight   // Position is the bottom-right corner
		};

		// Represents the occupied area (footprint) of an actor on a tilemap.
		// Holds position, size, and anchor, and can produce a Rect<T> for collision/clearance checks.
		template<typename T = float>
		struct Footprint
		{
			spatial::Pos<T> position;           // Anchor position in world space
			spatial::Size<T> size;              // Width/height of the footprint
			Anchor anchor = Anchor::Center;     // Anchor point for position interpretation

			// Returns the axis-aligned rectangle representing the footprint in world space.
			math::geometry::Rect<T> GetRect() const
			{
				switch (anchor)
				{
				case Anchor::Center:
					return {
						position.x - size.width / static_cast<T>(2),
						position.y - size.height / static_cast<T>(2),
						position.x + size.width / static_cast<T>(2),
						position.y + size.height / static_cast<T>(2)
					};
				case Anchor::TopLeft:
					return {
						position.x,
						position.y,
						position.x + size.width,
						position.y + size.height
					};
				case Anchor::TopRight:
					return {
						position.x - size.width,
						position.y,
						position.x,
						position.y + size.height
					};
				case Anchor::BottomLeft:
					return {
						position.x,
						position.y - size.height,
						position.x + size.width,
						position.y
					};
				case Anchor::BottomRight:
					return {
						position.x - size.width,
						position.y - size.height,
						position.x,
						position.y
					};
				default:
					return {
						position.x,
						position.y,
						position.x + size.width,
						position.y + size.height
					};
				}
			}
		};

		// Resolves footprint placement on a tile layer.
		// Performs validation (walkability, clearance) and resolution (nudging).
		class FootprintResolver
		{
		public:
			enum class CostStrategy
			{
				EuclidianSquared,
				NormalizedEuclidianSquared,
				GeometricDistance,	// not implemented yet
				Composite			// not implemented yet
			};

			// Construct a resolver with explicit parameters.
			FootprintResolver(
				std::function<bool(int, int)> isWalkable,								// Predicate to test walkability of a tile
				float epsilon = 0.01f,													// Tolerance for floating-point edge cases
				float maxHorizontalNudge = 1.0f,										// Maximum horizontal distance allowed for nudging
				float maxVerticalNudge = 1.0f,											// Maximum vertical distance allowed for nudging
				bool allowAnchorOverlap = false,										// Whether anchors may overlap blocked tiles
				CostStrategy costStrategy = CostStrategy::NormalizedEuclidianSquared	// cost calculation strategy
			) :
				m_epsilon(epsilon),
				m_maxHorizontalNudge(maxHorizontalNudge),
				m_maxVerticalNudge(maxVerticalNudge),
				m_allowAnchorOverlap(allowAnchorOverlap),
				m_isWalkable(isWalkable),
				m_currCostStrategyFunc(nullptr)
			{
				// assign the cost strategy function 
				switch (costStrategy)
				{
				case CostStrategy::NormalizedEuclidianSquared:
					m_currCostStrategyFunc = &FootprintResolver::CostStrategy_NormalizedEuclidianSquared;
					break;
				case CostStrategy::EuclidianSquared:
					m_currCostStrategyFunc = &FootprintResolver::CostStrategy_EuclidianSquared;
					break;
				default:
					break;
				}
			}

			// Checks if a footprint is valid without resolution.
			bool IsValid(
				const component::tile1::TileLayer& tileLayer,         // Tile layer to operate on
				const spatial::SizeF& tileSize,                         // Size of each tile in world units
				const Footprint<float>& footPrint                      // Footprint bounds (position, size, anchor)
			) const
			{
				// get the footprint rect based on anchor which is assumed to be in world space (tilemap coordinate)
				math::geometry::RectF footPrintBounds = footPrint.GetRect();

				// calculate the tile coordinates covered by the footprint
				// right and bottom uses std::ceil and subtract -1 to follow half open method. this ensures footprint that are exact tile size covers only single tile
				int left = static_cast<int>(std::floor(footPrintBounds.left / tileSize.width));
				int top = static_cast<int>(std::floor(footPrintBounds.top / tileSize.height));
				int right = static_cast<int>(std::ceil(footPrintBounds.right / tileSize.width)) - 1;
				int bottom = static_cast<int>(std::ceil(footPrintBounds.bottom / tileSize.height)) - 1;

				// iterate through all tiles covered by the footprint
				for (int row = top; row <= bottom; ++row)
				{
					for (int col = left; col <= right; ++col)
					{
						component::tile::TileCoord tileCoord{ row, col };

						// Check if the tile is within the bounds of the tile layer
						if (!tileLayer.IsValidTile(row, col) || !m_isWalkable(row, col))
						{
							return false;
						}
					}
				}

				return true;
			}

			// Attempts to resolve placement of a footprint.
			template< typename T>
			bool TryResolveFirstFind(
				const component::tile1::TileLayer& tileLayer,
				const spatial::SizeF& tileSize,
				const Footprint<float>& footPrint,
				Footprint<float>& outFootPrint) const
			{
				// check if current position is safe
				if (IsValid(tileLayer, tileSize, footPrint))
				{
					outFootPrint = footPrint;
					return true; // already safe
				}

				// get the footprint rect based on anchor which is assumed to be in world space (tilemap coordinate)
				math::geometry::RectF footPrintBounds = footPrint.GetRect();

				// quick reject: if anchor position of footprint is inside an unwalkable tile, then no point trying to nudge
				if (!m_allowAnchorOverlap)
				{
					component::tile::TileCoord centerTileCoord
					{
						static_cast<int>(std::floor(footPrint.position.y / tileSize.height)),
						static_cast<int>(std::floor(footPrint.position.x / tileSize.width))
					};

					if (!tileLayer.IsValidTile(centerTileCoord) || !m_isWalkable(centerTileCoord.row, centerTileCoord.col))
					{
						return false; // center position is inside an unwalkable tile
					}
				}

				// get the top-left and bottom-right tile coords of the footprint at current position
				component::tile::TileCoord topLeftTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.top / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.left / tileSize.width))
				};

				component::tile::TileCoord bottomRightTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.bottom / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.right / tileSize.width))
				};

				// attempt to nudge along each edge of the footprint
				float nudgeY = 0;
				float nudgeX = 0;
				{
					// check footprint's top edge if safe to land. if not, nudge downwards incrementally outwards snapping at the tile edges
					nudgeY = 0;
					for (int step = 0; nudgeY < m_maxVerticalNudge; step++)
					{
						// calculate the nudge distance to align with bottom edge of the tile above. the + 1 is because we want the tile below. + epsilon to avoid being exactly on the edge
						nudgeY = (topLeftTileCoord.row + 1 + step) * tileSize.height - footPrintBounds.top + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.y += nudgeY;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}

					// check footprint's right edge if safe to land. if not, nudge leftwards incrementally outwards snapping at the tile edges
					nudgeX = 0;
					for (int step = 0; nudgeX < m_maxHorizontalNudge; step++)
					{
						// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
						nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - step) * tileSize.width + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.x -= nudgeX;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}

					// check footprint's bottom edge if safe to land. if not, nudge upwards incrementally outwards snapping at the tile edges
					nudgeY = 0;
					for (int step = 0; nudgeY < m_maxVerticalNudge; step++)
					{
						// calculate the nudge distance to align with top edge of the tile below. the + epsilon is to avoid being exactly on the edge
						nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - step) * tileSize.height + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.y -= nudgeY;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}

					// check footprint's left edge if safe to land. if not, nudge rightwards incrementally outwards snapping at the tile edges
					nudgeX = 0;
					for (int step = 0; nudgeX < m_maxHorizontalNudge; step++)
					{
						// calculate the nudge distance to align with right edge of the tile to the right. the + epsilon is to avoid being exactly on the edge
						nudgeX = (topLeftTileCoord.col + 1 + step) * tileSize.width - footPrintBounds.left + m_epsilon;

						// apply the nudge
						Footprint nudgedFootprint = footPrint;
						nudgedFootprint.position.x += nudgeX;

						// check if the nudged position is safe
						if (IsValid(tileLayer, tileSize, nudgedFootprint))
						{
							outFootPrint = nudgedFootprint;
							return true;
						}
					}
				}

				// handle corner cases
				{
					// attempt to nudge the position of footprint at the top-left corner tile
					{
						// starting at tile where top-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where top-left of footprint is, nudge downwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								// start at row + 1 because row = 0 is the original position. we already know it's unsafe
								nudgeY = (topLeftTileCoord.row + row + 1) * tileSize.height - footPrintBounds.top + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y += nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									outFootPrint = nudgedFootprint;
									return true;
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-right corner tile
					{
						// starting at tile where bottom-right of footprint is, nudge leftwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

							// while at current tile column where bottom-right of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x -= nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									outFootPrint = nudgedFootprint;
									return true;
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the top-right corner tile
					nudgeX = 0;
					for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
					{
						// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
						nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

						// while at current tile column where top-right of footprint is, nudge downwards incrementally outwards snapping at the tile edges
						nudgeY = 0;
						for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
						{
							nudgeY = (topLeftTileCoord.row + 1 + row) * tileSize.height - footPrintBounds.top + m_epsilon;

							// nudge footprint based on current tile column and row and see if safe to land
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.x -= nudgeX;
							nudgedFootprint.position.y += nudgeY;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								outFootPrint = nudgedFootprint;
								return true;
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-left corner tile
					{
						// starting at tile where bottom-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where bottom-left of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									outFootPrint = nudgedFootprint;
									return true;
								}
							}
						}
					}
				}

				return false;
			}

			// Attempts to resolve placement of a footprint.
			bool TryResolve(
				const component::tile1::TileLayer& tileLayer,
				const spatial::SizeF& tileSize,
				const Footprint<float>& footPrint,
				Footprint<float>& outFootPrint) const
			{
				// check if current position is safe
				if (IsValid(tileLayer, tileSize, footPrint))
				{
					outFootPrint = footPrint;
					return true; // already safe
				}

				// get the footprint rect based on anchor which is assumed to be in world space (tilemap coordinate)
				math::geometry::RectF footPrintBounds = footPrint.GetRect();

				// quick reject: if anchor position of footprint is inside an unwalkable tile, then no point trying to nudge
				if (!m_allowAnchorOverlap)
				{
					component::tile::TileCoord anchorTileCoord
					{
						static_cast<int>(std::floor(footPrint.position.y / tileSize.height)),
						static_cast<int>(std::floor(footPrint.position.x / tileSize.width))
					};

					if (!tileLayer.IsValidTile(anchorTileCoord) || !m_isWalkable(anchorTileCoord.row, anchorTileCoord.col))
					{
						return false; // center position is inside an unwalkable tile
					}
				}

				// get the top-left and bottom-right tile coords of the footprint at current position
				component::tile::TileCoord topLeftTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.top / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.left / tileSize.width))
				};

				component::tile::TileCoord bottomRightTileCoord
				{
					static_cast<int>(std::floor(footPrintBounds.bottom / tileSize.height)),
					static_cast<int>(std::floor(footPrintBounds.right / tileSize.width))
				};

				// lambda wrapper for calling the current cost strategy 
				auto costOf = [this, &footPrint](const Footprint<float>& candidate) -> float
					{
						return (this->*m_currCostStrategyFunc)(footPrint, candidate);
					};

				// used to compare candidate nudged footprint if this is the best
				float bestCost = std::numeric_limits<float>::infinity();
				auto consider = [&](const Footprint<float>& candidate, Footprint<float>& current)
					{
						float candidateCost = costOf(candidate);
						if (candidateCost < bestCost)
						{
							bestCost = candidateCost;
							current = candidate;
							return true;
						}
						return false;
					};


				// attempt to nudge along each edge of the footprint
				Footprint bestFootprint;
				float nudgeY = 0;
				float nudgeX = 0;
				{
					nudgeY = 0;
					// DEBUG: TODO: 
					// max nudge is not being respected. i think what happens is that the latest calculated nudge can overlap max nudge
					// and wh
					for (int step = 0; nudgeY < m_maxVerticalNudge; step++)
					{
						// check footprint's top edge if safe to land. if not, nudge downwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with bottom edge of the tile above. the + 1 is because we want the tile below. + epsilon to avoid being exactly on the edge
							nudgeY = (topLeftTileCoord.row + 1 + step) * tileSize.height - footPrintBounds.top + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.y += nudgeY;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}

						// check footprint's bottom edge if safe to land. if not, nudge upwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with top edge of the tile below. the + epsilon is to avoid being exactly on the edge
							nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - step) * tileSize.height + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.y -= nudgeY;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}
					}

					nudgeX = 0;
					for (int step = 0; nudgeX < m_maxHorizontalNudge; step++)
					{
						// check footprint's right edge if safe to land. if not, nudge leftwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - step) * tileSize.width + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.x -= nudgeX;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}

						// check footprint's left edge if safe to land. if not, nudge rightwards incrementally outwards snapping at the tile edges
						{
							// calculate the nudge distance to align with right edge of the tile to the right. the + epsilon is to avoid being exactly on the edge
							nudgeX = (topLeftTileCoord.col + 1 + step) * tileSize.width - footPrintBounds.left + m_epsilon;

							// apply the nudge
							Footprint nudgedFootprint = footPrint;
							nudgedFootprint.position.x += nudgeX;

							// check if the nudged position is safe
							if (IsValid(tileLayer, tileSize, nudgedFootprint))
							{
								consider(nudgedFootprint, bestFootprint);
								break;
							}
						}
					}
				}

				// handle corner cases
				{
					// attempt to nudge the position of footprint at the top-left corner tile
					{
						// starting at tile where top-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where top-left of footprint is, nudge downwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								// start at row + 1 because row = 0 is the original position. we already know it's unsafe
								nudgeY = (topLeftTileCoord.row + row + 1) * tileSize.height - footPrintBounds.top + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y += nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-right corner tile
					{
						// starting at tile where bottom-right of footprint is, nudge leftwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

							// while at current tile column where bottom-right of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x -= nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the top-right corner tile
					{
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// calculate the nudge distance to align with left edge of the tile to the left. the + epsilon is to avoid being exactly on the edge
							nudgeX = footPrintBounds.right - (bottomRightTileCoord.col - col) * tileSize.width + m_epsilon;

							// while at current tile column where top-right of footprint is, nudge downwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = (topLeftTileCoord.row + 1 + row) * tileSize.height - footPrintBounds.top + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x -= nudgeX;
								nudgedFootprint.position.y += nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}

					// attempt to nudge the position of footprint at the bottom-left corner tile
					{
						// starting at tile where bottom-left of footprint is, nudge rightwards incrementally outwards snapping at the tile edges
						nudgeX = 0;
						for (int col = 0; nudgeX < m_maxHorizontalNudge; col++)
						{
							// start at col + 1 because col = 0 is the original position. we already know it's unsafe
							nudgeX = (topLeftTileCoord.col + col + 1) * tileSize.width - footPrintBounds.left + m_epsilon;

							// while at current tile column where bottom-left of footprint is, nudge upwards incrementally outwards snapping at the tile edges
							nudgeY = 0;
							for (int row = 0; nudgeY < m_maxVerticalNudge; row++)
							{
								nudgeY = footPrintBounds.bottom - (bottomRightTileCoord.row - row) * tileSize.height + m_epsilon;

								// nudge footprint based on current tile column and row and see if safe to land
								Footprint nudgedFootprint = footPrint;
								nudgedFootprint.position.x += nudgeX;
								nudgedFootprint.position.y -= nudgeY;

								// check if the nudged position is safe
								if (IsValid(tileLayer, tileSize, nudgedFootprint))
								{
									consider(nudgedFootprint, bestFootprint);
								}
							}
						}
					}
				}

				if (bestCost < std::numeric_limits<float>::infinity())
				{
					outFootPrint = bestFootprint;
					return true;
				}
				else
				{
					return false;
				}
			}

		private:
			// aliasing the cost strategy function signature. this function is to calculate the cost between original footprint position and candidate 
			using costStrategyFunc = float (FootprintResolver::*)(const Footprint<float>& original, const Footprint<float>& candidate) const;

			// NOTE: 
			// the epsilon is set to 0.01 by default. this might be too small for large footprints, and too big for small footprints
			// but in most cases, this is good enough. it is up to you to tune it to the right value depending on the size of your footprints
			float m_epsilon;

			// NOTE:
			// large footprints may cover large number of tiles. this could be a performance issue. handle with care and be aware of the size of the 
			// footprint and tile
			float m_maxHorizontalNudge;
			float m_maxVerticalNudge;
			bool m_allowAnchorOverlap;
			std::function<bool(int, int)> m_isWalkable;
			costStrategyFunc m_currCostStrategyFunc;

			// strategy to calculate cost. calculate the squared distance between position of original and candidate footprint
			float CostStrategy_EuclidianSquared(const Footprint<float>& original, const Footprint<float>& candidate) const
			{
				const float dx = candidate.position.x - original.position.x;
				const float dy = candidate.position.y - original.position.y;
				return (dx * dx + dy * dy);
			}

			// strategy to calculate cost. calculate the squared normalized distance between position of original and candidate footprint
			// this considers the aspect ratio of the footprint. for cases where aspect ratio is high e.g. width = 100, height = 10, this
			// uses the weight of the width and height in calculating the cost
			// halfwidth = original.size.width / 2
			// halfheight = original.size.height / 2
			// dx = (original.pos.x - candidate.pos.x) / halfwidth
			// dy = (original.pos.x - candidate.pos.y) / halfheight
			// cost = dx * dx + dy + dy
			float CostStrategy_NormalizedEuclidianSquared(const Footprint<float>& original, const Footprint<float>& candidate) const
			{
				const float halfWidth = original.size.width / 2;
				const float halfHeight = original.size.height / 2;

				const float dx = (candidate.position.x - original.position.x) / halfWidth;
				const float dy = (candidate.position.y - original.position.y) / halfHeight;

				return (dx * dx + dy * dy);
			}
		};
	}
}

