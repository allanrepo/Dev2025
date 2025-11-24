#pragma once
#include <vector>
#include "Sprite.h"
#include "ISizeable.h"
#include "IRenderer.h"
#include "Event.h"
#include "CSVFile.h"
#include <functional>

namespace component::tile
{
	struct Tile
	{
		int index;
	};

	struct TileCoord
	{
		int row;
		int col;

		bool operator==(const TileCoord& other) const
		{
			return row == other.row && col == other.col;
		}

		bool operator!=(const TileCoord& other) const
		{
			return !(*this == other);
		}
	};

	class TileSet
	{
	private:
		std::vector<graphics::resource::Sprite>  m_tiles;

	public:
	};

	template<typename T>
	class TileLayer : public spatial::IResizeable<int>
	{
	private:
		std::vector<T> m_map;
		spatial::Size<int> m_size;

	public:
		TileLayer()
		{
		}

		virtual void SetTile(int row, int col, const T& tile)
		{
			if (row < 0 || row >= m_size.height || col < 0 || col >= m_size.width)
			{
				throw std::out_of_range("TileLayer::SetTile - index out of bounds");
			}

			m_map[row * m_size.width + col] = tile;
		}

		virtual const T& GetTile(int row, int col) const
		{
			return m_map[row * m_size.width + col];
		}

		virtual bool IsValidTile(int row, int col) const
		{
			return !(row < 0 || row >= m_size.height || col < 0 || col >= m_size.width);
		}

		virtual bool IsValidTile(const TileCoord& tileCoord) const
		{
			return IsValidTile(tileCoord.row, tileCoord.col);
		}

		virtual const int GetWidth() const
		{
			return m_size.width;
		}

		virtual const int GetHeight() const
		{
			return m_size.height;
		}

		virtual const spatial::Size<int> GetSize() const
		{
			return m_size;
		}

		virtual void SetSize(const spatial::Size<int>& size)
		{
			m_map.resize(size.width * size.height);
			m_size = size;
		}

		virtual void SetHeight(const int height)
		{
			m_size.height = height;
		}

		virtual void SetWidth(const int width)
		{
			m_size.width = width;
		}
	};
}


#pragma once
#include <vector>
#include <memory>
#include <stdexcept>

namespace component::tile1
{
	// TileInstance (POD)
	struct TileInstance 
	{
		int typeId = -1;       // index into Tileset definitions
		float animPhase = 0.0; // optional per-instance state
		// other per-instance flags (fog, damage, etc.)
	};

	// ITile (definition)
	class ITile 
	{
	public:
		virtual ~ITile() = default;
		virtual bool IsWalkable() const = 0;
		virtual int GetCost() const = 0;
		// Add more behavior hooks as needed
	};

	// Example concrete definitions
	class WalkableTile : public ITile
	{
	public:
		bool IsWalkable() const override { return true; }
		int GetCost() const override { return 1; }
	};

	class ObstacleTile : public ITile
	{
	public:
		bool IsWalkable() const override { return false; }
		int GetCost() const override { return 999; }
	};

	// Tileset (definitions)
	class Tileset
	{
	private:
		std::vector<std::unique_ptr<ITile>> m_tiles;

	public:
		void Register(int id, std::unique_ptr<ITile> def)
		{
			if (id >= static_cast<int>(m_tiles.size()))
			{
				m_tiles.resize(id + 1);
			}
			m_tiles[id] = std::move(def);
		}

		bool IsValid(int id) const
		{
			return !(id < 0 || id >= static_cast<int>(m_tiles.size()) || !m_tiles[id]);
		}

		const ITile& GetTile(int id) const 
		{
			if (id < 0 || id >= static_cast<int>(m_tiles.size()) || !m_tiles[id])
			{
				throw std::runtime_error("Tileset::GetTile - invalid typeId");
			}
			return *m_tiles[id];
		}
	};

	// TileLayer 
	class TileLayer : public spatial::IResizeable<int>
	{
	private:
		std::vector<TileInstance> m_map;
		spatial::Size<int> m_size;

	public:
		// NOTE: passing by value because its lightweight POD
		void SetTileInstance(int row, int col, TileInstance tileInst)
		{
			if (!IsValidTile(row, col))
			{
				throw std::out_of_range("TileLayer::SetTileInstance - index out of bounds");
			}
			m_map[row * m_size.width + col] = tileInst;
		}

		const TileInstance& GetTileInstance(int row, int col) const 
		{
			if (!IsValidTile(row, col)) {
				throw std::out_of_range("TileLayer::GetTileInstance - index out of bounds");
			}
			return m_map[row * m_size.width + col];
		}

		virtual bool IsValidTile(int row, int col) const
		{
			return !(row < 0 || row >= m_size.height || col < 0 || col >= m_size.width);
		}

		virtual bool IsValidTile(const component::tile::TileCoord& tileCoord) const
		{
			return IsValidTile(tileCoord.row, tileCoord.col);
		}

		virtual const int GetWidth() const
		{
			return m_size.width;
		}

		virtual const int GetHeight() const
		{
			return m_size.height;
		}

		virtual const spatial::Size<int> GetSize() const
		{
			return m_size;
		}

		virtual void SetSize(const spatial::Size<int>& size)
		{
			m_map.resize(size.width * size.height);
			m_size = size;
		}

		virtual void SetHeight(const int height)
		{
			m_size.height = height;
		}

		virtual void SetWidth(const int width)
		{
			m_size.width = width;
		}
	};

	// helper method to check if given tile coordinate is walkable
	inline bool IsWalkable(const TileLayer& layer, const Tileset& tileset, int row, int col)
	{
		if (!layer.IsValidTile(row, col)) return false;
		int id = layer.GetTileInstance(row, col).typeId;
		return tileset.IsValid(id) && tileset.GetTile(id).IsWalkable();
	}
} // namespace navigation::tile
