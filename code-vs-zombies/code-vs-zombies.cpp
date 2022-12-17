#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Vector2 {
 public:
  int x;
  int y;

  Vector2(int x = 0, int y = 0) {
    this->x = x;
    this->y = y;
  }

  int distance(Vector2 point2) {
    Vector2 point1 = *this;
    return abs(point1.x - point2.x) + abs(point2.y - point2.y);
  }

  Vector2 closest_point(vector<Vector2> points, int starting_index = 0) {
    Vector2 point = *this;
    int min_distance = INT_MAX;
    Vector2 closest_point;
    for (int i = starting_index; i < points.size(); i++) {
      int distance = point.distance(points[i]);
      if (distance < min_distance) {
        min_distance = distance;
        closest_point = points[i];
      }
    }
    return closest_point;
  }

  friend ostream& operator<<(ostream& os, const Vector2& v) {
    os << "X: " << v.x << " Y: " << v.y;
    return os;
  }
};

class Human {
 public:
  Vector2 position = *new Vector2();
  const int zombie_move_distance = 400;
  const int player_move_distance = 1000;
  const int player_range = 2000;

  Human(int x, int y) {
    position.x = x;
    position.y = y;
  }

  int turns_to_die(vector<Vector2> zombie_positions) {
    Human human = *this;
    Vector2 closest_zombie = human.find_closest_zombie(zombie_positions);
    return human.position.distance(closest_zombie) / zombie_move_distance;
  }

  int turns_to_save(Vector2 player) {
    Human human = *this;
    return (human.position.distance(player) - player_range) / player_move_distance;
  }

  Vector2 find_closest_zombie(vector<Vector2> zombie_positions) {
    Human human = *this;
    return human.position.closest_point(zombie_positions);
  }

  bool can_be_saved(Vector2 player, vector<Vector2> zombie_positions) {
    Human h = *this;
    Vector2 human = h.position;
    Vector2 closest_zombie = find_closest_zombie(zombie_positions);
    int turns_until_death = turns_to_die(zombie_positions);
    int turns_until_saved = turns_to_save(player);
    return turns_until_death > turns_until_saved;
  }
};

int main() {
  Vector2 player = *new Vector2();

  // game loop
  while (1) {
    int x;
    int y;

    cin >> x >> y;
    cin.ignore();

    player.x = x;
    player.y = y;

    int human_count;
    cin >> human_count;
    cin.ignore();
    vector<Human> humans;
    vector<Vector2> human_positions;
    for (int i = 0; i < human_count; i++) {
      int human_id;
      int human_x;
      int human_y;
      cin >> human_id >> human_x >> human_y;
      cin.ignore();
      Human human = *new Human(human_x, human_y);
      humans.push_back(human);
      human_positions.push_back(human.position);
    }
    int zombie_count;
    cin >> zombie_count;
    cin.ignore();

    vector<Vector2> zombie_positions;
    for (int i = 0; i < zombie_count; i++) {
      int zombie_id;
      int zombie_x;
      int zombie_y;
      int zombie_xnext;
      int zombie_ynext;
      cin >> zombie_id >> zombie_x >> zombie_y >> zombie_xnext >> zombie_ynext;
      cin.ignore();
      Vector2 coords = *new Vector2(zombie_xnext, zombie_ynext);
      zombie_positions.push_back(coords);
    }

    Vector2 closest_human;

    for (int i = 0; i < humans.size(); i++) {
      Human human = humans[i];
      closest_human = player.closest_point(human_positions, i);
      cerr << closest_human << endl;
      Vector2 closest_zombie = human.find_closest_zombie(zombie_positions);
      if (human.can_be_saved(player, zombie_positions)) break;
    }
    cout << closest_human.x << " " << closest_human.y << endl;
  }
}

// How does the closest human change after I get closer to it? That doesn't seem right

// Maybe I should move toward zombies if I can get back to the human in time