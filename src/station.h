#pragma once
#include <map>
#include <vector>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* ========================= */
#define STATION_RANDOM_COUNT 100
#define STATION_LINK_RANGE 24
/* ========================= */

class Wall;

class Station
{
public:
	int id;
	glm::vec3 pos;
	std::map<int, float> stations_linked;

	float g;
	float h;
	float f;
	int parent_id;

	Station();
	Station(int id, float x, float y);
	Station(int id, glm::vec3 pos);
	~Station();

	void update(float g, float h, int parent_id);

	void search(Station* stations, int num_stations);

	void search(std::vector<Station> stations, Wall** walls, int num_walls);
};

static bool compare_station_f(std::pair<int, Station> p1, std::pair<int, Station> p2)
{
	return p1.second.f < p2.second.f;
}


static std::vector<int> a_star(std::vector<Station> stations, int start, int dest)
{
	auto clone = stations;
	std::vector<int> out = std::vector<int>();
	bool is_find = false;

	if (start == dest)
	{
		out.push_back(start);
		return out;
	}

	std::map<int, Station> open = std::map<int, Station>();
	std::map<int, Station> close = std::map<int, Station>();

	// 1.  close Ʈ 
	clone[start].update(0, 0, -1);
	close.insert(std::make_pair(start, clone[start]));

	int current_id = start;
	while (current_id != dest)
	{
		//  忡 Ǿִ    Ÿ
		std::map<int, float> Stations_linked = clone[current_id].stations_linked;

		// (in while) 2. ߳    ϸ鼭   f Open Ʈ 
		for (auto iter : Stations_linked)
		{
			// linked Station id
			int id = iter.first;

			// ptr :  ü ϱ  ͷ 
			Station* Station_linked = &clone[id];

			// distance : current Station ~ target Station
			float g = iter.second + clone[current_id].g;

			// distance : target Station ~ dest Station (linear distance without obstacle)
			float h = distance(Station_linked->pos, clone[dest].pos);

			if (Station_linked->f > g + h)
			{
				// close ִ ׸ ƴ϶ ֽȭ
				if (close.count(Station_linked->id) == 0)
				{
					Station_linked->update(g, h, current_id);
				}

				// Open List  Close List  ߰ (Open վ Close   )
				if (open.count(Station_linked->id) == 0 && close.count(Station_linked->id) == 0)
					open.insert({ Station_linked->id, *Station_linked });
			}
		}

		if (open.size() <= 0)
		{
			break;
		}

		// open list  f      տ  close list ֱ  ӽ 
		std::vector<std::pair<int, Station>> dump = std::vector<std::pair<int, Station>>();

		// (in while) 3. open list ִ ׸ ߿  ٰ Ǵϴ  close 
		if (open.size() > 0)
		{
			//    : map -> vector -> sort (require cmp function)
			dump = std::vector<std::pair<int, Station>>(open.begin(), open.end());
			std::sort(dump.begin(), dump.end(), compare_station_f);

			//  ª  ->  
			std::pair<int, Station> next_Station = dump.front();

			//  ª  close 
			close.insert(next_Station);

			// open ִ Station 
			open.erase(next_Station.first);

			current_id = next_Station.second.id;
		}
	}

	if (current_id == dest)
	{
		is_find = true;
	}

	if (is_find)
	{
		int k = dest;
		while (k != start)
		{
			out.push_back(k);
			k = clone[k].parent_id;
		}

		out.push_back(start);

		// Start -> End  
		std::reverse(out.begin(), out.end());

		return out;
	}
	else
	{
		return out;
	}
}