#include "station.h"

#include "common.h"

Station::Station() { }
Station::Station(int id, float x, float y)
{
	this->id = id;
	this->pos.x = x;
	this->pos.y = y;
	stations_linked = std::map<int, float>();

	g = INFINITY;
	h = INFINITY;
	f = INFINITY;
	parent_id = -1;
}
Station::Station(int id, glm::vec3 pos)
{
	this->id = id;
	this->pos = pos;
	stations_linked = std::map<int, float>();

	g = INFINITY;
	h = INFINITY;
	f = INFINITY;
	parent_id = -1;
}
Station::~Station()
{
	//std::cout << "Destruct node : " << id << std::endl;
}

void Station::update(float g, float h, int parent_id)
{
	if (this->g <= g)
		return;

	this->g = g;
	this->h = h;
	this->f = g + h;
	this->parent_id = parent_id;
}

void Station::search(Station* stations, int num_stations)
{
	float range = (float)STATION_LINK_RANGE;

	for (int i = 0; i < num_stations; i++)
	{
		if (i == this->id)
			continue;

		float dist = distance(this->pos, stations[i].pos);

		if (dist < range)
		{
			this->stations_linked.insert(std::make_pair(i, dist));
		}
	}
}

void Station::search(std::vector<Station> stations, Wall** walls, int num_walls)
{
	float range = (float)STATION_LINK_RANGE;

	for (int i = 0; i < stations.size(); i++)
	{
		if (i == id)
			continue;

		Station linking = stations[i];

		float dist = distance(pos, linking.pos);

		if (dist < range)
		{
			bool is_intersect = false;
			for (int j = 0; j < num_walls; j++)
			{
				Wall w = *walls[j];
				glm::vec3 w_wh = glm::vec3(w.width, 0, w.height);

				is_intersect = intersect_line_rect(pos, linking.pos, w.x0, w_wh);

				if (is_intersect)
					break;
			}

			if (!is_intersect)
			{
				stations_linked.insert(std::make_pair(i, dist));
			}
		}
	}
}

