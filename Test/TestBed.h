#pragma once
#include "Camera.h"
#include "Engine.h"
#include "Input.h"
#include "Logger.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include "ActorState.h"
#include "IActor.h"
#include "ActorFactory.h"
#include "DrawableSurface.h"
#include "ImageSurface.h"
#include "DX11ImageFileHelper.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>
#include <stdexcept>
#include "Pos.h"
#include "PathFinder.h"
#include "FootprintResolver.h"

namespace test
{
	class TestBed
	{
	private:
		struct Tile
		{
			int index;
		};
		engine::Engine m_engine;
		component::tile::TileLayer<Tile> m_tilemap;
		spatial::Camera m_camera;
		std::shared_ptr<graphics::resource::IFontAtlas> m_fontSmall;
		std::shared_ptr<graphics::resource::IFontAtlas> m_fontLarge;

		bool m_searchDone = false;

		spatial::SizeF m_tileSize;

		spatial::PosF m_lastMousePos;
		spatial::SizeF m_footPrintSize;

		navigation::tile::Footprint<float> m_startFP;
		navigation::tile::Footprint<float> m_goalFP;
		navigation::tile::PathFinder m_pathFinder;
		std::vector<component::tile::TileCoord> m_path;
		std::vector<spatial::PosF> m_wayPoints;
		navigation::tile::FootprintResolver m_footprintResolver;

	public:
		TestBed() :
			m_engine("DirectX11", "Batch"),
			m_camera({ 50, 50, 1024, 768 }),
			m_tileSize{ 50, 50 },
			m_footPrintSize{ 480, 480 },
			m_footprintResolver
			(
				[this](int row, int col) -> bool
				{
					Tile tile = m_tilemap.GetTile(row, col);
					return tile.index != 1; // walkable if index is not 1
				},
				0.1f, m_tileSize.width / 0.1f, m_tileSize.height / 0.1f, true
			),
			m_pathFinder(
#if true
				[this](int currRow, int currCol, int NextRow, int NextCol) -> bool
				{
					// quick check if the tile itself is walkable
					Tile tile = m_tilemap.GetTile(NextRow, NextCol);
					if (tile.index == 1) return false; // unwalkable

					if (navigation::tile::Constraints::IsPinchBlocked(
						m_tilemap, 
						{currRow, currCol}, 
						{NextRow, NextCol}, 
						m_startFP.size, 
						m_tileSize
					))
					{
						return false;
					}
					{
						//// Compute movement direction
						//int dRow = NextRow - currRow;
						//int dCol = NextCol - currCol;
						//navigation::tile::Direction dir = navigation::tile::Direction::Down;
						//if (dRow == -1 && dCol == 0)      dir = navigation::tile::Direction::Up;
						//else if (dRow == 1 && dCol == 0)  dir = navigation::tile::Direction::Down;
						//else if (dRow == 0 && dCol == -1) dir = navigation::tile::Direction::Left;
						//else if (dRow == 0 && dCol == 1)  dir = navigation::tile::Direction::Right;
						//else if (dRow == -1 && dCol == -1) dir = navigation::tile::Direction::UpLeft;
						//else if (dRow == -1 && dCol == 1)  dir = navigation::tile::Direction::UpRight;
						//else if (dRow == 1 && dCol == -1)  dir = navigation::tile::Direction::DownLeft;
						//else if (dRow == 1 && dCol == 1)   dir = navigation::tile::Direction::DownRight;
						//else
						//{
						//	return false; // invalid neighbor
						//}

						//// quick sanity check for footprints larger than single tile but smaller than 2 tiles.
						//// this checks if the footprint is moving at direction from current to next tile and checks if it can fit or not fit into the next tile
						//// if cannot fit, can bail out here quickly. no point resolving since we know it's guaranteed to fail.
						//{
						//	bool isFootprintWiderThanTile = m_startFP.size.width > m_tileSize.width;
						//	bool isFootprintTallerThanTile = m_startFP.size.height > m_tileSize.height;

						//	// check if next tile is pinched by obstacles
						//	if (isFootprintWiderThanTile)
						//	{
						//		bool isNextRightAdjTileWalkable = m_tilemap.IsValidTile(NextRow, NextCol + 1) && (m_tilemap.GetTile(NextRow, NextCol + 1).index != 1);
						//		bool isNextLefttAdjTileWalkable = m_tilemap.IsValidTile(NextRow, NextCol - 1) && (m_tilemap.GetTile(NextRow, NextCol - 1).index != 1);
						//		if (!isNextRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						//		{
						//			return false;
						//		}
						//	}

						//	if (isFootprintTallerThanTile)
						//	{
						//		bool isNextBottomAdjTileWalkable = m_tilemap.IsValidTile(NextRow + 1, NextCol) && (m_tilemap.GetTile(NextRow + 1, NextCol).index != 1);
						//		bool isNextTopAdjTileWalkable = m_tilemap.IsValidTile(NextRow - 1, NextCol) && (m_tilemap.GetTile(NextRow - 1, NextCol).index != 1);
						//		if (!isNextBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						//		{
						//			return false;
						//		}
						//	}

						//	// this checks for pinch where obstacles are not in same column or row
						//	switch (dir)
						//	{
						//	case navigation::tile::Direction::Up:
						//	{
						//		if (!isFootprintWiderThanTile) break;

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 0N1
						//		// 1C0
						//		//  ^
						//		bool isCurrLeftAdjTileWalkable = m_tilemap.IsValidTile(currRow, currCol - 1) && (m_tilemap.GetTile(currRow, currCol - 1).index != 1);
						//		bool isNextRightAdjTileWalkable = m_tilemap.IsValidTile(NextRow, NextCol + 1) && (m_tilemap.GetTile(NextRow, NextCol + 1).index != 1);
						//		if (!isCurrLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 1N0
						//		// 0C1
						//		//  ^
						//		bool isCurrRightAdjTileWalkable = m_tilemap.IsValidTile(currRow, currCol + 1) && (m_tilemap.GetTile(currRow, currCol + 1).index != 1);
						//		bool isNextLefttAdjTileWalkable = m_tilemap.IsValidTile(NextRow, NextCol - 1) && (m_tilemap.GetTile(NextRow, NextCol - 1).index != 1);
						//		if (!isCurrRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 0N1
						//		// 0C0
						//		// 100
						//		//  ^
						//		bool isCurrBottomLeftAdjTileWalkable = m_tilemap.IsValidTile(currRow + 1, currCol - 1) && (m_tilemap.GetTile(currRow + 1, currCol - 1).index != 1);
						//		if (!isCurrBottomLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 1N0
						//		// 0C0
						//		// 001
						//		//  ^
						//		bool isCurrBottomRightAdjTileWalkable = m_tilemap.IsValidTile(currRow + 1, currCol + 1) && (m_tilemap.GetTile(currRow + 1, currCol + 1).index != 1);
						//		if (!isCurrBottomRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						//		{
						//			return false;
						//		}
						//		break;
						//	}
						//	case navigation::tile::Direction::Down:
						//	{
						//		if (!isFootprintWiderThanTile) break;

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  v
						//		// 1C0
						//		// 0N1
						//		bool isCurrLeftAdjTileWalkable = m_tilemap.IsValidTile(currRow, currCol - 1) && (m_tilemap.GetTile(currRow, currCol - 1).index != 1);
						//		bool isNextRightAdjTileWalkable = m_tilemap.IsValidTile(NextRow, NextCol + 1) && (m_tilemap.GetTile(NextRow, NextCol + 1).index != 1);
						//		if (!isCurrLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  v
						//		// 0C1
						//		// 1N0
						//		bool isCurrRightAdjTileWalkable = m_tilemap.IsValidTile(currRow, currCol + 1) && (m_tilemap.GetTile(currRow, currCol + 1).index != 1);
						//		bool isNextLefttAdjTileWalkable = m_tilemap.IsValidTile(NextRow, NextCol - 1) && (m_tilemap.GetTile(NextRow, NextCol - 1).index != 1);
						//		if (!isCurrRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  v
						//		// 001
						//		// 0C0
						//		// 1N0
						//		bool isCurrTopRightAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol + 1) && (m_tilemap.GetTile(currRow - 1, currCol + 1).index != 1);
						//		if (!isCurrTopRightAdjTileWalkable && !isNextLefttAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  v
						//		// 100
						//		// 0C0
						//		// 0N1
						//		bool isCurrTopLeftAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol - 1) && (m_tilemap.GetTile(currRow - 1, currCol - 1).index != 1);
						//		if (!isCurrTopLeftAdjTileWalkable && !isNextRightAdjTileWalkable)
						//		{
						//			return false;
						//		}
						//		break;
						//	}
						//	case navigation::tile::Direction::Right:
						//	{
						//		if (!isFootprintTallerThanTile) break;

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  10
						//		// >CN
						//		//  01		
						//		bool isNextBottomAdjTileWalkable = m_tilemap.IsValidTile(NextRow + 1, NextCol) && (m_tilemap.GetTile(NextRow + 1, NextCol).index != 1);
						//		bool isCurrTopAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol) && (m_tilemap.GetTile(currRow - 1, currCol).index != 1);
						//		if (!isCurrTopAdjTileWalkable && !isNextBottomAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  01
						//		// >CN
						//		//  10		
						//		bool isNextTopAdjTileWalkable = m_tilemap.IsValidTile(NextRow - 1, NextCol) && (m_tilemap.GetTile(NextRow - 1, NextCol).index != 1);
						//		bool isCurrBottomAdjTileWalkable = m_tilemap.IsValidTile(currRow + 1, currCol) && (m_tilemap.GetTile(currRow + 1, currCol).index != 1);
						//		if (!isCurrBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  100
						//		// >0CN
						//		//  001							
						//		bool isCurrTopLeftAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol - 1) && (m_tilemap.GetTile(currRow - 1, currCol - 1).index != 1);
						//		if (!isCurrTopLeftAdjTileWalkable && !isNextBottomAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		//  001
						//		// >0CN
						//		//  100							
						//		bool isCurrBottomLeftAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol + 1) && (m_tilemap.GetTile(currRow - 1, currCol + 1).index != 1);
						//		if (!isCurrBottomLeftAdjTileWalkable && !isNextTopAdjTileWalkable)
						//		{
						//			return false;
						//		}
						//		break;
						//	}
						//	case navigation::tile::Direction::Left:
						//	{
						//		if (!isFootprintTallerThanTile) break;

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 01
						//		// NC<
						//		// 10		
						//		bool isNextBottomAdjTileWalkable = m_tilemap.IsValidTile(NextRow + 1, NextCol) && (m_tilemap.GetTile(NextRow + 1, NextCol).index != 1);
						//		bool isCurrTopAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol) && (m_tilemap.GetTile(currRow - 1, currCol).index != 1);
						//		if (!isCurrTopAdjTileWalkable && !isNextBottomAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 10
						//		// NC<
						//		// 01		
						//		bool isNextTopAdjTileWalkable = m_tilemap.IsValidTile(NextRow - 1, NextCol) && (m_tilemap.GetTile(NextRow - 1, NextCol).index != 1);
						//		bool isCurrBottomAdjTileWalkable = m_tilemap.IsValidTile(currRow + 1, currCol) && (m_tilemap.GetTile(currRow + 1, currCol).index != 1);
						//		if (!isCurrBottomAdjTileWalkable && !isNextTopAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 001
						//		// NC0<
						//		// 100							
						//		bool isCurrTopRighttAdjTileWalkable = m_tilemap.IsValidTile(currRow - 1, currCol + 1) && (m_tilemap.GetTile(currRow - 1, currCol + 1).index != 1);
						//		if (!isCurrTopRighttAdjTileWalkable && !isNextBottomAdjTileWalkable)
						//		{
						//			return false;
						//		}

						//		// if tile pattern is like below, direction going up, and footprint size > tile size, we can't pass
						//		// 100
						//		// NC0<
						//		// 001							
						//		bool isCurrBottomRightAdjTileWalkable = m_tilemap.IsValidTile(currRow + 1, currCol + 1) && (m_tilemap.GetTile(currRow + 1, currCol + 1).index != 1);
						//		if (!isCurrBottomRightAdjTileWalkable && !isNextTopAdjTileWalkable)
						//		{
						//			return false;
						//		}
						//		break;
						//	}
						//	default:
						//		break;
						//	}
						//}
					}

					// create a footprint where it is located at the center of this tile coordinate. let's set this as default position of the footprint
					// if this position is safe (no collision with any obstacle) then this tile is walkable. 
					// if not safe (collides with an obstacle), we'll try to nudge
					navigation::tile::Footprint<float> fp;
					fp.anchor = navigation::tile::Anchor::Center;
					fp.position.x = NextCol * m_tileSize.width + m_tileSize.width / 2;
					fp.position.y = NextRow * m_tileSize.height + m_tileSize.height / 2;
					fp.size = m_startFP.size;

					if (m_footprintResolver.IsValid(m_tilemap, m_tileSize, fp))
					{
						return true;
					}

					// try to nudge if possible
					navigation::tile::Footprint outFP;
					if (m_footprintResolver.TryResolve(m_tilemap, m_tileSize, fp, outFP))
					{
						component::tile::TileCoord outTC = GetTileCoordFromMapPosition(m_tilemap, m_tileSize, outFP.position);
						return (outTC.row == NextRow && outTC.col == NextCol);
					}

					return false;
				},
#else
				[this](int row, int col) -> bool
				{
					Tile tile = m_tilemap.GetTile(NextRow, NextCol);
					return tile.index != 1; // walkable if index is not 1
				},
#endif	
				1000,
				false,
				false
			)
		{
			m_engine.OnStart += event::Handler(this, &TestBed::OnStart);
			m_engine.OnUpdate += event::Handler(this, &TestBed::OnUpdate);
			m_engine.OnRender += event::Handler(this, &TestBed::OnRender);
			m_engine.OnResize += event::Handler(this, &TestBed::OnResize);

			input::Input::Instance().OnKeyDown += event::Handler(this, &TestBed::OnKeyDown);
			input::Input::Instance().OnMouseDown += event::Handler(this, &TestBed::OnMouseDown);
			input::Input::Instance().OnMouseMove += event::Handler(this, &TestBed::OnMouseMove);
			input::Input::Instance().OnMouseUp += event::Handler(this, &TestBed::OnMouseUp);

			m_engine.Run();
		}

		void OnResize(size_t width, size_t height)
		{
		}

		void OnMouseMove(int x, int y)
		{
			m_lastMousePos = { static_cast<float>(x), static_cast<float>(y) };
		}

		void OnMouseDown(int btn, int x, int y)
		{
			if (btn == 1) // left button
			{
				m_startFP.position = m_camera.ScreenToWorld({ (float)x, (float)y });
			}
			else if (btn == 2) // right button
			{
				m_goalFP.position = m_camera.ScreenToWorld({ (float)x, (float)y });
			}

			component::tile::TileCoord tileStartFP = GetTileCoordFromMapPosition(m_tilemap, m_tileSize, m_startFP.position); // debug only, useless
			component::tile::TileCoord tileGoalFP = GetTileCoordFromMapPosition(m_tilemap, m_tileSize, m_goalFP.position); // debug only, useless
			tileGoalFP;
		}

		void OnMouseUp(int btn, int x, int y)
		{
		}

		void OnKeyDown(int key)
		{

			if (key == 32) // space key
			{
			}
			if (key == 49) // 1
			{
				// set selected tile as obstacle
				spatial::PosF worldPos = m_camera.ScreenToWorld(m_lastMousePos);
				component::tile::TileCoord tileCoord
				{
					static_cast<int>(worldPos.y / m_tileSize.height),
					static_cast<int>(worldPos.x / m_tileSize.width)
				};
				if (tileCoord.row >= 0 && tileCoord.row < m_tilemap.GetHeight() && tileCoord.col >= 0 && tileCoord.col < m_tilemap.GetWidth())
				{
					Tile tile = m_tilemap.GetTile(tileCoord.row, tileCoord.col);
					tile.index = 1; // obstacle
					m_tilemap.SetTile(tileCoord.row, tileCoord.col, tile);
				}
			}
			if (key == 50) // 2
			{
				// set selected tile as obstacle
				spatial::PosF worldPos = m_camera.ScreenToWorld(m_lastMousePos);
				component::tile::TileCoord tileCoord
				{
					static_cast<int>(worldPos.y / m_tileSize.height),
					static_cast<int>(worldPos.x / m_tileSize.width)
				};
				if (tileCoord.row >= 0 && tileCoord.row < m_tilemap.GetHeight() && tileCoord.col >= 0 && tileCoord.col < m_tilemap.GetWidth())
				{
					Tile tile = m_tilemap.GetTile(tileCoord.row, tileCoord.col);
					tile.index = 0; // walkable
					m_tilemap.SetTile(tileCoord.row, tileCoord.col, tile);
				}
			}
			if (key == 51) // 3
			{
				// remove all obstacles
				for (int row = 0; row < m_tilemap.GetHeight(); row++)
				{
					for (int col = 0; col < m_tilemap.GetWidth(); col++)
					{
						Tile tile = m_tilemap.GetTile(row, col);
						if (tile.index == 1)
						{
							tile.index = 0;
							m_tilemap.SetTile(row, col, tile);
						}
					}
				}
			}
			if (key == 52) // 4
			{

			}
			if (key == 53)
			{
				m_startFP.size.width += 5;
				m_goalFP.size.width += 5;
			}
			if (key == 54)
			{
				m_startFP.size.width -= 5;
				m_goalFP.size.width -= 5;
			}
			if (key == 55)
			{
				m_startFP.size.height += 5;
				m_goalFP.size.height += 5;
			}

			if (key == 56)
			{
				m_startFP.size.height -= 5;
				m_goalFP.size.height -= 5;
			}
		}

		void OnStart()
		{
			SetTileLayer(m_tilemap, 16, 16, Tile{ 0 });

			m_camera.SetViewport(
				{
					50,
					50,
					50 + m_tilemap.GetWidth() * m_tileSize.width,
					50 + m_tilemap.GetHeight() * m_tileSize.height
				}
			);

			m_camera.SetWorldSize(
				m_tilemap.GetWidth() * m_tileSize.width,
				m_tilemap.GetHeight() * m_tileSize.height
			);

			m_engine.GetRenderer().SetClipRegion(m_camera.GetViewport());
			m_engine.GetRenderer().EnableClipping(true);

			// create utility font atlas
			m_fontSmall = std::make_shared<graphics::resource::FontAtlas>(graphics::factory::TextureFactory::Create());
			m_fontSmall->Initialize("Arial", 12);

			m_fontLarge = std::make_shared<graphics::resource::FontAtlas>(graphics::factory::TextureFactory::Create());
			m_fontLarge->Initialize("Arial", 24);

			m_goalFP.size = m_startFP.size = { 75, 75 };
			m_goalFP.anchor = m_startFP.anchor = navigation::tile::Anchor::Center;
			m_goalFP.position = m_startFP.position = { -10000, -10000 };
		}

		void OnUpdate(float delta)
		{
			// resolve positions of start and goal footprints in case they collide with obstacles
			{
				// DEBUG:: we just set random value here. fix this so that we set appropriate value
				m_footprintResolver.TryResolve(m_tilemap, m_tileSize, m_startFP, m_startFP);
				m_footprintResolver.TryResolve(m_tilemap, m_tileSize, m_goalFP, m_goalFP);
			}

			// get the path
			{
				component::tile::TileCoord tileStartFP = GetTileCoordFromMapPosition(m_tilemap, m_tileSize, m_startFP.position);
				component::tile::TileCoord tileGoalFP = GetTileCoordFromMapPosition(m_tilemap, m_tileSize, m_goalFP.position);

				if (m_tilemap.IsValidTile(tileStartFP) && m_tilemap.IsValidTile(tileGoalFP))
				{
					m_pathFinder.FindPath(
						math::geometry::Rect<int>{ 0, 0, m_tilemap.GetWidth(), m_tilemap.GetHeight() },
						GetTileCoordFromMapPosition(m_tilemap, m_tileSize, m_startFP.position),
						GetTileCoordFromMapPosition(m_tilemap, m_tileSize, m_goalFP.position),
						m_path,
						1000
					);
				}
			}

			// calculate waypoints
			{
				navigation::tile::Footprint<float> fp;
				fp.anchor = navigation::tile::Anchor::Center;
				fp.size = m_startFP.size;

				// let's ignore the first and last tile. they are start and goal positions
				m_wayPoints.clear();
				for (int i = 1; i + 1 < m_path.size(); i++)
				{
					// preferred position in tile is at center so we calculate center (at world/tile space)
					fp.position.x = m_path[i].col * m_tileSize.width + m_tileSize.width / 2;
					fp.position.y = m_path[i].row * m_tileSize.height + m_tileSize.height / 2;

					if (!m_footprintResolver.IsValid(m_tilemap, m_tileSize, fp))
					{
						// DEBUG:: we just set random value here. fix this so that we set appropriate value
						m_footprintResolver.TryResolve(m_tilemap, m_tileSize, fp, fp);
					}
					m_wayPoints.push_back(fp.position);
				}
			}


		}

		void OnRender()
		{
			m_engine.GetRenderer().EnableClipping(true);

			// draw the map
			RenderTileMap(m_tilemap);

			// draw the footprint at the start position
			m_engine.GetRenderer().Draw(m_camera.WorldToScreen(m_startFP.GetRect().GetTopLeft()), m_startFP.size, graphics::ColorF{ 0,1,0,0.5f }, 0);

			// draw the footprint at the goal position
			m_engine.GetRenderer().Draw(m_camera.WorldToScreen(m_goalFP.GetRect().GetTopLeft()), m_goalFP.size, graphics::ColorF{ 1,0,0,0.5f }, 0);

			// render path
			{
				for (const component::tile::TileCoord& tile : m_path)
				{
					spatial::PosF pos
					{
						m_tileSize.width * tile.col,
						m_tileSize.height * tile.row

					};

					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(pos), m_tileSize, graphics::ColorF{ 0,0,0.5,0.5f }, 0);
				}
			}

			// render waypoints
			{
				for (const spatial::PosF& pos : m_wayPoints)
				{
					math::VecF translate = { m_startFP.size.width / 2, m_startFP.size.height / 2 };
					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(pos - translate), m_startFP.size, graphics::ColorF{ 0,1,0,0.2f }, 0);
				}
			}

			// render text
			{
				m_engine.GetRenderer().EnableClipping(false);

				std::string str;

				str.clear();
				str.append("tile size: ");
				str.append(std::to_string((int)std::round(m_tileSize.width)));
				str.append(",");
				str.append(std::to_string((int)std::round(m_tileSize.height)));
				m_engine.GetRenderer().DrawText(
					m_fontLarge,
					str,
					900,
					50,
					1, 1, 1, 1
				);

				str.clear();
				str.append("foot size: ");
				str.append(std::to_string((int)std::round(m_startFP.size.width)));
				str.append(",");
				str.append(std::to_string((int)std::round(m_startFP.size.height)));
				m_engine.GetRenderer().DrawText(
					m_fontLarge,
					str,
					900,
					100,
					1, 1, 1, 1
				);
			}

			return;

		}

		void RenderTileMap(component::tile::TileLayer<Tile>& tilemap)
		{
			float thickness = 1.0f;
			spatial::SizeF inner{ m_tileSize.width - thickness * 2, m_tileSize.height - thickness * 2 };

			// draw the tile map
			for (int row = 0; row < tilemap.GetHeight(); row++)
			{
				for (int col = 0; col < tilemap.GetWidth(); col++)
				{
					Tile tile = tilemap.GetTile(row, col);

					graphics::ColorF color;
					switch (tile.index)
					{
					case 0:
						color = { 0.5f, 0.5f, 0.5f, 1 };
						break;
					case 1:
						color = { 0, 0, 0, 1 };
						break;
					default:
						color = { 0, 0, 0, 1 };
						break;
					}
					spatial::PosF pos
					{
						m_tileSize.width * col,
						m_tileSize.height * row
					};

					spatial::PosF posInner
					{
						m_tileSize.width * col + thickness,
						m_tileSize.height * row + thickness
					};

					// draw stuff
					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(pos), m_tileSize, { 0,0,0,1 }, 0);
					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(posInner), inner, color, 0);
				}
			}
		}

		void SetTileLayer(component::tile::TileLayer<Tile>& tileLayer, int width, int height, Tile tile)
		{
			tileLayer.SetSize({ width, height });
			for (int row = 0; row < height; row++)
			{
				for (int col = 0; col < width; col++)
				{
					tileLayer.SetTile(row, col, tile);
				}
			}
		}

		component::tile::TileCoord GetTileCoordFromMapPosition(const component::tile::TileLayer<Tile>& tileLayer, const spatial::SizeF& tileSize, const spatial::PosF pos)
		{
			return component::tile::TileCoord
			{
				static_cast<int>(std::floor(pos.y / tileSize.height)),
				static_cast<int>(std::floor(pos.x / tileSize.width))
			};
		}
	};

	class TestBed1
	{
	private:
		struct Tile
		{
			int index;
		};
		engine::Engine m_engine;
		component::tile1::TileLayer m_tilemap;
		component::tile1::Tileset m_tileset;
		spatial::Camera m_camera;
		std::shared_ptr<graphics::resource::IFontAtlas> m_fontSmall;
		std::shared_ptr<graphics::resource::IFontAtlas> m_fontLarge;

		bool m_searchDone = false;

		spatial::SizeF m_tileSize;

		spatial::PosF m_lastMousePos;
		spatial::SizeF m_footPrintSize;

		navigation::tile1::Footprint<float> m_startFP;
		navigation::tile1::Footprint<float> m_goalFP;
		navigation::tile::PathFinder m_pathFinder;
		std::vector<component::tile::TileCoord> m_path;
		std::vector<spatial::PosF> m_wayPoints;
		navigation::tile1::FootprintResolver m_footprintResolver;

	public:
		TestBed1() :
			m_engine("DirectX11", "Batch"),
			m_camera({ 50, 50, 1024, 768 }),
			m_tileSize{ 50, 50 },
			m_footPrintSize{ 480, 480 },
			m_footprintResolver
			(
				[this](int row, int col) -> bool
				{
					if (m_tilemap.IsValidTile(row, col))
					{
						int id = m_tilemap.GetTileInstance(row, col).typeId;
						if (m_tileset.IsValid(id))
						{
							return m_tileset.GetTile(id).IsWalkable();
						}
					}
					return false;
				},
				0.1f, m_tileSize.width / 0.1f, m_tileSize.height / 0.1f, true
			),
			m_pathFinder(
				[this](int currRow, int currCol, int NextRow, int NextCol) -> bool
				{
					auto isWalkable = [&](int row, int col) -> bool
						{
							if (m_tilemap.IsValidTile(row, col))
							{
								int id = m_tilemap.GetTileInstance(row, col).typeId;
								if (m_tileset.IsValid(id))
								{
									return m_tileset.GetTile(id).IsWalkable();
								}
							}
							return false;
						};

					// quick check if the tile itself is walkable
					if (!isWalkable(NextRow, NextCol)) return false;


					if (navigation::tile1::Constraints::IsPinchBlocked(
						m_tilemap,
						m_tileset,
						{ currRow, currCol },
						{ NextRow, NextCol },
						m_startFP.size,
						m_tileSize
					))
					{
						return false;
					}

					// create a footprint where it is located at the center of this tile coordinate. let's set this as default position of the footprint
					// if this position is safe (no collision with any obstacle) then this tile is walkable. 
					// if not safe (collides with an obstacle), we'll try to nudge
					navigation::tile1::Footprint<float> fp;
					fp.anchor = navigation::tile1::Anchor::Center;
					fp.position.x = NextCol * m_tileSize.width + m_tileSize.width / 2;
					fp.position.y = NextRow * m_tileSize.height + m_tileSize.height / 2;
					fp.size = m_startFP.size;

					if (m_footprintResolver.IsValid(m_tilemap, m_tileSize, fp))
					{
						return true;
					}

					// try to nudge if possible
					navigation::tile1::Footprint outFP;
					if (m_footprintResolver.TryResolve(m_tilemap, m_tileSize, fp, outFP))
					{
						component::tile::TileCoord outTC = GetTileCoordFromMapPosition(m_tileSize, outFP.position);
						return (outTC.row == NextRow && outTC.col == NextCol);
					}

					return false;
				},
				1000,
				false,
				false
			)
		{
			m_engine.OnStart += event::Handler(this, &TestBed1::OnStart);
			m_engine.OnUpdate += event::Handler(this, &TestBed1::OnUpdate);
			m_engine.OnRender += event::Handler(this, &TestBed1::OnRender);
			m_engine.OnResize += event::Handler(this, &TestBed1::OnResize);

			input::Input::Instance().OnKeyDown += event::Handler(this, &TestBed1::OnKeyDown);
			input::Input::Instance().OnMouseDown += event::Handler(this, &TestBed1::OnMouseDown);
			input::Input::Instance().OnMouseMove += event::Handler(this, &TestBed1::OnMouseMove);
			input::Input::Instance().OnMouseUp += event::Handler(this, &TestBed1::OnMouseUp);

			m_engine.Run();
		}

		void OnResize(size_t width, size_t height)
		{
		}

		void OnMouseMove(int x, int y)
		{
			m_lastMousePos = { static_cast<float>(x), static_cast<float>(y) };
		}

		void OnMouseDown(int btn, int x, int y)
		{
			if (btn == 1) // left button
			{
				m_startFP.position = m_camera.ScreenToWorld({ (float)x, (float)y });
			}
			else if (btn == 2) // right button
			{
				m_goalFP.position = m_camera.ScreenToWorld({ (float)x, (float)y });
			}

			component::tile::TileCoord tileStartFP = GetTileCoordFromMapPosition(m_tileSize, m_startFP.position); // debug only, useless
			component::tile::TileCoord tileGoalFP = GetTileCoordFromMapPosition(m_tileSize, m_goalFP.position); // debug only, useless
			tileGoalFP;
		}

		void OnMouseUp(int btn, int x, int y)
		{
		}

		void OnKeyDown(int key)
		{

			if (key == 32) // space key
			{
			}
			if (key == 49) // 1
			{
				// set selected tile as obstacle
				spatial::PosF worldPos = m_camera.ScreenToWorld(m_lastMousePos);
				component::tile::TileCoord tileCoord
				{
					static_cast<int>(worldPos.y / m_tileSize.height),
					static_cast<int>(worldPos.x / m_tileSize.width)
				};
				if (tileCoord.row >= 0 && tileCoord.row < m_tilemap.GetHeight() && tileCoord.col >= 0 && tileCoord.col < m_tilemap.GetWidth())
				{
					component::tile1::TileInstance tileInst;
					tileInst.typeId = 1;
					m_tilemap.SetTileInstance(tileCoord.row, tileCoord.col, tileInst);
				}
			}
			if (key == 50) // 2
			{
				// set selected tile as obstacle
				spatial::PosF worldPos = m_camera.ScreenToWorld(m_lastMousePos);
				component::tile::TileCoord tileCoord
				{
					static_cast<int>(worldPos.y / m_tileSize.height),
					static_cast<int>(worldPos.x / m_tileSize.width)
				};
				if (tileCoord.row >= 0 && tileCoord.row < m_tilemap.GetHeight() && tileCoord.col >= 0 && tileCoord.col < m_tilemap.GetWidth())
				{
					component::tile1::TileInstance tileInst;
					tileInst.typeId = 0;
					m_tilemap.SetTileInstance(tileCoord.row, tileCoord.col, tileInst);
				}
			}
			if (key == 51) // 3
			{
				// remove all obstacles
				for (int row = 0; row < m_tilemap.GetHeight(); row++)
				{
					for (int col = 0; col < m_tilemap.GetWidth(); col++)
					{
						if (!m_tileset.GetTile(m_tilemap.GetTileInstance(row, col).typeId).IsWalkable())
						{
							component::tile1::TileInstance tileInst;
							tileInst.typeId = 0;
							m_tilemap.SetTileInstance(row, col, tileInst);
						}
					}
				}
			}
			if (key == 52) // 4
			{

			}
			if (key == 53)
			{
				m_startFP.size.width += 5;
				m_goalFP.size.width += 5;
			}
			if (key == 54)
			{
				m_startFP.size.width -= 5;
				m_goalFP.size.width -= 5;
			}
			if (key == 55)
			{
				m_startFP.size.height += 5;
				m_goalFP.size.height += 5;
			}

			if (key == 56)
			{
				m_startFP.size.height -= 5;
				m_goalFP.size.height -= 5;
			}
		}

		void OnStart()
		{
			m_tileset.Register(0, std::make_unique<component::tile1::WalkableTile>());   // ID 0 → Walkable
			m_tileset.Register(1, std::make_unique<component::tile1::ObstacleTile>());   // ID 1 → Obstacle

			SetTileLayer(m_tilemap, 16, 16, component::tile1::TileInstance{0,0});

			m_camera.SetViewport(
				{
					50,
					50,
					50 + m_tilemap.GetWidth() * m_tileSize.width,
					50 + m_tilemap.GetHeight() * m_tileSize.height
				}
			);

			m_camera.SetWorldSize(
				m_tilemap.GetWidth() * m_tileSize.width,
				m_tilemap.GetHeight() * m_tileSize.height
			);

			m_engine.GetRenderer().SetClipRegion(m_camera.GetViewport());
			m_engine.GetRenderer().EnableClipping(true);

			// create utility font atlas
			m_fontSmall = std::make_shared<graphics::resource::FontAtlas>(graphics::factory::TextureFactory::Create());
			m_fontSmall->Initialize("Arial", 12);

			m_fontLarge = std::make_shared<graphics::resource::FontAtlas>(graphics::factory::TextureFactory::Create());
			m_fontLarge->Initialize("Arial", 24);

			m_goalFP.size = m_startFP.size = { 75, 75 };
			m_goalFP.anchor = m_startFP.anchor = navigation::tile1::Anchor::Center;
			m_goalFP.position = m_startFP.position = { -10000, -10000 };
		}

		void OnUpdate(float delta)
		{
			// resolve positions of start and goal footprints in case they collide with obstacles
			{
				// DEBUG:: we just set random value here. fix this so that we set appropriate value
				m_footprintResolver.TryResolve(m_tilemap, m_tileSize, m_startFP, m_startFP);
				m_footprintResolver.TryResolve(m_tilemap, m_tileSize, m_goalFP, m_goalFP);
			}

			// get the path
			{
				component::tile::TileCoord tileStartFP = GetTileCoordFromMapPosition(m_tileSize, m_startFP.position);
				component::tile::TileCoord tileGoalFP = GetTileCoordFromMapPosition(m_tileSize, m_goalFP.position);

				if (m_tilemap.IsValidTile(tileStartFP) && m_tilemap.IsValidTile(tileGoalFP))
				{
					m_pathFinder.FindPath(
						math::geometry::Rect<int>{ 0, 0, m_tilemap.GetWidth(), m_tilemap.GetHeight() },
						GetTileCoordFromMapPosition(m_tileSize, m_startFP.position),
						GetTileCoordFromMapPosition(m_tileSize, m_goalFP.position),
						m_path,
						1000
					);
				}
			}

			// calculate waypoints
			{
				navigation::tile1::Footprint<float> fp;
				fp.anchor = navigation::tile1::Anchor::Center;
				fp.size = m_startFP.size;

				// let's ignore the first and last tile. they are start and goal positions
				m_wayPoints.clear();
				for (int i = 1; i + 1 < m_path.size(); i++)
				{
					// preferred position in tile is at center so we calculate center (at world/tile space)
					fp.position.x = m_path[i].col * m_tileSize.width + m_tileSize.width / 2;
					fp.position.y = m_path[i].row * m_tileSize.height + m_tileSize.height / 2;

					if (!m_footprintResolver.IsValid(m_tilemap, m_tileSize, fp))
					{
						// DEBUG:: we just set random value here. fix this so that we set appropriate value
						m_footprintResolver.TryResolve(m_tilemap, m_tileSize, fp, fp);
					}
					m_wayPoints.push_back(fp.position);
				}
			}


		}

		void OnRender()
		{
			m_engine.GetRenderer().EnableClipping(true);

			// draw the map
			RenderTileMap(m_tilemap);

			// draw the footprint at the start position
			m_engine.GetRenderer().Draw(m_camera.WorldToScreen(m_startFP.GetRect().GetTopLeft()), m_startFP.size, graphics::ColorF{ 0,1,0,0.5f }, 0);

			// draw the footprint at the goal position
			m_engine.GetRenderer().Draw(m_camera.WorldToScreen(m_goalFP.GetRect().GetTopLeft()), m_goalFP.size, graphics::ColorF{ 1,0,0,0.5f }, 0);

			// render path
			{
				for (const component::tile::TileCoord& tile : m_path)
				{
					spatial::PosF pos
					{
						m_tileSize.width * tile.col,
						m_tileSize.height * tile.row

					};

					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(pos), m_tileSize, graphics::ColorF{ 0,0,0.5,0.5f }, 0);
				}
			}

			// render waypoints
			{
				for (const spatial::PosF& pos : m_wayPoints)
				{
					math::VecF translate = { m_startFP.size.width / 2, m_startFP.size.height / 2 };
					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(pos - translate), m_startFP.size, graphics::ColorF{ 0,1,0,0.2f }, 0);
				}
			}

			// render text
			{
				m_engine.GetRenderer().EnableClipping(false);

				std::string str;

				str.clear();
				str.append("tile size: ");
				str.append(std::to_string((int)std::round(m_tileSize.width)));
				str.append(",");
				str.append(std::to_string((int)std::round(m_tileSize.height)));
				m_engine.GetRenderer().DrawText(
					m_fontLarge,
					str,
					900,
					50,
					1, 1, 1, 1
				);

				str.clear();
				str.append("foot size: ");
				str.append(std::to_string((int)std::round(m_startFP.size.width)));
				str.append(",");
				str.append(std::to_string((int)std::round(m_startFP.size.height)));
				m_engine.GetRenderer().DrawText(
					m_fontLarge,
					str,
					900,
					100,
					1, 1, 1, 1
				);
			}

			return;

		}

		void RenderTileMap(component::tile1::TileLayer& tilemap)
		{
			float thickness = 1.0f;
			spatial::SizeF inner{ m_tileSize.width - thickness * 2, m_tileSize.height - thickness * 2 };

			// draw the tile map
			for (int row = 0; row < tilemap.GetHeight(); row++)
			{
				for (int col = 0; col < tilemap.GetWidth(); col++)
				{
					component::tile1::TileInstance tileInst = tilemap.GetTileInstance(row, col);

					graphics::ColorF color;
					switch (tileInst.typeId)
					{
					case 0:
						color = { 0.5f, 0.5f, 0.5f, 1 };
						break;
					case 1:
						color = { 0, 0, 0, 1 };
						break;
					default:
						color = { 0, 0, 0, 1 };
						break;
					}
					spatial::PosF pos
					{
						m_tileSize.width * col,
						m_tileSize.height * row
					};

					spatial::PosF posInner
					{
						m_tileSize.width * col + thickness,
						m_tileSize.height * row + thickness
					};

					// draw stuff
					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(pos), m_tileSize, { 0,0,0,1 }, 0);
					m_engine.GetRenderer().Draw(m_camera.WorldToScreen(posInner), inner, color, 0);
				}
			}
		}

		void SetTileLayer(component::tile1::TileLayer& tileLayer, int width, int height, component::tile1::TileInstance tileInst)
		{
			tileLayer.SetSize({ width, height });
			for (int row = 0; row < height; row++)
			{
				for (int col = 0; col < width; col++)
				{
					tileLayer.SetTileInstance(row, col, tileInst);
				}
			}
		}

		component::tile::TileCoord GetTileCoordFromMapPosition(const spatial::SizeF& tileSize, const spatial::PosF pos)
		{
			return component::tile::TileCoord
			{
				static_cast<int>(std::floor(pos.y / tileSize.height)),
				static_cast<int>(std::floor(pos.x / tileSize.width))
			};
		}
	};

}

