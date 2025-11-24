#pragma once
#include "Tile.h"

namespace navigation
{
	namespace tile
	{
		constexpr int CardinalCost = 10;
		constexpr int DiagonalCost = 14;

		struct Node
		{
			component::tile::TileCoord pos;
			int g = 0;
			int h = 0;
			component::tile::TileCoord parent;
			bool closed = false;
			bool open = false;

			int f() const
			{
				return g + h;
			}
		};

		enum class HeuristicType
		{
			Manhattan,
			Euclidean,
			Octile
		};

		class PathFinder
		{
		private:
			// for debugging purposes, we keep track of all nodes, open tiles, and closed tiles
			std::vector<std::vector<Node>> m_nodes;
			std::vector<component::tile::TileCoord> m_openTiles;
			std::vector<component::tile::TileCoord> m_closedTiles;

			bool m_diagonal;
			int m_maxSteps;
			bool m_cutCorners;
			std::function<bool(int, int, int, int)> m_isWalkable;
			navigation::tile::HeuristicType m_heuristicType;

			int Heuristic(const component::tile::TileCoord& a, const component::tile::TileCoord& b) const
			{
				switch (m_heuristicType)
				{
				case navigation::tile::HeuristicType::Manhattan:
					return HeuristicManhattan(a, b);
				case navigation::tile::HeuristicType::Euclidean:
					return HeuristicEuclidean(a, b);
				case navigation::tile::HeuristicType::Octile:
					return HeuristicOctile(a, b);
				default:
					throw std::invalid_argument("Unknown heuristic type");
				}
			}

			int HeuristicEuclidean(const component::tile::TileCoord& a, const component::tile::TileCoord& b) const
			{
				// get distance in rows and columns like in Manhattan heuristic
				int distanceRow = std::abs(a.row - b.row);
				int distanceCol = std::abs(a.col - b.col);

				return static_cast<int>(CardinalCost * std::sqrt(distanceCol * distanceCol + distanceRow * distanceRow));
			}

			int HeuristicManhattan(const component::tile::TileCoord& a, const component::tile::TileCoord& b) const
			{
				// get distance in rows and columns like in Manhattan heuristic
				int distanceRow = std::abs(a.row - b.row);
				int distanceCol = std::abs(a.col - b.col);

				return (distanceCol + distanceRow) * CardinalCost;
			}

			// Octile heuristic
			int HeuristicOctile(const component::tile::TileCoord& a, const component::tile::TileCoord& b) const
			{
				// get distance in rows and columns like in Manhattan heuristic
				int distanceRow = std::abs(a.row - b.row);
				int distanceCol = std::abs(a.col - b.col);

				// the diagonal distance is the minimum of the two distances
				int diagonalDistance = std::min<int>(distanceRow, distanceCol);

				// the cardinal distance is the difference between the two distances
				int cardinalDistance = std::abs(distanceRow - distanceCol);

				// cost is 14 for diagonal movement and 10 for cardinal movement. total cost is sum of both
				return diagonalDistance * DiagonalCost + cardinalDistance * CardinalCost;
			}

			std::vector<component::tile::TileCoord> GetNeighbors(
				const component::tile::TileCoord& pos,
				const int width, const int height
			) const
			{
				std::vector<component::tile::TileCoord> neighbors;

				// iterate through all adjacent tiles of the given tile coord, including those from its diagonals			
				for (int dr = -1; dr <= 1; ++dr)
				{
					for (int dc = -1; dc <= 1; ++dc)
					{
						// skip the tile itself
						if (dr == 0 && dc == 0)
						{
							continue;
						}

						// if diagonal movement is not allowed, skip diagonal neighbors
						if (!m_diagonal && dr != 0 && dc != 0)
						{
							continue;
						}

						int row = pos.row + dr;
						int col = pos.col + dc;

						// the width and height are supposed to be the size of the grid/map. any tile coord outside this range is invalid
						if (row < 0 || row >= height ||
							col < 0 || col >= width)
						{
							continue;
						}

						// add to neighbors list as local coordinates to the region
						neighbors.push_back({ row, col });
					}
				}

				return neighbors;
			}

		public:
			PathFinder(
				std::function<bool(int, int, int, int)> isWalkable,
				int maxSteps = 1000,
				bool diagonal = false,
				bool cutCorners = false,
				navigation::tile::HeuristicType heuristicType = navigation::tile::HeuristicType::Octile
			) :
				m_isWalkable(isWalkable),
				m_maxSteps(maxSteps),
				m_diagonal(diagonal),
				m_cutCorners(cutCorners),
				m_heuristicType(heuristicType)
			{
			}

			const std::vector<component::tile::TileCoord>& GetOpenTiles() const
			{
				return m_openTiles;
			}

			const std::vector<component::tile::TileCoord>& GetClosedTiles() const
			{
				return m_closedTiles;
			}

			const std::vector<std::vector<Node>>& GetNodes() const
			{
				return m_nodes;
			}

			void EnableCutCorners(bool enabled)
			{
				m_cutCorners = enabled;
			}

			void EnableDiagonal(bool enabled)
			{
				m_diagonal = enabled;
			}

			void SetMaxSteps(int steps)
			{
				m_maxSteps = steps;
			}

			bool FindPath(
				const math::geometry::Rect<int>& region,
				const component::tile::TileCoord& start,
				const component::tile::TileCoord& goal,
				std::vector<component::tile::TileCoord>& outPath,
				int debugSteps = 0
			)
			{
				// clear previous data
				m_openTiles.clear();
				m_closedTiles.clear();
				outPath.clear();

				// set size of the region and initialize nodes
				int width = region.right - region.left;
				int height = region.bottom - region.top;
				m_nodes.assign(height, std::vector<Node>(width));

				// translate start and goal to region coordinates
				component::tile::TileCoord regionStart = { start.row - region.top, start.col - region.left };
				component::tile::TileCoord regionGoal = { goal.row - region.top, goal.col - region.left };

				// initialize start node	
				m_nodes[regionStart.row][regionStart.col].pos = regionStart;						// tile coordinate
				m_nodes[regionStart.row][regionStart.col].g = 0;									// cost from start
				m_nodes[regionStart.row][regionStart.col].h = Heuristic(regionStart, regionGoal);	// heuristic cost to goal
				m_nodes[regionStart.row][regionStart.col].open = true;								// mark as in open list

				// add start node's tile coordinate to open list
				m_openTiles.push_back(regionStart);

				debugSteps--;
				if (debugSteps <= 0) return false;

				int steps = m_maxSteps;
				while (!m_openTiles.empty() && steps-- > 0)
				{
					// find node in open list with lowest f = g + h
					auto bestIt = m_openTiles.begin();
					for (auto it = m_openTiles.begin(); it != m_openTiles.end(); ++it)
					{
						// this is the node of the current tile coordinate
						const Node& a = m_nodes[it->row][it->col];

						// this is the node of the best tile coordinate found so far
						const Node& b = m_nodes[bestIt->row][bestIt->col];

						// if f is the same, prefer node with lower h
						int af = a.g + a.h;
						int bf = b.g + b.h;
						if (af < bf || (af == bf && a.h < b.h)) bestIt = it;
					}

					// copy tile coordinate of the node with the lowest f from open list
					component::tile::TileCoord currentTile = *bestIt;

					// get reference to the node with the lowest f 
					Node& currentNode = m_nodes[currentTile.row][currentTile.col];

					// since we now have a copy of the tile coordinate of the node with with the lowest f, we can remove it from open list
					m_openTiles.erase(bestIt);

					// since this node is now being processed, mark it as closed
					currentNode.open = false;
					currentNode.closed = true;
					m_closedTiles.push_back(currentTile);

					// did we reach the goal?
					if (currentTile == regionGoal)
					{
						// for now, we store the path in reverse order (from goal to start)
						component::tile::TileCoord tc = regionGoal;
						while (tc != regionStart)
						{
							// now we translate back to world coordinates
							outPath.push_back({ tc.row + region.top, tc.col + region.left });

							// move to parent
							tc = m_nodes[tc.row][tc.col].parent;
						}
						// finally, add the start tile in world coordinates
						outPath.push_back(start);

						// reverse the path to be from start to goal
						std::reverse(outPath.begin(), outPath.end());
						return true;
					}

					// iterate over neighbor tiles of the current tile
					for (const component::tile::TileCoord& neighborTile : GetNeighbors(currentTile, width, height))
					{
						// get the tile coordinates of this neighbor tile
						int neighborTileRow = region.top + neighborTile.row;
						int neighborTileCol = region.left + neighborTile.col;

						// skip non-walkable tiles. note that we check walkability in world coordinates
						if (!m_isWalkable(currentTile.row, currentTile.col, neighborTileRow, neighborTileCol)) continue;

						// if cutting corners is not allowed, skip diagonal neighbors that would require cutting corners
						if (!m_cutCorners &&
							neighborTile.row != currentTile.row &&
							neighborTile.col != currentTile.col
							)
						{
							// if current tile and neighbor tile are diagonal to each other, then check if both adjacent orthogonal tiles are walkable
							if (!m_isWalkable(currentTile.row, currentTile.col, region.top + currentTile.row, region.left + neighborTile.col) ||
								!m_isWalkable(currentTile.row, currentTile.col, region.top + neighborTile.row, region.left + currentTile.col))
							{
								continue;
							}
						}

						// get the node of the neighbor tile. if this tile is already closed, skip it
						Node& neighborNode = m_nodes[neighborTile.row][neighborTile.col];
						if (neighborNode.closed)
						{
							continue;
						}

						// at this point, this neighbor tile will need to be analyzed
						// are we moving diagonally if we are to move from current tile to this neighbor?
						// if yes, which direction?
						{

						}


						// calculate tentative g cost considering diagonal movement
						int tentativeG = neighborTile.row != currentTile.row && neighborTile.col != currentTile.col ?
							currentNode.g + 14 :	// diagonal movement cost is 14
							currentNode.g + 10;		// orthogonal movement cost is 10

						// if this neighbor node is not in open list yet
						if (!neighborNode.open)
						{
							// initialize neighbor node
							neighborNode.pos = neighborTile;
							neighborNode.parent = currentTile;

							// g cost is cost from start tile to this neighbor tile via current tile
							neighborNode.g = tentativeG;

							// h cost is heuristic cost from this neighbor tile to goal tile
							// both nighborTile and regionGoal are in region coordinates
							neighborNode.h = Heuristic(neighborTile, regionGoal);

							// add it to open list
							m_openTiles.push_back(neighborTile);
							neighborNode.open = true;
						}

						// else if this neighbor node is already in open list, check if this path to neighbor tile is better (lower g cost)
						else if (tentativeG < neighborNode.g)
						{
							// update parent to current tile
							neighborNode.parent = currentTile;

							// update g cost to the lower tentative g cost
							neighborNode.g = tentativeG;
						}
					}

					debugSteps--;
					if (debugSteps <= 0) return false;
				}


				return true;
			}
		};

		static std::vector<spatial::PosF> GetWayPoints(std::vector<component::tile::TileCoord>& path)
		{
			std::vector<spatial::PosF> wp;

			if (path.size() < 2)
			{
				return wp;	
			}

			wp.push_back(
				{
					(float)path[0].col,
					(float)path[0].row
				}
			);

			math::VecF currDir
			{
				(float)(path[1].col - path[0].col),
				(float)(path[1].row - path[0].row)
			};

			for (size_t i = 2; i < path.size(); i++)
			{
				math::VecF dir
				{
					(float)(path[i].col - path[i-1].col),
					(float)(path[i].row - path[i-1].row)
				};

				if (dir.x != currDir.x || dir.y != currDir.y)
				{
					wp.push_back(
						{
							(float)path[i-1].col,
							(float)path[i-1].row
						}
					);

					currDir = dir;
				}
			}


			wp.push_back(
				{
					(float)path[path.size() - 1].col,
					(float)path[path.size() - 1].row
				}
			);

			return wp;
		}
	}

}

